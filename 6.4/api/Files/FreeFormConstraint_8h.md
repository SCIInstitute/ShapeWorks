---
title: Libs/Optimize/ParticleSystem/FreeFormConstraint.h

---

# Libs/Optimize/ParticleSystem/FreeFormConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md)** <br>[FreeFormConstraint]().  |




## Source code

```cpp
#pragma once

#include <Libs/Mesh/Mesh.h>

#include "Constraint.h"

class vtkFloatArray;

namespace shapeworks {


class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  bool readyForOptimize() const;

  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d &pt) const override { return mesh_->getFFCValue(pt); }

  void setDefinition(vtkSmartPointer<vtkPolyData> polyData);

  vtkSmartPointer<vtkPolyData> getDefinition() { return definitionPolyData_; };

  void applyToPolyData(vtkSmartPointer<vtkPolyData> polyData);

  std::vector<std::vector<Eigen::Vector3d>> &boundaries();

  Eigen::Vector3d getQueryPoint() { return queryPoint_; };

  void setQueryPoint(Eigen::Vector3d queryPoint) { queryPoint_ = queryPoint; };

  void computeBoundaries();

  bool isSet();

  void setPainted(bool painted);

  void reset();

 private:

  vtkFloatArray *createFFCPaint(vtkSmartPointer<vtkPolyData> polyData);

  std::shared_ptr<shapeworks::Mesh> mesh_;

  vtkSmartPointer<vtkPolyData> definitionPolyData_;
  bool painted_ = false;

  std::vector<std::vector<Eigen::Vector3d>> boundaries_;
  Eigen::Vector3d queryPoint_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-22 at 21:16:04 +0000