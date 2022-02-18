#include <Data/Session.h>
#include <Visualization/PlaneWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkFollower.h>
#include <vtkHandleWidget.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolygonalHandleRepresentation3D.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <QColor>

namespace shapeworks {

//-----------------------------------------------------------------------------
class PlaneCallback : public vtkCommand {
 public:
  PlaneCallback() = default;

  static PlaneCallback *New() { return new PlaneCallback; }

  void Execute(vtkObject *caller, unsigned long, void *user_data) override { widget_->store_positions(); }

  void setWidget(PlaneWidget *widget) { widget_ = widget; };

 private:
  PlaneWidget *widget_;
};

//-----------------------------------------------------------------------------
PlaneWidget::PlaneWidget(Viewer *viewer) : viewer_(viewer) {
  sphere_ = vtkSmartPointer<vtkSphereSource>::New();
  callback_ = vtkSmartPointer<PlaneCallback>::New();
  callback_->setWidget(this);
}

//-----------------------------------------------------------------------------
PlaneWidget::~PlaneWidget() { clear_planes(); }

//-----------------------------------------------------------------------------
void PlaneWidget::update_planes() {
  auto shape = viewer_->get_shape();

  if (!shape || !viewer_->get_show_landmarks()) {
    clear_planes();
    return;
  }

  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();

  int num_points = 0;

  for (int i = 0; i < domain_names.size(); i++) {
    if (i < shape->constraints().size()) {
      auto &planes = shape->constraints()[i].getPlaneConstraints();
      for (auto &plane : planes) {
        num_points += plane.points().size();
      }
    }
  }

  while (handles_.size() < num_points) {
    handles_.push_back(create_handle());
  }

  while (handles_.size() > num_points) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_.pop_back();
  }

  int handle = 0;
  for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    if (domain_id >= shape->constraints().size()) {
      continue;
    }
    auto &planes = shape->constraints()[domain_id].getPlaneConstraints();

    int plane_it = 0;
    for (auto &plane : planes) {
      QColor colors[6] = {QColor(255, 0, 0),   QColor(0, 255, 0),    QColor(0, 0, 255),
                          QColor(255, 128, 0), QColor(255, 64, 255), QColor(0, 128, 64)};
      QColor qcolor = colors[plane_it];
      plane_it = (plane_it + 1) % 6;
      for (auto &point : plane.points()) {
        auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[handle]->GetRepresentation());
        double xyz[3];

        xyz[0] = point[0];
        xyz[1] = point[1];
        xyz[2] = point[2];

        double xyzt[3];
        auto transform = viewer_->get_landmark_transform(domain_id);
        transform->TransformPoint(xyz, xyzt);
        rep->SetWorldPosition(xyzt);
        rep->SetLabelVisibility(session->get_show_landmarks());
        // rep->SetLabelText(definitions[domain_id][point_id].name_.c_str());
        rep->GetLabelTextActor()->GetProperty()->SetColor(1, 1, 1);

        assign_handle_to_domain(handles_[handle], domain_id);

        // QColor qcolor(Qt::green);
        double color[3];
        color[0] = qcolor.red() / 255.0;
        color[1] = qcolor.green() / 255.0;
        color[2] = qcolor.blue() / 255.0;
        rep->GetProperty()->SetColor(color);

        bool enabled = true;
        // bool visible = definitions[domain_id][point_id].visible_;
        bool visible = true;                       // TODO: do we need visibility settings?
        if (!session->get_landmark_drag_mode()) {  /// TODO plane version?
          enabled = false;
        }

        if (!visible) {
          handles_[handle]->SetShowInactive(0);
          handles_[handle]->SetEnabled(1);
          handles_[handle]->SetEnabled(0);
        } else {
          handles_[handle]->SetShowInactive(1);
          handles_[handle]->SetEnabled(1);
          handles_[handle]->SetEnabled(0);
          if (enabled) {
            handles_[handle]->SetEnabled(1);
          }
        }
        handle++;
      }
    }
  }
  update_glyph_properties();
}

