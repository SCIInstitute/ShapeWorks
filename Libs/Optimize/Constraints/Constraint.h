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
  void setZ(double inz) { z_ = inz; }
  double getZ() { return z_; }
  void setMus(std::vector<double> inmu) { mus_ = inmu; }
  std::vector<double> getMus() { return mus_; }
  void setLambda(double inLambda) { lambda_ = inLambda; }
  double getLambda() { return lambda_; }

  virtual Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const = 0;
  virtual double constraintEval(const Eigen::Vector3d &pt) const = 0;

  void updateZ(const Eigen::Vector3d &pt, double C);

  void updateMu(const Eigen::Vector3d &pt, double C, size_t index);

  Eigen::Vector3d lagragianGradient(const Eigen::Vector3d &pt, double C, size_t index) const;

 protected:
  int sgn(double val) { return (double(0) < val) - (val < double(0)); }

  // For augmented lagrangian
  std::vector<double> mus_;
  double z_;
  double lambda_;
};

}  // namespace shapeworks
