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

	setLatencySamples(HRIRBuffer::HRIR_SIZE  / 2); // "almost true" ofc...HRTF isn't really linear phase...
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
	crossover_.setSampleRate(sampleRate);
	crossoverOutput.setSize(2, samplesPerBlock);
	hrirFilterL.prepare(samplesPerBlock);
	hrirFilterR.prepare(samplesPerBlock);
	monoInputBuffer.setSize(1, samplesPerBlock);
}

void HrtfBiAuralAudioProcessor::releaseResources()
{
}

void HrtfBiAuralAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer&)
{
	if (bypassed_)
		return;

	auto bufferLChannel = buffer.getWritePointer(0);
	auto bufferRChannel = buffer.getWritePointer(1);
	const auto bufferLength = buffer.getNumSamples();

	// downmix to mono in case of a stereo input
	// by adding from the right channel to left channel
	if (getTotalNumInputChannels() == 2)
	{
		buffer.addFrom(0, 0, bufferRChannel, bufferLength);
		buffer.applyGain(0.5f);
	}
	monoInputBuffer.copyFrom(0, 0, buffer, 0, 0, bufferLength);

	// split the input signal into two bands, only freqs above crossover's f0
	// will be spatialized
	crossover_.process(buffer, 0, crossoverOutput);

	// we need to copy the hi-pass input to buffers
	buffer.copyFrom(0, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);
	buffer.copyFrom(1, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);

	// actual hrir filtering
	const auto& hrir = hrtfContainer_.hrir();
	hrirFilterL.setImpulseResponse(hrir.leftEarIR);
	hrirFilterR.setImpulseResponse(hrir.rightEarIR);
	hrirFilterL.process(bufferLChannel, bufferLength);
	hrirFilterR.process(bufferRChannel, bufferLength);

	// fill stereo output
	const auto wetRatio = wetPercent_->value() / 100;
	const auto dryRatio = 1 - wetRatio;
	const auto lowPassInput = crossoverOutput.getReadPointer(Crossover::loPassChannelIndex);
	for (auto i = 0; i < bufferLength; i++)
	{
		const auto monoIn = *monoInputBuffer.getReadPointer(0, i);
		bufferLChannel[i] = wetRatio * (lowPassInput[i] + bufferLChannel[i]) + dryRatio * monoIn;
		bufferRChannel[i] = wetRatio * (lowPassInput[i] + bufferRChannel[i]) + dryRatio * monoIn;
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
}

void HrtfBiAuralAudioProcessor::toggleBypass(bool bypass)
{
	bypassed_ = bypass;
	reset();
}

void HrtfBiAuralAudioProcessor::reset()
{
	crossover_.reset();
	hrirFilterL.reset();
	hrirFilterR.reset();
}

void HrtfBiAuralAudioProcessor::onAudioParameterChanged(AtomicAudioParameter* parameter)
{
	if (parameter == crossoverFreq_)
		crossover_.setCrossoverFrequency(parameter->value());
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
