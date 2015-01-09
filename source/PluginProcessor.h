#pragma once
#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <map>
#include "../JuceLibraryCode/JuceHeader.h"

#include "HRTF.h"

struct Crossover
{
	IIRFilter loPass;
	IIRFilter hiPass;
	float f0 = 300.f;
	int fs = 44100;
	void set(float fs, float f0)
	{
		loPass.setCoefficients(IIRCoefficients::makeLowPass(fs, f0));
		hiPass.setCoefficients(IIRCoefficients::makeHighPass(fs, f0));
		this->fs = fs;
		this->f0 = f0;
	}
};

class HrtfBiAuralAudioProcessor : public AudioProcessor
{
public:
	HrtfBiAuralAudioProcessor();
	~HrtfBiAuralAudioProcessor();

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	const String getName() const override;

	int getNumParameters() override;
	float getParameter(int index) override;
	void setParameter(int index, float newValue) override;

	const String getParameterName(int index) override;
	const String getParameterText(int index) override;

	const String getInputChannelName(int channelIndex) const override;
	const String getOutputChannelName(int channelIndex) const override;
	bool isInputChannelStereoPair(int index) const override;
	bool isOutputChannelStereoPair(int index) const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool silenceInProducesSilenceOut() const override;
	double getTailLengthSeconds() const override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void updateHrir(float, float);

	Crossover crossover;
	HRTFContainer hrtfContainer;

private:
	HrirBuffer currentHrir;
	std::vector<float> tailL, tailR;
	int nTaps = 200;
	float tRate = 0.5f;

	std::vector<float> convolve(const float* x, int xlen, const float* h, int hlen);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessor)
};

#endif
