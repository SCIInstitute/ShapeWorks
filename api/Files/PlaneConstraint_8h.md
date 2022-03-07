---
title: Libs/Optimize/ParticleSystem/PlaneConstraint.h

---

# Libs/Optimize/ParticleSystem/PlaneConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md)**  |




## Source code

```cpp
#pragma once

#include <vector>

#include "Constraint.h"

#include <vtkSmartPointer.h>

class vtkPlane;

namespace shapeworks {

class PlaneConstraint : public Constraint {
 public:
  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override;

  Eigen::Vector3d getPlaneNormal() { return planeNormal_; }

  void setPlaneNormal(const Eigen::Vector3d &inPlane) { planeNormal_ = inPlane; }

  Eigen::Vector3d getPlanePoint() { return planePoint_; }
  void setPlanePoint(const vnl_vector<double> &point) { planePoint_ = Eigen::Vector3d(point[0], point[1], point[2]); }
  void setPlanePoint(const Eigen::Vector3d &p) { planePoint_ = p; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return -planeNormal_; }

  std::vector<Eigen::Vector3d> &points() { return points_; };
  double getOffset();
  void setOffset(double offset);

  double constraintEval(const Eigen::Vector3d &pt) const override;

  void updatePlaneFromPoints();

  vtkSmartPointer<vtkPlane> getVTKPlane();

 private:
  Eigen::Vector3d planeNormal_;
  Eigen::Vector3d planePoint_;

  std::vector<Eigen::Vector3d> points_;
  double offset_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-03-07 at 00:21:28 +0000
