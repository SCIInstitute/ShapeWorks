#pragma once

#include "Shapeworks.h"

namespace shapeworks
{

// A logical region of an Image or a Mesh
class Region
{
public:
  std::vector<Coord::value_type> min{1000000000, 1000000000, 1000000000};
  std::vector<Coord::value_type> max{-1000000000, -1000000000, -1000000000};
  Region(const Dims &dims) : min{0, 0, 0} {
    if (0 != (dims[0] + dims[1] + dims[2])) {
      max = {static_cast<Coord::value_type>(dims[0]) - 1,
             static_cast<Coord::value_type>(dims[1]) - 1,
             static_cast<Coord::value_type>(dims[2]) - 1};
    }
  }
  Region(const Coord &_min, const Coord &_max) :
    min{_min[0], _min[1], _min[2]}, max{_max[0], _max[1], _max[2]} {}
  Region() = default;
  bool operator==(const Region &other) const { return min == other.min && max == other.max; }

  /// verified min/max do not create an inverted or an empty region
  bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

  Coord origin() const { return Coord({min[0], min[1], min[2]}); }
  Dims size() const {
    return Dims({static_cast<Dims::value_type>(max[0] - min[0]),
                 static_cast<Dims::value_type>(max[1] - min[1]),
                 static_cast<Dims::value_type>(max[2] - min[2])});
  }

  /// grows or shrinks the region by the specified amount
  void pad(int padding);

  /// shrink this region down to the smallest portions of both
  void shrink(const Region &other);

  /// grow this region up to the largest portions of both
  void grow(const Region &other);

  /// expand this region to include this point
  void expand(const Coord &pt);

};

std::ostream &operator<<(std::ostream &os, const Region &region);

} // namespace shapeworks
