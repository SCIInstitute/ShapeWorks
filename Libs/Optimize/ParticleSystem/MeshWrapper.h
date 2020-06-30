#pragma once

#include "vnl/vnl_vector_fixed.h"

#include "itkParticleDomain.h"
#include "DomainType.h"

namespace shapeworks
{
class MeshWrapper
{
public:
  typedef typename itk::ParticleDomain::PointType PointType;

  virtual double ComputeDistance(PointType pointa, PointType pointb) const = 0;
  virtual PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<double, DIMENSION> vector) const = 0;

  virtual PointType GetPointOnMesh() const = 0;

  virtual const PointType &GetMeshLowerBound() const = 0;
  virtual const PointType &GetMeshUpperBound() const = 0;

  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(const PointType & pointa, vnl_vector_fixed<double, DIMENSION> & vector) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p) const = 0;

  virtual PointType SnapToMesh(PointType pointa) const = 0;
};

}
