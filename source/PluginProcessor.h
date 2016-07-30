#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "HRTFContainer.h"
#include "HRIRFilter.h"
#include "FIRFilter.h"
#include "Crossover.h"


class AtomicAudioParameter;

class HrtfBiAuralAudioProcessor : public AudioProcessor
{
public:
	HrtfBiAuralAudioProcessor();
	~HrtfBiAuralAudioProcessor();

public: // AudioProcessor implementation
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	double getTailLengthSeconds() const override;
	void reset() override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

public:
	void updateHRTF(double, double);
	void toggleBypass(bool bypass);
	void onAudioParameterChanged(AtomicAudioParameter* parameter);

	bool isHRIRLoaded() const;

	AtomicAudioParameter* getCrossoverFrequencyParameter() const;
	AtomicAudioParameter* getWetParameter() const;
	AtomicAudioParameter* getGainParameter() const;

private:
	HRIRFilter hrirFilterL;
	HRIRFilter hrirFilterR;
	HRTFContainer hrtfContainer_;
	AudioSampleBuffer crossoverOutput;
	Crossover crossover_;
	AudioSampleBuffer monoInputBuffer;
	
	bool bypassed_;
	bool hrirLoaded_;

	AtomicAudioParameter* crossoverFreq_;
	AtomicAudioParameter* wetPercent_;
	AtomicAudioParameter* gainDb_;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessor)
};
