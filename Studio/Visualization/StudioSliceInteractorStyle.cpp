#include <Visualization/Lightbox.h>
#include <Visualization/StudioSliceInteractorStyle.h>
#include <vtkImageProperty.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>

namespace shapeworks {

vtkStandardNewMacro(StudioSliceInteractorStyle);

//-----------------------------------------------------------------------------
StudioSliceInteractorStyle::StudioSliceInteractorStyle() {}

//-----------------------------------------------------------------------------
StudioSliceInteractorStyle::~StudioSliceInteractorStyle() {}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnLeftButtonDown() {
  // forward events
  vtkInteractorStyleImage::OnLeftButtonDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnRightButtonDown() {
  // forward events
  vtkInteractorStyleImage::OnRightButtonDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnKeyDown() {
  int* click_pos = this->GetInteractor()->GetEventPosition();

  // char keycode = this->GetInteractor()->GetKeyCode();
  std::string keysym = GetInteractor()->GetKeySym();
  if (keysym == "Up" | keysym == "Down") {
    lightbox_->handle_key(click_pos, keysym);
  }

  this->GetInteractor()->Render();

  // forward events
  vtkInteractorStyleTrackballCamera::OnKeyDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::WindowLevel() {
  vtkInteractorStyleImage::WindowLevel();
  if (CurrentImageProperty) {
    double window = CurrentImageProperty->GetColorWindow();
    double level = CurrentImageProperty->GetColorLevel();
    lightbox_->set_shared_window_and_level(window, level);
  }
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::set_lightbox(Lightbox* lightbox) { this->lightbox_ = lightbox; }

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelForward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelBackward() {
  // do nothing so that it will be passed on to Qt
}

}  // namespace shapeworks
