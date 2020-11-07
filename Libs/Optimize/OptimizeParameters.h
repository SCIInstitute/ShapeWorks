#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class Optimize;

class OptimizeParameters {

public:

  explicit OptimizeParameters(ProjectHandle project);
  void save_to_project();

  bool set_up_optimize(Optimize *optimize);

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

  std::vector<bool> get_use_normals();
  void set_use_normals(std::vector<bool> use_normals);

  double get_normals_strength();
  void set_normals_strength(double value);

  bool get_use_procrustes();
  void set_use_procrustes(bool value);

  bool get_use_procrustes_scaling();
  void set_use_procrustes_scaling(bool value);

  int get_procrustes_interval();
  void set_procrustes_interval(int value);

  bool get_use_multiscale();
  void set_use_multiscale(bool value);

  int get_multiscale_particles();
  void set_multiscale_particles(int value);

  
private:

  Parameters params_;
  ProjectHandle project_;

};

}
