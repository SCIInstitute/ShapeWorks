---
title: Studio/src/Visualization/StudioSliceInteractorStyle.h

---

# Studio/src/Visualization/StudioSliceInteractorStyle.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-03-07 at 00:21:28 +0000
