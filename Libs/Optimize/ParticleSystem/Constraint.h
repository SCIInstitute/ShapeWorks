#pragma once

#include "ConstraintType.h"
#include <vnl_vector.h>
#include "vnl/vnl_math.h"
#include "Eigen/Core"

namespace itk
{

class Constraint{
public:

  virtual bool isViolated(const vnl_vector<double> &pt) const = 0;
  virtual bool isViolated(const Eigen::Vector3d &pt) const = 0;
  shapeworks::ConstraintType GetConstraintType(){ return type; }
  void setConstraintType(shapeworks::ConstraintType inType){ type = inType;}

private:
  shapeworks::ConstraintType type;
};


}
