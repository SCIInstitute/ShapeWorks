#include <vtkObjectFactory.h>

#include <Visualization/Lightbox.h>
#include <Visualization/StudioInteractorStyle.h>

vtkStandardNewMacro( StudioInteractorStyle );

StudioInteractorStyle::StudioInteractorStyle()
{}

StudioInteractorStyle::~StudioInteractorStyle()
{}

void StudioInteractorStyle::OnLeftButtonDown()
{
/*
  std::cerr << "left button down\n";
  if ( this->Interactor->GetAltKey() )
  //if ( 1 )
  {
    std::cerr << "ctrl down too\n";
    int* clickPos = this->GetInteractor()->GetEventPosition();
    this->lightbox_->handle_pick( clickPos );
  }
*/
  // forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void StudioInteractorStyle::OnRightButtonDown()
{
/*
  int* clickPos = this->GetInteractor()->GetEventPosition();
  this->lightbox_->handle_pick( clickPos );
*/
  // forward events
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void StudioInteractorStyle::set_lightbox( Lightbox* lightbox )
{
  this->lightbox_ = lightbox;
}

void StudioInteractorStyle::OnMouseWheelForward()
{
  // do nothing so that it will be passed on to Qt
}

void StudioInteractorStyle::OnMouseWheelBackward()
{
  // do nothing so that it will be passed on to Qt
}

void StudioInteractorStyle::OnKeyDown()
{
  int* click_pos = this->GetInteractor()->GetEventPosition();

  char keycode = this->GetInteractor()->GetKeyCode();

  switch ( keycode )
  {
  case '1':
    this->lightbox_->handle_pick( click_pos, true );
    break;

  case '2':
    this->lightbox_->handle_pick( click_pos, false );
    break;
  }

  this->GetInteractor()->Render();

  // forward events
  vtkInteractorStyleTrackballCamera::OnKeyDown();
}
