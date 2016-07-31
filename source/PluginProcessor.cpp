#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AtomicAudioParameter.h"


HrtfBiAuralAudioProcessor::HrtfBiAuralAudioProcessor()
	: bypassed(false)
	, crossoverFreqParam(new AtomicAudioParameter("CROSS FREQ", "Hz", { 20, 2000 }, 150))
	, wetPercentParam(new AtomicAudioParameter("Wet", "%", { 0, 100 }, 100))
	, gainDbParam(new AtomicAudioParameter("Gain", "dB", { -15, 15 }, 0))
{
	// register parameters
	addParameter(crossoverFreqParam);
	addParameter(wetPercentParam);
	addParameter(gainDbParam);

	// load HRIR
	auto thisDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory();
	try
	{
		hrtfContainer.loadHrir(thisDir.getFullPathName() + "/hrir/kemar.bin");
		hrirLoaded = true;
	}
	catch (std::ios_base::failure&)
	{
		hrirLoaded = false;
		bypassed = true;
	}
	hrtfContainer.updateHRIR(0, 0);

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
	crossover.setSampleRate(sampleRate);
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
	if (bypassed)
		return;

	processParameterChanges();

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
	crossover.process(buffer, 0, crossoverOutput);

	// we need to copy the hi-pass input to buffers
	buffer.copyFrom(0, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);
	buffer.copyFrom(1, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);

	// actual hrir filtering
	const auto& hrir = hrtfContainer.hrir();
	hrirFilterL.setImpulseResponse(hrir.leftEarIR);
	hrirFilterR.setImpulseResponse(hrir.rightEarIR);
	hrirFilterL.process(bufferLChannel, bufferLength);
	hrirFilterR.process(bufferRChannel, bufferLength);

	// fill stereo output
	const auto wetRatio = wetPercentParam->getValueAndMarkRead() / 100;
	const auto dryRatio = 1 - wetRatio;
	const auto lowPassInput = crossoverOutput.getReadPointer(Crossover::loPassChannelIndex);
	for (auto i = 0; i < bufferLength; i++)
	{
		const auto monoIn = *monoInputBuffer.getReadPointer(0, i);
		const auto gain = Decibels::decibelsToGain(gainDbParam->getValueAndMarkRead());
		bufferLChannel[i] = gain * wetRatio * (lowPassInput[i] + bufferLChannel[i]) + dryRatio * monoIn;
		bufferRChannel[i] = gain * wetRatio * (lowPassInput[i] + bufferRChannel[i]) + dryRatio * monoIn;
	}
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
	hrtfContainer.updateHRIR(azimuth, elevation);
}

void HrtfBiAuralAudioProcessor::toggleBypass(bool bypass)
{
	bypassed = bypass;
	reset();
}

void HrtfBiAuralAudioProcessor::reset()
{
	crossover.reset();
	hrirFilterL.reset();
	hrirFilterR.reset();
}

bool HrtfBiAuralAudioProcessor::isHRIRLoaded() const
{
	return hrirLoaded;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getCrossoverFrequencyParameter() const
{
	return crossoverFreqParam;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getWetParameter() const
{
	return wetPercentParam;
}

AtomicAudioParameter* HrtfBiAuralAudioProcessor::getGainParameter() const
{
	return gainDbParam;
}

void HrtfBiAuralAudioProcessor::processParameterChanges()
{
	if (crossoverFreqParam->hasNewValue())
	{
		crossover.setCrossoverFrequency(crossoverFreqParam->getValueAndMarkRead());
	}
}
