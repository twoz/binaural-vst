#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "HRTF.h"
#include "FFTFilter.h"


class HrtfBiAuralAudioProcessor : public AudioProcessor
{
	friend class HrtfBiAuralAudioProcessorEditor;
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

	void updateHRTF(double, double);
	void reset();

private:
	struct Crossover
	{
		IIRFilter loPass;
		IIRFilter hiPass;
		float f0 = 300.f;
		int fs = 44100;
		void set(int fs, float f0)
		{
			loPass.setCoefficients(IIRCoefficients::makeLowPass(fs, f0));
			hiPass.setCoefficients(IIRCoefficients::makeHighPass(fs, f0));
			this->fs = fs;
			this->f0 = f0;
		}
	} crossover;

	FFTFilter filters[2];
	HRTFContainer hrtfContainer;
	HrirBuffer currentHrir;
	float crossfadeRate;
	bool crossfading;
	bool bypassed;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessor)
};