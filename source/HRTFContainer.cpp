#include "HRTFContainer.h"


HRTFContainer::HRTFContainer()
	:
	hrir_()
{
}

HRTFContainer::~HRTFContainer()
{
}

void HRTFContainer::updateHRIR(double azimuth, double elevation)
{
	if (triangulation_ == nullptr)
		return;

	// Iterate through all the faces of the triangulation
	for (auto fit = triangulation_->fbegin(); fit != triangulation_->fend(); ++fit)
	{
		auto vertexA = triangulation_->Org(fit);
		auto vertexB = triangulation_->Dest(fit);
		auto vertexC = triangulation_->Apex(fit);
		auto A = triangulation_->point_at_vertex_id(vertexA);
		auto B = triangulation_->point_at_vertex_id(vertexB);
		auto C = triangulation_->point_at_vertex_id(vertexC);

		double T[] = {A[0] - C[0], A[1] - C[1],
			B[0] - C[0], B[1] - C[1]};
		double invT[] = {T[3], -T[1], -T[2], T[0]};
		auto det = 1 / (T[0] * T[3] - T[1] * T[2]);
		for (int i = 0; i < 4; ++i)
			invT[i] *= det;

		double X[] = {azimuth - C[0], elevation - C[1]};

		// Barycentric coordinates of point X
		auto g1 = invT[0] * X[0] + invT[2] * X[1];
		auto g2 = invT[1] * X[0] + invT[3] * X[1];
		auto g3 = 1 - g1 - g2;

		// If any of the barycentric coordinate is negative, the point
		// does not lay inside the triangle, so continue the loop.
		if (g1 < 0 || g2 < 0 || g3 < 0)
			continue;
		else
		{
			auto& irA = hrirDict_[(int)A[0]][getElvIndex(std::lround(A[1]))];
			auto& irB = hrirDict_[(int)B[0]][getElvIndex(std::lround(B[1]))];
			auto& irC = hrirDict_[(int)C[0]][getElvIndex(std::lround(C[1]))];
			// Fill HRIR array and return
			for (size_t i = 0; i < hrir_[0].size(); ++i)
			{
				hrir_[0][i] = g1 * irA[0][i] + g2 * irB[0][i] + g3 * irC[0][i];
				hrir_[1][i] = g1 * irA[1][i] + g2 * irB[1][i] + g3 * irC[1][i];
			}
			return;
		}
	}
	// If the query point was not found , return zero-ed impulse response
	return;
}

const HrirBuffer& HRTFContainer::hrir() const
{
	return hrir_;
}

void HRTFContainer::loadHrir(String filename)
{
	FileInputStream istream(filename);
	if (istream.openedOk())
	{
		std::vector<tpp::Delaunay::Point> points;
		int azimuths[] = {-90, -80, -65, -55, -45, -40, -35, -30, -25, -20,
			-15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 55, 65, 80, 90};
		for (auto azm : azimuths)
		{

			hrirDict_.insert(std::make_pair(azm, std::array<HrirBuffer, 52>()));
			// -90 deg
			istream.read(hrirDict_[azm][0][0].data(), 200 * sizeof(float));
			istream.read(hrirDict_[azm][0][1].data(), 200 * sizeof(float));
			points.push_back(tpp::Delaunay::Point(azm, -90));
			// 50 elevations
			for (int i = 1; i < 51; ++i)
			{
				istream.read(hrirDict_[azm][i][0].data(), 200 * sizeof(float));
				istream.read(hrirDict_[azm][i][1].data(), 200 * sizeof(float));
				points.push_back(tpp::Delaunay::Point(azm, -45 + 5.625 * (i - 1)));
			}
			// 270 deg
			istream.read(hrirDict_[azm][51][0].data(), 200 * sizeof(float));
			istream.read(hrirDict_[azm][51][1].data(), 200 * sizeof(float));
			points.push_back(tpp::Delaunay::Point(azm, 270));
		}
		triangulation_ = new tpp::Delaunay(points);
		triangulation_->Triangulate();
	}
	else
		throw std::ios_base::failure("Failed to open HRIR file");
}

int HRTFContainer::getElvIndex(int elv)
{
	if (elv == -90)
		return 0;
	else if (elv == 270)
		return 51;
	else
		return std::lroundf((elv + 45) / 5.625f);
}
