#pragma once

#include <Eigen/Core>
#include <vector>

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  //! Initialize particle system from a list of paths to particle files
  explicit ParticleSystemEvaluation(const std::vector<std::string>& paths);

  //! Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  explicit ParticleSystemEvaluation(const Eigen::MatrixXd& matrix);

  //! Get the matrix representation of the particle system
  const Eigen::MatrixXd& get_matrix() const { return matrix_; };

  //! Get the paths to the particle files
  const std::vector<std::string>& get_paths() const { return paths_; }

  //! Number of samples
  int num_samples() const { return matrix_.cols(); }

  //! Dimensions (e.g. x/y/z * number of particles)
  int num_dims() const { return matrix_.rows(); }

  //! Perform an exact comparison of two particle systems
  bool exact_compare(const ParticleSystemEvaluation& other) const;

  //! Perform an evaluation comparison of two particle systems
  bool evaluation_compare(const ParticleSystemEvaluation& other) const;

  //! Read a particle file
  static bool read_particle_file(std::string filename, Eigen::VectorXd& points);

 private:
  friend struct SharedCommandData;

  ParticleSystemEvaluation() {
  }  // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd matrix_;
  std::vector<std::string> paths_;
};
}  // namespace shapeworks
