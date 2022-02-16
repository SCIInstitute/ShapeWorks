#pragma once

#include <vector>

#include "Constraint.h"
#include "Eigen/Dense"
#include "FreeFormConstraint.h"
#include "Mesh.h"
#include "PlaneConstraint.h"
#include "SphereConstraint.h"
#include "itkPoint.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"

namespace shapeworks {

class Constraints {
 public:
  using Point3 = itk::Point<double, 3>;

  Constraints() { active_ = false; }

  ~Constraints() {}

  // Set constraints
  void addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b, const vnl_vector<double> &c);
  void addSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r);
  void addFreeFormConstraint(std::shared_ptr<shapeworks::Mesh> mesh);

  // Transforms
  bool transformConstraints(const vnl_matrix_fixed<double, 4, 4> &transform);
  bool transformPlanes(const vnl_matrix_fixed<double, 4, 4> &transform);

  // Apply functions
  std::stringstream applyBoundaryConstraints(vnl_vector_fixed<double, 3> &gradE, const Point3 &pos);
  std::stringstream applyBoundaryConstraints(vnl_vector_fixed<float, 3> &gradE, const Point3 &pos);
  std::stringstream applyPlaneConstraints(vnl_vector_fixed<double, 3> &gradE, const Point3 &pos);

  // Write constraints
  bool writePlanes(std::string filename) { return true; }
  bool writeSpheres(std::string filename) { return true; }
  bool writeFreeFormConstraint(std::string filename) { return true; }

  // Is defined? functions
  bool isCuttingPlaneDefined() const { return !planeConstraints_.empty(); }
  bool isCuttingSphereDefined() const { return !sphereConstraints_.empty(); }

  // Plane constraint
  std::vector<PlaneConstraint> &getPlaneConstraints() { return planeConstraints_; }
  std::vector<SphereConstraint> &getSphereConstraints() { return sphereConstraints_; }

  // Is any constraint violated by point pos?
  bool isAnyViolated(const Point3 &pos);

  // Constraint violations
  std::vector<int> planesViolated(Eigen::Vector3d pt);

  void printAll();

  std::string violationReport(const Point3 &pos);

  std::vector<std::vector<double>> violationReportData(const Point3 &pos);

  // ============================
  // Augmented Lagragian Fuctions
  // ============================
  // Energy gradient computations
  vnl_vector_fixed<double, 3> constraintsGradient(const Point3 &pos) const;

  // Lagragian gradient computation
  vnl_vector_fixed<double, 3> constraintsLagrangianGradient(const Point3 &pos, const Point3 &prepos, double C);

  // Parameters lambda, mu and z initialization
  void InitializeLagrangianParameters(double lambda, double mu, double z);

  void UpdateZs(const Point3 &pos, double C);

  void UpdateMus(const Point3 &pos, double C);

  bool GetActive() { return active_; }
  void SetActive(bool ac) { active_ = ac; }

 protected:
  std::vector<PlaneConstraint> planeConstraints_;
  std::vector<SphereConstraint> sphereConstraints_;
  std::vector<FreeFormConstraint> freeFormConstraints_;

 private:
  // Projections and intersects
  bool active_;
  Eigen::Vector3d projectOntoLine(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d p);
  Eigen::Vector3d linePlaneIntersect(Eigen::Vector3d n, Eigen::Vector3d p0, Eigen::Vector3d l0, Eigen::Vector3d l);
  bool PlanePlaneIntersect(Eigen::Vector3d n1, Eigen::Vector3d p1, Eigen::Vector3d n2, Eigen::Vector3d p2,
                           Eigen::Vector3d &l0_result, Eigen::Vector3d &l1_result);
};

}  // namespace shapeworks
