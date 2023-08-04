#pragma once

#include <Particles/ParticleSystemEvaluation.h>

#include <memory>

namespace shapeworks {

//! Evaluation of particle using surface normals to determine which particles have inconsistent normals across subjects
/*!
 * The ParticleNormalEvaluation class evaluates the surface normals at each particle and determines if they are
 * consistent across subjects (e.g. good particles), or inconsistent across subject (e.g. bad particles).
 *
 */

class VtkMeshWrapper;
class ParticleNormalEvaluation {
 public:
  static std::vector<double> evaluate_particle_normals(const Eigen::MatrixXd& particles,
                                                       const Eigen::MatrixXd& normalss);

  static std::vector<bool> threshold_particle_normals(std::vector<double> angles, double max_angle_degrees);

  //! Compute normals at particle positions
  static Eigen::MatrixXd compute_particle_normals(const Eigen::MatrixXd& particles,
                                                  std::vector<std::shared_ptr<VtkMeshWrapper>> meshes);

 private:
};

}  // namespace shapeworks
