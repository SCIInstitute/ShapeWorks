#pragma once

#include <vector>
#include <Eigen/Core>
#include "itkParticlePositionReader.h"

namespace shapeworks {

class ParticleSystem {
public:
  ParticleSystem(const std::vector<std::string> &paths);

  const Eigen::MatrixXd &Particles() const
  {
    return P;
  };

  const std::vector<std::string> &Paths() const
  {
    return paths;
  }

  int N() const
  {
    return P.cols();
  }

  int D() const
  {
    return P.rows();
  }

private:
  friend struct SharedCommandData;

  ParticleSystem() {} // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd P;
  std::vector<std::string> paths;
};
}
