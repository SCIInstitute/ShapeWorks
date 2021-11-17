---
title: Studio/src/Data/StudioParticles.h

---

# Studio/src/Data/StudioParticles.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md)** <br>Representation of correspondence points for a shape including multiple domains.  |




## Source code

```cpp
#pragma once

#include <vector>
#include <vnl_vector.h>
#include <vtkSmartPointer.h>
#include <itkPoint.h>

class vtkTransform;

namespace shapeworks {


class StudioParticles {

public:
  StudioParticles();

  void set_local_particles(int domain, std::vector<itk::Point<double>> particles);
  void set_world_particles(int domain, std::vector<itk::Point<double>> particles);

  void set_local_particles(int domain, vnl_vector<double> particles);
  void set_world_particles(int domain, vnl_vector<double> particles);

  std::vector<vnl_vector<double>> get_local_particles();
  std::vector<vnl_vector<double>> get_world_particles();

  vnl_vector<double> get_local_particles(int domain);
  vnl_vector<double> get_world_particles(int domain);
  vnl_vector<double> get_raw_world_particles(int domain);

  vnl_vector<double> get_combined_local_particles();
  vnl_vector<double> get_combined_global_particles();

  std::vector<itk::Point<double>> get_local_points(int domain);
  std::vector<itk::Point<double>> get_world_points(int domain);

  int get_domain_for_combined_id(int id);

  void set_transform(vtkSmartPointer<vtkTransform> transform);
  void set_procrustes_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms);

private:

  void transform_global_particles();

  std::vector<itk::Point<double>> vnl_to_point_vector(const vnl_vector<double>& vnl);

  vnl_vector<double> combine(const std::vector<vnl_vector<double>>& vnl);

  void set_particles(int domain, std::vector<itk::Point<double>> particles, bool local);
  std::vector<vnl_vector<double>> local_particles_; // one for each domain
  std::vector<vnl_vector<double>> global_particles_; // one for each domain
  std::vector<vnl_vector<double>> transformed_global_particles_; // one for each domain

  vtkSmartPointer<vtkTransform> transform_;
  std::vector<vtkSmartPointer<vtkTransform>> procrustes_transforms_;
};
}
```


-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000
