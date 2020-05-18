#pragma once

#include <vector>
#include <Eigen/Core>
#include "itkParticlePositionReader.h"

class ParticleSystem
{
public:
  ParticleSystem();

  void LoadParticles(const std::vector<std::string> &paths);

  const Eigen::MatrixXd &Particles() const
  {
    AssertLoaded();
    return P;
  };

  const std::vector<std::string> &Paths() const
  {
    AssertLoaded();
    return paths;
  }

  int N() const
  {
    AssertLoaded();
    return P.cols();
  }

  int D() const
  {
    AssertLoaded();
    return P.rows();
  }

private:
  Eigen::MatrixXd P;
  std::vector<std::string> paths;
  bool isLoaded = false;

  void AssertLoaded() const
  { assert(isLoaded); }
};

