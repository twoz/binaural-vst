#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Crossover
{
public:
	enum
	{
		loPassChannelIndex = 0,
		hiPassChannelIndex = 1
	};

	Crossover();
	void set(double samplingRate, double crossoverFrequency);
	void setCrossoverFrequency(double crossoverFrequency);
	void setSampleRate(double sampleRate);
	void reset();

	void process(const AudioSampleBuffer& input, int sourceChannelIndex, AudioSampleBuffer& output);

private:
	IIRFilter loPass;
	IIRFilter hiPass;
	double crossoverFrequency{ 0 };
	double sampleRate{ 0 };
};
