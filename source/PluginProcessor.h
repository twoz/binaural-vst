#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "HRTFContainer.h"
#include "FIRFilter.h"


class AudioParameter;

class HrtfBiAuralAudioProcessor :
	public AudioProcessor
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
	bool silenceInProducesSilenceOut() const override;
	double getTailLengthSeconds() const override;

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
	void reset();
	void onAudioParameterChanged(AudioParameter* parameter);

	bool isHRIRLoaded() const;

	AudioParameter* getCrossoverFrequencyParameter() const;
	AudioParameter* getWetParameter() const;
	AudioParameter* getGainParameter() const;

private:
	struct Crossover
	{
		IIRFilter loPass;
		IIRFilter hiPass;
		double f0 = 150.;
		double fs = 44100.;
		void set(double fs, double f0)
		{
			loPass.setCoefficients(IIRCoefficients::makeLowPass(fs, f0));
			hiPass.setCoefficients(IIRCoefficients::makeHighPass(fs, f0));
			this->fs = fs;
			this->f0 = f0;
		}
	} crossover_;
	FIRFilter filters_[2];
	HRTFContainer hrtfContainer_;
	HrirBuffer currentHrir_;
	std::vector<float> loPassIn_;
	std::vector<float> hiPassIn_;
	std::vector<float> buffers_[2];
	float crossfadeRate;
	
	bool crossfading_;
	bool bypassed_;
	bool hrirLoaded_;

	AudioParameter* crossoverFreq_;
	AudioParameter* wetPercent_;
	AudioParameter* gainDb_;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessor)
};
