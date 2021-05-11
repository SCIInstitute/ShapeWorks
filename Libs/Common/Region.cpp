#include "Region.h"

namespace shapeworks {

void Region::pad(int padding)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] -= padding;
		max[i] += padding;
	}
}

void Region::shrink(const Region &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::max(min[i], other.min[i]);
		max[i] = std::min(max[i], other.max[i]);
	}
}

void Region::grow(const Region &other)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] = std::min(min[i], other.min[i]);
		max[i] = std::max(max[i], other.max[i]);
	}
}

void Region::expand(const Coord &pt)
{
	for (auto i=0; i<3; i++)
	{
		min[i] = std::min(min[i], pt[i]);
		max[i] = std::max(max[i], pt[i]);
	}
}

std::ostream &operator<<(std::ostream &os, const Region &r)
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
