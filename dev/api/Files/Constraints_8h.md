---
title: Libs/Optimize/Constraints/Constraints.h

---

# Libs/Optimize/Constraints/Constraints.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Constraints](../Classes/classshapeworks_1_1Constraints.md)**  |




## Source code

```cpp
#pragma once

#include <vector>

#include "Constraint.h"
#include "Eigen/Dense"
#include "FreeFormConstraint.h"
#include "Libs/Mesh/Mesh.h"
#include "PlaneConstraint.h"
#include "itkPoint.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"

namespace shapeworks {

/* Constraints Roadmap
 * For future tinkerers, this is a guide to all the code relevant to constraints to help you navigate the optimizer and change things if desired. In simple terms,
 *      constraints define areas of interest by getting violation regions to repel particles by attaching a mechanism to all gradient updates that push them back
 *      if a violation does occur. "Look up quadratic penalty method" for more information.
 *
 * - Reading json constraints: From Libs/Optimize/OptimizeParameters::set_up_optimize -> Libs/Optimize/Constraints/Constraints::read
 *
 * - Initialization: The various steps in the constraints initialization are
 *          +Libs/Optimize/Optimize::Initialize and ParticleSystem::AdvancedAllParticleSplitting initialize quadratic penalty mus.
 *          +Libs/Optimize/Domain/{Image,Contour,Mesh}Domain determine the picking of the initial particle position to be optimized which is assured not to violate any constraint here
 *          +Libs/Optimize/Sampler::AllocateDomainsAndNeighborhoods sets the constraints within the constraints class via Libs/Optimize/Constraints/Constraints::addSphere and addPlane, and initializes FFCs via Sampler::initialize_ffcs -> Constraints::addFreeFormConstraint
 *          +Libs/Optimize/Sampler::initialize_ffcs also computes the gradient fields to allow distance and gradient queries
 *
 * - Optimization: For optimization, the relevant constraints code is in Libs/Optimize/GradientDescentOptimizer::StartAdaptiveGaussSeidelOptimization in Step B,
 *      where it calls GradientDescentOptimizer::AugmentedLagrangianConstraints to tweak any updates by our constraints class corrections to not violate constraints.
 *      See constraints class.
 *
 * - Constraints class: This class implements a quadratic penalty style optimizer for the entire ShapeWorks particle system to apply boundary constraints.
 *      This Libs/Optimize/Constraints/ directory contains the following files
 *          +Constraints: Contains the processing of multiple interacting constraints and computing gradients for the optimizer. Constains all constraints for a full particle system (multiple shapes/domains).
 *          +Constraint: A general class for any type of constraint. Contains only one constraint, but has quadratic penalty equations to facilitate any single-constraint operations.
 *              *PlaneConstraint: Cutting plane constraints that use the equation of a plane to compute distances and gradients.
 *              *SphereConstraint(deprecated): It uses the equation of a sphere.
 *              *Free-form constraints: Use a signed geodesic mesh field to represent regions of interest of arbitrary shape, see below.
 *
 * - Mesh fields used for FFCs: FFCs use mesh field representations which are of class shapeworks::Mesh, located in Libs/Mesh/Mesh. Within Libs/Mesh/Mesh, the relevant functions are
 *          +Mesh::clip: Clips by cutting plane
 *          +Mesh::clipByField: Clips by a value field
 *          +Mesh::getFFCValue: Allows getting the shortest signed geodesic distance of a point to any boundary
 *          +Mesh::getFFCGradient: Allows getting the direction to the boundary. This might be the opposite direction for violated
 *
 * - The Parameter mesh_ffc_mode is exposed through Libs/Optimize/OptimizeParameters and are passed to Libs/Optimize/Optimize
 *          +mesh_ffc_mode: when running on meshes, 0 is for mesh clipping (default) and 1 is for the quadratic penalty
 *
 * - DEPRECATED: Reading xml constraints: Constraints from xmls are read via Libs/Optimize/OptimizeParameterFile::read_cutting_planes and read_cutting_spheres,
 *      then go through Optimize.cpp::SetCuttingPlane -> Sampler->SetCuttingPlane and Libs/Optimize/Constraints/Constraints::addPlane. Same for spheres and FFCs (both deprecated).
 *
*/

class Constraints {
 public:
  using Point3 = itk::Point<double, 3>;

  Constraints() { active_ = false; }

  ~Constraints() {}

  // Set constraints
  void addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b, const vnl_vector<double> &c);
  void addFreeFormConstraint(std::shared_ptr<shapeworks::Mesh> mesh);

  // Transforms
  bool transformConstraints(const vnl_matrix_fixed<double, 4, 4> &transform);
  bool transformPlanes(const vnl_matrix_fixed<double, 4, 4> &transform);

  // Constraint get function
  std::vector<PlaneConstraint> &getPlaneConstraints() { return planeConstraints_; }
  FreeFormConstraint& getFreeformConstraint();

  bool isAnyViolated(const Point3 &pos);

  void printAll();

  std::string violationReport(const Point3 &pos, size_t index);

  std::vector<std::vector<double>> violationReportData(const Point3 &pos);

  // ============================
  // Augmented Lagragian Fuctions
  // ============================

  vnl_vector_fixed<double, 3> constraintsLagrangianGradient(const Point3 &pos, const Point3 &prepos, double C, size_t index);

  void InitializeLagrangianParameters(std::vector<double> mus);

  void UpdateMus(const Point3 &pos, double C, size_t index);

  bool GetActive() { return active_; }
  void SetActive(bool ac) { active_ = ac; }

  void read(std::string filename);
  void write(std::string filename);

  bool hasConstraints();

  void clipMesh(Mesh& mesh);

 private:
  std::vector<PlaneConstraint> planeConstraints_;
  FreeFormConstraint freeFormConstraint_;

  bool active_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-13 at 17:18:12 +0000
