#pragma once
#include <cassert>
#include <cmath>
#include <complex>
#include <vector>
#include "Util.h"


class OouraFFT
{
public:
	// prepare for fft/ifft
	// nfft - size of the future transforms
	void init(size_t nfft);

	// in must have length nfft
	// out must have length nfft/2 + 1
	void fft(float* in, std::complex<float>* out);

	// out must have length nfft
	// in must have length nfft/2 + 1
	void ifft(std::complex<float>* in, float* out);

private:
	// original routines from the ooura fft (fast version, radix 4/2)
	// see http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html
	void rdft(int n, int isgn, double *a, int *ip, double *w);
	void makewt(int nw, int * ip, double * w);
	void makect(int nc, int * ip, double * c);
	void bitrv2(int n, int * ip, double * a);
	void cftfsub(int n, double * a, double * w);
	void cftbsub(int n, double * a, double * w);
	void rftfsub(int n, double * a, int nc, double * c);
	void rftbsub(int n, double * a, int nc, double * c);
	void cft1st(int n, double * a, double * w);
	void cftmdl(int n, int l, double * a, double * w);

	std::vector<int> ip_; // work area for bit reversal
	std::vector<double> sineTable_;
	std::vector<double> buffer_;
};
