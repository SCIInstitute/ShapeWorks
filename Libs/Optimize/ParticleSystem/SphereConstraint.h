#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"

namespace itk
{

class SphereConstraint: public Constraint{
public:

  bool isViolated(const vnl_vector<double> &pt) const {
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = pt[0]; pt_eigen(1) = pt[1]; pt_eigen(2) = pt[2];
    return isViolated(pt_eigen);
  }

  bool isViolated(const Eigen::Vector3d &pt) const {
    double distance_from_center = sqrt(pow(pt(0)-center(0),2)+
                                        pow(pt(1)-center(1),2)+
                                        pow(pt(2)-center(2),2));

    if(distance_from_center < radius) return true;
    return false;
  }

  void printC() const{
      std::cout << "radius " << radius << " center " << center.transpose() << std::endl;
  }

  Eigen::Vector3d GetCenter(){return center;}
  void SetCenter(Eigen::Vector3d inCenter){center = inCenter;}
  double GetRadius(){return radius;}
  void SetRadius(double inRadius){radius = inRadius;}

private:
  double radius;
  Eigen::Vector3d center;
};


}
