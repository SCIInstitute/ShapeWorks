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

#include <vtkTransform.h>
#include <vtkSmartPointer.h>

#include <vector>

namespace shapeworks {

class ProjectUtils {
public:

  // convert a list of doubles from a spreadsheet to a vtkTransform
  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  // convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)
  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);
};
}
```


-------------------------------

Updated on 2021-12-03 at 20:11:58 +0000
