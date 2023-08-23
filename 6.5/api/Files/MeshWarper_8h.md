---
title: Libs/Mesh/MeshWarper.h
summary: Mesh warping capability. 

---

# Libs/Mesh/MeshWarper.h

Mesh warping capability.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

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

#include <vtkPolyData.h>

#include <Eigen/Eigen>
#include <vector>

namespace shapeworks {

class MeshWarper {
 public:
  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh, const Eigen::MatrixXd& reference_particles,
                          const Eigen::MatrixXd& landmarks = {});

  bool generate_warp();

  bool get_warp_available();

  bool is_contour() const { return is_contour_; }

  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::MatrixXd& particles);

  Eigen::MatrixXd extract_landmarks(vtkSmartPointer<vtkPolyData> warped_mesh);

  bool is_contour() { return this->is_contour_; }

  std::map<int, int> get_landmarks_map() const { return landmarks_map_; }

  std::vector<int> get_good_particle_indices() const { return good_particles_; }

  const Eigen::MatrixXd& get_warp_matrix() const { return this->warp_; }

  bool has_bad_particles() const { return this->bad_particle_count() > 0; }

  vtkSmartPointer<vtkPolyData> get_reference_mesh() { return this->reference_mesh_; }

  const Eigen::MatrixXd& get_reference_particles() const { return this->reference_particles_; }

  static vtkSmartPointer<vtkPolyData> prep_mesh(vtkSmartPointer<vtkPolyData> mesh);

 protected:
  virtual void update_progress(float p) {}

 private:
  bool check_warp_ready();

  void add_particle_vertices(Eigen::MatrixXd& vertices);

  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  void split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                          std::vector<vtkSmartPointer<vtkIdList>>& new_triangles);

  void find_good_particles();

  bool find_landmarks_vertices_on_ref_mesh();

  static vtkSmartPointer<vtkPolyData> clean_mesh(vtkSmartPointer<vtkPolyData> mesh);

  static vtkSmartPointer<vtkPolyData> recreate_mesh(vtkSmartPointer<vtkPolyData> mesh);

  bool generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF, const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W);

  vtkSmartPointer<vtkPolyData> warp_mesh(const Eigen::MatrixXd& points);

  size_t bad_particle_count() const { return size_t(reference_particles_.rows()) - good_particles_.size(); }

  // Members
  Eigen::MatrixXi faces_;
  Eigen::MatrixXd vertices_;
  Eigen::MatrixXd warp_;
  Eigen::MatrixXd landmarks_points_;

  std::vector<int> good_particles_;

  bool needs_warp_ = true;

  bool warp_available_ = false;

  std::map<int, int> landmarks_map_;  // map the landmarks id (Key) to the vertex(point) id (Value) belonging to the clean Reference mesh
  vtkSmartPointer<vtkPolyData> incoming_reference_mesh_;
  vtkSmartPointer<vtkPolyData> reference_mesh_;
  Eigen::MatrixXd reference_particles_;
  bool is_contour_ = false;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-08-23 at 16:14:56 +0000
