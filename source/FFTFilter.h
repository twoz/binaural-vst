#pragma once

#include <vector>
#include "kiss_fft/kiss_fftr.h"


class FFTFilter
{
public:
	void setImpulseResponse(const float* ir, size_t length);

	void processSamples(float* samples, int numSamples);

	void reset();

private:
	std::vector<float> ir;
	std::vector<float> tail;
};

inline kiss_fft_cpx operator*(const kiss_fft_cpx& a, const kiss_fft_cpx& b)
{
	return kiss_fft_cpx{
		a.r * b.r - a.i * b.i,
		(a.r + a.i)*(b.r + b.i) - a.r * b.r - a.i * b.i
	};
}

inline kiss_fft_cpx operator*(const float& a, const kiss_fft_cpx& b)
{
	kiss_fft_cpx c;
	c.r = a * b.r;
	c.i = a * b.i;
	return c;
}

inline kiss_fft_cpx operator+(const kiss_fft_cpx& a, const kiss_fft_cpx& b)
{
	return kiss_fft_cpx{
		a.r + b.r,
		a.i + a.i
	};
}

inline kiss_fft_cpx operator-(const kiss_fft_cpx& a, const kiss_fft_cpx& b)
{
	return kiss_fft_cpx{
		a.r - b.r,
		a.i - a.i
	};
}

