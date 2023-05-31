#pragma once

#include <vector>

#include "Constraint.h"
#include "Eigen/Dense"
#include "FreeFormConstraint.h"
#include "Libs/Mesh/Mesh.h"
#include "PlaneConstraint.h"
#include "SphereConstraint.h"
#include "itkPoint.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"

namespace shapeworks {

/**
 * \class Constraints
 * \ingroup Group-Constraints
 *
 * This class contains the set of boundary constraints that are appleid to the shapes in the dataset. They are used to isolate areas of interest on shape surfaces/volumes. These boundary constraints currently take the form of cutting planes, cutting spheres (deprecated), or free-form constraints (FFCs).
 * Do not confuse these with the ApplyConstraints function found in the "domain" code, they refer to the action of snapping to the surface and do not serve to isolate areas of interest.
 *
 */

class Constraints {
 public:
  using Point3 = itk::Point<double, 3>;

  /// Constraints are inactive when initialized
  Constraints() { active_ = false; }

  ~Constraints() {}

  // Set constraints
  /// Adds a cutting-plane boundary constraint. Constraints are used to isolate areas of interest on shape surfaces
  void addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b, const vnl_vector<double> &c);
  /// Adds a sphere boundary constraint. Constraints are used to isolate areas of interest on shape surfaces
  void addSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r);
  /// Adds the free-form boundary constraint, or FFC. Constraints are used to isolate areas of interest on shape surfaces
  void addFreeFormConstraint(std::shared_ptr<shapeworks::Mesh> mesh);

  // Transforms
  /// Applies transformations to free-form constraints (FFCs)
  bool transformConstraints(const vnl_matrix_fixed<double, 4, 4> &transform);
  /// Applies transformations to cutting-plane constraints
  bool transformPlanes(const vnl_matrix_fixed<double, 4, 4> &transform);

  // Is defined? functions
  /// Returns true if at least one cutting plane is defined
  bool isCuttingPlaneDefined() const { return !planeConstraints_.empty(); }
  /// Returns true if at least one sphere constraint is defined
  bool isCuttingSphereDefined() const { return !sphereConstraints_.empty(); }

  // Constraint get function
  /// Returns the vector that constains all plane constraints, of type PlaneConstraint. See class PlaneConstraint for more info
  std::vector<PlaneConstraint> &getPlaneConstraints() { return planeConstraints_; }
  /// Returns the vector that constains all sphere constraints, of type SphereConstraint. See class SphereConstraint for more info
  std::vector<SphereConstraint> &getSphereConstraints() { return sphereConstraints_; }
  /// Returns the free form constraint, of type FreeFormConstraint. See class FreeFormConstraint for more info
  FreeFormConstraint& getFreeformConstraint();

  /// Returns true if any constraint is violated by point pos
  bool isAnyViolated(const Point3 &pos);

  /// Returns true if any cutting-plane constraint is violated by point pt
  std::vector<int> planesViolated(Eigen::Vector3d pt);

  /// Prints all constraints in a neat format. Make sure to disable multithreading if printing within to optimization to avoid jumbled output
  void printAll();

  /// Prints out a comprehensive violation report with each constraint that is violated and the magnitude of such violation by point pos. Index can be 0 if one does not have the index.
  std::string violationReport(const Point3 &pos, size_t index);

  /// Returns the distance to every cosntraint with [0] plane, [1] sphere, and [2] FFC
  std::vector<std::vector<double>> violationReportData(const Point3 &pos);

  // ============================
  // Augmented Lagragian Fuctions
  // ============================
  // Energy gradient computations
  vnl_vector_fixed<double, 3> constraintsGradient(const Point3 &pos) const;

  /// Returns the constraint gradient. prepos is the current particle, pos is the particle with an optimization update applied, C can be 1, and index is the index of the particle.
  vnl_vector_fixed<double, 3> constraintsLagrangianGradient(const Point3 &pos, const Point3 &prepos, double C, size_t index);

  ///Initializes lagrangian parameter mus.
  void InitializeLagrangianParameters(std::vector<double> mus);

  /// Updates mus, the momentum variable of the augmented lagrangian
  void UpdateMus(const Point3 &pos, double C, size_t index);

  /// Gets the variable active, which determines whether constraints are being used
  bool GetActive() { return active_; }
  /// Sets the variable active, which determines whether constraints are being used
  void SetActive(bool ac) { active_ = ac; }

  /// Reads in a constraint json file named filename
  void read(std::string filename);
  /// Writes out a constraint json file named filename
  void write(std::string filename);

  bool hasConstraints();

  /// Clips the mesh that has been passed into it by the constraints stored in this object.
  void clipMesh(Mesh& mesh);

 private:
  /// Vector containing the cutting-plane boundary constraints. Constraints are used to isolate areas of interest on shape surfaces
  std::vector<PlaneConstraint> planeConstraints_;
  /// Vector containing the sphere boundary constraints. Constraints are used to isolate areas of interest on shape surfaces
  std::vector<SphereConstraint> sphereConstraints_;
  /// The object that constains a free-form boundary constraints. Constraints are used to isolate areas of interest on shape surfaces
  FreeFormConstraint freeFormConstraint_;

  /// Determines whether constraints are used in the optimizer
  bool active_;
};

}  // namespace shapeworks
