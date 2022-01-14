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

  static unsigned rngSeed() { return _rngSeed; }

  static void setRngSeed(const unsigned seed);

  static unsigned generateNumber() { return mt(); }

  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string &pathname);

  // TODO: in C++17 this is a standard function
  static bool exists(const std::string& filename);

  static Matrix33 getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat);

private:
  static unsigned _rngSeed;
  static std::mt19937 mt;
};

} // shapeworks
```


-------------------------------

Updated on 2022-01-14 at 02:26:01 +0000
