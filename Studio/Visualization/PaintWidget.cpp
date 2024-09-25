
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
#include "vtkImageActorPointPlacer.h"
#include "vtkImageActor.h"

#include <Logging.h>

namespace shapeworks {

vtkStandardNewMacro(PaintWidget);

class StudioSphereRepresentation : public vtkWidgetRepresentation {
 public:
  //----------------------------------------------------------------------
  static StudioSphereRepresentation* New();

  //----------------------------------------------------------------------
  StudioSphereRepresentation() {
    sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();
    sphere_source_->SetThetaResolution(32);
    sphere_source_->SetPhiResolution(32);
    sphere_source_->SetRadius(7);

    property_ = vtkSmartPointer<vtkProperty>::New();
    property_->SetColor(0.7, 0.7, 0.7);
    property_->SetOpacity(0.4);
    property_->SetLineWidth(0.1);
    property_->SetPointSize(3);

    mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper_->SetInputConnection(sphere_source_->GetOutputPort());

    actor_ = vtkSmartPointer<vtkActor>::New();
    actor_->SetMapper(mapper_);
    actor_->SetProperty(property_);
    set_visible(false);
  }

  //----------------------------------------------------------------------
  virtual void BuildRepresentation() {}

  //----------------------------------------------------------------------
  virtual void GetActors(vtkPropCollection* pc) { actor_->GetActors(pc); }

  //----------------------------------------------------------------------
  virtual void ReleaseGraphicsResources(vtkWindow* win) { actor_->ReleaseGraphicsResources(win); }

  //----------------------------------------------------------------------
  virtual int RenderOverlay(vtkViewport* viewport) {
    if (actor_->GetVisibility()) {
      return actor_->RenderOverlay(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) {
    if (actor_->GetVisibility()) {
      return actor_->RenderTranslucentPolygonalGeometry(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  virtual int HasTranslucentPolygonalGeometry() { return actor_->HasTranslucentPolygonalGeometry(); }

  //----------------------------------------------------------------------
  virtual int RenderOpaqueGeometry(vtkViewport* viewport) {
    if (actor_->GetVisibility()) {
      return actor_->RenderOpaqueGeometry(viewport);
    }
    return 0;
  }

  //----------------------------------------------------------------------
  void set_size(double size) { sphere_source_->SetRadius(size); }

  //----------------------------------------------------------------------
  double get_size() { return sphere_source_->GetRadius(); }

  //----------------------------------------------------------------------
  void set_position(double pos[3]) { actor_->SetPosition(pos); }

  //----------------------------------------------------------------------
  void set_color(float r, float g, float b) { property_->SetColor(r, g, b); }

  //----------------------------------------------------------------------
  void set_visible(bool visible) {
    if (visible) {
      actor_->VisibilityOn();
    } else {
      actor_->VisibilityOff();
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
  mouse_in_window_ = false;
  ManagesCursor = 0;
  WidgetState = PaintWidget::Start;

  PointPlacer = NULL;
  Renderer = NULL;

  // These are the event callbacks supported by this widget
  CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent, vtkWidgetEvent::Select, this,
                                          PaintWidget::StartPaintAction);
  CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent, vtkWidgetEvent::EndSelect, this,
                                          PaintWidget::EndPaintAction);
  CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this,
                                          PaintWidget::MoveAction);

  CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent, vtkWidgetEvent::Translate, this,
                                          PaintWidget::StartEraseAction);
  CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent, vtkWidgetEvent::EndTranslate, this,
                                          PaintWidget::EndEraseAction);

  CallbackMapper->SetCallbackMethod(vtkCommand::LeaveEvent, vtkWidgetEvent::Scale, this,
                                          PaintWidget::LeaveAction);

  CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, 100, this, PaintWidget::KeyPressAction);

  CreateDefaultRepresentation();
}

//----------------------------------------------------------------------
PaintWidget::~PaintWidget() {}

//----------------------------------------------------------------------
void PaintWidget::CreateDefaultRepresentation() {
  if (!WidgetRep) {
    StudioSphereRepresentation* rep = StudioSphereRepresentation::New();
    WidgetRep = rep;
    sphere_cursor_ = rep;
  }
}

//----------------------------------------------------------------------
void PaintWidget::SetEnabled(int enabling) { Superclass::SetEnabled(enabling); }

//----------------------------------------------------------------------
bool PaintWidget::use_point_placer(double displayPos[2], int newState) {
  double worldPos[3];
  double worldOrient[9] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

  if (!Renderer->IsInViewport(displayPos[0], displayPos[1])) {
    return false;
  }

  // cast PointPlacer to vtkImageActorPointPlacer
  vtkImageActorPointPlacer* image_point_place = dynamic_cast<vtkImageActorPointPlacer*>(this->PointPlacer);
  if (image_point_place) {
    auto input = image_point_place->GetImageActor()->GetInput();
    if (!input) {
      SW_DEBUG("no input!?!!?!?");
    }


  }


  if (!PointPlacer->ComputeWorldPosition(Renderer, displayPos, worldPos, worldOrient)) {
    SW_DEBUG("point placer: leaving");
    LeaveAction(this);
    set_cursor(VTK_CURSOR_DEFAULT);
    return false;
  }

  WidgetState = newState;
  SW_DEBUG("use point placer: update position");
  sphere_cursor_->set_position(worldPos);
  sphere_cursor_->set_visible(true);

  if (WidgetState == PaintWidget::Paint || WidgetState == PaintWidget::Erase) {
    if (WidgetState == PaintWidget::Paint) {
      viewer_->handle_paint(displayPos, worldPos);
      ////paint_position( this, worldPos );
    } else if (WidgetState == PaintWidget::Erase) {
      ////erase_position( this, worldPos );
    }

    ///// update?

    EventCallbackCommand->SetAbortFlag(1);
  }

  Render();

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
  if (!GetEnabled()) {
    vtkErrorMacro(<< "Enable widget before initializing");
  }
}

//----------------------------------------------------------------------
void PaintWidget::PrintSelf(ostream& os, vtkIndent indent) {
  // Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  Superclass::PrintSelf(os, indent);

  os << indent << "WidgetState: " << WidgetState << endl;
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
    MoveAction(this);
  }
}
}  // namespace shapeworks
