---
title: Libs/Common/ShapeworksUtils.h

---

# Libs/Common/ShapeworksUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeWorksUtils](../Classes/classshapeworks_1_1ShapeWorksUtils.md)**  |




## Source code

```cpp
#pragma once

#include <oneapi/tbb/global_control.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <chrono>
#include <random>

#include "Shapeworks.h"

namespace shapeworks {

class ShapeWorksUtils {
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

 public:
  static time_point now() { return std::chrono::steady_clock::now(); }

  static double elapsed(time_point start, time_point end, bool print_elapsed = true);

  static unsigned get_rng_seed() { return rng_seed_; }

  static void set_rng_seed(const unsigned seed);

  static unsigned get_random_number() { return mt_(); }

  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string& pathname);

  // TODO: in C++17 this is a standard function
  static bool file_exists(const std::string& filename);

  static void setup_console_logging(bool show_progress, bool xml_status);

  static void set_progress_callback(void* ptr);

  static void setup_threads();

  static Matrix33 convert_matrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 get_offset(const vtkSmartPointer<vtkMatrix4x4>& mat);

 private:
  static unsigned rng_seed_;
  static std::mt19937 mt_;
  static std::unique_ptr<tbb::global_control> tbb_global_control_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-11 at 18:16:34 +0000
