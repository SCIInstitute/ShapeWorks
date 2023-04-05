---
title: Studio/Visualization/StudioVtkOutputWindow.h

---

# Studio/Visualization/StudioVtkOutputWindow.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md)** <br>Implementation of vtkOutputWindow to capture and display VTK error messages.  |




## Source code

```cpp
#pragma once

#include <vtkOutputWindow.h>

#include <QObject>

namespace shapeworks {

class StudioVtkOutputWindow : public QObject, public vtkOutputWindow {
  Q_OBJECT;

 public:
  static StudioVtkOutputWindow* New();

  vtkTypeMacro(StudioVtkOutputWindow, vtkOutputWindow);

  StudioVtkOutputWindow();

  void DisplayErrorText(const char* text) override;
  void DisplayWarningText(const char* text) override;
  void DisplayGenericWarningText(const char* text) override;
  void DisplayDebugText(const char* text) override;
 private:
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-04-05 at 02:08:59 +0000
