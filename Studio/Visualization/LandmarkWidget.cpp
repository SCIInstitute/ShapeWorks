#include <Data/Session.h>
#include <Visualization/LandmarkWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkFollower.h>
#include <vtkGlyph3D.h>
#include <vtkHandleWidget.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygonalHandleRepresentation3D.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <QColor>

namespace shapeworks {

//-----------------------------------------------------------------------------
class LandmarkCallback : public vtkCommand {
 public:
  LandmarkCallback() = default;

  static LandmarkCallback *New() { return new LandmarkCallback; }

  void Execute(vtkObject *caller, unsigned long, void *user_data) override { widget_->store_positions(); }

  void setWidget(LandmarkWidget *widget) { widget_ = widget; };

 private:
  LandmarkWidget *widget_;
};

//-----------------------------------------------------------------------------
LandmarkWidget::LandmarkWidget(Viewer *viewer) : viewer_(viewer) {
  sphere_ = vtkSmartPointer<vtkSphereSource>::New();
  callback_ = vtkSmartPointer<LandmarkCallback>::New();
  callback_->setWidget(this);

  glyph_points_ = vtkSmartPointer<vtkPoints>::New();
  glyph_points_->SetDataTypeToDouble();
  glyph_point_set_ = vtkSmartPointer<vtkPolyData>::New();
  glyph_point_set_->SetPoints(glyph_points_);
  glyph_point_set_->GetPointData()->SetScalars(vtkSmartPointer<vtkFloatArray>::New());

  glyphs_ = vtkSmartPointer<vtkGlyph3D>::New();
  glyphs_->SetInputData(glyph_point_set_);
  glyphs_->ScalingOn();
  glyphs_->ClampingOff();
  glyphs_->SetScaleModeToDataScalingOff();
  glyphs_->SetSourceConnection(sphere_->GetOutputPort());
  glyphs_->GeneratePointIdsOn();

  glyph_lut_ = vtkSmartPointer<vtkLookupTable>::New();
  glyph_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  glyph_mapper_->SetInputConnection(glyphs_->GetOutputPort());
  glyph_mapper_->SetLookupTable(glyph_lut_);

  glyph_actor_ = vtkSmartPointer<vtkActor>::New();
  glyph_actor_->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  glyph_actor_->GetProperty()->SetDiffuse(0.8);
  glyph_actor_->GetProperty()->SetSpecular(0.3);
  glyph_actor_->GetProperty()->SetSpecularPower(10.0);
  glyph_actor_->SetMapper(glyph_mapper_);

  glyphs_->SetColorModeToColorByScalar();
  glyphs_->SetScaleModeToDataScalingOff();
  glyph_mapper_->SetColorModeToMapScalars();
  glyph_mapper_->ScalarVisibilityOn();
}

//-----------------------------------------------------------------------------
LandmarkWidget::~LandmarkWidget() { clear_landmark_handles(); }

//-----------------------------------------------------------------------------
void LandmarkWidget::update_landmarks() {
  auto shape = viewer_->get_shape();

  if (is_glyph_mode()) {
    clear_landmark_handles();
    update_glyphs();
    return;
  }
  viewer_->get_renderer()->RemoveActor(glyph_actor_);

  if (!shape || !viewer_->get_session()->get_show_landmarks()) {
    clear_landmark_handles();
    return;
  }

  auto &landmarks = shape->landmarks();
  int num_points = landmarks.rows();

  while (handles_.size() < num_points) {
    handles_.push_back(create_handle());
  }

  while (handles_.size() > num_points) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_.pop_back();
  }
  auto session = viewer_->get_session();
  auto definitions = session->get_project()->get_all_landmark_definitions();
  auto domain_names = session->get_project()->get_domain_names();

  for (int i = 0; i < num_points; i++) {
    auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[i]->GetRepresentation());
    double xyz[3];
    int domain_id = landmarks(i, 0);
    int point_id = landmarks(i, 1);
    xyz[0] = landmarks(i, 2);
    xyz[1] = landmarks(i, 3);
    xyz[2] = landmarks(i, 4);

    double xyzt[3];
    auto transform = viewer_->get_landmark_transform(domain_id);
    transform->TransformPoint(xyz, xyzt);
    rep->SetWorldPosition(xyzt);
    rep->SetLabelVisibility(session->get_show_landmark_labels());
    rep->SetLabelText(definitions[domain_id][point_id].name_.c_str());
    rep->GetLabelTextActor()->GetProperty()->SetColor(1, 1, 1);

    assign_handle_to_domain(handles_[i], domain_id);

    QColor qcolor(QString::fromStdString(definitions[domain_id][point_id].color_));
    double color[3];
    color[0] = qcolor.red() / 255.0;
    color[1] = qcolor.green() / 255.0;
    color[2] = qcolor.blue() / 255.0;
    rep->GetProperty()->SetColor(color);
    rep->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
    rep->GetProperty()->SetDiffuse(0.8);
    rep->GetProperty()->SetSpecular(0.3);
    rep->GetProperty()->SetSpecularPower(10.0);

    bool enabled = is_drag_mode();
    bool visible = definitions[domain_id][point_id].visible_;

