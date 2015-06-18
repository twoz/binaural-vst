#pragma once
#include <complex>
#include <vector>
#include "OouraFFT.h"
#include "Util.h"


/**
* Class for fast (FFT based) finite-impulse-response (mono) filtering.
*/
class FIRFilter
{
public:
	FIRFilter();
	~FIRFilter();

	/**
	* Prepares the filter for processing.
	* Must be called before the first call to processSamples. After the filter is setup,
	* impulse response can be changed any time as long as it's the same size.
	*
	* @param bufferSize size of the input time data
	* @param irSize size of the impulse response
	*/
	void init(size_t bufferSize, size_t irSize);

	/**
	* Sets the impulse response of this filter.
	* It will copy 'irSize' samples from the given array.
	*/
	void setImpulseResponse(const float* ir);

	/**
	* Process given array of samples, in-place.
	*/
	void process(float* in);

	/**
	* Resets the internal state of the filter (removes tail from the previous processing block).
	*/
	void reset();

private:
	OouraFFT oouraFFT;

	ComplexVector<float> H_; // transfer function
	ComplexVector<float> freqBuffer_; // input's dft buffer
	std::vector<float> ir_; // impulse response
	std::vector<float> tail_; // processing tail (buffer for the next block)
	std::vector<float> timeBuffer_; // buffer of the input samples

	size_t irSize_;
	size_t bufferSize_;
	size_t nfft_;
	bool initialized_;
};
