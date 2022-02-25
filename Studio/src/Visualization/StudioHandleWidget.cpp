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
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkWidgetEvent::Select, this,
                                          StudioHandleWidget::CustomSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent, vtkWidgetEvent::Scale, this,
                                          StudioHandleWidget::RightClickSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this,
                                          StudioHandleWidget::CustomMoveAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent, vtkWidgetEvent::EndSelect, this,
                                          StudioHandleWidget::CustomEndSelectAction);
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
    self->plane_widget_->handle_right_click(self->domain_, self->plane_, self->point_);
  }
}

//-----------------------------------------------------------------------------
void StudioHandleWidget::CustomMoveAction(vtkAbstractWidget *w) {
  StudioHandleWidget *self = reinterpret_cast<StudioHandleWidget *>(w);

  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // Set the cursor appropriately
  if (self->WidgetState == vtkHandleWidget::Start) {
    int state = self->WidgetRep->GetInteractionState();
    self->WidgetRep->ComputeInteractionState(X, Y);
    self->SetCursor(self->WidgetRep->GetInteractionState());
    // Must rerender if we change appearance
    if (reinterpret_cast<vtkHandleRepresentation *>(self->WidgetRep)->GetActiveRepresentation() &&
        state != self->WidgetRep->GetInteractionState()) {
      self->Render();
    }
    return;
  }

  if (!self->EnableTranslation) {
    return;
  }

  // if (self->Interactor->GetShiftKey()) {
  if (self->shift_active_) {
    int diff = Y - self->start_y_;
    std::cerr << "shift key active! diff = " << diff << "\n";
    if (self->plane_widget_) {
      self->plane_widget_->set_plane_offset(self->domain_, self->plane_, diff);
    }
    // Got this event, we are finished
    self->EventCallbackCommand->SetAbortFlag(1);
    self->Render();
    return;
  } else {
    // Okay, adjust the representation
    double eventPosition[2];
    eventPosition[0] = static_cast<double>(X);
    eventPosition[1] = static_cast<double>(Y);
    self->WidgetRep->WidgetInteraction(eventPosition);
  }
  // Got this event, we are finished
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
  self->Render();
}

//-----------------------------------------------------------------------------
void StudioHandleWidget::CustomSelectAction(vtkAbstractWidget *w) {
  StudioHandleWidget *self = reinterpret_cast<StudioHandleWidget *>(w);

  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  self->WidgetRep->ComputeInteractionState(X, Y);
  if (self->WidgetRep->GetInteractionState() == vtkHandleRepresentation::Outside) {
    return;
  }

  // We are definitely selected
  if (!self->Parent) {
    self->GrabFocus(self->EventCallbackCommand);
  }

  if (self->Interactor->GetShiftKey()) {
    self->start_y_ = Y;
    self->shift_active_ = true;
  }

  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetRep->StartWidgetInteraction(eventPos);

  self->WidgetState = vtkHandleWidget::Active;
  reinterpret_cast<vtkHandleRepresentation *>(self->WidgetRep)->SetInteractionState(vtkHandleRepresentation::Selecting);

  self->GenericAction(self);
}

//-----------------------------------------------------------------------------
void StudioHandleWidget::CustomEndSelectAction(vtkAbstractWidget *w) {
  StudioHandleWidget *self = reinterpret_cast<StudioHandleWidget *>(w);

  if (self->WidgetState != vtkHandleWidget::Active) {
    return;
  }

  // Return state to not selected
  self->WidgetState = vtkHandleWidget::Start;

  // Highlight as necessary
  self->WidgetRep->Highlight(0);

  // stop adjusting
  if (!self->Parent) {
    self->ReleaseFocus();
  }

  if (self->shift_active_) {
    // now re-cut and place landmarks
  }
  self->shift_active_ = false;
  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
  self->WidgetState = vtkHandleWidget::Start;
  self->Render();
}
}  // namespace shapeworks
