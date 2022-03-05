#include <Data/Session.h>
#include <Data/StudioLog.h>
#include <Visualization/PlaneWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkCutter.h>
#include <vtkFollower.h>
#include <vtkHandleWidget.h>
#include <vtkPlaneSource.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalHandleRepresentation3D.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkWidgetEventTranslator.h>

#include <QColor>
#include <QMenu>
#include <QtGui>

#include "StudioHandleWidget.h"

static const QColor colors[6] = {QColor(226, 149, 61), QColor(127, 165, 123), QColor(176, 64, 86),
                                 QColor(84, 78, 171),  QColor(205, 216, 86),  QColor(100, 129, 163)};
static constexpr int max_colors = 6;

namespace shapeworks {

namespace actions {
const QString FLIP_PLANE("Flip plane normal");
const QString DELETE_PLANE("Delete plane");
const QString APPLY_PLANE("Apply plane to all other shapes");
}  // namespace actions

//-----------------------------------------------------------------------------
class PlaneCallback : public vtkCommand {
 public:
  PlaneCallback() = default;

  static PlaneCallback *New() { return new PlaneCallback; }

  void Execute(vtkObject *caller, unsigned long event_id, void *user_data) override { widget_->store_positions(); }

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
  auto session = viewer_->get_session();

  if (!shape || !session->get_show_planes()) {
    clear_planes();
    return;
  }

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
    for (int plane_number = 0; plane_number < planes.size(); plane_number++) {
      auto &plane = planes[plane_number];
      QColor qcolor = colors[plane_it];
      plane_it = (plane_it + 1) % max_colors;
      for (int i = 0; i < plane.points().size(); i++) {
        auto &point = plane.points()[i];
        auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[handle]->GetRepresentation());
        double xyz[3];

        xyz[0] = point[0];
        xyz[1] = point[1];
        xyz[2] = point[2];

        double xyzt[3];
        auto transform = viewer_->get_landmark_transform(domain_id);
        transform->TransformPoint(xyz, xyzt);
        rep->SetWorldPosition(xyzt);
        rep->GetLabelTextActor()->GetProperty()->SetColor(1, 1, 1);

        assign_handle_to_domain(handles_[handle], domain_id);

        handles_[handle]->set_domain(domain_id);
        handles_[handle]->set_plane(plane_number);
        handles_[handle]->set_point(i);
        handles_[handle]->set_plane_widget(this);

        double color[3];
        color[0] = qcolor.red() / 255.0;
        color[1] = qcolor.green() / 255.0;
        color[2] = qcolor.blue() / 255.0;
        rep->GetProperty()->SetColor(color);

        bool enabled = true;
        bool visible = session->get_planes_active();

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
  }

  // remove if needed
  while (plane_sources_.size() > num_planes) {
    viewer_->get_renderer()->RemoveViewProp(plane_actors_[plane_actors_.size() - 1]);
    plane_actors_.pop_back();
    plane_mappers_.pop_back();
    plane_sources_.pop_back();
  }

  auto shape = viewer_->get_shape();

  if (!session->get_show_planes() || !shape) {
    return;
  }

  int plane_it = 0;
  int plane_color = 0;
  for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    if (domain_id >= shape->constraints().size()) {
      continue;
    }
    auto &planes = shape->constraints()[domain_id].getPlaneConstraints();

    double size = 100;
    auto meshes = viewer_->get_meshes();
    if (meshes.valid()) {
      size = meshes.meshes()[domain_id]->get_largest_dimension_size();
    } else {
      continue;
    }

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

      Eigen::Vector3d center = plane.getPlanePoint();
      center = center + plane.getPlaneNormal() * plane.getOffset() / get_offset_scale(domain_id);

