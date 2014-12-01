#include <vtkObjectFactory.h>

#include <Visualization/Viewer.h>
#include <Visualization/StudioInteractorStyle.h>

vtkStandardNewMacro( StudioInteractorStyle );

StudioInteractorStyle::StudioInteractorStyle()
{
  LastPickedActor = NULL;
  LastPickedProperty = vtkProperty::New();
}

void StudioInteractorStyle::OnLeftButtonDown()
{

  std::cerr << "Left button down!\n";
  int* clickPos = this->GetInteractor()->GetEventPosition();

  // Pick from this location.
  vtkSmartPointer<vtkPropPicker>  picker =
    vtkSmartPointer<vtkPropPicker>::New();
  picker->Pick( clickPos[0], clickPos[1], 0, this->GetDefaultRenderer() );

  double* pos = picker->GetPickPosition();
  // If we picked something before, reset its property
  if ( this->LastPickedActor )
  {
    this->LastPickedActor->GetProperty()->DeepCopy( this->LastPickedProperty );
  }
  this->LastPickedActor = picker->GetActor();
  if ( this->LastPickedActor )
  {
    // Save the property of the picked actor so that we can
    // restore it next time
    this->LastPickedProperty->DeepCopy( this->LastPickedActor->GetProperty() );
    // Highlight the picked actor by changing its properties
    this->LastPickedActor->GetProperty()->SetColor( 1.0, 0.0, 0.0 );
    this->LastPickedActor->GetProperty()->SetDiffuse( 1.0 );
    this->LastPickedActor->GetProperty()->SetSpecular( 0.0 );
  }

  // Forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

StudioInteractorStyle::~StudioInteractorStyle()
{
  LastPickedProperty->Delete();
}

void StudioInteractorStyle::OnRightButtonDown()
{
  std::cerr << "Right button down!\n";
  int* clickPos = this->GetInteractor()->GetEventPosition();
  this->viewer_->handle_pick( clickPos );
}

void StudioInteractorStyle::set_viewer( Viewer* viewer )
{
  this->viewer_ = viewer;
}
