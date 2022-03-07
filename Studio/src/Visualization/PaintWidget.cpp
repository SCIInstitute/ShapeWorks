
#include "PaintWidget.h"

#include <QCursor>

#include "Viewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkEvent.h"
#include "vtkObjectFactory.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkPointPlacer.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolygonalSurfacePointPlacer.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkWidgetEvent.h"

namespace shapeworks {

vtkStandardNewMacro(PaintWidget);

class StudioSphereRepresentation : public vtkWidgetRepresentation {
 public:
  //----------------------------------------------------------------------
  static StudioSphereRepresentation* New();

  //----------------------------------------------------------------------
  StudioSphereRepresentation() {
    this->sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();
    this->sphere_source_->SetThetaResolution(32);
    this->sphere_source_->SetPhiResolution(32);
    this->sphere_source_->SetRadius(7);

    this->property_ = vtkSmartPointer<vtkProperty>::New();
    this->property_->SetColor(0.7, 0.7, 0.7);
    this->property_->SetOpacity(0.4);
    this->property_->SetLineWidth(0.1);
    this->property_->SetPointSize(3);
    // this->property_->SetRepresentationToWireframe();

    this->mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    this->mapper_->SetInputConnection(this->sphere_source_->GetOutputPort());

    this->actor_ = vtkSmartPointer<vtkActor>::New();
    this->actor_->SetMapper(this->mapper_);
    this->actor_->SetProperty(this->property_);
    this->set_visible(false);
  }

  //----------------------------------------------------------------------
  virtual void BuildRepresentation() {}

  //----------------------------------------------------------------------
  virtual void GetActors(vtkPropCollection* pc) { this->actor_->GetActors(pc); }

  //----------------------------------------------------------------------
  virtual void ReleaseGraphicsResources(vtkWindow* win) { this->actor_->ReleaseGraphicsResources(win); }

  //----------------------------------------------------------------------
  virtual int RenderOverlay(vtkViewport* viewport) {
    if (this->actor_->GetVisibility()) {
      return this->actor_->RenderOverlay(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) {
    if (this->actor_->GetVisibility()) {
      return this->actor_->RenderTranslucentPolygonalGeometry(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  virtual int HasTranslucentPolygonalGeometry() { return this->actor_->HasTranslucentPolygonalGeometry(); }

  //----------------------------------------------------------------------
  virtual int RenderOpaqueGeometry(vtkViewport* viewport) {
    if (this->actor_->GetVisibility()) {
      return this->actor_->RenderOpaqueGeometry(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  void set_size(double size) { this->sphere_source_->SetRadius(size); }

  //----------------------------------------------------------------------
  double get_size() { return this->sphere_source_->GetRadius(); }

  //----------------------------------------------------------------------
  void set_position(double pos[3]) { this->actor_->SetPosition(pos); }

  //----------------------------------------------------------------------
  void set_color(float r, float g, float b) { this->property_->SetColor(r, g, b); }

  //----------------------------------------------------------------------
  void set_visible(bool visible) {
    if (visible) {
      this->actor_->VisibilityOn();
    } else {
      this->actor_->VisibilityOff();
    }
  }

 private:
  vtkSmartPointer<vtkActor> actor_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkSphereSource> sphere_source_;
  vtkSmartPointer<vtkProperty> property_;
};

vtkStandardNewMacro(StudioSphereRepresentation);

//----------------------------------------------------------------------
PaintWidget::PaintWidget() {
  this->mouse_in_window_ = false;
  this->ManagesCursor = 0;
  this->WidgetState = PaintWidget::Start;

  this->PointPlacer = NULL;
  this->Renderer = NULL;

  // These are the event callbacks supported by this widget
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkWidgetEvent::Select, this,
                                          PaintWidget::StartPaintAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent, vtkWidgetEvent::EndSelect, this,
                                          PaintWidget::EndPaintAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this,
                                          PaintWidget::MoveAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent, vtkWidgetEvent::Translate, this,
                                          PaintWidget::StartEraseAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent, vtkWidgetEvent::EndTranslate, this,
                                          PaintWidget::EndEraseAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeaveEvent, vtkWidgetEvent::Scale, this,
                                          PaintWidget::LeaveAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, 100, this, PaintWidget::KeyPressAction);

  // this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyReleaseEvent,
  // 100, this, PaintWidget::KeyPressAction);

  this->CreateDefaultRepresentation();
}

//----------------------------------------------------------------------
PaintWidget::~PaintWidget() {}

//----------------------------------------------------------------------
void PaintWidget::CreateDefaultRepresentation() {
  if (!this->WidgetRep) {
    StudioSphereRepresentation* rep = StudioSphereRepresentation::New();
    this->WidgetRep = rep;
    this->sphere_cursor_ = rep;
  }
}

//----------------------------------------------------------------------
void PaintWidget::SetEnabled(int enabling) { this->Superclass::SetEnabled(enabling); }

//----------------------------------------------------------------------
bool PaintWidget::use_point_placer(double displayPos[2], int newState) {
  double worldPos[3];
  double worldOrient[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

  if (!this->Renderer->IsInViewport(displayPos[0], displayPos[1])) {
    return false;
  }

  if (!this->PointPlacer->ComputeWorldPosition(this->Renderer, displayPos, worldPos, worldOrient)) {
    this->LeaveAction(this);
    this->set_cursor(VTK_CURSOR_DEFAULT);
    return false;
  }

  this->WidgetState = newState;
  this->sphere_cursor_->set_position(worldPos);
  this->sphere_cursor_->set_visible(true);

  if (this->WidgetState == PaintWidget::Paint || this->WidgetState == PaintWidget::Erase) {
    if (this->WidgetState == PaintWidget::Paint) {
      viewer_->handle_ffc_paint(displayPos, worldPos);
      ////paint_position( this, worldPos );
    } else if (this->WidgetState == PaintWidget::Erase) {
      ////erase_position( this, worldPos );
    }

    ///// update?

    this->EventCallbackCommand->SetAbortFlag(1);
  }

  this->Render();

  return true;
}

//-------------------------------------------------------------------------
void PaintWidget::StartPaintAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);

  if (self->Interactor->GetShiftKey() || self->Interactor->GetAltKey() || self->Interactor->GetControlKey()) {
    return;
  }

  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  double pos[2];
  pos[0] = X;
  pos[1] = Y;

  if (self->WidgetState != PaintWidget::Erase) {
    self->use_point_placer(pos, PaintWidget::Paint);
  } else {
    self->EventCallbackCommand->SetAbortFlag(1);
  }
}

//-------------------------------------------------------------------------
void PaintWidget::EndPaintAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);

  if (self->WidgetState == PaintWidget::Paint) {
    self->WidgetState = PaintWidget::Start;
  }
}

//-------------------------------------------------------------------------
void PaintWidget::MoveAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);
  self->mouse_in_window_ = true;

  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  double pos[2];
  pos[0] = X;
  pos[1] = Y;

  self->use_point_placer(pos, self->WidgetState);
}

//----------------------------------------------------------------------
void PaintWidget::StartEraseAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);
  if (self->Interactor->GetShiftKey() || self->Interactor->GetAltKey() || self->Interactor->GetControlKey()) {
    return;
  }

  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  double pos[2];
  pos[0] = X;
  pos[1] = Y;

