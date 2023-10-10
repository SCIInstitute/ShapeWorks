---
title: Libs/Optimize/Domain/MeshDomain.h

---

# Libs/Optimize/Domain/MeshDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshDomain](../Classes/classshapeworks_1_1MeshDomain.md)**  |




## Source code

```cpp
#pragma once

#include <itkObjectFactory.h>

#include "MeshWrapper.h"
#include "ParticleDomain.h"

namespace shapeworks {

class MeshDomain : public ParticleDomain {
 public:
  using Pointer = std::shared_ptr<MeshDomain>;

  DomainType GetDomainType() const override { return DomainType::Mesh; }

  bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override;
  bool ApplyVectorConstraints(VectorDoubleType &gradE, const PointType &pos) const;
  VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType &gradE, const PointType &pos, int idx) const override;
  PointType UpdateParticlePosition(const PointType &point, int idx, VectorDoubleType &update) const override;

  virtual void InvalidateParticlePosition(int idx) const override;

  double GetCurvature(const PointType &p, int idx) const override {
    // TODO Why not return the actual curvature
    return GetSurfaceMeanCurvature();
  }

  inline double GetSurfaceMeanCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.15;
  }

  inline double GetSurfaceStdDevCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.02;
  }

  const PointType &GetLowerBound() const override { return mesh_wrapper_->GetMeshLowerBound(); }
  const PointType &GetUpperBound() const override { return mesh_wrapper_->GetMeshUpperBound(); }

  PointType GetZeroCrossingPoint() const override {
    // TODO Hong
    // Apply constraints somehow
    if (mesh_wrapper_ == nullptr) {
      // Fixed domain. Unsure if this is the correct thing to do, but it preserves existing behaviour.
      PointType p;
      p[0] = p[1] = p[2] = 0;
      return p;
    }
    return mesh_wrapper_->GetPointOnMesh();
  }

  PointType GetValidLocationNear(PointType p) const override {
    PointType valid;
    valid[0] = p[0];
    valid[1] = p[1];
    valid[2] = p[2];
    ApplyConstraints(valid, -1);
    return valid;
  }

  double GetSurfaceArea() const override {
    // TODO return actual surface area
    return 0;
  }

  double GetMaxDiameter() const override;

  inline vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const override {
    return mesh_wrapper_->SampleNormalAtPoint(point, idx);
  }

  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &point, int idx) const override {
    return mesh_wrapper_->SampleNormalAtPoint(point, idx);
  }

  inline GradNType SampleGradNAtPoint(const PointType &p, int idx) const override {
    return mesh_wrapper_->SampleGradNAtPoint(p, idx);
  }

  inline double Distance(const PointType &a, int idx_a, const PointType &b, int idx_b,
                         vnl_vector_fixed<double, DIMENSION> *out_grad = nullptr) const override {
    return mesh_wrapper_->ComputeDistance(a, idx_a, b, idx_b, out_grad);
  }

  inline double SquaredDistance(const PointType &a, int idx_a, const PointType &b, int idx_b) const override {
    double dist = mesh_wrapper_->ComputeDistance(a, idx_a, b, idx_b);
    return dist * dist;
  }

  inline bool IsWithinDistance(const PointType &a, int idx_a, const PointType &b, int idx_b, double test_dist,
                               double &dist) const override {
    return mesh_wrapper_->IsWithinDistance(a, idx_a, b, idx_b, test_dist, dist);
  }

  void DeleteImages() override {
    // TODO Change this to a generic delete function
  }

  void DeletePartialDerivativeImages() override {
    // TODO Change this to a generic delete function
  }

  void SetMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh_) {
    m_FixedDomain = false;
    mesh_wrapper_ = mesh_;
    sw_mesh_ = std::make_shared<Mesh>(mesh_wrapper_->GetPolydata());
  }

  std::shared_ptr<Mesh> GetSWMesh() const { return sw_mesh_; }

  void UpdateZeroCrossingPoint() override {}

 private:
  std::shared_ptr<MeshWrapper> mesh_wrapper_;
  std::shared_ptr<Mesh> sw_mesh_;
  PointType zero_crossing_point_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-10 at 02:21:13 +0000
