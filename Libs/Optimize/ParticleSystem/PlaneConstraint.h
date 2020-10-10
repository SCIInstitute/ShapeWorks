#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"
#include <iostream>

namespace itk
{

class PlaneConstraint: public Constraint{
public:

  bool isViolated(const vnl_vector<double> &pt) const{
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = pt[0]; pt_eigen(1) = pt[1]; pt_eigen(2) = pt[2];
    return isViolated(pt_eigen);
  }

  bool isViolated(const Eigen::Vector3d &pt) const{
    double dist = planeNormal.dot(pt-planePoint);
    if(dist < 0)return true;
    return false;
  }

  void printC() const{
      std::cout << "normal " << planeNormal.transpose() << " point " << planePoint.transpose() << std::endl;
  }

  Eigen::Vector3d GetPlaneNormal(){return planeNormal;}
  void SetPlaneNormal(const Eigen::Vector3d& inPlane){planeNormal = inPlane;}
  Eigen::Vector3d GetPlanePoint(){return planePoint;}
  void SetPlanePoint(const vnl_vector<double> & ina){
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = ina[0]; pt_eigen(1) = ina[1]; pt_eigen(2) = ina[2];
    planePoint = pt_eigen;
  }
  void SetPlanePoint(const Eigen::Vector3d & p){planePoint = p;}

private:
  Eigen::Vector3d planeNormal;
  Eigen::Vector3d planePoint;
};


}
