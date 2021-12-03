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

#include <QObject>
#include <vtkOutputWindow.h>

namespace shapeworks {

class StudioVtkOutputWindow : public QObject, public vtkOutputWindow {
Q_OBJECT;

public:
  static StudioVtkOutputWindow* New();

vtkTypeMacro(StudioVtkOutputWindow, vtkOutputWindow);

  StudioVtkOutputWindow();

  void DisplayErrorText(const char* text) override;
  void DisplayWarningText(const char* text) override;

Q_SIGNALS:
  void warning(QString message);
  void error(QString message);

private:

};
}
```


-------------------------------

Updated on 2021-12-03 at 02:18:12 +0000
