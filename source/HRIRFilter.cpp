#include "HRIRFilter.h"

namespace
{
	void freqDomainMultiplyWithCrossfade(const ComplexVector<float>& inputDFT, 
		const ComplexVector<float>& targetFilter,
		const ComplexVector<float>& currentFilter,
		ComplexVector<float>& outputDFT)
	{
		const auto& X = inputDFT;
		const auto& H0 = currentFilter;
		const auto& H1 = targetFilter;
		outputDFT[0] = X[0] * (H0[0] + H1[0]) + 0.5f * X[1] * (-H0[1] + H1[1]);
		for (auto i = 1u; i < outputDFT.size() - 1; ++i)
		{
			outputDFT[i] = 0.5f * X[i - 1] * (-H0[i - 1] + H1[i - 1])
				+ X[i] * (H0[i] + H1[i])
				+ 0.5f * X[i + 1] * (-H0[i + 1] + H1[i + 1]);
		}
		const auto sz = outputDFT.size() - 1;
		outputDFT[sz] = 0.5f * X[sz - 1] * (-H0[sz - 1] + H1[sz - 1]) + X[sz] * (H0[sz] + H1[sz]);
	}
}


void HRIRFilter::setImpulseResponse(const HRIRBuffer::ImpulseResponse& impulseResponse)
{
	currentTargetFilterIndex ^= 1;
	auto& zeroPadIR = zeroPaddedIR[currentTargetFilterIndex];
	std::copy(impulseResponse.begin(), impulseResponse.end(), zeroPadIR.begin());
	oouraFFT.fft(zeroPadIR.data(), transferFunction[currentTargetFilterIndex].data());
}

void HRIRFilter::prepare(int samplesPerBlock)
{
	// block size must be power of 2 and nfft twice the size of it
	jassert(isPowerOf2(samplesPerBlock));
	nfft = 2 * samplesPerBlock;
	oouraFFT.init(nfft);
	for (auto i = 0; i < 2; ++i)
	{
		zeroPaddedIR[i].resize(nfft);
		transferFunction[i].resize(nfft / 2 + 1);
	}
	inputDFT.resize(nfft / 2 + 1);
	outputDFT.resize(nfft / 2 + 1);
	inputBuffer.resize(nfft);
	outputBuffer.resize(nfft);
}

void HRIRFilter::process(float* samples, int numSamples)
{
	jassert(numSamples == nfft / 2);
	// overlap-save: we are buffering nfft input samples
	// shift to the left and buffer numSamples of input
	std::copy(inputBuffer.begin() + numSamples, inputBuffer.end(), inputBuffer.begin());
	std::copy(samples, samples + numSamples, inputBuffer.data() + numSamples);
	oouraFFT.fft(inputBuffer.data(), inputDFT.data());

	freqDomainMultiplyWithCrossfade(inputDFT, transferFunction[currentTargetFilterIndex],
		transferFunction[currentTargetFilterIndex ^ 1], outputDFT);

	oouraFFT.ifft(outputDFT.data(), outputBuffer.data());
	// overlap-save: discard leftmost (nfft - numSamples) samples
	const auto scale = 1.f / nfft;
	for (auto i = 0; i < numSamples; ++i)
	{
		samples[i] = scale * outputBuffer[i + numSamples];
	}
}

void HRIRFilter::reset()
{
	std::fill(inputBuffer.begin(), inputBuffer.end(), 0.f);
}
