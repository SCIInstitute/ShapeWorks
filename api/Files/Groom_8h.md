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

#include <Libs/Project/Project.h>
#include <Libs/Image/Image.h>

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

protected:

  virtual void update_progress()
  {};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_ = 0;

private:

  int get_total_ops();

  void increment_progress(int amount = 1);

  bool image_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  std::string get_output_filename(std::string input, DomainType domain_type);

  bool run_alignment();

  static std::vector<std::vector<double>> get_icp_transforms(const std::vector<Mesh> meshes, size_t reference);

  static std::vector<double> get_identity_transform();
  static std::vector<double> get_center_transform(const Mesh &mesh);
  static std::vector<double> convert_transform(itk::AffineTransform<double, 3>::Pointer transform);

  Mesh get_mesh(int subject, int domain);


  Vector3 center(Image& image);
  void isolate(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool skip_grooming_ = false;

  bool abort_ = false;

  tbb::mutex mutex_;

};
}
```


-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000
