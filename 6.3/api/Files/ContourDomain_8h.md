---
title: Libs/Optimize/ParticleSystem/ContourDomain.h

---

# Libs/Optimize/ParticleSystem/ContourDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ContourDomain](../Classes/classshapeworks_1_1ContourDomain.md)**  |




## Source code

```cpp
#pragma once

#include <itkObjectFactory.h>
#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkLine.h>
#include <vtkPolyData.h>

#include <Eigen/Dense>

#include "ParticleDomain.h"

namespace shapeworks {

class ContourDomain : public ParticleDomain {
 public:
  using Pointer = std::shared_ptr<ContourDomain>;

  explicit ContourDomain() {}
  virtual ~ContourDomain() {}

  void SetPolyLine(vtkSmartPointer<vtkPolyData> poly_data);

  DomainType GetDomainType() const override { return DomainType::Contour; }

  virtual bool ApplyConstraints(PointType& p, int idx, bool dbg = false) const override;

  virtual PointType UpdateParticlePosition(const PointType& point, int idx, VectorDoubleType& update) const override;

  virtual VectorDoubleType ProjectVectorToSurfaceTangent(VectorDoubleType& gradE, const PointType& pos,
                                                         int idx) const override;

  virtual VectorFloatType SampleNormalAtPoint(const PointType& point, int idx) const override {
    throw std::runtime_error("Contours do not have normals");
  }

  virtual VectorFloatType SampleGradientAtPoint(const PointType& point, int idx) const override {
    throw std::runtime_error("Contours do not have gradients");
  }

  virtual GradNType SampleGradNAtPoint(const PointType& p, int idx) const override {
    throw std::runtime_error("Contours do not have gradient of normals");
  }

  virtual PointType GetValidLocationNear(PointType p) const override {
    this->ApplyConstraints(p, -1);
    return p;
  }

  virtual double GetMaxDiameter() const override {
    // todo copied from MeshDomain: should this not be the length of the bounding box diagonal?
    const PointType bb = upper_bound_ - lower_bound_;
    return std::max({bb[0], bb[1], bb[2]});
  }

  virtual void UpdateZeroCrossingPoint() override {}

  double GetCurvature(const PointType& p, int idx) const override { return GetSurfaceMeanCurvature(); }

  virtual double GetSurfaceMeanCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.15;
  }

  virtual double GetSurfaceStdDevCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.02;
  }

  double Distance(const PointType& a, int idx_a, const PointType& b, int idx_b,
                  VectorDoubleType* out_grad = nullptr) const override;

  double SquaredDistance(const PointType& a, int idx_a, const PointType& b, int idx_b) const override;

  const PointType& GetLowerBound() const override { return lower_bound_; }

  const PointType& GetUpperBound() const override { return upper_bound_; }

  PointType GetZeroCrossingPoint() const override {
    PointType out;
    double dist;
    int closest_line = GetLineForPoint(upper_bound_.GetDataPointer(), -1, dist, out.GetDataPointer());
    return out;
  }

  double GetSurfaceArea() const override { throw std::runtime_error("Contours do not have area"); }

  void DeleteImages() override {
    // TODO what?
  }
  void DeletePartialDerivativeImages() override {
    // TODO what?
  }

  void InvalidateParticlePosition(int idx) const override;

  PointType GetPositionAfterSplit(const PointType& pt, const VectorDoubleType& local_direction,
                                  const VectorDoubleType& global_direction, double epsilon) const override;

 private:
  void ComputeBounds();
  void ComputeGeodesics(vtkSmartPointer<vtkPolyData> poly_data);

  int GetLineForPoint(const double pt[3], int idx, double& closest_distance, double closest_pt[3]) const;
  double ComputeLineCoordinate(const double pt[3], int line) const;

  // Return the number of lines that consist of i-th point
  int NumberOfLinesIncidentOnPoint(int i) const;

  PointType GeodesicWalk(const PointType& start_pt, int idx, const Eigen::Vector3d& update_vec) const;

  int NumberOfLines() const;
  int NumberOfPoints() const;

  Eigen::Vector3d GetPoint(int id) const;

  PointType lower_bound_, upper_bound_;

  vtkSmartPointer<vtkPolyData> poly_data_;
  vtkSmartPointer<vtkCellLocator> cell_locator_;
  std::vector<vtkSmartPointer<vtkLine>> lines_;

  // Geodesics between all point pairs. Assumes the number of points is very small
  Eigen::MatrixXd geodesics_;

  // cache which line a particle is on
  mutable std::vector<int> particle_lines_;
  // store some information about the last geodesic query. The next one will most likely reuse this
  mutable int geo_lq_idx_ = -1;
  mutable int geo_lq_line_ = -1;
  mutable double geo_lq_dist_ = -1;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-03-29 at 17:35:40 -0600
