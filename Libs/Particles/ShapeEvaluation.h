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

  //! Generalization for 1 to max_modes modes (all modes if max_modes < 0)
  static Eigen::VectorXd compute_full_generalization(const ParticleSystemEvaluation& particle_system,
                                                     std::function<void(float)> progress_callback = nullptr,
                                                     std::function<bool()> check_abort = nullptr,
                                                     bool surface_distance_mode = false, int max_modes = -1);

  static double compute_specificity(const ParticleSystemEvaluation& particle_system, int num_mode,
                                    const std::string& save_to = "", bool surface_distance_mode = false);

  //! Specificity for 1 to max_modes modes (all modes if max_modes < 0)
  static Eigen::VectorXd compute_full_specificity(const ParticleSystemEvaluation& particle_system,
                                                  std::function<void(float)> progress_callback = nullptr,
                                                  std::function<bool()> check_abort = nullptr,
                                                  bool surface_distance_mode = false, int max_modes = -1);
};
}  // namespace shapeworks
