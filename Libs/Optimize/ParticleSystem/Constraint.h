#pragma once

#include <vnl_vector.h>

#include <iostream>

#include "ConstraintType.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"

namespace shapeworks {

class Constraint {
 public:
  bool isViolated(const vnl_vector<double> &pt) const { return isViolated(Eigen::Vector3d(pt[0], pt[1], pt[2])); }
  virtual bool isViolated(const Eigen::Vector3d &pt) const = 0;
  virtual void print() const = 0;
  shapeworks::ConstraintType getConstraintType() { return type_; }
  void setConstraintType(shapeworks::ConstraintType inType) { type_ = inType; }

  // For augmented lagrangian
  void setZ(double inz) { z_ = inz; }
  double getZ() { return z_; }
  void setMu(double inmu) { mu_ = inmu; }
  double getMu() { return mu_; }
  void setLambda(double inLambda) { lambda_ = inLambda; }
  double getLambda() { return lambda_; }

  virtual Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const = 0;
  virtual double constraintEval(const Eigen::Vector3d &pt) const = 0;

  void updateZ(const Eigen::Vector3d &pt, double C);

  void updateMu(const Eigen::Vector3d &pt, double C);

  Eigen::Vector3d lagragianGradient(const Eigen::Vector3d &pt, double C) const;

 protected:
  shapeworks::ConstraintType type_;

  int sgn(double val) { return (double(0) < val) - (val < double(0)); }

  // For augmented lagrangian
  double mu_;
  double z_;
  double lambda_;
};

}  // namespace shapeworks
