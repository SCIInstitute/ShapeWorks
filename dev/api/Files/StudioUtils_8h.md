---
title: Studio/src/Utils/StudioUtils.h

---

# Studio/src/Utils/StudioUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioUtils](../Classes/classshapeworks_1_1StudioUtils.md)** <br>Utilities for Studio.  |




## Source code

```cpp
#pragma once

#include <Project/Project.h>
class QWidget;

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QImage>
#include <QStringList>

class vtkImageData;

namespace shapeworks {

class StudioUtils {
 public:
  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);

  static QStringList to_string_list(std::vector<std::string> vector);

  static QImage vtk_image_to_qimage(vtkSmartPointer<vtkImageData> image_data);

  static vtkSmartPointer<vtkPolyData> reverse_poly_data(vtkSmartPointer<vtkPolyData> poly_data);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-11-04 at 00:00:05 +0000
