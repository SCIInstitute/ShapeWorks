---
title: Libs/Application/ShapeWorksVtkOutputWindow.h

---

# Libs/Application/ShapeWorksVtkOutputWindow.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeWorksVtkOutputWindow](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md)** <br>Implementation of vtkOutputWindow to capture and display VTK error messages.  |




## Source code

```cpp
#pragma once

#include <vtkOutputWindow.h>

#include <QObject>

namespace shapeworks {

class ShapeWorksVtkOutputWindow : public QObject, public vtkOutputWindow {
  Q_OBJECT;

 public:
  static ShapeWorksVtkOutputWindow* New();

  vtkTypeMacro(ShapeWorksVtkOutputWindow, vtkOutputWindow);

  ShapeWorksVtkOutputWindow();

  void DisplayErrorText(const char* text) override;
  void DisplayWarningText(const char* text) override;
  void DisplayGenericWarningText(const char* text) override;
  void DisplayDebugText(const char* text) override;
 private:
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-13 at 18:47:50 +0000
