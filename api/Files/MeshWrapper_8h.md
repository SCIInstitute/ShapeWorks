---
title: Libs/Optimize/ParticleSystem/MeshWrapper.h

---

# Libs/Optimize/ParticleSystem/MeshWrapper.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md)**  |




## Source code

```cpp
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
  typedef typename itk::ParticleDomain::GradNType GradNType;

  // Computed distance between points. If out_grad != nullptr, returns the gradient of the distance in that vector
  virtual double ComputeDistance(const PointType& pointa, int idx_a,
                                 const PointType& pointb, int idx_b,
                                 vnl_vector_fixed<double, 3> *out_grad=nullptr) const = 0;

  // Return true if the distance between point a and b is within test_dist
  virtual bool IsWithinDistance(const PointType &a, int idx_a,
                                const PointType &b, int idx_b,
                                double test_dist, double& dist) const = 0;

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

  virtual bool IsGeodesicsEnabled() const = 0;
};

}
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000
