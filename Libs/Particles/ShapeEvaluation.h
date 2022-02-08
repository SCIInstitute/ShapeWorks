#pragma once

#include <string>
#include <Eigen/Core>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_svd.h>
#include "ParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {

class ShapeEvaluation
{
public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullCompactness(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static Eigen::VectorXd ComputeFullCompactnessInWithinSubspace(const Eigen::MatrixXd &WithinMatrix, std::function<void(float)> progress_callback = nullptr);

  static Eigen::VectorXd ComputeFullCompactnessInBetweenSubspace(const Eigen::MatrixXd &BetweenMatrix, std::function<void(float)> progress_callback = nullptr);

  static double ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullGeneralization(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static Eigen::VectorXd ComputeFullGeneralizationMlpca(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static double ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullSpecificity(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);
};
}