//-----------------------------------------------------------------------------
void PlaneWidget::store_positions() {
  auto shape = viewer_->get_shape();

  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();

  int handle = 0;
  for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    if (domain_id >= shape->constraints().size()) {
      continue;
    }

    auto &planes = shape->constraints()[domain_id].getPlaneConstraints();

    for (auto &plane : planes) {
      for (auto &point : plane.points()) {
        vtkPolygonalHandleRepresentation3D *rep =
            vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[handle++]->GetRepresentation());

        double position[3];
        rep->GetWorldPosition(position);

        auto transform = viewer_->get_landmark_transform(domain_id);

        double xyzt[3];
        auto inverse = vtkSmartPointer<vtkTransform>::New();
        inverse->DeepCopy(transform);
        inverse->Inverse();
        inverse->TransformPoint(position, xyzt);

        point[0] = xyzt[0];
        point[1] = xyzt[1];
        point[2] = xyzt[2];
      }
    }
  }
}

//-----------------------------------------------------------------------------
void PlaneWidget::update_glyph_properties() {
  sphere_->SetThetaResolution(viewer_->get_glyph_quality());
  sphere_->SetPhiResolution(viewer_->get_glyph_quality());
  sphere_->SetRadius(viewer_->get_glyph_size());
  sphere_->Update();
}

//-----------------------------------------------------------------------------
void PlaneWidget::clear_planes() {
  while (!handles_.empty()) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_[handles_.size() - 1]->SetInteractor(nullptr);
    handles_.pop_back();
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkHandleWidget> PlaneWidget::create_handle() {
  auto handle = vtkSmartPointer<vtkHandleWidget>::New();

  vtkPolygonalHandleRepresentation3D *rep = vtkPolygonalHandleRepresentation3D::New();

  static_cast<vtkPolygonalHandleRepresentation3D *>(rep)->SetHandle(sphere_->GetOutput());

  auto point_placer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  for (const auto &actor : viewer_->get_surface_actors()) {
    point_placer->AddProp(actor);
  }
  auto shape = viewer_->get_shape();
  auto meshes = viewer_->get_meshes();
  if (meshes.valid()) {
    for (size_t i = 0; i < meshes.meshes().size(); i++) {
      MeshHandle mesh = meshes.meshes()[i];
      vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
      point_placer->GetPolys()->AddItem(poly_data);
    }
  }
  point_placer->SetDistanceOffset(0);
  rep->SetPointPlacer(point_placer);

  handle->EnableAxisConstraintOff();

  vtkRenderWindowInteractor *iren = viewer_->get_renderer()->GetRenderWindow()->GetInteractor();
  handle->SetDefaultRenderer(viewer_->get_renderer());
  handle->SetInteractor(iren);
  handle->SetRepresentation(rep);
  handle->SetAllowHandleResize(false);

  double selectedColor[3] = {1.0, 1.0, 1.0};

  rep->GetProperty()->SetLineWidth(1.0);
  rep->GetSelectedProperty()->SetColor(selectedColor);
  rep->Delete();

  handle->AddObserver(vtkCommand::InteractionEvent, callback_);
  handle->EnabledOn();
  return handle;
}

//-----------------------------------------------------------------------------
void PlaneWidget::assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id) {
  vtkPolygonalHandleRepresentation3D *rep =
      vtkPolygonalHandleRepresentation3D::SafeDownCast(handle->GetRepresentation());

  auto point_placer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  auto actors = viewer_->get_surface_actors();
  if (domain_id < actors.size()) {
    point_placer->AddProp(actors[domain_id]);
  }

  auto shape = viewer_->get_shape();
  auto meshes = viewer_->get_meshes();
  if (meshes.valid() && domain_id < meshes.meshes().size()) {
    MeshHandle mesh = meshes.meshes()[domain_id];
    vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();
    point_placer->GetPolys()->AddItem(poly_data);
  }
  point_placer->SetDistanceOffset(0);
  rep->SetPointPlacer(point_placer);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
