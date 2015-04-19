#pragma once

#include <cmath>

const double Pi = 3.1415926535897932384626433832795;

template <typename Type>
struct Point3Cartesian
{
	Type x;
	Type y;
	Type z;
	Point3Cartesian<Type>(Type x, Type y, Type z)
		:
		x(x),
		y(y),
		z(z)
	{
	}
};

template <typename Type>
struct Point3DoublePolar
{
	Type radius;
	Type azimuth;
	Type elevation;
	Point3DoublePolar(Type radius, Type azimuth, Type elevation)
		:
		radius(radius),
		azimuth(azimuth),
		elevation(elevation)
	{
	}
};

template <typename Type>
inline Type deg2rad(Type deg)
{
	return deg * Pi / 180;
}

template <typename Type>
inline Type rad2deg(Type rad)
{
	return rad * 180 / Pi;
}

template <typename Type>
inline Point3DoublePolar<Type> cartesianToInteraural(const Point3Cartesian<Type>& p)
{
	Type radius = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	Type azimuth = std::asin(p.x / radius);
	Type elevation = std::atan2(p.z, p.y);
	if (p.y < 0 && p.z < 0)
		elevation += 2 * Pi;

	return Point3DoublePolar<Type>{ radius, azimuth, elevation };
}

template <typename Type>
inline Point3Cartesian<Type> interauralToCartesian(const Point3DoublePolar<Type>& p)
{
	Type x = p.radius * std::sin(p.azimuth);
	Type y = p.radius * std::cos(p.azimuth) * std::cos(p.elevation);
	Type z = p.radius * std::cos(p.azimuth) * std::sin(p.elevation);

	return Point3Cartesian<Type>{ x, y, z };
}

template <class T1, class T2>
inline std::vector<float> convolve(const T1& x1, const T2& x2)
{
	std::vector<float> y(x1.size() + x2.size() - 1, 0);
	for (int n = 0; n < y.size(); ++n)
	{
		for (int k = 0; k < x2.size(); ++k)
		{
			if (n - k >= 0 && n - k < x1.size())
				y[n] += x1[n - k] * x2[k];
		}
	}
	return y;
}

