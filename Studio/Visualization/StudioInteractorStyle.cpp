#include <Logging.h>
#include <Visualization/Lightbox.h>
#include <Visualization/StudioInteractorStyle.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>

namespace shapeworks {

vtkStandardNewMacro(StudioInteractorStyle);

//-----------------------------------------------------------------------------
StudioInteractorStyle::StudioInteractorStyle() {}

//-----------------------------------------------------------------------------
StudioInteractorStyle::~StudioInteractorStyle() {}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnLeftButtonDown() {
  if (this->Interactor->GetControlKey()) {
    int* clickPos = this->GetInteractor()->GetEventPosition();
    this->lightbox_->handle_pick(clickPos, true, true);
    return;
  }

  // forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnRightButtonDown() {
  right_button_down_ = true;
  right_click_dragged_ = false;
  // forward events
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnRightButtonUp() {
  right_button_down_ = true;
  vtkInteractorStyleTrackballCamera::OnRightButtonUp();
  if (!right_click_dragged_) {
    int* pos = GetInteractor()->GetEventPosition();
    vtkRenderer* renderer = GetInteractor()->FindPokedRenderer(pos[0], pos[1]);
    lightbox_->handle_right_click(pos, renderer);
  }
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::set_lightbox(Lightbox* lightbox) { this->lightbox_ = lightbox; }

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseWheelForward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseWheelBackward() {
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnKeyDown() {
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
void StudioInteractorStyle::Dolly() {
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
void StudioInteractorStyle::Rotate() {
  vtkInteractorStyleTrackballCamera::Rotate();
  this->lightbox_->reset_camera_clipping_range();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseMove() {
  int* clickPos = this->GetInteractor()->GetEventPosition();
  this->lightbox_->handle_hover(clickPos);

  if (right_button_down_) {
    right_click_dragged_ = true;
  }
  vtkInteractorStyleTrackballCamera::OnMouseMove();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::Dolly(double factor) {
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
}  // namespace shapeworks
