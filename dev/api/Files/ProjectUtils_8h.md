---
title: Libs/Project/ProjectUtils.h

---

# Libs/Project/ProjectUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ProjectUtils](../Classes/classshapeworks_1_1ProjectUtils.md)**  |




## Source code

```cpp
#pragma once

#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#include <vector>

namespace shapeworks {

class ProjectUtils {
 public:
  // convert a list of doubles from a spreadsheet to a vtkTransform
  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  // convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)
  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-22 at 21:16:04 +0000