#pragma once

#include <iostream>

#include "Libs/Optimize/Constraints/Constraint.h"

namespace shapeworks {

/**
 * \class SphereConstraint
 * \ingroup Group-Constraints
 *
 *  Encapsulate functionality related to plane constraints. Inherits from Constraint. See Constraint.h for more information
 *
 */

class SphereConstraint : public Constraint {
 public:
  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override { std::cout << "radius " << radius_ << " center " << center_.transpose() << std::endl; }

  Eigen::Vector3d getCenter() { return center_; }
  void setCenter(Eigen::Vector3d inCenter) { center_ = inCenter; }

  double getRadius() { return radius_; }
  void setRadius(double inRadius) { radius_ = inRadius; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override {
    Eigen::Vector3d grad = (pt - center_) / (pt - center_).norm();
    return -grad;
  }

  Eigen::Vector3d constraintGradientSphere(const Eigen::Vector3d &pt, const Eigen::Vector3d &updpt) const;

  double constraintEval(const Eigen::Vector3d &pt) const override {
    double val = (pt - center_).norm() - radius_;
    return -val;
  }

  double constraintEvalSphere(const Eigen::Vector3d &pt, const Eigen::Vector3d &updpt) const;

  Eigen::Vector3d lagragianGradient(const Eigen::Vector3d &pt, const Eigen::Vector3d &updpt, double C, size_t index) const;

 private:
  double radius_;
  Eigen::Vector3d center_;
};

}  // namespace shapeworks
