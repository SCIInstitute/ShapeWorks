#pragma once

#include "vnl/vnl_vector_fixed.h"

#include "itkParticleDomain.h"

namespace shapeworks
{
class MeshWrapper
{
public:
  typedef typename itk::ParticleDomain<3>::PointType PointType;

  virtual double ComputeDistance(PointType pointa, PointType pointb) const = 0;
  virtual PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<float, 3> vector) const = 0;

  virtual PointType GetPointOnMesh() const = 0;

  virtual PointType GetMeshLowerBound() const = 0;
  virtual PointType GetMeshUpperBound() const = 0;

  virtual vnl_vector_fixed<float, 3> ProjectVectorToSurfaceTangent(const PointType & pointa, vnl_vector_fixed<float, 3> & vector) const = 0;
  virtual vnl_vector_fixed<float, 3> SampleNormalAtPoint(PointType p) const = 0;

  virtual PointType SnapToMesh(PointType pointa) const = 0;
};

}
