#include "PlaneConstraint.h"

#include <vtkPlane.h>
#include <vtkTriangle.h>

#include <Eigen/Geometry>
#include <cmath>
#include <iostream>

namespace shapeworks {

//-----------------------------------------------------------------------------
bool PlaneConstraint::isViolated(const Eigen::Vector3d &pt) const {
  double dist = planeNormal_.dot(pt - planePoint_);
  if (dist < 0) {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
void PlaneConstraint::print() const {
  std::cout << "normal " << planeNormal_.transpose() << " point " << planePoint_.transpose() << std::endl;
}

//-----------------------------------------------------------------------------
double PlaneConstraint::getOffset() { return offset_; }

//-----------------------------------------------------------------------------
void PlaneConstraint::setOffset(double offset) { offset_ = offset; }

//-----------------------------------------------------------------------------
double PlaneConstraint::constraintEval(const Eigen::Vector3d &pt) const {
  double val = -planeNormal_.dot(pt - planePoint_);
  return val;
}

//-----------------------------------------------------------------------------
void PlaneConstraint::updatePlaneFromPoints() {
  if (points().size() == 3) {
    planePoint_ = (points_[0] + points_[1] + points_[2]) / 3.0;
    vtkTriangle::ComputeNormal(points_[0].data(), points_[1].data(), points_[2].data(), planeNormal_.data());
  }
}

//-----------------------------------------------------------------------------
void PlaneConstraint::updatePointsFromPlane() {
  double norm = planeNormal_.norm();
  if (norm < 1e-12) {
    // degenerate normal, leave the points unchanged
    return;
  }
  Eigen::Vector3d normal = planeNormal_ / norm;

  // preserve the current triangle size if we have three points, otherwise use a default radius
  double radius = 1.0;
  if (points_.size() == 3) {
    Eigen::Vector3d centroid = (points_[0] + points_[1] + points_[2]) / 3.0;
    radius = ((points_[0] - centroid).norm() + (points_[1] - centroid).norm() + (points_[2] - centroid).norm()) / 3.0;
  }
  if (radius < 1e-6) {
    radius = 1.0;
  }

  // build an orthonormal basis (u, v) spanning the plane such that u x v = normal
  Eigen::Vector3d arbitrary = (std::abs(normal[0]) < 0.9) ? Eigen::Vector3d(1, 0, 0) : Eigen::Vector3d(0, 1, 0);
  Eigen::Vector3d u = normal.cross(arbitrary).normalized();
  Eigen::Vector3d v = normal.cross(u);  // unit length since normal and u are orthonormal

  // equilateral triangle centered at planePoint_ so that its centroid is planePoint_ and its
  // computed normal (see updatePlaneFromPoints) matches planeNormal_
  const double kSqrt3Over2 = 0.8660254037844386;  // sqrt(3)/2
  points_.resize(3);
  points_[0] = planePoint_ + radius * u;
  points_[1] = planePoint_ + radius * (-0.5 * u + kSqrt3Over2 * v);
  points_[2] = planePoint_ + radius * (-0.5 * u - kSqrt3Over2 * v);
}

//-----------------------------------------------------------------------------
void PlaneConstraint::setPlaneCenter(const Eigen::Vector3d &center) {
  // translate the defining points so their centroid becomes the new center (preserves shape/normal)
  if (points_.size() == 3) {
    Eigen::Vector3d centroid = (points_[0] + points_[1] + points_[2]) / 3.0;
    Eigen::Vector3d delta = center - centroid;
    for (auto &point : points_) {
      point += delta;
    }
  }
  planePoint_ = center;
}

//-----------------------------------------------------------------------------
void PlaneConstraint::setPlaneNormalDirection(const Eigen::Vector3d &normal) {
  double norm = normal.norm();
  if (norm < 1e-12) {
    // degenerate target normal, leave the plane unchanged
    return;
  }
  Eigen::Vector3d new_normal = normal / norm;

  if (points_.size() == 3) {
    updatePlaneFromPoints();  // make sure planePoint_/planeNormal_ reflect the current points
    Eigen::Vector3d old_normal = planeNormal_;
    if (old_normal.norm() > 1e-12) {
      // rotate the points about the center so the triangle's normal becomes new_normal (preserves shape)
      Eigen::Quaterniond rotation = Eigen::Quaterniond::FromTwoVectors(old_normal.normalized(), new_normal);
      Eigen::Vector3d center = planePoint_;
      for (auto &point : points_) {
        point = center + rotation * (point - center);
      }
      planeNormal_ = new_normal;
      return;
    }
  }

  // no valid points to rotate (degenerate or missing): rebuild them from the center and new normal
  planeNormal_ = new_normal;
  updatePointsFromPlane();
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPlane> PlaneConstraint::getVTKPlane() {
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetNormal(getPlaneNormal().data());
  plane->SetOrigin(getPlanePoint().data());
  return plane;
}

}  // namespace shapeworks
