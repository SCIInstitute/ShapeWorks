---
title: Libs/Analyze/ParticleArea.h

---

# Libs/Analyze/ParticleArea.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleArea](../Classes/classshapeworks_1_1ParticleArea.md)**  |




## Source code

```cpp
#pragma once

#include <itkPoint.h>

#include <Eigen/Core>
#include <QColor>

#include "vtkPolyData.h"

namespace shapeworks {

class ParticleArea {
 public:
  static void assign_vertex_particles(vtkSmartPointer<vtkPolyData> poly_data,
                                      std::vector<itk::Point<double>> particles);

  static void assign_vertex_colors(vtkSmartPointer<vtkPolyData> poly_data, std::vector<QColor> colors);

  static void assign_vertex_areas(vtkSmartPointer<vtkPolyData> poly_data, Eigen::VectorXd areas);

  static std::vector<QColor> colors_from_lut(vtkSmartPointer<vtkLookupTable> lut);

  static Eigen::VectorXd compute_particle_triangle_areas(vtkSmartPointer<vtkPolyData> poly_data,
                                                         std::vector<itk::Point<double>> particles);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-14 at 17:07:25 +0000
