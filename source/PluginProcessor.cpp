#pragma once
#include "PluginProcessor.h"
#include "PluginEditor.h"

HrtfBiAuralAudioProcessor::HrtfBiAuralAudioProcessor()
	:
	currentHrir(),
	crossfadeRate(0.06f),
	crossfading(true),
	bypassed(false)
{
	crossover.set(44100, crossover.f0);
}

HrtfBiAuralAudioProcessor::~HrtfBiAuralAudioProcessor()
{
}
const String HrtfBiAuralAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

int HrtfBiAuralAudioProcessor::getNumParameters()
{
	return 0;
}

float HrtfBiAuralAudioProcessor::getParameter(int)
{
	return 0.0f;
}

void HrtfBiAuralAudioProcessor::setParameter(int, float)
{
}

const String HrtfBiAuralAudioProcessor::getParameterName(int)
{
	return String();
}

const String HrtfBiAuralAudioProcessor::getParameterText(int)
{
	return String();
}

const String HrtfBiAuralAudioProcessor::getInputChannelName(int channelIndex) const
{
	return String(channelIndex + 1);
}

const String HrtfBiAuralAudioProcessor::getOutputChannelName(int channelIndex) const
{
	return String(channelIndex + 1);
}

bool HrtfBiAuralAudioProcessor::isInputChannelStereoPair(int) const
{
	return true;
}

bool HrtfBiAuralAudioProcessor::isOutputChannelStereoPair(int) const
{
	return true;
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

bool HrtfBiAuralAudioProcessor::silenceInProducesSilenceOut() const
{
	return false;
}

double HrtfBiAuralAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int HrtfBiAuralAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
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
	crossover.set(sampleRate, crossover.f0);
	reset();
}

void HrtfBiAuralAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

void HrtfBiAuralAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer&)
{
	// get a pointer to the left channel data
	auto in = buffer.getWritePointer(0);
	auto bufferLength = buffer.getNumSamples();
	// if bypassed copy from left to right channel and return
	if (bypassed)
	{
		buffer.copyFrom(1, 0, in, bufferLength);
		return;
	}

	// split the input signal into two bands, only freqs above crossover's f0
	// will be spatialized
	std::vector<float> loPassIn(in, in + bufferLength);
	crossover.loPass.processSamples(&loPassIn[0], bufferLength);
	crossover.hiPass.processSamples(in, bufferLength);

	// interpolate the impulse response to minimalize audible cracks caused by
	// changing the source direction
	if (crossfading)
	{
		auto& targetHrir = hrtfContainer.hrir;
		float diff[2], totalDiff = 0.f;
		for (int i = 0; i < targetHrir[0].size(); ++i)
		{
			diff[0] = targetHrir[0][i] - currentHrir[0][i];
			diff[1] = targetHrir[1][i] - currentHrir[1][i];
			currentHrir[0][i] += diff[0] * crossfadeRate;
			currentHrir[1][i] += diff[1] * crossfadeRate;

			totalDiff += std::fabsf(diff[0]) + std::fabsf(diff[1]);
		}

		// TODO: condition for stopping crossfading
		/*	if (totalDiff < 0.1f)
				crossfading = false;*/

	}
	// update impule response
	filters[0].setImpulseResponse(currentHrir[0].data(), HRIR_LENGTH);
	filters[1].setImpulseResponse(currentHrir[1].data(), HRIR_LENGTH);

	auto outL = in;
	auto outR = buffer.getWritePointer(1);
	// since the buffer is {1, 2} copy left channel to the right channel
	buffer.copyFrom(1, 0, in, bufferLength);
	filters[0].processSamples(outL, bufferLength);
	filters[1].processSamples(outR, bufferLength);

	// add the low frequency (non-spatialized) component
	for (int i = 0; i < bufferLength; i++)
	{
		outL[i] += loPassIn[i];
		outR[i] += loPassIn[i];
	}
}

//==============================================================================
bool HrtfBiAuralAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HrtfBiAuralAudioProcessor::createEditor()
{
	return new HrtfBiAuralAudioProcessorEditor(*this);
}

//==============================================================================
void HrtfBiAuralAudioProcessor::getStateInformation(MemoryBlock&)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void HrtfBiAuralAudioProcessor::setStateInformation(const void*, int)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new HrtfBiAuralAudioProcessor();
}

void HrtfBiAuralAudioProcessor::updateHRTF(double azimuth, double elevation)
{
	hrtfContainer.interpolate(azimuth, elevation);
	crossfading = true;
}

void HrtfBiAuralAudioProcessor::reset()
{
	crossover.loPass.reset();
	crossover.hiPass.reset();
	filters[0].reset();
	filters[1].reset();
}