---
title: Libs/Analyze/Analyze.h

---

# Libs/Analyze/Analyze.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Analyze](../Classes/classshapeworks_1_1Analyze.md)** <br>High level analyze API.  |




## Source code

```cpp
#pragma once

#include <Project/Project.h>
#include <ParticleShapeStatistics.h>
#include "Shape.h"

namespace shapeworks {


class Analyze {
 public:
  Analyze(ProjectHandle project);

  void run_offline_analysis(std::string outfile);

  ShapeList get_shapes();

  int get_num_modes();

  Particles get_mean_shape_points();
  ShapeHandle get_mean_shape();

  Particles get_shape_points(int mode, double value);
  ShapeHandle get_mode_shape(int mode, double value);

  bool groups_active() { return false; }

  ShapeHandle create_shape_from_points(Particles points);

 private:

  bool update_shapes();
  bool compute_stats();

  Particles convert_from_combined(const Eigen::VectorXd& points);

  ProjectHandle project_;

  // collection of shapes
  ShapeList shapes_;

  ShapeList group1_list_;
  ShapeList group2_list_;


  std::shared_ptr<MeshManager> mesh_manager_;

  ParticleShapeStatistics stats_;
  bool stats_ready_ = false;

};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-11-11 at 02:30:38 +0000
