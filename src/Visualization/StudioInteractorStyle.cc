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

  // Forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void StudioInteractorStyle::OnRightButtonDown()
{
  std::cerr << "Right button down!\n";
  int* clickPos = this->GetInteractor()->GetEventPosition();
  //this->viewer_->handle_pick( clickPos );

  this->lightbox_->handle_pick( clickPos );

  // Forward events
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void StudioInteractorStyle::set_lightbox( Lightbox* lightbox )
{
  this->lightbox_ = lightbox;
}
