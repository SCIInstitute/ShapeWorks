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

  if ( this->Interactor->GetControlKey() )
  {
    int* clickPos = this->GetInteractor()->GetEventPosition();
    this->lightbox_->handle_pick( clickPos );
  }

  // Forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void StudioInteractorStyle::OnRightButtonDown()
{
  std::cerr << "Right button down!\n";

  // Forward events
  vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

void StudioInteractorStyle::set_lightbox( Lightbox* lightbox )
{
  this->lightbox_ = lightbox;
}
