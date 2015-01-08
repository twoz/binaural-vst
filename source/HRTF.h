#pragma once
#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#define PI 3.1415926535

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include "../JuceLibraryCode/JuceHeader.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>                   Delaunay;
typedef K::Point_2                                          Point2;

using HrirBuffer = std::array<std::array<float, 200>, 2>;

template <typename Type>
Type deg2rad(Type deg)
{
	return deg * PI / 180;
}

template <typename Type>
Type rad2deg(Type rad)
{
	return rad * 180 / PI;
}

struct Subject
{
	int id;
	String name;
	float headWidth;
	float headHeight; 
	float headDepth;
	std::map<int, std::array<HrirBuffer, 52>> hrirDict;
	Delaunay triangulation;

	String toShortString()
	{
		return "ID: " + String(id) + " (" + name + ") " + "W: " + String(headWidth, 1) + " H: " + String(headHeight, 1) + " D: " + String(headDepth, 1);
	}

	String toLongString()
	{
		String text;
		if (name == "F")
			text = "Female";
		else if (name == "M")
			text = "Male";
		else if (name == "KEMAR L")
			text = "KEMAR (large pinnae)";
		else
			text = "KEMAR (small pinnae)";
		text += "\nHead width: " + String(headWidth, 1) + " cm, height: " + String(headHeight, 1) + " cm, depth: " + String(headDepth, 1) + " cm ";
		return text;
	}
};

class HRTFContainer
{
public:
	HRTFContainer();
	void interpolate(float azimuth, float elevation);
	
	std::vector<Subject>& getSubjects() { return subjects; }
	Subject& getCurrentSubject() { return subjects[currentSubjectIndex]; }
	void setCurrentSubject(int id);

	HrirBuffer hrir;
private:
	int getElvIndex(int elv);
	void loadSubjectHrir(Subject&);
	
	std::vector<Subject> subjects;
	int currentSubjectIndex = -1;
	File thisDir;
};