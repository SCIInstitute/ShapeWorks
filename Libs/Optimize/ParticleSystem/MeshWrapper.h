#pragma once

#include <vnl/vnl_vector_fixed.h>

#include "itkParticleDomain.h"
#include "DomainType.h"

#include "Constraints.h"

namespace shapeworks
{
class MeshWrapper
{
public:
  typedef typename itk::ParticleDomain::PointType PointType;
  typedef typename itk::ParticleDomain::GradNType GradNType;

  void SetConstraints(std::shared_ptr<itk::Constraints> constraints)
  {
    this->constraints_ = constraints;
  }

  // Computed distance between points. (Currently euclidean)
  virtual double ComputeDistance(PointType pointa, PointType pointb) const = 0;
  // Returns updated point position after applying the update vector to the initial position.
  virtual PointType GeodesicWalk(PointType pointa, int idx, vnl_vector_fixed<double, DIMENSION> vector) const = 0;

  // Returns a point on the mesh.
  virtual PointType GetPointOnMesh() const = 0;

  // Returns minimum corner of bounding box.
  virtual const PointType &GetMeshLowerBound() const = 0;
  // Returns maximum corner of bounding box.
  virtual const PointType &GetMeshUpperBound() const = 0;

  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(const PointType & pointa, int idx, vnl_vector_fixed<double, DIMENSION> & vector) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p, int idx) const = 0;
  virtual GradNType SampleGradNAtPoint(PointType p, int idx) const = 0;

  // Returns closest point on mesh to pointa.
  virtual PointType SnapToMesh(PointType pointa, int idx) const = 0;

  virtual void InvalidateParticle(int idx) {};

protected:

  std::shared_ptr<itk::Constraints> constraints_;
};

}
