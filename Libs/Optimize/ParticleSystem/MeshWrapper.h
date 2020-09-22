#pragma once

#include <vnl/vnl_vector_fixed.h>

#include "itkParticleDomain.h"
#include "DomainType.h"

namespace shapeworks
{
class MeshWrapper
{
public:
  typedef typename itk::ParticleDomain::PointType PointType;

  // Computed distance between points. (Currently euclidean)
  virtual double ComputeDistance(PointType pointa, PointType pointb) const = 0;
  // Returns updated point position after applying the update vector to the initial position.
  virtual PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<double, DIMENSION> vector) const = 0;

  // Returns a point on the mesh.
  virtual PointType GetPointOnMesh() const = 0;

  // Returns minimum corner of bounding box.
  virtual const PointType &GetMeshLowerBound() const = 0;
  // Returns maximum corner of bounding box.
  virtual const PointType &GetMeshUpperBound() const = 0;

  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(const PointType & pointa, vnl_vector_fixed<double, DIMENSION> & vector) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p) const = 0;

  // Returns closest point on mesh to pointa.
  virtual PointType SnapToMesh(PointType pointa) const = 0;
};

}
