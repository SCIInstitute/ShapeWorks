#pragma once

#include <Eigen/Core>
#include <string>

#include "ParticleSystemEvaluation.h"

namespace shapeworks {

class ShapeEvaluation {
 public:
  ShapeEvaluation();

  static double compute_compactness(const ParticleSystemEvaluation& particle_system, int num_modes,
                                    const std::string& save_to = "");

  static Eigen::VectorXd compute_full_compactness(const ParticleSystemEvaluation& particle_system,
                                                  std::function<void(float)> progress_callback = nullptr);

  static double compute_generalization(const ParticleSystemEvaluation& particle_system, int num_modes,
                                       const std::string& save_to = "", bool surface_distance_mode = false);

  static Eigen::VectorXd compute_full_generalization(const ParticleSystemEvaluation& particle_system,
                                                     std::function<void(float)> progress_callback = nullptr,
                                                     std::function<bool()> check_abort = nullptr,
                                                     bool surface_distance_mode = false);

  static double compute_specificity(const ParticleSystemEvaluation& particle_system, int num_mode,
                                    const std::string& save_to = "", bool surface_distance_mode = false);

  static Eigen::VectorXd compute_full_specificity(const ParticleSystemEvaluation& particle_system,
                                                  std::function<void(float)> progress_callback = nullptr,
                                                  std::function<bool()> check_abort = nullptr,
                                                  bool surface_distance_mode = false);
};
}  // namespace shapeworks
