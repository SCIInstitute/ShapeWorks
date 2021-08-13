#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"
#include <vector>

#include "Mesh.h"

namespace itk
{

class FreeFormConstraint: public Constraint{
public:
    FreeFormConstraint(shapeworks::Mesh mesh) : mesh(mesh){}

  bool isViolated(const vnl_vector<double> &pt) const {return false;}

  bool isViolated(const Eigen::Vector3d &pt) const {return true;}

  void printC() const{
      std::cout << "FF" << std::endl;
  }

  Eigen::Vector3d ConstraintGradient(const Eigen::Vector3d &pt) const{
      return mesh.GetFFCGradient(pt);
    }

    double ConstraintEval(const Eigen::Vector3d &pt) const{
      return mesh.GetFFCValue(pt);
    }

private:
    shapeworks::Mesh & mesh;
};


}
