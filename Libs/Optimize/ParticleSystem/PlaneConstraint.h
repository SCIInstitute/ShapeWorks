#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"

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
    Eigen::Vector3d plane_normal = plane.head<3>();

    double dist = plane_normal.dot(pt) + plane(3);

    if(dist < 0)return true;
    return false;
  }

  Eigen::Vector4d GetPlane(){return plane;}
  void SetPlane(Eigen::Vector4d inPlane){plane = inPlane;}
  Eigen::Vector3d GetA(){return a;}
  void SetA(const vnl_vector<double> & ina){
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = ina[0]; pt_eigen(1) = ina[1]; pt_eigen(2) = ina[2];
    a = pt_eigen;}
  Eigen::Vector3d GetB(){return b;}
  void SetB(const vnl_vector<double> & inb){
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = inb[0]; pt_eigen(1) = inb[1]; pt_eigen(2) = inb[2];
    b = pt_eigen;}
  Eigen::Vector3d GetC(){return c;}
  void SetC(const vnl_vector<double> & inc){
    Eigen::Vector3d pt_eigen;
    pt_eigen(0) = inc[0]; pt_eigen(1) = inc[1]; pt_eigen(2) = inc[2];
    c = pt_eigen;}

private:
  Eigen::Vector4d plane;
  Eigen::Vector3d a;
  Eigen::Vector3d b;
  Eigen::Vector3d c;
};


}
