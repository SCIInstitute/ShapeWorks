#pragma once

#include <Eigen/Core>
#include <vector>

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  //! Initialize particle system from a list of paths to particle files
  explicit ParticleSystemEvaluation(const std::vector<std::string>& paths);

  //! Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  explicit ParticleSystemEvaluation(const Eigen::MatrixXd& matrix, int num_values_per_particle = 3);

  //! Get the matrix representation of the particle system
  const Eigen::MatrixXd& get_matrix() const { return matrix_; };

  //! Get the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar)
  int get_num_values_per_particle() const { return num_values_per_particle_; }

  //! Get the paths to the particle files
  const std::vector<std::string>& get_paths() const { return paths_; }

  //! Number of samples
  long num_samples() const { return matrix_.cols(); }

  //! Dimensions (e.g. x/y/z * number of particles)
  long num_dims() const { return matrix_.rows(); }

  //! Perform an exact comparison of two particle systems
  bool exact_compare(const ParticleSystemEvaluation& other) const;

  //! Perform an evaluation comparison of two particle systems
  bool evaluation_compare(const ParticleSystemEvaluation& other) const;

  //! Read a particle file into an Eigen vector
  static bool read_particle_file(std::string filename, Eigen::VectorXd& points);

 private:
  friend struct SharedCommandData;

  //! only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"
  ParticleSystemEvaluation() {}

  Eigen::MatrixXd matrix_;
  std::vector<std::string> paths_;
  int num_values_per_particle_ = 3;  // e.g. 3 for x/y/z, 4 for x/y/z/scalar, 1 for scalar-only
};
}  // namespace shapeworks
