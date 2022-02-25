#include "StudioHandleWidget.h"

#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>


#include "PlaneWidget.h"
#include "vtkEvent.h"
#include "vtkHandleRepresentation.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"

namespace shapeworks {

vtkStandardNewMacro(StudioHandleWidget);

//-----------------------------------------------------------------------------
StudioHandleWidget::StudioHandleWidget() {
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent, vtkWidgetEvent::Scale, this,
                                          StudioHandleWidget::RightClickSelectAction);
}

//-----------------------------------------------------------------------------
void StudioHandleWidget::RightClickSelectAction(vtkAbstractWidget *w) {
  StudioHandleWidget *self = reinterpret_cast<StudioHandleWidget *>(w);

  double eventPos[2];
  eventPos[0] = static_cast<double>(self->Interactor->GetEventPosition()[0]);
  eventPos[1] = static_cast<double>(self->Interactor->GetEventPosition()[1]);

  self->WidgetRep->StartWidgetInteraction(eventPos);
  if (self->WidgetRep->GetInteractionState() == vtkHandleRepresentation::Outside) {
    return;
  }

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();

  if (self->plane_widget_) {
    self->plane_widget_->handle_right_click(self->domain_,self->plane_,self->point_);
  }
}
}  // namespace shapeworks
