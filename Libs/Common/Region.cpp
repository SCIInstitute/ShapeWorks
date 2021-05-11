#include "Region.h"

namespace shapeworks {

LogicalRegion::LogicalRegion(const PhysicalRegion &region) {
  min = Coord({static_cast<Coord::value_type>(std::floor(region.min[0])),
               static_cast<Coord::value_type>(std::floor(region.min[1])),
               static_cast<Coord::value_type>(std::floor(region.min[2]))});
  max = Coord({static_cast<Coord::value_type>(std::floor(region.max[0])),
               static_cast<Coord::value_type>(std::floor(region.max[1])),
               static_cast<Coord::value_type>(std::floor(region.max[2]))});
}

LogicalRegion& LogicalRegion::pad(int padding)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] -= padding;
		max[i] += padding;
	}

  return *this;
}

LogicalRegion& LogicalRegion::shrink(const LogicalRegion &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::max(min[i], other.min[i]);
		max[i] = std::min(max[i], other.max[i]);
	}

  return *this;
}

LogicalRegion& LogicalRegion::grow(const LogicalRegion &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::min(min[i], other.min[i]);
		max[i] = std::max(max[i], other.max[i]);
	}

  return *this;
}

LogicalRegion& LogicalRegion::expand(const Coord &pt)
{
	for (auto i=0; i<3; i++)
	{
		min[i] = std::min(min[i], pt[i]);
		max[i] = std::max(max[i], pt[i]);
	}

  return *this;
}

std::ostream &operator<<(std::ostream &os, const LogicalRegion &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}



PhysicalRegion& PhysicalRegion::pad(double padding)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] -= padding;
		max[i] += padding;
	}

  return *this;
}

PhysicalRegion& PhysicalRegion::shrink(const PhysicalRegion &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::max(min[i], other.min[i]);
		max[i] = std::min(max[i], other.max[i]);
	}

  return *this;
}

PhysicalRegion& PhysicalRegion::grow(const PhysicalRegion &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::min(min[i], other.min[i]);
		max[i] = std::max(max[i], other.max[i]);
	}

  return *this;
}

PhysicalRegion& PhysicalRegion::expand(const Point &pt)
{
	for (auto i=0; i<3; i++)
	{
		min[i] = std::min(min[i], pt[i]);
		max[i] = std::max(max[i], pt[i]);
	}

  return *this;
}

std::ostream &operator<<(std::ostream &os, const PhysicalRegion &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

}
