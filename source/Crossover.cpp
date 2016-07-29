#include "Crossover.h"

Crossover::Crossover()
{
	set(44100., 150.);
}

void Crossover::set(double sampleRate, double crossoverFrequency)
{
	loPass.setCoefficients(IIRCoefficients::makeLowPass(sampleRate, crossoverFrequency));
	hiPass.setCoefficients(IIRCoefficients::makeHighPass(sampleRate, crossoverFrequency));
	this->sampleRate = sampleRate;
	this->crossoverFrequency = crossoverFrequency;
}

void Crossover::setCrossoverFrequency(double crossoverFrequency)
{
	set(sampleRate, crossoverFrequency);
}

void Crossover::setSampleRate(double sampleRate)
{
	set(sampleRate, crossoverFrequency);
}

void Crossover::reset()
{
	loPass.reset();
	hiPass.reset();
}

void Crossover::process(const AudioSampleBuffer& input, int sourceChannelIndex, AudioSampleBuffer& output)
{
	const auto samplesToProcess = jmin(input.getNumSamples(), output.getNumSamples());
	output.copyFrom(loPassChannelIndex, 0, input, sourceChannelIndex, 0, samplesToProcess);
	output.copyFrom(hiPassChannelIndex, 0, input, sourceChannelIndex, 0, samplesToProcess);
	loPass.processSamples(output.getWritePointer(loPassChannelIndex), samplesToProcess);
	hiPass.processSamples(output.getWritePointer(hiPassChannelIndex), samplesToProcess);
}
