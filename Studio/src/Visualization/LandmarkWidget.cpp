#include <Data/Session.h>
#include <Visualization/LandmarkWidget.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
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
LandmarkWidget::LandmarkWidget(Viewer *viewer) : viewer_(viewer) { sphere_ = vtkSmartPointer<vtkSphereSource>::New(); }

//-----------------------------------------------------------------------------
void LandmarkWidget::update_landmarks() {
  sphere_->SetThetaResolution(viewer_->get_glyph_quality());
  sphere_->SetPhiResolution(viewer_->get_glyph_quality());
  sphere_->SetRadius(viewer_->get_glyph_size());
  sphere_->Update();

  auto shape = viewer_->get_shape();

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
    rep->SetWorldPosition(xyz);
    double color[3];
    QColor qcolor(QString::fromStdString(definitions[i].color_));
    color[0] = qcolor.red() / 255.0;
    color[1] = qcolor.green() / 255.0;
    color[2] = qcolor.blue() / 255.0;
    rep->GetProperty()->SetColor(color);
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkHandleWidget> LandmarkWidget::create_handle() {
  std::cerr << "create handle\n";

  auto handle = vtkSmartPointer<vtkHandleWidget>::New();

  vtkPolygonalHandleRepresentation3D *rep = vtkPolygonalHandleRepresentation3D::New();

  static_cast<vtkPolygonalHandleRepresentation3D *>(rep)->SetHandle(sphere_->GetOutput());

  auto pointPlacer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
  for (const auto &actor : viewer_->get_surface_actors()) {
    pointPlacer->AddProp(actor);
  }
  auto shape = viewer_->get_shape();
  auto meshes = viewer_->get_meshes();
  if (meshes.valid()) {
    for (size_t i = 0; i < meshes.meshes().size(); i++) {
      MeshHandle mesh = meshes.meshes()[i];
      vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

      pointPlacer->GetPolys()->AddItem(poly_data);
    }
  }
  pointPlacer->SetDistanceOffset(0);
  rep->SetPointPlacer(pointPlacer);

  handle->EnableAxisConstraintOff();

  vtkRenderWindowInteractor *iren = viewer_->get_renderer()->GetRenderWindow()->GetInteractor();
  handle->SetInteractor(iren);
  handle->SetRepresentation(rep);

  int foo = 24;

  // Set some defaults on the handle widget
  double color[3] = {((double)(foo % 4)) / 3.0, ((double)((foo + 3) % 7)) / 6.0, (double)(foo % 2)};
  double selectedColor[3] = {1.0, 0.0, 0.0};

  rep->GetProperty()->SetColor(color);
  rep->GetProperty()->SetLineWidth(1.0);
  rep->GetSelectedProperty()->SetColor(selectedColor);

  // const char *label = "foo";
  // rep->SetLabelVisibility(1);
  // rep->SetLabelText(label);
  rep->Delete();

  handle->EnabledOn();
  return handle;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
