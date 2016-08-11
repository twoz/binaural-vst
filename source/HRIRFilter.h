#pragma once
#include "OouraFFT.h"
#include "HRTFContainer.h"
#include "Util.h"

/** Single-channel (left or right) HRIR filter.
    Uses frequency-domain crossfading to avoid audio waveform discontinuities that arise when changing the impulse response.
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
	OouraFFT oouraFFT;
	ComplexVector<float> transferFunction[2];
	ComplexVector<float> inputDFT;
	ComplexVector<float> outputDFT;
	std::vector<float> zeroPaddedIR[2];
	std::vector<float> inputBuffer;
	std::vector<float> outputBuffer;
	int currentTargetFilterIndex = 0;
	size_t nfft = 0u;
};
