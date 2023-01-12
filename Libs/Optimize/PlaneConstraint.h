#pragma once

#include <vtkSmartPointer.h>

#include <vector>

#include "Constraint.h"

class vtkPlane;

namespace shapeworks {

//! PlaneConstraint
/*!
 * Encapsulate functionality related to plane constraints
 */
class PlaneConstraint : public Constraint {
 public:
  bool isViolated(const Eigen::Vector3d &pt) const override;

  void print() const override;

  //! Get plane normal
  Eigen::Vector3d getPlaneNormal() { return planeNormal_; }

  //! Set plane normal
  void setPlaneNormal(const Eigen::Vector3d &inPlane) { planeNormal_ = inPlane; }

  //! Get plane center point
  Eigen::Vector3d getPlanePoint() { return planePoint_; }
  //! Set plane center point
  void setPlanePoint(const vnl_vector<double> &point) { planePoint_ = Eigen::Vector3d(point[0], point[1], point[2]); }
  //! Set plane center point
  void setPlanePoint(const Eigen::Vector3d &p) { planePoint_ = p; }

  Eigen::Vector3d constraintGradient(const Eigen::Vector3d &pt) const override { return -planeNormal_; }

  std::vector<Eigen::Vector3d> &points() { return points_; };
  double getOffset();
  void setOffset(double offset);

  double constraintEval(const Eigen::Vector3d &pt) const override;

  void updatePlaneFromPoints();

  //! Return this plane as a vtkPlane
  vtkSmartPointer<vtkPlane> getVTKPlane();

 private:
  Eigen::Vector3d planeNormal_;
  Eigen::Vector3d planePoint_;

  std::vector<Eigen::Vector3d> points_;
  double offset_ = 0;
};

}  // namespace shapeworks