  if (self->WidgetState != PaintWidget::Paint) {
    self->use_point_placer(pos, PaintWidget::Erase);
  } else {
    self->EventCallbackCommand->SetAbortFlag(1);
  }
}

//----------------------------------------------------------------------
void PaintWidget::EndEraseAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);

  if (self->WidgetState == PaintWidget::Erase) {
    self->WidgetState = PaintWidget::Start;
  }
  return;
}

//----------------------------------------------------------------------
void PaintWidget::LeaveAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);
  self->sphere_cursor_->set_visible(false);
  self->Render();
  self->mouse_in_window_ = false;
}

//----------------------------------------------------------------------
void PaintWidget::KeyPressAction(vtkAbstractWidget* w) {
  PaintWidget* self = reinterpret_cast<PaintWidget*>(w);

  char key = self->Interactor->GetKeyCode();

  if (key == '[') {
    //// decrease paint size?
  } else if (key == ']') {
    //// increase paint size?
  }
}

//----------------------------------------------------------------------
void PaintWidget::Initialize(vtkPolyData* pd, int state) {
  if (!this->GetEnabled()) {
    vtkErrorMacro(<< "Enable widget before initializing");
  }
}

//----------------------------------------------------------------------
void PaintWidget::PrintSelf(ostream& os, vtkIndent indent) {
  // Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os, indent);

  os << indent << "WidgetState: " << this->WidgetState << endl;
}

//----------------------------------------------------------------------
void PaintWidget::set_viewer(Viewer* viewer) { viewer_ = viewer; }

//----------------------------------------------------------------------
void PaintWidget::set_brush_size(double size) { sphere_cursor_->set_size(size); }

//----------------------------------------------------------------------
double PaintWidget::get_brush_size() { return sphere_cursor_->get_size(); }

//----------------------------------------------------------------------
void PaintWidget::set_brush_color(float r, float g, float b) { sphere_cursor_->set_color(r, g, b); }

//----------------------------------------------------------------------
void PaintWidget::set_cursor(int requestedShape) { return; }

//----------------------------------------------------------------------
void PaintWidget::update_position() {
  if (mouse_in_window_) {
    this->MoveAction(this);
  }
}
}  // namespace shapeworks
