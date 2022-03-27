#pragma once

#include "itkPoint.h"
namespace itk {
/**
    Struct containing a Point and an index value associated with a point.  This
    object is used mainly by itkParticleNeighborhood*/
template <unsigned int VDimension>
struct ParticlePointIndexPair {
  typedef itk::Point<double, VDimension> PointType;
  ParticlePointIndexPair() {}
  ParticlePointIndexPair(const PointType &p, unsigned int i = 0) {
    Point = p;
    Index = i;
  }
  ParticlePointIndexPair(const ParticlePointIndexPair &o) { this->operator=(o); }
  const ParticlePointIndexPair &operator=(const ParticlePointIndexPair &o) {
    Point = o.Point;
    Index = o.Index;
    return *this;
  }
  PointType Point;
  unsigned int Index;
};

}  // namespace itk
