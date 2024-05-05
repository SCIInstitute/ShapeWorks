#pragma once

#include <Eigen/Core>
#include <string>

#include "ParticleSystemEvaluation.h"

namespace shapeworks {

class ShapeEvaluation {
 public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double compute_compactness(const ParticleSystemEvaluation& ParticleSystemEvaluation, const int nModes,
                                   const std::string& saveTo = "");

  static Eigen::VectorXd compute_full_compactness(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                std::function<void(float)> progress_callback = nullptr);

  static double compute_generalization(const ParticleSystemEvaluation& particle_system, const int num_modes,
                                       const std::string& save_to = "");

  static Eigen::VectorXd compute_full_generalization(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                   std::function<void(float)> progress_callback = nullptr);

  static double compute_specificity(const ParticleSystemEvaluation& ParticleSystemEvaluation, const int nModes,
                                   const std::string& saveTo = "");

  static Eigen::VectorXd compute_full_specificity(const ParticleSystemEvaluation& ParticleSystemEvaluation,
                                                std::function<void(float)> progress_callback = nullptr);
};
}  // namespace shapeworks
