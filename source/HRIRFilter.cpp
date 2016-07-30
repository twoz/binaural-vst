#include "HRIRFilter.h"


void HRIRFilter::setImpulseResponse(const HRIRBuffer::ImpulseResponse& impulseResponse)
{
	currentTargetFilterIndex ^= 1;
	filters[currentTargetFilterIndex].setImpulseResponse(impulseResponse.data());
}

void HRIRFilter::prepare(int samplesPerBlock)
{
	filters[0].init(samplesPerBlock, HRIRBuffer::HRIR_SIZE);
	filters[1].init(samplesPerBlock, HRIRBuffer::HRIR_SIZE);
	buffer.setSize(2, samplesPerBlock);
}

void HRIRFilter::process(float* samples, int numSamples)
{
	buffer.copyFrom(0, 0, samples, numSamples);
	buffer.copyFrom(1, 0, samples, numSamples);
	auto fadeInSamples = buffer.getWritePointer(0);
	auto fadeOutSamples = buffer.getWritePointer(1);
	filters[currentTargetFilterIndex].process(fadeInSamples);
	filters[currentTargetFilterIndex ^ 1].process(fadeOutSamples);
	// Crossfade across whole block range
	for (auto i = 0; i < numSamples; ++i)
	{
		// sin^2 + cos^2 = 1 
		const auto c = cos(0.5f * float_Pi * i / numSamples);
		const auto fadeInMultiplier = 0.5f - 0.5f * c; // sin^2
		const auto fadeOutMultiplier = 0.5f + 0.5f * c; // cos^2
		samples[i] = fadeInMultiplier * fadeInSamples[i] + fadeOutMultiplier * fadeOutSamples[i];
	}
}

void HRIRFilter::reset()
{
	filters[0].reset();
	filters[1].reset();
}
