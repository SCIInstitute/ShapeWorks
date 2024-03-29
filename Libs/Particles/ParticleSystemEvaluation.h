#pragma once

#include <Eigen/Core>
#include <vector>

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  ParticleSystemEvaluation(const std::vector<std::string>& paths);

  // Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  ParticleSystemEvaluation(const Eigen::MatrixXd& matrix);

  const Eigen::MatrixXd& Particles() const { return matrix_; };

  const std::vector<std::string>& Paths() const { return paths_; }

  //! Number of samples
  int N() const { return matrix_.cols(); }

  //! Dimensions (e.g. x/y/z * number of particles)
  int D() const { return matrix_.rows(); }

  bool ExactCompare(const ParticleSystemEvaluation& other) const;

  bool EvaluationCompare(const ParticleSystemEvaluation& other) const;

  static bool ReadParticleFile(std::string filename, Eigen::VectorXd& points);

 private:
  friend struct SharedCommandData;

  ParticleSystemEvaluation() {
  }  // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd matrix_;
  std::vector<std::string> paths_;
};
}  // namespace shapeworks
