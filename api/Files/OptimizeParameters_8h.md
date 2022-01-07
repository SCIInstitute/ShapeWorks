---
title: Libs/Optimize/OptimizeParameters.h

---

# Libs/Optimize/OptimizeParameters.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::OptimizeParameters](../Classes/classshapeworks_1_1OptimizeParameters.md)**  |




## Source code

```cpp
#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class Optimize;

class OptimizeParameters {

public:

  explicit OptimizeParameters(ProjectHandle project);
  void save_to_project();

  std::string get_optimize_output_prefix();
  void set_optimize_output_prefix(std::string prefix);

  bool set_up_optimize(Optimize* optimize);

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

  double get_narrow_band();
  void set_narrow_band(double value);

  int get_verbosity();
  void set_verbosity(int value);

  void set_abort_load(bool value);

  void set_load_callback(const std::function<void(int)>& f);

private:

  std::string get_output_prefix();

  Parameters params_;
  ProjectHandle project_;

  std::function<void(int)> load_callback_;

  bool abort_load_ = false;

};

}
```


-------------------------------

Updated on 2022-01-07 at 20:14:46 +0000
