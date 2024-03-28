#pragma once

#include <Eigen/Core>
#include <string>

#include "ParticleSystemEvaluation.h"

namespace shapeworks {

class ShapeEvaluation {
 public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double ComputeCompactness(const ParticleSystemEvaluation& ParticleSystemEvaluation, const int nModes,
                                   const std::string& saveTo = "");

  static Eigen::VectorXd ComputeFullCompactness(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                std::function<void(float)> progress_callback = nullptr);

  static double ComputeGeneralization(const ParticleSystemEvaluation& ParticleSystemEvaluation, const int nModes,
                                      const std::string& saveTo = "");

  static Eigen::VectorXd ComputeFullGeneralization(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                   std::function<void(float)> progress_callback = nullptr);

  static double ComputeSpecificity(const ParticleSystemEvaluation& ParticleSystemEvaluation, const int nModes,
                                   const std::string& saveTo = "");

  static Eigen::VectorXd ComputeFullSpecificity(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                std::function<void(float)> progress_callback = nullptr);
};
}  // namespace shapeworks
