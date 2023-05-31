#pragma once

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vector>

#include <Eigen/Core>

#include "ConstraintType.h"

namespace shapeworks {

/**
 * \class Constraint
 * \ingroup Group-Constraints
 *
 *  This class is the general constraint class. Each instance represents a single constraint, either cutting-plane, sphere or free-form. They all inherit from this class. This class containts all the infrastructure to handle gradients and evaluations, which is shared among all constraint types.
 *  NOTE: Not actually using the augmented lagrangian. We are using quadratic penalty and not lagrangian because it works better.
 *
 */

class Constraint {
 public:
  /// Returns if pt in vnl_vector format is violated by the constraint
  bool isViolated(const vnl_vector<double> &pt) const { return isViolated(Eigen::Vector3d(pt[0], pt[1], pt[2])); }
  /// Returns if pt in Eigen format is violated by the constraint
  virtual bool isViolated(const Eigen::Vector3d &pt) const = 0;
  /// Prints the constraint neatly
  virtual void print() const = 0;

  // For augmented lagrangian
  /// Initializes mu
  void setMus(std::vector<double> inmu) { mus_ = inmu; }
  /// Gets mu
  std::vector<double> getMus() { return mus_; }

  /// Returns the gradient of the constraint
  virtual Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const = 0;
  /// Returns the evaluation on the constraint, i.e. the signed distance to the constraint boundary
  virtual double constraintEval(const Eigen::Vector3d &pt) const = 0;

  /// Updates the value of mu according to the augmented lagrangian update
  void updateMu(const Eigen::Vector3d &pt, double C, size_t index);

  /// Computes the lagrangian gradient based on lagrangian inequality equations. NOTE: Not actually lagrangian. We are using quadratic penalty and not lagrangian because it works better.
  Eigen::Vector3d lagragianGradient(const Eigen::Vector3d &pt, double C, size_t index) const;

 protected:
  /// Returns the sign of the double
  int sgn(double val) { return (double(0) < val) - (val < double(0)); }

  // For augmented lagrangian
  /// Mu is the lagrangian momentum term
  std::vector<double> mus_;
};

}  // namespace shapeworks
