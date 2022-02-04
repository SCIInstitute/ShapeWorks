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
  /*
  if (this->Interactor->GetControlKey()) {
    int* clickPos = this->GetInteractor()->GetEventPosition();
    this->lightbox_->handle_pick(clickPos, true, true);
    return;
  }
  */

  std::cerr << "slice left button down\n";
  // forward events
  vtkInteractorStyleImage::OnLeftButtonDown();
  // this->GetCurrentRenderer()->ResetCameraClippingRange();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnRightButtonDown() {
  /*
     int* clickPos = this->GetInteractor()->GetEventPosition();
     this->lightbox_->handle_pick( clickPos );
   */
  // forward events
  vtkInteractorStyleImage::OnRightButtonDown();
  // this->GetCurrentRenderer()->ResetCameraClippingRange();
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
  double window = CurrentImageProperty->GetColorWindow();
  double level = CurrentImageProperty->GetColorLevel();
  lightbox_->set_shared_window_and_level(window, level);
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::set_lightbox(Lightbox* lightbox) { this->lightbox_ = lightbox; }
/*
//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelForward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseWheelBackward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnKeyDown() {
  int* click_pos = this->GetInteractor()->GetEventPosition();

  char keycode = this->GetInteractor()->GetKeyCode();
  std::string keysym = GetInteractor()->GetKeySym();

  switch (keycode) {
    case '1':
      this->lightbox_->handle_pick(click_pos, true, false);
      break;

    case '2':
      this->lightbox_->handle_pick(click_pos, false, false);
      break;

    case 'r':
      this->lightbox_->reset_camera();
      break;
  }

  if (keysym == "Up" | keysym == "Down") {
    lightbox_->handle_key(click_pos, keysym);
  }

  this->GetInteractor()->Render();

  // forward events
  vtkInteractorStyleTrackballCamera::OnKeyDown();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::Dolly() {
  if (this->CurrentRenderer == NULL) {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;
  double* center = this->CurrentRenderer->GetCenter();
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  double dyf = this->MotionFactor * dy / center[1];
  this->Dolly(pow(1.1, dyf));
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::Rotate() {
  vtkInteractorStyleTrackballCamera::Rotate();
  this->lightbox_->reset_camera_clipping_range();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::OnMouseMove() {
  int* clickPos = this->GetInteractor()->GetEventPosition();
  this->lightbox_->handle_hover(clickPos);

  vtkInteractorStyleTrackballCamera::OnMouseMove();
}

//-----------------------------------------------------------------------------
void StudioSliceInteractorStyle::Dolly(double factor) {
  if (this->CurrentRenderer == NULL) {
    return;
  }

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection()) {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
  } else {
    camera->Dolly(factor);
    if (this->AutoAdjustCameraClippingRange) {
      this->lightbox_->reset_camera_clipping_range();
    }
  }

  if (this->Interactor->GetLightFollowCamera()) {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }

  this->Interactor->Render();
}
*/
}  // namespace shapeworks
