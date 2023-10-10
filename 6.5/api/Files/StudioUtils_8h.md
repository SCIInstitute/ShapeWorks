---
title: Studio/Utils/StudioUtils.h

---

# Studio/Utils/StudioUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

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
class vtkRenderer;

namespace shapeworks {

class StudioUtils {
 public:
  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);

  static QStringList to_string_list(std::vector<std::string> vector);

  static QImage vtk_image_to_qimage(vtkSmartPointer<vtkImageData> image_data);

  static vtkSmartPointer<vtkPolyData> reverse_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

  static QString get_platform_string();

  static void add_viewport_border(vtkRenderer* renderer, double* color);

};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-10 at 07:25:30 +0000