      plane.updatePlaneFromPoints();
      auto tmp = vtkSmartPointer<vtkPlaneSource>::New();
      plane_sources_[plane_it] = tmp;
      plane_mappers_[plane_it]->SetInputConnection(tmp->GetOutputPort());
      plane_sources_[plane_it]->SetNormal(0, 0, 1);
      plane_sources_[plane_it]->SetPoint1(size, 0, 0);
      plane_sources_[plane_it]->SetPoint2(0, size, 0);
      plane_sources_[plane_it]->SetNormal(plane.getPlaneNormal().data());
      plane_sources_[plane_it]->SetCenter(center.data());
      plane_actors_[plane_it]->SetUserTransform(transform);
      plane_actors_[plane_it]->GetProperty()->SetColor(qcolor.red() / 255.0, qcolor.green() / 255.0,
                                                       qcolor.blue() / 255.0);
      viewer_->get_renderer()->AddViewProp(plane_actors_[plane_it]);

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
  // remove handles
  while (!handles_.empty()) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_[handles_.size() - 1]->SetInteractor(nullptr);
    handles_.pop_back();
  }
  // remove planes
  while (!plane_sources_.empty()) {
    viewer_->get_renderer()->RemoveViewProp(plane_actors_[plane_actors_.size() - 1]);
    plane_actors_.pop_back();
    plane_mappers_.pop_back();
    plane_sources_.pop_back();
  }
}

//-----------------------------------------------------------------------------
void PlaneWidget::handle_right_click(int domain, int plane, int point) {
  QMenu *menu = new QMenu(nullptr);
  menu->setAttribute(Qt::WA_DeleteOnClose);
  menu->addAction(actions::FLIP_PLANE)->setProperty("id", (int)1);
  menu->addAction(actions::DELETE_PLANE)->setProperty("id", (int)2);
  menu->addAction(actions::APPLY_PLANE)->setProperty("id", (int)2);
  menu->popup(QCursor::pos());

  auto session = viewer_->get_session();

  session->connect(menu, &QMenu::triggered, menu, [=](QAction *action) {
    std::cerr << "callback: " << action->text().toStdString() << domain << "," << plane << "," << point << "\n";
    if (action->text() == actions::FLIP_PLANE) {
      flip_plane(domain, plane);
    } else if (action->text() == actions::DELETE_PLANE) {
      delete_plane(domain, plane);
    } else if (action->text() == actions::APPLY_PLANE) {
      apply_plane(domain, plane);
    }
  });
}

//-----------------------------------------------------------------------------
void PlaneWidget::delete_plane(int domain, int plane_id) {
  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();
  auto shape = viewer_->get_shape();

  assert(domain < domain_names.size());
  auto &constraints = shape->get_constraints(domain);
  auto &planes = constraints.getPlaneConstraints();
  assert(plane_id < planes.size());
  planes.erase(planes.begin() + plane_id);
  session->trigger_planes_changed();
}

//-----------------------------------------------------------------------------
void PlaneWidget::flip_plane(int domain, int plane_id) {
  auto &plane = get_plane_reference(domain, plane_id);
  auto session = viewer_->get_session();
  auto &points = plane.points();

  if (points.size() != 3) {
    STUDIO_SHOW_ERROR("Plane doesn't have 3 points");
    return;
  }

  Eigen::Vector3d tmp = points[0];
  points[0] = points[2];
  points[2] = tmp;
  session->trigger_planes_changed();
}

//-----------------------------------------------------------------------------
void PlaneWidget::apply_plane(int domain, int plane_id) {
  auto &plane = get_plane_reference(domain, plane_id);
  auto session = viewer_->get_session();
  if (plane.points().size() != 3) {
    STUDIO_SHOW_ERROR("Can't apply plane that doesn't have 3 points");
    return;
  }

  for (auto &shape : session->get_shapes()) {
    if (shape == viewer_->get_shape()) {
      continue;
    }

    // transform from source shape to common space
    auto base_transform = viewer_->get_shape()->get_transform(domain);

    // transform from common space to destination space
    auto inverse = vtkSmartPointer<vtkTransform>::New();
    inverse->DeepCopy(shape->get_transform(domain));
    inverse->Inverse();

    // copy plane and transform plane
    PlaneConstraint new_plane = plane;
    for (int i = 0; i < 3; i++) {
      base_transform->TransformPoint(new_plane.points()[i].data(), new_plane.points()[i].data());
      inverse->TransformPoint(new_plane.points()[i].data(), new_plane.points()[i].data());
    }

    // update plane center and normal
    new_plane.updatePlaneFromPoints();

    shape->get_constraints(domain).getPlaneConstraints().push_back(new_plane);
  }

  session->trigger_planes_changed();
}

