#ifndef STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H
#define STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>

class Viewer;

// Handle mouse events
class StudioInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
  static StudioInteractorStyle* New();
  vtkTypeMacro( StudioInteractorStyle, vtkInteractorStyleTrackballCamera );

  void set_viewer( Viewer *viewer );

  StudioInteractorStyle();

  virtual ~StudioInteractorStyle();

  virtual void OnLeftButtonDown();
  virtual void OnRightButtonDown();

private:
  vtkActor* LastPickedActor;
  vtkProperty* LastPickedProperty;

  Viewer *viewer_;
};

#endif /* STUDIO_VISUALIZATION_STUDIOINTERACTORSTYLE_H */
