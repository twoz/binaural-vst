#pragma once
#include "FIRFilter.h"
#include "HRTFContainer.h"

/** Single-channel (left or right) HRIR filter.
    Uses time-domain output crossfading to avoid audio waveform discontinuities that arise when changing the impulse response.
    Expected calling order: setImpulseResponse(), process()
*/
class HRIRFilter
{
public:
	void setImpulseResponse(const HRIRBuffer::ImpulseResponse& impulseResponse);
	void prepare(int samplesPerBlock);
	void process(float* samples, int numSamples);
	void reset();

private:
	AudioSampleBuffer buffer;
	FIRFilter filters[2];
	int currentTargetFilterIndex = 0;
};
