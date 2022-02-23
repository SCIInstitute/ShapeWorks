#include <Data/Session.h>
#include <Visualization/PlaneWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkFollower.h>
#include <vtkHandleWidget.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalHandleRepresentation3D.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <QColor>

static const QColor colors[6] = {QColor(226, 149, 61), QColor(127, 165, 123), QColor(176, 64, 86),
                                 QColor(84, 78, 171),  QColor(205, 216, 86),  QColor(100, 129, 163)};
static constexpr int max_colors = 6;

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
void PlaneWidget::update_plane_points() {
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
      QColor qcolor = colors[plane_it];
      plane_it = (plane_it + 1) % max_colors;
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
        bool visible = true;                            // TODO: do we need visibility settings?
        if (!session->get_landmark_drag_mode() && 0) {  /// TODO plane version?
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
void PlaneWidget::update_planes() {

  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();

  int num_planes = count_complete_planes();

  // add if needed
  while (plane_sources_.size() < num_planes) {
    auto plane_source = vtkSmartPointer<vtkPlaneSource>::New();
    plane_sources_.push_back(plane_source);
    auto plane_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    plane_mapper->SetInputConnection(plane_source->GetOutputPort());
    plane_mappers_.push_back(plane_mapper);
    auto plane_actor = vtkSmartPointer<vtkActor>::New();
    plane_actor->SetMapper(plane_mapper);
    plane_actor->GetProperty()->SetOpacity(0.5);
    plane_actor->GetProperty()->SetColor(0, 0, 0.8);
    plane_actor->GetProperty()->SetEdgeColor(1.0, 1.0, 1.0);
    plane_actor->GetProperty()->SetLineWidth(4.0);
    plane_actor->GetProperty()->SetEdgeVisibility(1);
    plane_actor->SetPickable(false);
    plane_actors_.push_back(plane_actor);
    viewer_->get_renderer()->AddViewProp(plane_actor);
    std::cerr << "Added a plane!\n";
  }

  // remove if needed
  while (plane_sources_.size() > num_planes) {
    viewer_->get_renderer()->RemoveViewProp(plane_actors_[plane_actors_.size() - 1]);
    plane_actors_.pop_back();
    plane_mappers_.pop_back();
    plane_sources_.pop_back();
  }

  auto shape = viewer_->get_shape();

  int plane_it = 0;
  int plane_color = 0;
  for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    if (domain_id >= shape->constraints().size()) {
      continue;
    }
    auto &planes = shape->constraints()[domain_id].getPlaneConstraints();

    for (auto &plane : planes) {
      QColor qcolor = colors[plane_color];
      plane_color = (plane_color + 1) % max_colors;

      if (plane.points().size() < 3) {
        continue;
      }
      auto transform = viewer_->get_landmark_transform(domain_id);

      double xyzt[3];
      transform->TransformPoint(plane.getPlanePoint().data(), xyzt);
      double normal[3];
      transform->TransformVector(plane.getPlaneNormal().data(), normal);

      plane.updatePlaneFromPoints();
      auto tmp = vtkSmartPointer<vtkPlaneSource>::New();
      plane_sources_[plane_it] = tmp;
      plane_mappers_[plane_it]->SetInputConnection(tmp->GetOutputPort());
      plane_sources_[plane_it]->SetNormal(0, 0, 1);
      plane_sources_[plane_it]->SetPoint1(100, 0, 0);
      plane_sources_[plane_it]->SetPoint2(0, 100, 0);
      plane_sources_[plane_it]->SetNormal(plane.getPlaneNormal().data());
      plane_sources_[plane_it]->SetCenter(plane.getPlanePoint().data());
      plane_actors_[plane_it]->SetUserTransform(transform);
      plane_actors_[plane_it]->GetProperty()->SetColor(qcolor.red() / 255.0, qcolor.green() / 255.0,
                                                       qcolor.blue() / 255.0);

      plane_it++;
    }
  }
}

//-----------------------------------------------------------------------------
void PlaneWidget::update() {
  if (block_update_) {
    return;
  }
  update_plane_points();
  update_planes();
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

    auto inverse = viewer_->get_inverse_landmark_transform(domain_id);

    auto &planes = shape->constraints()[domain_id].getPlaneConstraints();

    for (auto &plane : planes) {
      for (auto &point : plane.points()) {
        auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[handle++]->GetRepresentation());

        double position[3];
        rep->GetWorldPosition(position);

        double xyzt[3];
        inverse->TransformPoint(position, xyzt);

        point[0] = xyzt[0];
        point[1] = xyzt[1];
        point[2] = xyzt[2];
      }
    }
  }
  update_planes();
  block_update_ = true;
  session->trigger_planes_changed();
  block_update_ = false;
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
  /// TODO: check ownership of this representation object
  auto *rep = vtkPolygonalHandleRepresentation3D::New();
  rep->SetHandle(sphere_->GetOutput());

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
  auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handle->GetRepresentation());

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
int PlaneWidget::count_complete_planes() {
  auto shape = viewer_->get_shape();

  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();

  int num_planes = 0;

  for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    if (domain_id < shape->constraints().size()) {
      auto &planes = shape->constraints()[domain_id].getPlaneConstraints();
      for (auto &plane : planes) {
        if (plane.points().size() == 3) {
          num_planes++;
        }
      }
    }
  }
  return num_planes;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
