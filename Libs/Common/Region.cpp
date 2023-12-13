#include "Region.h"

namespace shapeworks {

IndexRegion &IndexRegion::pad(int padding) {
  for (auto i = 0; i < 3; i++) {
    min[i] -= padding;
    max[i] += padding;
  }

  return *this;
}

std::ostream &operator<<(std::ostream &os, const IndexRegion &r) {
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

PhysicalRegion::PhysicalRegion(std::string str) {
  // parse string into doubles using space as delimiter
  std::istringstream iss(str);
  std::vector<double> values;
  std::string token;
  while (std::getline(iss, token, ' ')) {
    values.push_back(std::stod(token));
  }
  if (values.size() != 6) {
    throw std::runtime_error("PhysicalRegion::PhysicalRegion: invalid string");
  }
  min = Point({values[0], values[1], values[2]});
  max = Point({values[3], values[4], values[5]});
}

PhysicalRegion &PhysicalRegion::shrink(const PhysicalRegion &other) {
  for (auto i = 0; i < 3; i++) {
    min[i] = std::max(min[i], other.min[i]);
    max[i] = std::min(max[i], other.max[i]);
  }

  return *this;
}

PhysicalRegion &PhysicalRegion::expand(const PhysicalRegion &other) {
  for (auto i = 0; i < 3; i++) {
    min[i] = std::min(min[i], other.min[i]);
    max[i] = std::max(max[i], other.max[i]);
  }

  return *this;
}

PhysicalRegion &PhysicalRegion::expand(const Point &pt) {
  for (auto i = 0; i < 3; i++) {
    min[i] = std::min(min[i], pt[i]);
    max[i] = std::max(max[i], pt[i]);
  }

  return *this;
}

std::ostream &operator<<(std::ostream &os, const PhysicalRegion &r) {
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

PhysicalRegion &PhysicalRegion::pad(double padding) {
  min -= Point({padding, padding, padding});
  max += Point({padding, padding, padding});

  return *this;
}

std::string PhysicalRegion::to_string() const {
  std::stringstream ss;
  ss << min[0] << " " << min[1] << " " << min[2] << " " << max[0] << " " << max[1] << " " << max[2];
  return ss.str();
}

}  // namespace shapeworks
