#include "kiss_fft/kiss_fft.c"
#include "kiss_fft/kiss_fftr.c"
#include "FFTFilter.h"

void FFTFilter::setImpulseResponse(const float* ir, size_t length)
{
	this->ir = std::vector<float>(ir, ir + length);
}

void FFTFilter::processSamples(float* samples, int numSamples)
{
	/* todo: handle case when buffer to process is shorter than impulse response */
	if (numSamples < ir.size())
		return;

	auto nfft = kiss_fftr_next_fast_size_real(numSamples + ir.size() - 1);
	if (tail.size() != nfft - numSamples)
		tail.resize(nfft - numSamples);

	std::vector<float> timeData(samples, samples + numSamples);
	/* zero-padding */
	timeData.resize(nfft);
	ir.resize(nfft);
	/* nfft-point FFT of impulse response */
	std::vector<kiss_fft_cpx> H(nfft);
	auto cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
	kiss_fftr(cfg, ir.data(), H.data());
	kiss_fft_free(cfg);
	/* nfft-point FFT of input samples */
	std::vector<kiss_fft_cpx> freqData(nfft);
	cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
	kiss_fftr(cfg, timeData.data(), freqData.data());
	kiss_fft_free(cfg);
	/* multiply and normalize */
	auto scale = 1.f / nfft;
	for (int i = 0; i < nfft; ++i)
		freqData[i] = scale * freqData[i] * H[i];
	/* IFFT of the product */
	cfg = kiss_fftr_alloc(nfft, 1, 0, 0);
	kiss_fftri(cfg, freqData.data(), timeData.data());
	kiss_fftr_free(cfg);

	for (auto i = 0; i < numSamples; ++i)
	{
		samples[i] = timeData[i];
		if (i < tail.size())
			samples[i] += tail[i];
	}

	for (size_t i = 0; i < tail.size(); ++i)
	{
		tail[i] = timeData[i + numSamples];
	}
}

void FFTFilter::reset()
{
	tail.clear();
}
