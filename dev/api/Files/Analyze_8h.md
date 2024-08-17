---
title: Libs/Analyze/Analyze.h

---

# Libs/Analyze/Analyze.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Analyze](../Classes/classshapeworks_1_1Analyze.md)** <br>High level analyze API.  |




## Source code

```cpp
#pragma once

#include <ParticleShapeStatistics.h>
#include <Project/Project.h>

#include "Shape.h"

namespace shapeworks {


class Analyze {
 public:

  enum AlignmentType {
    Global = -2,
    Local = -1,
  };


  Analyze(ProjectHandle project);

  void run_offline_analysis(std::string outfile, float range, float steps);

  ShapeList get_shapes();

  int get_num_subjects();

  int get_num_modes();

  int get_num_particles();

  Particles get_mean_shape_points();

  ShapeHandle get_mean_shape();

  Particles get_group_shape_particles(double ratio);
  ShapeHandle get_group_shape(double ratio);

  Particles get_shape_points(int mode, double value);
  ShapeHandle get_mode_shape(int mode, double value);

  bool groups_active() { return false; }

  ShapeHandle create_shape_from_points(Particles points);

  Eigen::VectorXd get_subject_features(int subject, std::string feature_name);

  void set_group_selection(std::string group_name, std::string group1, std::string group2);

  ParticleSystemEvaluation get_local_particle_system(int domain);

 private:
  bool update_shapes();
  bool compute_stats();

  void initialize_mesh_warper();

  Particles convert_from_combined(const Eigen::VectorXd& points);

  ProjectHandle project_;

  // collection of shapes
  ShapeList shapes_;

  ShapeList group1_list_;
  ShapeList group2_list_;

  std::shared_ptr<MeshManager> mesh_manager_;

  ParticleShapeStatistics stats_;
  bool stats_ready_ = false;

  std::string selected_group_;
  std::string group1_;
  std::string group2_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-08-17 at 17:02:54 +0000
