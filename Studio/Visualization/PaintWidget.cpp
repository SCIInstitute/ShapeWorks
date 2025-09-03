
#include "PaintWidget.h"

#include <Logging.h>
#include <Visualization/StudioImageActorPointPlacer.h>

#include <QCursor>

#include "Viewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkEvent.h"
#include "vtkImageActor.h"
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
    sphere_source_ = vtkSmartPointer<vtkSphereSource>::New();
    sphere_source_->SetThetaResolution(128);
    sphere_source_->SetPhiResolution(128);
    sphere_source_->SetRadius(7);

    property_ = vtkSmartPointer<vtkProperty>::New();
    property_->SetColor(0.7, 0.7, 0.7);
    property_->SetOpacity(0.4);
    property_->SetLineWidth(0.1);
    property_->SetPointSize(3);
    property_->SetEdgeColor(1, 1, 1);

    plane1_ = vtkSmartPointer<vtkPlane>::New();
    plane2_ = vtkSmartPointer<vtkPlane>::New();

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
  void set_circle_mode(bool circle_mode) {
    if (circle_mode) {
      double position[3] = {0, 0, 0};  // Adjust these to match your application
      double normal1[3] = {0, 0, 1};   // Adjust to match your slice orientation
      double normal2[3] = {0, 0, -1};

      plane1_->SetOrigin(position);
      plane1_->SetNormal(normal1);

      plane2_->SetOrigin(position);
      plane2_->SetNormal(normal2);

      property_->SetOpacity(1.0);
      property_->LightingOff();
      property_->SetLineWidth(5.0);
      property_->SetEdgeVisibility(true);
      property_->SetRepresentationToWireframe();
      mapper_->RemoveAllClippingPlanes();
      mapper_->AddClippingPlane(plane1_);
      mapper_->AddClippingPlane(plane2_);

    } else {
      property_->LightingOn();
      property_->SetLineWidth(0.1);
      property_->SetRepresentationToSurface();
      mapper_->RemoveAllClippingPlanes();
    }
  }

  //----------------------------------------------------------------------
  void update_plane(vtkPlane* plane) {
    // set the two planes as shifted slightly from the slice plane along the normal
    double position[3];
    double normal[3];
    plane->GetOrigin(position);
    plane->GetNormal(normal);

    double shift = 1.0;
    double position1[3] = {position[0] + shift * normal[0], position[1] + shift * normal[1],
                           position[2] + shift * normal[2]};
    double position2[3] = {position[0] - shift * normal[0], position[1] - shift * normal[1],
                           position[2] - shift * normal[2]};

    plane1_->SetOrigin(position2);
    plane1_->SetNormal(normal);
    plane2_->SetOrigin(position1);
    // flip the normal
    normal[0] = -normal[0];
    normal[1] = -normal[1];
    normal[2] = -normal[2];
    plane2_->SetNormal(normal);
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

  vtkSmartPointer<vtkPlane> plane1_;
  vtkSmartPointer<vtkPlane> plane2_;
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
  CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, PaintWidget::MoveAction);

  /*
  CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent, vtkWidgetEvent::Translate, this,
                                    PaintWidget::StartEraseAction);
  CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent, vtkWidgetEvent::EndTranslate, this,
                                    PaintWidget::EndEraseAction);
*/
  CallbackMapper->SetCallbackMethod(vtkCommand::LeaveEvent, vtkWidgetEvent::Scale, this, PaintWidget::LeaveAction);

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

  if (!PointPlacer->ComputeWorldPosition(Renderer, displayPos, worldPos, worldOrient)) {
    LeaveAction(this);
    set_cursor(VTK_CURSOR_DEFAULT);
    return false;
  }

  if (circle_mode_) {
    // update planes
    vtkPlane* plane = viewer_->slice_view().get_slice_plane();
    sphere_cursor_->update_plane(plane);
  }

  WidgetState = newState;
  sphere_cursor_->set_position(worldPos);
  sphere_cursor_->set_visible(true);

  if (WidgetState == PaintWidget::Paint || WidgetState == PaintWidget::Erase) {
    if (WidgetState == PaintWidget::Paint) {
      if (circle_mode_) {
        // we have to transform the point back to the image space in case there is alignment
        auto transform = viewer_->get_inverse_image_transform();
        transform->TransformPoint(worldPos, worldPos);
      }

      viewer_->handle_paint(displayPos, worldPos);
    }

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
void PaintWidget::set_circle_mode(bool circle_mode) {
  circle_mode_ = circle_mode;
  sphere_cursor_->set_circle_mode(circle_mode);
}

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
