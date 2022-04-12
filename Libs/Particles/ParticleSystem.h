#pragma once

#include <vector>
#include <Eigen/Core>

#include "itkParticlePositionReader.h"
#include "Shapeworks.h"

namespace shapeworks {

class ParticleSystem 
{
public:
  ParticleSystem(const std::vector<std::string> &paths);

  // Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  ParticleSystem(const Eigen::MatrixXd &matrix);

  const Eigen::MatrixXd &Particles() const
  {
    return P;
  };

  const std::vector<std::string> &Paths() const
  {
    return paths;
  }

  //! Number of samples
  int N() const
  {
    return P.cols();
  }

  //! Dimensions (e.g. x/y/z * number of particles)
  int D() const
  {
    return P.rows();
  }

  bool ExactCompare(const ParticleSystem& other) const;

  bool EvaluationCompare(const ParticleSystem& other) const;

  static bool ReadParticleFile(std::string filename, Eigen::VectorXd& points);

private:
  friend struct SharedCommandData;

  ParticleSystem() {} // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd P;
  std::vector<std::string> paths;
};
}
