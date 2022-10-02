---
title: Studio/src/Visualization/StudioVtkOutputWindow.h

---

# Studio/src/Visualization/StudioVtkOutputWindow.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-10-02 at 18:47:34 +0000
