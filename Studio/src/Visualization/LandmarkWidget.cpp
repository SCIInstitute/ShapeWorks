#include <Data/Session.h>
#include <Visualization/LandmarkWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCommand.h>
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
class LandmarkCallback : public vtkCommand {
 public:
  LandmarkCallback() = default;

  static LandmarkCallback *New() { return new LandmarkCallback; }

  void Execute(vtkObject *caller, unsigned long, void *user_data) override { widget_->update_positions(); }

  void setWidget(LandmarkWidget *widget) { widget_ = widget; };

 private:
  LandmarkWidget *widget_;
};

//-----------------------------------------------------------------------------
LandmarkWidget::LandmarkWidget(Viewer *viewer) : viewer_(viewer) {
  sphere_ = vtkSmartPointer<vtkSphereSource>::New();
  callback_ = vtkSmartPointer<LandmarkCallback>::New();
  callback_->setWidget(this);
}

//-----------------------------------------------------------------------------
LandmarkWidget::~LandmarkWidget() { clear_landmarks(); }

//-----------------------------------------------------------------------------
void LandmarkWidget::update_landmarks() {
  auto shape = viewer_->get_shape();

  if (!shape) {
    clear_landmarks();
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
  auto definitions = session->get_project()->get_landmarks();

  for (int i = 0; i < num_points; i++) {
    vtkPolygonalHandleRepresentation3D *rep =
        vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[i]->GetRepresentation());
    double xyz[3];
    xyz[0] = landmarks(i, 2);
    xyz[1] = landmarks(i, 3);
    xyz[2] = landmarks(i, 4);
    std::cerr << "pos[" << i << "] = " << xyz[0] << "," << xyz[1] << "," << xyz[2] << "\n";
    rep->SetWorldPosition(xyz);
    double color[3];
    QColor qcolor(QString::fromStdString(definitions[i].color_));
    color[0] = qcolor.red() / 255.0;
    color[1] = qcolor.green() / 255.0;
    color[2] = qcolor.blue() / 255.0;
    rep->GetProperty()->SetColor(color);
  }
  update_glyph_properties();
}

//-----------------------------------------------------------------------------
void LandmarkWidget::update_positions() {
  double position[3];
  auto shape = viewer_->get_shape();
  auto &landmarks = shape->landmarks();

  for (int i = 0; i < handles_.size(); i++) {
    vtkPolygonalHandleRepresentation3D *rep =
        vtkPolygonalHandleRepresentation3D::SafeDownCast(handles_[i]->GetRepresentation());
    rep->GetWorldPosition(position);

    landmarks(i, 2) = position[0];
    landmarks(i, 3) = position[1];
    landmarks(i, 4) = position[2];
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
void LandmarkWidget::clear_landmarks() {
  while (!handles_.empty()) {
    handles_[handles_.size() - 1]->SetEnabled(0);
    handles_[handles_.size() - 1]->SetInteractor(nullptr);
    handles_.pop_back();
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkHandleWidget> LandmarkWidget::create_handle() {
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

  // const char *label = "foo";
  // rep->SetLabelVisibility(1);
  // rep->SetLabelText(label);
  rep->Delete();

  handle->AddObserver(vtkCommand::InteractionEvent, callback_);
  handle->EnabledOn();
  return handle;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
