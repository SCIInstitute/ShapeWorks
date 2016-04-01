#ifndef STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H
#define STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>

class Lightbox;

// Handle mouse events
class StudioInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  static StudioInteractorStyle* New();
  vtkTypeMacro( StudioInteractorStyle, vtkInteractorStyleTrackballCamera );

  void set_lightbox( Lightbox* lightbox );

  StudioInteractorStyle();

  virtual ~StudioInteractorStyle();

  virtual void OnLeftButtonDown();
  virtual void OnRightButtonDown();

  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();

  virtual void OnKeyDown();

private:

  Lightbox* lightbox_;
};

#endif /* STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H */
