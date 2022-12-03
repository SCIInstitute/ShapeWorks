---
title: Libs/Common/ShapeworksUtils.h

---

# Libs/Common/ShapeworksUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeworksUtils](../Classes/classshapeworks_1_1ShapeworksUtils.md)**  |




## Source code

```cpp
#pragma once

#include "Shapeworks.h"

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>

#include <chrono>
#include <random>

namespace shapeworks {

class ShapeworksUtils
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

public:

  static time_point now() { return std::chrono::steady_clock::now(); }

  static double elapsed(time_point start, time_point end, bool print_elapsed = true);

  static unsigned rngSeed() { return rngSeed_; }

  static void setRngSeed(const unsigned seed);

  static unsigned generateNumber() { return mt_(); }

  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string &pathname);

  // TODO: in C++17 this is a standard function
  static bool exists(const std::string& filename);

  static Matrix33 getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat);

private:
  static unsigned rngSeed_;
  static std::mt19937 mt_;
};

} // shapeworks
```


-------------------------------

Updated on 2022-12-03 at 09:41:17 +0000
