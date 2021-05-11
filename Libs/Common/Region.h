#pragma once

#include "Shapeworks.h"

namespace shapeworks
{

// A logical region of an Image or a Mesh
class LogicalRegion;

// A physical region of an Image or a Mesh
class PhysicalRegion;


class LogicalRegion
{
public:
  Coord min{1000000000, 1000000000, 1000000000};
  Coord max{-1000000000, -1000000000, -1000000000};

  LogicalRegion(const Dims &dims) : min{0, 0, 0} {
    if (0 != (dims[0] + dims[1] + dims[2])) {
      max = {static_cast<Coord::value_type>(dims[0]) - 1,
             static_cast<Coord::value_type>(dims[1]) - 1,
             static_cast<Coord::value_type>(dims[2]) - 1};
    }
  }
  LogicalRegion(const Coord &_min, const Coord &_max) :
    min{_min[0], _min[1], _min[2]}, max{_max[0], _max[1], _max[2]} {}
  LogicalRegion() = default;
  LogicalRegion(const PhysicalRegion &region);

  bool operator==(const LogicalRegion &other) const { return min == other.min && max == other.max; }

  /// verified min/max do not create an inverted or an empty region
  bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

  Coord origin() const { return Coord({min[0], min[1], min[2]}); }
  Dims size() const {
    return Dims({static_cast<Dims::value_type>(max[0] - min[0] + 1),
                 static_cast<Dims::value_type>(max[1] - min[1] + 1),
                 static_cast<Dims::value_type>(max[2] - min[2] + 1)});
  }

  /// grows or shrinks the region by the specified amount
  LogicalRegion& pad(int padding);

  /// shrink this region down to the smallest portions of both
  LogicalRegion& shrink(const LogicalRegion &other);

  /// grow this region up to the largest portions of both
  LogicalRegion& grow(const LogicalRegion &other);

  /// expand this region to include this point
  LogicalRegion& expand(const Coord &pt);

};

std::ostream &operator<<(std::ostream &os, const LogicalRegion &region);


class PhysicalRegion
{
public:
  Point min;
  Point max;

  PhysicalRegion(const Point &size) {
    min = Point({0, 0, 0});
    if (0 != (size[0] + size[1] + size[2])) {
      max = Point({size[0], size[1], size[2]});
    }
  }

  PhysicalRegion(const Point &_min, const Point &_max) {
    min[0] = _min[0];
    min[1] = _min[1];
    min[2] = _min[2];
    max[0] = _max[0];
    max[1] = _max[1];
    max[2] = _max[2];
  }
  
  PhysicalRegion() {
    min = Point({1000000000, 1000000000, 1000000000});
    max = Point({-1000000000, -1000000000, -1000000000});
  }

  PhysicalRegion(const LogicalRegion &region) {
    min = toPoint(region.min);
    max = toPoint(region.max);
  }

  bool operator==(const PhysicalRegion &other) const {
    return min == other.min && max == other.max;
  }

  /// verified min/max do not create an inverted or an empty region
  bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

  Point origin() const { return min; }
  Point size() const { return max - min; }

  /// grows or shrinks the region by the specified amount
  PhysicalRegion& pad(double padding);

  /// shrink this region down to the smallest portions of both
  PhysicalRegion& shrink(const PhysicalRegion &other);

  /// grow this region up to the largest portions of both
  PhysicalRegion& grow(const PhysicalRegion &other);

  /// expand this region to include this point
  PhysicalRegion& expand(const Point &pt);

};

std::ostream &operator<<(std::ostream &os, const PhysicalRegion &region);

} // namespace shapeworks
