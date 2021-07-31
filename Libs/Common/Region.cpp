#include "Region.h"

namespace shapeworks {

IndexRegion& IndexRegion::pad(int padding)
{
	for (auto i = 0; i < 3; i++)
	{
		min[i] -= padding;
		max[i] += padding;
	}

  return *this;
}

std::ostream &operator<<(std::ostream &os, const IndexRegion &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
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

PhysicalRegion& PhysicalRegion::expand(const PhysicalRegion &other)
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

PhysicalRegion& PhysicalRegion::pad(double padding)
{
  min -= Point({padding, padding, padding});
  max += Point({padding, padding, padding});

  return *this;
}

}
