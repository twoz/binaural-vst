#pragma once
#include <complex>
#include <vector>


template <typename T>
using ComplexVector = std::vector<std::complex<T>>;

const float Pi = 3.14159265358f;

inline bool isPowerOf2(size_t val)
{
	return (val == 1 || (val & (val - 1)) == 0);
}

inline int nextPowerOf2(int x)
{
	if (x < 0)
		return 0;
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

template <typename T>
inline T mapToRange(T value, T minFrom, T maxFrom, T minTo, T maxTo)
{
	return ((value - minFrom) / (maxFrom - minFrom)) * (maxTo - minTo) + minTo;
}

template <typename T>
inline T clamp(const T& x, const T& lower, const T& upper)
{
	return std::min<T>(std::max<T>(x, lower), upper);
}

template <typename Type>
struct Point3Cartesian
{
	Type x;
	Type y;
	Type z;
};

template <typename Type>
struct Point3DoublePolar
{
	Type radius;
	Type azimuth;
	Type elevation;
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

	return Point3DoublePolar < Type > { radius, azimuth, elevation };
}

template <typename Type>
inline Point3Cartesian<Type> interauralToCartesian(const Point3DoublePolar<Type>& p)
{
	Type x = p.radius * std::sin(p.azimuth);
	Type y = p.radius * std::cos(p.azimuth) * std::cos(p.elevation);
	Type z = p.radius * std::cos(p.azimuth) * std::sin(p.elevation);

	return Point3Cartesian < Type > { x, y, z };
}

template <typename Type>
inline Point3DoublePolar<Type> sphericalToInteraural(const Point3DoublePolar<Type>& p)
{
	Type x = p.radius * std::cos(p.elevation) * std::sin(p.azimuth);
	Type y = p.radius * std::cos(p.elevation) * std::cos(p.azimuth);
	Type z = p.radius * std::sin(p.elevation);

	return cartesianToInteraural(Point3Cartesian < Type > {x, y, z});
}