    if (!visible) {
      handles_[i]->SetShowInactive(0);
      handles_[i]->SetEnabled(1);
      handles_[i]->SetEnabled(0);
    } else {
      handles_[i]->SetShowInactive(1);
      handles_[i]->SetEnabled(1);
      handles_[i]->SetEnabled(0);
      if (enabled) {
        handles_[i]->SetEnabled(1);
      }
    }
  }
  update_glyph_properties();
}

//-----------------------------------------------------------------------------
void LandmarkWidget::store_positions() {
  auto shape = viewer_->get_shape();
  auto &landmarks = shape->landmarks();

  for (int i = 0; i < handles_.size(); i++) {
    auto *rep = vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[i]->GetRepresentation());

    double position[3];
    rep->GetWorldPosition(position);

    int domain_id = landmarks(i, 0);

    auto transform = viewer_->get_landmark_transform(domain_id);

    double xyzt[3];
    auto inverse = vtkSmartPointer<vtkTransform>::New();
    inverse->DeepCopy(transform);
    inverse->Inverse();
    inverse->TransformPoint(position, xyzt);

    landmarks(i, 2) = xyzt[0];
    landmarks(i, 3) = xyzt[1];
    landmarks(i, 4) = xyzt[2];
  }
}

//-----------------------------------------------------------------------------
void LandmarkWidget::update_glyph_properties() {
  sphere_->SetThetaResolution(viewer_->get_glyph_quality());
  sphere_->SetPhiResolution(viewer_->get_glyph_quality());
  sphere_->SetRadius(viewer_->get_glyph_size());
  sphere_->Update();
}

//-----------------------------------------------------------------------------
void LandmarkWidget::clear_landmark_handles() {
  while (!handles_.empty()) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_[handles_.size() - 1]->SetInteractor(nullptr);
    handles_.pop_back();
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkHandleWidget> LandmarkWidget::create_handle() {
  auto handle = vtkSmartPointer<vtkHandleWidget>::New();
  auto *rep = vtkPolygonalHandleRepresentation3D::New();

  static_cast<vtkPolygonalHandleRepresentation3D *>(rep)->SetHandle(sphere_->GetOutput());

  auto point_placer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  for (const auto &actor : viewer_->get_unclipped_surface_actors()) {
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
  rep->GetSelectedProperty()->SetColor(selectedColor);
  rep->Delete();

  handle->AddObserver(vtkCommand::InteractionEvent, callback_);
  handle->EnabledOn();
  return handle;
}

//-----------------------------------------------------------------------------
bool LandmarkWidget::is_drag_mode() {
  auto session = viewer_->get_session();

  if (!session->get_landmark_drag_mode() || !session->get_landmarks_active()) {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool LandmarkWidget::is_glyph_mode() {
  auto session = viewer_->get_session();

  if (session->get_landmarks_active() && (session->get_landmark_drag_mode() || session->get_show_landmark_labels())) {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
void LandmarkWidget::update_glyphs() {
  auto shape = viewer_->get_shape();
  if (!shape || !viewer_->get_session()->get_show_landmarks()) {
    viewer_->get_renderer()->RemoveActor(glyph_actor_);
    return;
  }
  auto &landmarks = shape->landmarks();
  int num_points = landmarks.rows();
  auto session = viewer_->get_session();

  auto definitions = session->get_project()->get_all_landmark_definitions();

  vtkFloatArray *scalars = (vtkFloatArray *)(glyph_point_set_->GetPointData()->GetScalars());

  glyph_points_->Reset();
  scalars->Reset();

  glyph_lut_->SetNumberOfColors(num_points);
  glyph_lut_->SetNumberOfTableValues(num_points);
  glyph_lut_->SetTableRange(0, num_points);
  glyph_mapper_->SetScalarRange(0, num_points);
  for (int i = 0; i < num_points; i++) {
    double xyz[3];
    int domain_id = landmarks(i, 0);
    int point_id = landmarks(i, 1);
    xyz[0] = landmarks(i, 2);
    xyz[1] = landmarks(i, 3);
    xyz[2] = landmarks(i, 4);

    bool visible = definitions[domain_id][point_id].visible_;
    if (!visible) {
      continue;
    }

    double xyzt[3];
    auto transform = viewer_->get_landmark_transform(domain_id);
    transform->TransformPoint(xyz, xyzt);
    scalars->InsertNextValue(i);
    glyph_points_->InsertNextPoint(xyzt[0], xyzt[1], xyzt[2]);

    QColor qcolor(QString::fromStdString(definitions[domain_id][point_id].color_));
    double color[3];
    color[0] = qcolor.red() / 255.0;
    color[1] = qcolor.green() / 255.0;
    color[2] = qcolor.blue() / 255.0;
    glyph_lut_->SetTableValue(i, color[0], color[1], color[2]);
  }

  viewer_->get_renderer()->AddActor(glyph_actor_);
}

//-----------------------------------------------------------------------------
void LandmarkWidget::assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id) {
  vtkPolygonalHandleRepresentation3D *rep =
      vtkPolygonalHandleRepresentation3D::SafeDownCast(handle->GetRepresentation());

  auto point_placer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  auto actors = viewer_->get_unclipped_surface_actors();
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
