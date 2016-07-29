#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AtomicAudioParameter.h"


HrtfBiAuralAudioProcessor::HrtfBiAuralAudioProcessor()
	: bypassed_(false)
	, crossoverFreq_(new AtomicAudioParameter("CROSS FREQ", "Hz", 20, 2000, 150))
	, wetPercent_(new AtomicAudioParameter("Wet", "%", 0, 100, 100))
	, gainDb_(new AtomicAudioParameter("Gain", "dB", -15, 15, 0))
{
	// register parameters
	addParameter(crossoverFreq_);
	addParameter(wetPercent_);
	addParameter(gainDb_);

	// initialize crossover
	onAudioParameterChanged(crossoverFreq_);

	// load HRIR
	auto thisDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory();
	try
	{
		hrtfContainer_.loadHrir(thisDir.getFullPathName() + "/hrir/kemar.bin");
		hrirLoaded_ = true;
	}
	catch (std::ios_base::failure&)
	{
		hrirLoaded_ = false;
		bypassed_ = true;
	}
	hrtfContainer_.updateHRIR(0, 0);

	setLatencySamples(HRIR_LENGTH  / 2); // "almost true" ofc...HRTF isn't really linear phase...
}

HrtfBiAuralAudioProcessor::~HrtfBiAuralAudioProcessor()
{
}
const String HrtfBiAuralAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool HrtfBiAuralAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool HrtfBiAuralAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

double HrtfBiAuralAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int HrtfBiAuralAudioProcessor::getNumPrograms()
{
	return 1;
}

int HrtfBiAuralAudioProcessor::getCurrentProgram()
{
	return 0;
}

void HrtfBiAuralAudioProcessor::setCurrentProgram(int)
{
}

const String HrtfBiAuralAudioProcessor::getProgramName(int)
{
	return String();
}

void HrtfBiAuralAudioProcessor::changeProgramName(int, const String&)
{
}

void HrtfBiAuralAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	crossover_.set(sampleRate, crossover_.f0);
	loPassIn_.resize(samplesPerBlock);
	hiPassIn_.resize(samplesPerBlock);
	buffers_[0].resize(samplesPerBlock);
	buffers_[1].resize(samplesPerBlock);
	filters_[0].init(samplesPerBlock, HRIR_LENGTH);
	filters_[1].init(samplesPerBlock, HRIR_LENGTH);
	//hrirInterpValue.reset(sampleRate, defaultHrirInterpolationTime * samplesPerBlock / buffers_[0].size());
}

void HrtfBiAuralAudioProcessor::releaseResources()
{
}

void HrtfBiAuralAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer&)
{
	if (bypassed_)
		return;

	// TODO: Interpolation/crossfading since simply changing the filter's impulse response causes waveform discontinuities
	const auto& hrir = hrtfContainer_.hrir();
	filters_[0].setImpulseResponse(hrir[0].data());
	filters_[1].setImpulseResponse(hrir[1].data());

	auto inL = buffer.getWritePointer(0);
	auto inR = buffer.getWritePointer(1);
	auto bufferLength = buffer.getNumSamples();

	// downmix to mono in case of a stereo input
	// by adding from the right channel to left channel
	if (getTotalNumInputChannels() == 2)
	{
		buffer.addFrom(0, 0, inR, bufferLength);
		buffer.applyGain(0.5f);
	}

	// split the input signal into two bands, only freqs above crossover's f0
	// will be spatialized
	memcpy(loPassIn_.data(), inL, bufferLength * sizeof(float));
	memcpy(hiPassIn_.data(), inL, bufferLength * sizeof(float));
	crossover_.loPass.processSamples(loPassIn_.data(), bufferLength);
	crossover_.hiPass.processSamples(hiPassIn_.data(), bufferLength);

	// we need to copy the hi-pass input to buffers
	memcpy(buffers_[0].data(), hiPassIn_.data(), bufferLength * sizeof(float));
	memcpy(buffers_[1].data(), hiPassIn_.data(), bufferLength * sizeof(float));

	// actual hrir filtering
	filters_[0].process(buffers_[0].data());
	filters_[1].process(buffers_[1].data());

	// copy to output
	auto outL = inL;
	auto outR = inR;

	// fill stereo output
	const auto wet = wetPercent_->value() / 100;
	const auto dry = 1 - wet;
	float monoIn;
	for (int i = 0; i < bufferLength; i++)
	{
		monoIn = inL[i];
		outL[i] = wet * (loPassIn_[i] + buffers_[0][i]) + dry * monoIn;
		outR[i] = wet * (loPassIn_[i] + buffers_[1][i]) + dry * monoIn;
	}

	// apply output gain
	buffer.applyGain(Decibels::decibelsToGain(gainDb_->value()));
}

bool HrtfBiAuralAudioProcessor::hasEditor() const
{
	return true;
}

AudioProcessorEditor* HrtfBiAuralAudioProcessor::createEditor()
{
	return new HrtfBiAuralAudioProcessorEditor(*this);
}

void HrtfBiAuralAudioProcessor::getStateInformation(MemoryBlock&)
{
}

void HrtfBiAuralAudioProcessor::setStateInformation(const void*, int)
{
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new HrtfBiAuralAudioProcessor();
}

void HrtfBiAuralAudioProcessor::updateHRTF(double azimuth, double elevation)
{
	hrtfContainer_.updateHRIR(azimuth, elevation);
	hrirCrossfadeValue = 0.f;
}

void HrtfBiAuralAudioProcessor::toggleBypass(bool bypass)
{
	bypassed_ = bypass;
	reset();
}

void HrtfBiAuralAudioProcessor::reset()
{
	crossover_.loPass.reset();
	crossover_.hiPass.reset();
	filters_[0].reset();
	filters_[1].reset();
}

void HrtfBiAuralAudioProcessor::onAudioParameterChanged(AtomicAudioParameter* parameter)
{
	if (parameter == crossoverFreq_)
		crossover_.set(crossover_.fs, parameter->value());
}

bool HrtfBiAuralAudioProcessor::isHRIRLoaded() const
{
	return hrirLoaded_;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getCrossoverFrequencyParameter() const
{
	return crossoverFreq_;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getWetParameter() const
{
	return wetPercent_;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getGainParameter() const
{
	return gainDb_;
}
