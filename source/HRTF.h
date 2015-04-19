#pragma once

#include <array>
#include <vector>
#include <memory>
#include <map>

#include "../JuceLibraryCode/JuceHeader.h"
#include "triangle++/del_interface.hpp"
#include "Helpers.h"


#define HRIR_LENGTH 200
using HrirBuffer = std::array < std::array<float, HRIR_LENGTH>, 2 > ;

struct SubjectInfo
{
	String name;
	String description;
	float headWidth;
	float headHeight;
	float headDepth;
};

class Subject
{
public:
	Subject(String pathToHRIR, String name, String description, float headWidth, float headHeight, float headDepth);

	HrirBuffer interpolateHRIR(double azimuth, double elevation);

	const SubjectInfo info;
private:
	void loadHrir(String filename);
	int getElvIndex(int elv);

	std::map<int, std::array<HrirBuffer, 52>> hrirDict;
	ScopedPointer<tpp::Delaunay> triangulation;
};

class HRTFContainer
{
public:
	HRTFContainer();
	~HRTFContainer();

	void interpolate(double azimuth, double elevation);

	std::vector<SubjectInfo> getSubjectsInfo() const;
	SubjectInfo getCurrentSubjectInfo() const;
	int getCurrentSubjectIndex() const;
	void setCurrentSubject(size_t index);
	void setCurrentSubject(String name);

	HrirBuffer hrir;
private:

	std::vector<Subject> subjects;
	int currentSubjectIndex = -1;
};