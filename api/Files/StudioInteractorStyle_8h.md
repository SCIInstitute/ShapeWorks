---
title: Studio/src/Visualization/StudioInteractorStyle.h

---

# Studio/src/Visualization/StudioInteractorStyle.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioInteractorStyle](../Classes/classshapeworks_1_1StudioInteractorStyle.md)**  |




## Source code

```cpp
#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>

namespace shapeworks {

class Lightbox;

// Handle mouse events
class StudioInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
  static StudioInteractorStyle* New();
vtkTypeMacro(StudioInteractorStyle, vtkInteractorStyleTrackballCamera);

  void set_lightbox(Lightbox* lightbox);

  StudioInteractorStyle();

  virtual ~StudioInteractorStyle();

  void OnLeftButtonDown() override;
  void OnRightButtonDown() override;

  void OnMouseWheelForward() override;
  void OnMouseWheelBackward() override;

  void OnKeyDown() override;
  void Dolly() override;

  void Rotate() override;

  void OnMouseMove() override;

protected:

  void Dolly(double factor) override;
private:

  Lightbox* lightbox_;
};

}
```


-------------------------------

Updated on 2022-02-04 at 06:14:24 +0000
