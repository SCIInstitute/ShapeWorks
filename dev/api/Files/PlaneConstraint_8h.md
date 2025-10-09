---
title: Libs/Optimize/Constraints/PlaneConstraint.h

---

# Libs/Optimize/Constraints/PlaneConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PlaneConstraint](../Classes/classshapeworks_1_1PlaneConstraint.md)**  |




## Source code

```cpp
#pragma once

#include <vtkSmartPointer.h>

#include <vector>

#include "Libs/Optimize/Constraints/Constraint.h"

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
  double offset_ = 0;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-09 at 04:04:23 +0000
