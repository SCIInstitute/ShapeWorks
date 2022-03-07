#pragma once

#include <Libs/Mesh/Mesh.h>

#include "Constraint.h"

namespace shapeworks {

class FreeFormConstraint : public Constraint {
 public:
  FreeFormConstraint() {}

  void setMesh(std::shared_ptr<shapeworks::Mesh> mesh) { mesh_ = mesh; }

  std::shared_ptr<shapeworks::Mesh> getMesh() { return mesh_; }

  bool isViolated(const Eigen::Vector3d &pt) const override {
    if (constraintEval(pt) >= 0) {
      return false;
    } else {
      return true;
    }
  }

  void print() const override { std::cout << "FF" << std::endl; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return mesh_->getFFCGradient(pt); }

  double constraintEval(const Eigen::Vector3d &pt) const override { return mesh_->getFFCValue(pt); }

 private:
  std::shared_ptr<shapeworks::Mesh> mesh_;
};

}  // namespace shapeworks
