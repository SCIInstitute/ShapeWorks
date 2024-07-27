---
title: Libs/Optimize/Domain/ParticleDomain.h

---

# Libs/Optimize/Domain/ParticleDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**  |




## Source code

```cpp
#pragma once

#include "DomainType.h"
#include "Libs/Optimize/Constraints/Constraints.h"
#include "itkDataObject.h"
#include "itkPoint.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

class ParticleDomain {
 public:
  typedef std::shared_ptr<ParticleDomain> Pointer;

  using PointType = itk::Point<double, 3>;
  using GradNType = vnl_matrix_fixed<float, 3, 3>;
  using VectorDoubleType = vnl_vector_fixed<double, 3>;
  using VectorFloatType = vnl_vector_fixed<float, 3>;

  virtual bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const = 0;

  // todo update should be const?
  virtual PointType UpdateParticlePosition(const PointType &point, int idx, VectorDoubleType &update) const = 0;

  virtual void InvalidateParticlePosition(int idx) const {}

  virtual VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType &gradE, const PointType &pos,
                                                         int idx) const = 0;
  virtual VectorFloatType SampleGradientAtPoint(const PointType &point, int idx) const = 0;
  virtual VectorFloatType SampleNormalAtPoint(const PointType &point, int idx) const = 0;
  virtual GradNType SampleGradNAtPoint(const PointType &p, int idx) const = 0;

  virtual double Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                          VectorDoubleType *out_grad = nullptr) const {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = a[i] - b[i];
      }
    }
    return a.EuclideanDistanceTo(b);
  }
  virtual double SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const {
    return a.SquaredEuclideanDistanceTo(b);
  }
  virtual bool IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                                double &distance) const {
    distance = this->Distance(a, idx_a, b, idx_b);
    return distance < test_dist;
  }

  virtual double GetCurvature(const PointType &p, int idx) const = 0;
  virtual double GetSurfaceMeanCurvature() const = 0;
  virtual double GetSurfaceStdDevCurvature() const = 0;

  virtual const PointType &GetLowerBound() const = 0;
  virtual const PointType &GetUpperBound() const = 0;

  virtual PointType GetZeroCrossingPoint() const = 0;
  virtual double GetSurfaceArea() const = 0;

  virtual PointType GetValidLocationNear(PointType p) const = 0;
  virtual double GetMaxDiameter() const = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;
  virtual void UpdateZeroCrossingPoint() = 0;

  bool IsDomainFixed() const { return m_FixedDomain; }
  virtual shapeworks::DomainType GetDomainType() const = 0;

  std::shared_ptr<shapeworks::Constraints> GetConstraints() const { return constraints; }

  // Use `random` to advance a particle and return a new position
  virtual PointType GetPositionAfterSplit(const PointType &pt, const VectorDoubleType &local_direction,
                                          const VectorDoubleType &global_direction, double epsilon) const {
    // todo this has been copied from itkParticleSystem::AdvancedAllParticleSplitting.
    //  Ideally, we should compute a direction that is "consistent" depending on the domain type and use the
    //  `UpdateParticlePosition` API to advance the particle. See ContourDomain for an example. Leaving this be for
    //  now because we'd have to retest all MeshDomain and ImageDomain use cases if this behaviour changes.
    PointType new_pt;
    for (unsigned int k = 0; k < 3; k++) {
      new_pt[k] = pt[k] + epsilon * local_direction[k] / 5.;
    }
    return new_pt;
  }

  void SetDomainID(int id) { this->m_DomainID = id; }

  void SetDomainName(std::string name) { this->m_DomainName = name; }

 protected:
  // is this a fixed domain or not?  We start as fixed and if an image or mesh is set, we set this to false
  bool m_FixedDomain{true};

  int m_DomainID{-1};
  std::string m_DomainName;

  ParticleDomain() { this->constraints = std::make_shared<shapeworks::Constraints>(); }
  virtual ~ParticleDomain() {}

  std::shared_ptr<shapeworks::Constraints> constraints;

 private:
  ParticleDomain(const ParticleDomain &);  // purposely not implemented
  void operator=(const ParticleDomain &);  // purposely not implemented
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-27 at 05:46:32 +0000
