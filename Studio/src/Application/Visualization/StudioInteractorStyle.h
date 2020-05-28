#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>

class Lightbox;

// Handle mouse events
class StudioInteractorStyle : public vtkInteractorStyleTrackballCamera
{
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


protected:

   void Dolly(double factor) override;
private:

  Lightbox* lightbox_;
};
