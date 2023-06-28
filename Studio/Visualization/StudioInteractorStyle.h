#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindowInteractor.h>

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
  void OnRightButtonUp() override;

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
  bool right_click_dragged_{false};
  bool right_button_down_{false};
};

}  // namespace shapeworks
