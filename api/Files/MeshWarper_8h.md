---
title: Libs/Mesh/MeshWarper.h
summary: Mesh warping capability. 

---

# Libs/Mesh/MeshWarper.h

Mesh warping capability.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md)**  |

## Detailed Description

Mesh warping capability. 

The MeshWarper provides an object to warp meshes for surface reconstruction 




## Source code

```cpp
#pragma once

#include <vector>
#include <vtkPolyData.h>
#include <Eigen/Eigen>

namespace shapeworks {

class MeshWarper {

public:

  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                          const Eigen::MatrixXd& reference_particles);

  bool get_warp_available();

  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::MatrixXd& particles);

  bool is_contour() { return this->is_contour_; }

protected:

  virtual void update_progress(float p) {}

private:

  bool generate_warp();

  void add_particle_vertices();

  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  void split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                          std::vector<vtkSmartPointer<vtkIdList>>& new_triangles);

  void find_good_particles();

  bool check_warp_ready();

  static vtkSmartPointer<vtkPolyData> prep_mesh(vtkSmartPointer<vtkPolyData> mesh);

  static vtkSmartPointer<vtkPolyData> clean_mesh(vtkSmartPointer<vtkPolyData> mesh);

  vtkSmartPointer<vtkPolyData> recreate_mesh(vtkSmartPointer<vtkPolyData> mesh);

  bool generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF,
                            const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W);

  vtkSmartPointer<vtkPolyData> warp_mesh(const Eigen::MatrixXd& points);

  Eigen::MatrixXi faces_;
  Eigen::MatrixXd vertices_;
  Eigen::MatrixXd warp_;

  std::vector<int> good_particles_;

  bool needs_warp_ = true;

  bool warp_available_ = false;

  vtkSmartPointer<vtkPolyData> incoming_reference_mesh_;
  vtkSmartPointer<vtkPolyData> reference_mesh_;
  Eigen::MatrixXd reference_particles_;
  bool is_contour_ = false;
};
}
```


-------------------------------

Updated on 2021-11-14 at 17:52:11 +0000
