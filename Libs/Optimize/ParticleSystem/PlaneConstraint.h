#pragma once

#include <iostream>

#include "Constraint.h"

namespace shapeworks {

class PlaneConstraint : public Constraint {
 public:
  bool isViolated(const Eigen::Vector3d &pt) const override {
    double dist = planeNormal_.dot(pt - planePoint_);
    if (dist < 0) {
      return true;
    }
    return false;
  }

  void print() const override {
    std::cout << "normal " << planeNormal_.transpose() << " point " << planePoint_.transpose() << std::endl;
  }

  Eigen::Vector3d getPlaneNormal() { return planeNormal_; }

  void setPlaneNormal(const Eigen::Vector3d &inPlane) { planeNormal_ = inPlane; }

  Eigen::Vector3d getPlanePoint() { return planePoint_; }

  void setPlanePoint(const vnl_vector<double> &point) {
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = point[0];
    pt_eigen(1) = point[1];
    pt_eigen(2) = point[2];
    planePoint_ = pt_eigen;
  }

  void setPlanePoint(const Eigen::Vector3d &p) { planePoint_ = p; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return -planeNormal_; }

  double constraintEval(const Eigen::Vector3d &pt) const override {
    double val = -planeNormal_.dot(pt - planePoint_);
    return val;
  }

 private:
  Eigen::Vector3d planeNormal_;
  Eigen::Vector3d planePoint_;
};

}  // namespace shapeworks