//-----------------------------------------------------------------------------
void PlaneWidget::set_plane_offset(int domain, int plane_id, int offset) {
  auto &plane = get_plane_reference(domain, plane_id);
  plane.setOffset(offset);
  update_planes();
}

//-----------------------------------------------------------------------------
void PlaneWidget::finalize_plane_offset(int domain, int plane_id) {
  auto &plane = get_plane_reference(domain, plane_id);
  auto vtk_plane = plane.getVTKPlane();
  auto meshes = viewer_->get_meshes();
  if (!meshes.valid() || domain >= meshes.meshes().size()) {
    return;
  }

  if (plane.points().size() != 3) {
    // silently revert
    return;
  }

  PlaneConstraint original = plane;

  MeshHandle mesh = meshes.meshes()[domain];
  auto poly_data = mesh->get_poly_data();

  Eigen::Vector3d center = plane.getPlanePoint();
  center = center + plane.getPlaneNormal() * plane.getOffset() / get_offset_scale(domain);
  vtk_plane->SetOrigin(center.data());

  auto cutter = vtkSmartPointer<vtkCutter>::New();
  cutter->SetCutFunction(vtk_plane);
  cutter->SetInputData(poly_data);
  cutter->Update();

  vtkSmartPointer<vtkPolyData> cut = cutter->GetOutput();
  if (cut->GetNumberOfPoints() != 0) {
    auto locator = vtkSmartPointer<vtkPointLocator>::New();
    locator->SetDataSet(cut);
    locator->BuildLocator();
    for (auto &point : plane.points()) {
      int id = locator->FindClosestPoint(point.data());
      cut->GetPoint(id, point.data());
    }
  }

  if (plane.points()[0] == plane.points()[1] || plane.points()[1] == plane.points()[2] ||
      plane.points()[0] == plane.points()[2]) {
    STUDIO_LOG_MESSAGE("Plane finalizing found co-incident points, reverting plane");
    plane = original;
  }

  // set the offset to 0
  plane.setOffset(0);
  update();
  viewer_->get_session()->trigger_planes_changed();
}

//-----------------------------------------------------------------------------
vtkSmartPointer<StudioHandleWidget> PlaneWidget::create_handle() {
  auto handle = vtkSmartPointer<StudioHandleWidget>::New();
  /// TODO: check ownership of this representation object
  auto *rep = vtkPolygonalHandleRepresentation3D::New();
  rep->SetHandle(sphere_->GetOutput());

  auto point_placer = viewer_->get_point_placer();
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
void PlaneWidget::assign_handle_to_domain(vtkSmartPointer<StudioHandleWidget> handle, int domain_id) {
  auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handle->GetRepresentation());

  auto point_placer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  auto actors = viewer_->get_clipped_surface_actors();
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
  if (!shape) {
    return 0;
  }

  auto session = viewer_->get_session();

  if (!session->get_show_planes()) {
    return 0;
  }

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
PlaneConstraint &PlaneWidget::get_plane_reference(int domain, int plane) {
  auto session = viewer_->get_session();
  auto domain_names = session->get_project()->get_domain_names();
  auto shape = viewer_->get_shape();

  assert(domain < domain_names.size());
  auto &constraints = shape->get_constraints(domain);
  auto &planes = constraints.getPlaneConstraints();
  assert(plane < planes.size());
  return planes[plane];
}

//-----------------------------------------------------------------------------
double PlaneWidget::get_offset_scale(int domain_id) {
  double size = 100;
  auto meshes = viewer_->get_meshes();
  if (meshes.valid()) {
    size = meshes.meshes()[domain_id]->get_largest_dimension_size();
  }

  auto window_size = viewer_->get_renderer()->GetRenderWindow()->GetSize();
  auto viewport = viewer_->get_renderer()->GetViewport();

  double viewport_height = viewport[3] - viewport[1];
  double window_height = window_size[1];

  double viewport_pixels = viewport_height * window_height;

  return viewport_pixels * (1.0 / size);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
