---
title: Libs/Groom/Groom.h

---

# Libs/Groom/Groom.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Groom](../Classes/classshapeworks_1_1Groom.md)** <br>High level groom API.  |




## Source code

```cpp
#pragma once

#include <Image/Image.h>
#include <Project/Project.h>

#include "GroomParameters.h"

namespace shapeworks {


class Groom {
 public:
  Groom(ProjectHandle project);

  bool run();

  void abort();

  bool get_aborted();

  static double compute_landmark_distance(vtkSmartPointer<vtkPoints> one, vtkSmartPointer<vtkPoints> two);

  static vtkSmartPointer<vtkMatrix4x4> compute_landmark_transform(vtkSmartPointer<vtkPoints> source,
                                                                  vtkSmartPointer<vtkPoints> target);

 protected:
  std::atomic<float> progress_ = 0;
  std::atomic<int> total_ops_ = 0;
  std::atomic<int> progress_counter_ = 0;

 private:
  int get_total_ops();

  void increment_progress(int amount = 1);

  bool image_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_image_pipeline(Image& image, GroomParameters params);

  bool mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_mesh_pipeline(Mesh& mesh, GroomParameters params);

  bool contour_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  std::string get_output_filename(std::string input, DomainType domain_type);

  bool run_alignment();

  void assign_transforms(std::vector<std::vector<double>> transforms, int domain, bool global = false);

  static std::vector<std::vector<double>> get_icp_transforms(const std::vector<Mesh> meshes, Mesh reference);
  static std::vector<std::vector<double>> get_landmark_transforms(
      const std::vector<vtkSmartPointer<vtkPoints>> landmarks, size_t reference);

  static std::vector<double> get_identity_transform();
  static void add_reflect_transform(vtkSmartPointer<vtkTransform> transform, const std::string& reflect_axis);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Image& image);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Mesh& mesh);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, vtkSmartPointer<vtkPoints> points);

  std::vector<vtkSmartPointer<vtkPoints>> get_combined_points();

  Mesh get_mesh(int subject, int domain, bool transformed = false);

  vtkSmartPointer<vtkPoints> get_landmarks(int subject, int domain);

  int find_reference_landmarks(std::vector<vtkSmartPointer<vtkPoints>> landmarks);

  void fix_origin(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool abort_ = false;

  std::mutex mutex_;

  std::set<std::string> used_names_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-03 at 18:21:07 +0000
