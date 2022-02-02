---
title: Libs/Groom/Groom.h

---

# Libs/Groom/Groom.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Groom](../Classes/classshapeworks_1_1Groom.md)** <br>High level groom API.  |




## Source code

```cpp
#pragma once

#include <GroomParameters.h>
#include <Libs/Image/Image.h>
#include <Libs/Project/Project.h>
#include <tbb/atomic.h>
#include <tbb/mutex.h>

namespace shapeworks {


class Groom {
 public:
  Groom(ProjectHandle project);

  virtual bool run();

  void set_skip_grooming(bool skip);

  void abort();

  bool get_aborted();

  static double compute_landmark_distance(vtkSmartPointer<vtkPoints> one, vtkSmartPointer<vtkPoints> two);

  static vtkSmartPointer<vtkMatrix4x4> compute_landmark_transform(vtkSmartPointer<vtkPoints> source,
                                                                  vtkSmartPointer<vtkPoints> target);

 protected:
  virtual void update_progress(){};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_ = 0;

 private:
  int get_total_ops();

  void increment_progress(int amount = 1);

  bool image_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_image_pipeline(Image& image, GroomParameters params);

  bool mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_mesh_pipeline(Mesh& mesh, GroomParameters params);

  std::string get_output_filename(std::string input, DomainType domain_type);

  bool run_alignment();

  void assign_transforms(std::vector<std::vector<double>> transforms, int domain, bool global = false);

  static std::vector<std::vector<double>> get_icp_transforms(const std::vector<Mesh> meshes, size_t reference);
  static std::vector<std::vector<double>> get_landmark_transforms(
      const std::vector<vtkSmartPointer<vtkPoints>> landmarks, size_t reference);

  static std::vector<double> get_identity_transform();
  static void add_reflect_transform(vtkSmartPointer<vtkTransform> transform, const std::string& reflect_axis);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Image& image);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Mesh& mesh);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, vtkSmartPointer<vtkPoints> points);

  std::vector<vtkSmartPointer<vtkPoints>> get_combined_points();

  Mesh get_mesh(int subject, int domain);

  vtkSmartPointer<vtkPoints> get_landmarks(int subject, int domain);

  int find_reference_landmarks(std::vector<vtkSmartPointer<vtkPoints>> landmarks);

  void fix_origin(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool skip_grooming_ = false;

  bool abort_ = false;

  tbb::mutex mutex_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-02-02 at 18:18:10 +0000
