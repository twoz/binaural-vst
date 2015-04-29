#include "HRTF.h"

Subject::Subject(String pathToHRIR, String name, String description,
	float headWidth, float headHeight, float headDepth)
	:
	info({name, description, headWidth, headHeight, headDepth}),
	triangulation(nullptr)
{
	loadHrir(pathToHRIR);
}


void Subject::loadHrir(String filename)
{
	FileInputStream istream(filename);
	if (istream.openedOk())
	{
		std::vector<tpp::Delaunay::Point> points;
		int azimuths[] = {-90, -80, -65, -55, -45, -40, -35, -30, -25, -20,
			-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90};
		for (auto azm : azimuths)
		{

			hrirDict.insert(std::make_pair(azm, std::array<HrirBuffer, 52>()));
			// -90 deg
			istream.read(hrirDict[azm][0][0].data(), 200 * sizeof(float));
			istream.read(hrirDict[azm][0][1].data(), 200 * sizeof(float));
			points.push_back(tpp::Delaunay::Point(azm, -90));
			// 50 elevations
			for (int i = 1; i < 51; ++i)
			{
				istream.read(hrirDict[azm][i][0].data(), 200 * sizeof(float));
				istream.read(hrirDict[azm][i][1].data(), 200 * sizeof(float));
				points.push_back(tpp::Delaunay::Point(azm, -45 + 5.625 * (i - 1)));
			}
			// 270 deg
			istream.read(hrirDict[azm][51][0].data(), 200 * sizeof(float));
			istream.read(hrirDict[azm][51][1].data(), 200 * sizeof(float));
			points.push_back(tpp::Delaunay::Point(azm, 270));
		}
		triangulation = new tpp::Delaunay(points);
		triangulation->Triangulate();
	}
	else
		throw std::ios_base::failure("Failed to open HRIR file");
}

HrirBuffer Subject::interpolateHRIR(double azimuth, double elevation)
{
	/* Iterate through all the faces of the triangulation */
	for (auto fit = triangulation->fbegin(); fit != triangulation->fend(); ++fit)
	{
		auto vertexA = triangulation->Org(fit);
		auto vertexB = triangulation->Dest(fit);
		auto vertexC = triangulation->Apex(fit);
		auto A = triangulation->point_at_vertex_id(vertexA);
		auto B = triangulation->point_at_vertex_id(vertexB);
		auto C = triangulation->point_at_vertex_id(vertexC);

		double T[] = {A[0] - C[0], A[1] - C[1],
			B[0] - C[0], B[1] - C[1]};
		double invT[] = {T[3], -T[1], -T[2], T[0]};
		auto det = 1 / (T[0] * T[3] - T[1] * T[2]);
		for (int i = 0; i < 4; ++i)
			invT[i] *= det;

		double X[] = {azimuth - C[0], elevation - C[1]};

		/* Barycentric coordinates of point X. */
		auto g1 = invT[0] * X[0] + invT[2] * X[1];
		auto g2 = invT[1] * X[0] + invT[3] * X[1];
		auto g3 = 1 - g1 - g2;

		/* If any of the barycentric coordinate is negative, the point
		does not lay inside the triangle, so continue the loop.
		todo: move to the opposite triangle
		*/
		if (g1 < 0 || g2 < 0 || g3 < 0)
			continue;
		else
		{
			auto& irA = hrirDict[(int)A[0]][getElvIndex(std::lround(A[1]))];
			auto& irB = hrirDict[(int)B[0]][getElvIndex(std::lround(B[1]))];
			auto& irC = hrirDict[(int)C[0]][getElvIndex(std::lround(C[1]))];
			/* Fill HRIR array and return */
			HrirBuffer hrir;
			for (size_t i = 0; i < hrir[0].size(); ++i)
			{
				hrir[0][i] = g1 * irA[0][i] + g2 * irB[0][i] + g3 * irC[0][i];
				hrir[1][i] = g1 * irA[1][i] + g2 * irB[1][i] + g3 * irC[1][i];
			}
			return hrir;
		}
	}
	/* If the query point was not found , return zero-ed impulse response */
	return HrirBuffer();
}

int Subject::getElvIndex(int elv)
{
	if (elv == -90)
		return 0;
	else if (elv == 270)
		return 51;
	else
		return std::lroundf((elv + 45) / 5.625f);
}

HRTFContainer::HRTFContainer()
	:
	hrir()
{
	auto thisDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory();
	String filenames[] = {"kemar.bin", "male.bin", "female.bin"};
	String names[] = {"Kemar", "Male", "Female"};
	for (int i = 0; i < 3; ++i)
	{
		try
		{
			subjects.emplace_back(Subject(
				thisDir.getFullPathName() + "/hrir/" + filenames[i],
				names[i],
				"",
				0,
				0,
				0));
		}
		catch (std::ios_base::failure)
		{
			// dont crash, just dont add to a vector
		}
	}

	setCurrentSubject("Kemar");
	interpolate(0.0, 0.0);
}

HRTFContainer::~HRTFContainer()
{
}

void HRTFContainer::interpolate(double azimuth, double elevation)
{
	hrir = subjects[currentSubjectIndex].interpolateHRIR(azimuth, elevation);
}

void HRTFContainer::setCurrentSubject(size_t index)
{
	if (index >= 0 && index < subjects.size())
		currentSubjectIndex = index;
}

void HRTFContainer::setCurrentSubject(String name)
{
	for (size_t i = 0; i < subjects.size(); ++i)
	{
		if (subjects[i].info.name == name)
		{
			currentSubjectIndex = i;
			break;
		}
	}
}

SubjectInfo HRTFContainer::getCurrentSubjectInfo() const
{
	return subjects[currentSubjectIndex].info;
}

std::vector<SubjectInfo> HRTFContainer::getSubjectsInfo() const
{
	std::vector<SubjectInfo> subjectsInfo;
	for (auto& subject : subjects)
		subjectsInfo.push_back(subject.info);
	return subjectsInfo;
}

int HRTFContainer::getCurrentSubjectIndex() const
{
	return currentSubjectIndex;
}