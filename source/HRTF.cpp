#include "HRTF.h"

HRTFContainer::HRTFContainer()
{
	thisDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory();
	FileInputStream istream(File(thisDir.getFullPathName() + "/hrir/subjects_data.txt"));
	if (istream.openedOk())
	{
		while (!istream.isExhausted())
		{
			String line = istream.readNextLine();
			std::stringstream ss(line.toStdString());
			std::string item;
			std::vector<std::string> elems;
			while (std::getline(ss, item, ';')) {
				elems.push_back(item);
			}
			Subject subject;
			subject.id = std::stoi(elems[0]);
			if (subject.id == 21)
				subject.name = "KEMAR L";
			else if (subject.id == 165)
				subject.name = "KEMAR S";
			else
				subject.name = elems[1];
			subject.headWidth = std::atof(elems[2].c_str());
			subject.headHeight = std::atof(elems[3].c_str());
			subject.headDepth = std::atof(elems[4].c_str());
			subjects.push_back(subject);
		}
	}

	std::sort(std::begin(subjects), std::end(subjects),
		[](const Subject& lhs, const Subject& rhs)
	{
		return (lhs.headDepth * lhs.headHeight * lhs.headWidth) <
			(rhs.headDepth * rhs.headHeight * rhs.headWidth);
	});

	hrir = HrirBuffer();
	for (int i = 0; i < hrir[0].size(); ++i)
	{
		hrir[0][i] = 0.f;
		hrir[1][i] = 0.f;
	}
}

void HRTFContainer::interpolate(float azimuth, float elevation)
{	
	if (currentSubjectIndex == -1)
		return;

	Subject& currentSubject = subjects.at(currentSubjectIndex);
	// find the triangle containing the query point
	auto face = currentSubject.triangulation.inexact_locate(Point2(azimuth, elevation));
	// if not null, update hrir
	if (face != nullptr)
	{
		auto tri = currentSubject.triangulation.triangle(face);
		// compute barycentric coordinates
		// g = (X - C) * T^-1 where T = [A - C; B- C]
		// vertices of the triangle
		auto A = tri[0];
		auto B = tri[1];
		auto C = tri[2];

		double T [] = { A[0] - C[0], A[1] - C[1],
			B[0] - C[0], B[1] - C[1] };
		double invT [] = { T[3], -T[1], -T[2], T[0] };
		auto det = 1 / (T[0] * T[3] - T[1] * T[2]);
		for (int i = 0; i < 4; ++i)
			invT[i] *= det;

		double X [] = { azimuth - C[0], elevation - C[1] };
		auto g1 = invT[0] * X[0] + invT[2] * X[1];
		auto g2 = invT[1] * X[0] + invT[3] * X[1];
		auto g3 = 1 - g1 - g2;

		try 
		{
			auto& hrirDict = currentSubject.hrirDict;
			auto& irA = hrirDict.at(A[0]).at(getElvIndex(A[1]));
			auto& irB = hrirDict.at(B[0]).at(getElvIndex(B[1]));
			auto& irC = hrirDict.at(C[0]).at(getElvIndex(C[1]));
			// update hrir
			for (int i = 0; i < hrir[0].size(); ++i)
			{
				hrir[0][i] = g1 * irA[0][i] + g2 * irB[0][i] + g3 * irC[0][i];
				hrir[1][i] = g1 * irA[1][i] + g2 * irB[1][i] + g3 * irC[1][i];
			}
		}
		catch (std::out_of_range e) {
		}
	}
}

void HRTFContainer::setCurrentSubject(int id)
{
	for (int i = 0; i < subjects.size(); ++i)
	{
		if (subjects[i].id == id)
		{
			currentSubjectIndex = i;
			if (subjects[i].hrirDict.empty())
				loadSubjectHrir(subjects.at(i));
			return;
		}
	}
}

void HRTFContainer::loadSubjectHrir(Subject& subject)
{
	String idString = String(subject.id);
	if (subject.id < 10)
		idString = "00" + idString;
	else if (subject.id < 100)
		idString = "0" + idString;

	FileInputStream istream(File(thisDir.getFullPathName() + "/hrir/subject_" + idString + ".bin"));

	if (istream.openedOk())
	{
		std::vector<Point2> points;
		int azimuths [] = { -90, -80, -65, -55, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90 };
		auto& hrirDict = subject.hrirDict;
		for (auto azm : azimuths)
		{

			hrirDict.insert(std::make_pair(azm, std::array<HrirBuffer, 52>()));
			// -90 deg
			istream.read(hrirDict[azm][0][0].data(), 200 * sizeof(float));
			istream.read(hrirDict[azm][0][1].data(), 200 * sizeof(float));
			points.push_back({ azm, -90 });
			// 50 elevations
			for (int i = 1; i < 51; ++i)
			{
				istream.read(hrirDict[azm][i][0].data(), 200 * sizeof(float));
				istream.read(hrirDict[azm][i][1].data(), 200 * sizeof(float));
				points.push_back({ azm, -45 + 5.625*i });
			}
			// 270 deg
			istream.read(hrirDict[azm][51][0].data(), 200 * sizeof(float));
			istream.read(hrirDict[azm][51][1].data(), 200 * sizeof(float));
			points.push_back({ azm, 270 });
		}
		// triangulation
		subject.triangulation.insert(std::begin(points), std::end(points));
	}
}

int HRTFContainer::getElvIndex(int elv)
{ 
	if (elv == -90)
		return 0;
	else if (elv == 270)
		return 51;
	else
		return roundl((elv + 45) / 5.625f);
}
