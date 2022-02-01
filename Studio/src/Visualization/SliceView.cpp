// vtk
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCutter.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkNamedColors.h>

// shapeworks
#include <Visualization/SliceView.h>
#include <Visualization/Viewer.h>

namespace shapeworks {
//-----------------------------------------------------------------------------
SliceView::SliceView(Viewer *viewer) : viewer_(viewer) {
  image_slice_ = vtkSmartPointer<vtkImageSlice>::New();
  slice_mapper_ = vtkSmartPointer<vtkImageSliceMapper>::New();

  cut_transform_filter_ = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  cutter_ = vtkSmartPointer<vtkCutter>::New();
  stripper_ = vtkSmartPointer<vtkStripper>::New();
  cut_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  cut_actor_ = vtkSmartPointer<vtkActor>::New();
}

//-----------------------------------------------------------------------------
void SliceView::set_volume(vtkSmartPointer<vtkImageData> volume) {
  volume_ = volume;
  slice_mapper_->SetInputData(volume_);
  image_slice_->SetMapper(slice_mapper_);

  auto transform = viewer_->get_image_transform();
  image_slice_->SetUserTransform(transform);

  MeshGroup mesh_group = viewer_->get_meshes();
  if (!mesh_group.valid()) {
    return;
  }
  auto poly_data = mesh_group.meshes()[0]->get_poly_data();
  cut_transform_filter_->SetInputData(poly_data);
  cut_transform_filter_->SetTransform(transform);

  cutter_->SetInputConnection(cut_transform_filter_->GetOutputPort());
  cutter_->SetCutFunction(slice_mapper_->GetSlicePlane());

  stripper_->SetInputConnection(cutter_->GetOutputPort());
  stripper_->PassCellDataAsFieldDataOn();
  // stripper_->
  stripper_->Update();

  cut_mapper_->SetInputConnection(stripper_->GetOutputPort());
  // cut_mapper_->SetColorModeToDefault()
  // cut_mapper_->SetScalarModeToUseFieldData();
  // cut_mapper_->SelectColorArray(0);
  // cut_mapper_->SetLookupTable(poly_data->GetCellData()->GetScalars()->GetLookupTable());
  // cut_mapper_->SetColorModeToMapScalars();
  // cut_mapper_->SetColorModeToDefault();
  // cut_mapper_->UseLookupTableScalarRangeOn();
  // cut_mapper_->ScalarVisibilityOff();
  cut_mapper_->ScalarVisibilityOff();
  cut_mapper_->SetScalarVisibility(false);

  //cut_actor_->GetProperty()->SetLineWidth(15);
  // cut_actor_->GetProperty()->SetDiffuseColor(1,1,0.25);
  //cut_actor_->GetProperty()->SetColor(1, 1, 0.25);
  //cut_actor_->GetProperty()->SetEdgeColor(1, 1, 0.25);

  cut_actor_->SetMapper(cut_mapper_);

  //cut_actor_->GetProperty()->SetDiffuseColor(247.0 / 255.0, 150.0 / 255.0, 155.0 / 255.0);  //  # Look like red
  //cut_actor_->GetProperty()->SetSpecular(0.3);
  //cut_actor_->GetProperty()->SetSpecularPower(20);
  vtkNew<vtkNamedColors> colors;
  cut_actor_->GetProperty()->SetColor(1, 1, 0.25);
//  cut_actor_->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
  cut_actor_->GetProperty()->SetLineWidth(3);
  cut_actor_->GetProperty()->SetAmbient(1.0);
  cut_actor_->GetProperty()->SetDiffuse(0.0);
}

//-----------------------------------------------------------------------------
bool SliceView::is_image_loaded() { return volume_ != nullptr; }

//-----------------------------------------------------------------------------
void SliceView::update_renderer() {
  auto renderer = viewer_->get_renderer();
  if (is_image_loaded()) {
    renderer->AddActor(image_slice_);
    renderer->AddActor(cut_actor_);
  } else {
    renderer->RemoveViewProp(image_slice_);
    renderer->RemoveViewProp(cut_actor_);
  }
}

//-----------------------------------------------------------------------------
void SliceView::update_camera() {
  auto renderer = viewer_->get_renderer();
  if (!is_image_loaded()) {
    renderer->GetActiveCamera()->SetParallelProjection(0);
    return;
  }

  double origin[3];
  int dims[3];
  double spacing[3];
  volume_->GetOrigin(origin);
  volume_->GetDimensions(dims);
  volume_->GetSpacing(spacing);
  int max_slice_num = dims[2];
  std::cout << "dims: " << dims[0] << "\t" << dims[1] << "\t" << dims[2] << "\n";
  std::cout << "spaces: " << spacing[0] << "\t" << spacing[1] << "\t" << spacing[2] << "\n";
  std::cout << "max slice number: " << max_slice_num << "\n";

  double center_x = (spacing[0] * dims[0] / 2) + origin[0];
  double center_y = (spacing[1] * dims[1] / 2) + origin[1];

  renderer->GetActiveCamera()->SetPosition(center_x, center_y, spacing[2] * (max_slice_num + 1));
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->GetActiveCamera()->SetFocalPoint(center_x, center_y, 0);
  renderer->GetActiveCamera()->SetParallelScale(spacing[1] * dims[1]);
  renderer->GetActiveCamera()->SetParallelProjection(1);
  renderer->ResetCamera();
}

//-----------------------------------------------------------------------------
void SliceView::handle_key(std::string key) {
  if (!is_image_loaded()) {
    return;
  }

  if (key == "Up") {
    image_slice_number_++;
    slice_mapper_->SetSliceNumber(image_slice_number_);
    viewer_->get_renderer()->GetRenderWindow()->Render();
  } else if (key == "Down") {
    image_slice_number_--;
    slice_mapper_->SetSliceNumber(image_slice_number_);
    viewer_->get_renderer()->GetRenderWindow()->Render();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
