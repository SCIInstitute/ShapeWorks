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

#include <itkPoint.h>
#include <vnl/vnl_vector.h>
#include <vtkSmartPointer.h>

#include <cassert>
#include <vector>

#include "Shapeworks.h"

class vtkTransform;

namespace shapeworks {


class StudioParticles {
 public:
  StudioParticles();

  void set_local_particles(int domain, std::vector<itk::Point<double>> particles);
  void set_world_particles(int domain, std::vector<itk::Point<double>> particles);

  void set_local_particles(int domain, Eigen::VectorXd particles);
  void set_world_particles(int domain, Eigen::VectorXd particles);

  std::vector<Eigen::VectorXd> get_local_particles();
  std::vector<Eigen::VectorXd> get_world_particles();

  Eigen::VectorXd get_local_particles(int domain);
  Eigen::VectorXd get_world_particles(int domain);
  Eigen::VectorXd get_raw_world_particles(int domain);

  Eigen::VectorXd get_combined_local_particles() const;
  Eigen::VectorXd get_combined_global_particles() const;
  void set_combined_global_particles(const Eigen::VectorXd &particles);

  std::vector<itk::Point<double>> get_local_points(int domain);
  std::vector<itk::Point<double>> get_world_points(int domain);

  int get_domain_for_combined_id(int id);

  void set_transform(vtkSmartPointer<vtkTransform> transform);
  void set_procrustes_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms);

  Eigen::VectorXd get_difference_vectors(const StudioParticles& other);

 private:
  void transform_global_particles();

  std::vector<itk::Point<double>> eigen_to_point_vector(const Eigen::VectorXd& particles);

  Eigen::VectorXd combine(const std::vector<Eigen::VectorXd>& particles) const;

  void set_particles(int domain, std::vector<itk::Point<double>> particles, bool local);
  std::vector<Eigen::VectorXd> local_particles_;               // one for each domain
  std::vector<Eigen::VectorXd> global_particles_;              // one for each domain
  std::vector<Eigen::VectorXd> transformed_global_particles_;  // one for each domain

  vtkSmartPointer<vtkTransform> transform_;
  std::vector<vtkSmartPointer<vtkTransform>> procrustes_transforms_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-06-13 at 01:32:51 -0600
