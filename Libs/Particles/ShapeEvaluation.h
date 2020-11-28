#pragma once

#include <string>
#include <Eigen/Core>
#include "ParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {

class ShapeEvaluation
{
public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static double ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static double ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");
};
}
