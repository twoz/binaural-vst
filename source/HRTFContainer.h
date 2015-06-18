#pragma once
#include <array>
#include <vector>
#include <memory>
#include <map>
#include "../JuceLibraryCode/JuceHeader.h"
#include "triangle++/del_interface.hpp"


#define HRIR_LENGTH 200
using HrirBuffer = std::array < std::array<float, HRIR_LENGTH>, 2 >;

class HRTFContainer
{
public:
	HRTFContainer();
	~HRTFContainer();

	void updateHRIR(double azimuth, double elevation);
	const HrirBuffer& hrir() const;

private:
	void loadHrir(String filename);
	int getElvIndex(int elv);

	std::map<int, std::array<HrirBuffer, 52>> hrirDict_;
	ScopedPointer<tpp::Delaunay> triangulation_;
	HrirBuffer hrir_;
};
