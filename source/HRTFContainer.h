#pragma once
#include <array>
#include <map>
#include <atomic>
#include "../JuceLibraryCode/JuceHeader.h"

struct HRIRBuffer
{
	static const auto HRIR_SIZE = 200u;
	using ImpulseResponse = std::array<float, HRIR_SIZE>;

	ImpulseResponse leftEarIR;
	ImpulseResponse rightEarIR;
};

class HRTFContainer
{
public:
	HRTFContainer();
	~HRTFContainer();

	void updateHRIR(double azimuth, double elevation);
	const HRIRBuffer& hrir() const;

	void loadHrir(String filename);
private:
	static int getElvIndex(int elv);

	std::map<int, std::array<HRIRBuffer, 52>> hrirDict_;
	ScopedPointer<class Delaunay> triangulation_;
	HRIRBuffer hrir_[2];
	std::atomic_int hrirReadIndex;
};
