#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"

namespace itk
{

class FreeFormConstraint: public Constraint{
public:

  bool isViolated(const vnl_vector<double> &pt) const {return false;}

  bool isViolated(const Eigen::Vector3d &pt) const {return true;}

  void printC() const{
      std::cout << "FF" << std::endl;
  }

  Eigen::Vector3d ConstraintGradient(const Eigen::Vector3d &pt) const{
      Eigen::Vector3d grad;
      return grad;
    }

    double ConstraintEval(const Eigen::Vector3d &pt) const{
      double val;
      return val;
    }

private:

};


}
