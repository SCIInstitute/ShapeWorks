#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <Visualization/Lightbox.h>
#include <Visualization/StudioInteractorStyle.h>

namespace shapeworks {

vtkStandardNewMacro(StudioInteractorStyle);

//-----------------------------------------------------------------------------
StudioInteractorStyle::StudioInteractorStyle()
{}

//-----------------------------------------------------------------------------
StudioInteractorStyle::~StudioInteractorStyle()
{}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnLeftButtonDown()
{
   if ( this->Interactor->GetControlKey() )
   {
    int* clickPos = this->GetInteractor()->GetEventPosition();
    this->lightbox_->handle_pick( clickPos, true, true );
    return;
   }

// forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  //this->GetCurrentRenderer()->ResetCameraClippingRange();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnRightButtonDown()
{
/*
   int* clickPos = this->GetInteractor()->GetEventPosition();
   this->lightbox_->handle_pick( clickPos );
 */
// forward events
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
  //this->GetCurrentRenderer()->ResetCameraClippingRange();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::set_lightbox(Lightbox* lightbox)
{
  this->lightbox_ = lightbox;
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseWheelForward()
{
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseWheelBackward()
{
  // do nothing so that it will be passed on to Qt
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnKeyDown()
{
  int* click_pos = this->GetInteractor()->GetEventPosition();

  char keycode = this->GetInteractor()->GetKeyCode();

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

  this->GetInteractor()->Render();

  // forward events
  vtkInteractorStyleTrackballCamera::OnKeyDown();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::Dolly()
{
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
void StudioInteractorStyle::Rotate()
{
  vtkInteractorStyleTrackballCamera::Rotate();
  this->lightbox_->reset_camera_clipping_range();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::OnMouseMove()
{
  int* clickPos = this->GetInteractor()->GetEventPosition();
  this->lightbox_->handle_hover( clickPos );

  vtkInteractorStyleTrackballCamera::OnMouseMove();
}

//-----------------------------------------------------------------------------
void StudioInteractorStyle::Dolly(double factor)
{
  if (this->CurrentRenderer == NULL) {
    return;
  }

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection()) {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
  }
  else {
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
}
