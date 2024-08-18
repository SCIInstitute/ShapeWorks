---
title: Libs/Optimize/Constraints/Constraint.h

---

# Libs/Optimize/Constraints/Constraint.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Constraint](../Classes/classshapeworks_1_1Constraint.md)**  |




## Source code

```cpp
#pragma once

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vector>

#include <Eigen/Core>

#include "ConstraintType.h"

namespace shapeworks {

class Constraint {
 public:
  bool isViolated(const vnl_vector<double> &pt) const { return isViolated(Eigen::Vector3d(pt[0], pt[1], pt[2])); }
  virtual bool isViolated(const Eigen::Vector3d &pt) const = 0;
  virtual void print() const = 0;

  // For augmented lagrangian
  void setMus(std::vector<double> inmu) { mus_ = inmu; }
  std::vector<double> getMus() { return mus_; }

  virtual Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const = 0;
  virtual double constraintEval(const Eigen::Vector3d &pt) const = 0;

  void updateMu(const Eigen::Vector3d &pt, double C, size_t index);

  Eigen::Vector3d lagragianGradient(const Eigen::Vector3d &pt, double C, size_t index) const;

 protected:
  int sgn(double val) { return (double(0) < val) - (val < double(0)); }

  // For augmented lagrangian
  std::vector<double> mus_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-08-18 at 15:46:15 +0000
