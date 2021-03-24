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

  ParticleSystem(const Eigen::MatrixXd &matrix);

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

  std::vector<std::vector<Point>> toVector();

private:
  friend struct SharedCommandData;

  ParticleSystem() {} // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd P;
  std::vector<std::string> paths;
};
}
