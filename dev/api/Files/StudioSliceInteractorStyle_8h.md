---
title: Studio/Visualization/StudioSliceInteractorStyle.h

---

# Studio/Visualization/StudioSliceInteractorStyle.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioSliceInteractorStyle](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md)**  |




## Source code

```cpp
#pragma once

#include <vtkInteractorStyleImage.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindowInteractor.h>

namespace shapeworks {

class Lightbox;

// Handle mouse events
class StudioSliceInteractorStyle : public vtkInteractorStyleImage {
 public:
  static StudioSliceInteractorStyle* New();
  vtkTypeMacro(StudioSliceInteractorStyle, vtkInteractorStyleImage);

  void set_lightbox(Lightbox* lightbox);

  StudioSliceInteractorStyle();

  virtual ~StudioSliceInteractorStyle();

  void OnLeftButtonDown() override;
  void OnRightButtonDown() override;

  void OnKeyDown() override;

  void WindowLevel() override;

  /*
  void OnMouseWheelForward() override;
  void OnMouseWheelBackward() override;

  void Dolly() override;

  void Rotate() override;

  void OnMouseMove() override;
*/
 protected:
//  void Dolly(double factor) override;

 private:
  Lightbox* lightbox_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-05-04 at 20:03:05 +0000