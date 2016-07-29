#pragma once
#include <array>
#include <map>
#include <atomic>
#include "../JuceLibraryCode/JuceHeader.h"

#define HRIR_LENGTH 200
using HrirBuffer = std::array < std::array<float, HRIR_LENGTH>, 2 >;

class HRTFContainer
{
public:
	HRTFContainer();
	~HRTFContainer();

	void updateHRIR(double azimuth, double elevation);
	const HrirBuffer& hrir() const;

	void loadHrir(String filename);
private:
	static int getElvIndex(int elv);

	std::map<int, std::array<HrirBuffer, 52>> hrirDict_;
	ScopedPointer<class Delaunay> triangulation_;
	HrirBuffer hrir_[2];
	std::atomic_int hrirReadIndex;
};
