#pragma once

#include <Project/Project.h>

#include <functional>

namespace shapeworks {

class Optimize;

/**
 * \class OptimizeParameters
 * \ingroup Group-Optimize
 *
 * This class encapsulated processing of Optimization parameters
 */
class OptimizeParameters {
 public:
  explicit OptimizeParameters(ProjectHandle project);
  void save_to_project();

  std::string get_optimize_output_prefix();
  void set_optimize_output_prefix(std::string prefix);

  std::vector<int> get_number_of_particles();
  void set_number_of_particles(std::vector<int> number_of_particles);

  double get_initial_relative_weighting();
  void set_initial_relative_weighting(double value);

  double get_relative_weighting();
  void set_relative_weighting(double value);

  double get_starting_regularization();
  void set_starting_regularization(double value);

  double get_ending_regularization();
  void set_ending_regularization(double value);

  int get_iterations_per_split();
  void set_iterations_per_split(int value);

  int get_optimization_iterations();
  void set_optimization_iterations(int value);

  bool get_use_geodesic_distance();
  void set_use_geodesic_distance(bool value);

  int get_geodesic_cache_multiplier();
  void set_geodesic_cache_multiplier(int value);

  std::vector<bool> get_use_normals();
  void set_use_normals(std::vector<bool> use_normals);

  double get_normals_strength();
  void set_normals_strength(double value);

  bool get_use_disentangled_ssm();
  void set_use_disentangled_ssm(bool value);

  bool get_use_procrustes();
  void set_use_procrustes(bool value);

  bool get_use_procrustes_scaling();
  void set_use_procrustes_scaling(bool value);

  bool get_use_procrustes_rotation_translation();
  void set_use_procrustes_rotation_translation(bool value);

  int get_procrustes_interval();
  void set_procrustes_interval(int value);

  bool get_use_multiscale();
  void set_use_multiscale(bool value);

  int get_multiscale_particles();
  void set_multiscale_particles(int value);

  double get_narrow_band();
  void set_narrow_band(double value);

  int get_verbosity();
  void set_verbosity(int value);

  bool get_mesh_ffc_mode();
  void set_mesh_ffc_mode(bool value);

  bool get_use_landmarks();
  void set_use_landmarks(bool value);

  bool get_use_fixed_subjects();
  void set_use_fixed_subjects(bool value);
  std::string get_fixed_subjects_column();
  void set_fixed_subject_column(std::string column);
  std::string get_fixed_subjects_choice();
  void set_fixed_subjects_choice(std::string choice);

  void set_abort_load(bool value);

  void set_load_callback(const std::function<void(int)>& f);

  bool set_up_optimize(Optimize* optimize);

  bool is_subject_fixed(std::shared_ptr<Subject> subject);

  int get_checkpoint_interval();
  void set_checkpoint_interval(int iterations);

  bool get_save_init_splits();
  void set_save_init_splits(bool enabled);

  bool get_keep_checkpoints();
  void set_keep_checkpoints(bool enabled);


 private:
  std::string get_output_prefix();

  Parameters params_;
  ProjectHandle project_;

  std::function<void(int)> load_callback_;

  bool abort_load_ = false;
};

}  // namespace shapeworks
