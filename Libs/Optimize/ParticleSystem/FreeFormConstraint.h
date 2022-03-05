#pragma once

#include <Libs/Mesh/Mesh.h>

#include "Constraint.h"

namespace shapeworks {

class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d &pt) const override { return mesh_->getFFCValue(pt); }

  //! Set polydata where per-vertex free form constraint definition exists
  void setDefinition(vtkSmartPointer<vtkPolyData> polyData) { definition_ = polyData; };

  //! Get polydata where per-vertex free form constraint definition exists
  vtkSmartPointer<vtkPolyData> getDefinition() { return definition_; };

 private:
  std::shared_ptr<shapeworks::Mesh> mesh_;

  vtkSmartPointer<vtkPolyData> definition_;
};

}  // namespace shapeworks
