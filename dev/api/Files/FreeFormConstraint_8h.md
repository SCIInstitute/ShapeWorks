---
title: Libs/Optimize/Constraints/FreeFormConstraint.h

---

# Libs/Optimize/Constraints/FreeFormConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md)**  |




## Source code

```cpp
#pragma once

#include "Constraint.h"
#include "Libs/Mesh/Mesh.h"

class vtkFloatArray;

namespace shapeworks {

class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  bool readyForOptimize() const;

  bool isViolated(const Eigen::Vector3d& pt) const override;

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d& pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d& pt) const override { return mesh_->getFFCValue(pt); }

  void setDefinition(vtkSmartPointer<vtkPolyData> polyData);

  vtkSmartPointer<vtkPolyData> getDefinition() { return definitionPolyData_; };

  void applyToPolyData(vtkSmartPointer<vtkPolyData> polyData);

  std::vector<std::vector<Eigen::Vector3d>>& boundaries();

  Eigen::Vector3d getQueryPoint() { return queryPoint_; };

  void setQueryPoint(Eigen::Vector3d queryPoint) { queryPoint_ = queryPoint; };

  void computeBoundaries();

  void setInoutPolyData(vtkSmartPointer<vtkPolyData> polyData) { inoutPolyData_ = polyData; }

  vtkSmartPointer<vtkPolyData> getInoutPolyData() { return inoutPolyData_; };

  void createInoutPolyData();

  bool isSet();

  void setPainted(bool painted);

  void reset();

  void computeGradientFields(std::shared_ptr<Mesh> mesh);

  void convertLegacyFFC(vtkSmartPointer<vtkPolyData> polyData);


 private:

  vtkSmartPointer<vtkFloatArray> computeInOutForFFCs(vtkSmartPointer<vtkPolyData> polyData, Eigen::Vector3d query,
                                                     vtkSmartPointer<vtkPolyData> halfmesh);

  std::vector<Eigen::Matrix3d> setGradientFieldForFFCs(std::shared_ptr<Mesh> mesh,
                                                       vtkSmartPointer<vtkDoubleArray> absvalues, Eigen::MatrixXd V,
                                                       Eigen::MatrixXi F);

  vtkFloatArray* getInOutScalars();
  vtkFloatArray* createFFCPaint(vtkSmartPointer<vtkPolyData> polyData);

  std::shared_ptr<shapeworks::Mesh> mesh_;

  vtkSmartPointer<vtkPolyData> definitionPolyData_;
  bool painted_ = false;

  std::vector<std::vector<Eigen::Vector3d>> boundaries_;
  Eigen::Vector3d queryPoint_;
  vtkSmartPointer<vtkPolyData> inoutPolyData_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-08-08 at 20:29:41 +0000
