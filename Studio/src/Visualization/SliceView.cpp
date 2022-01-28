// vtk
#include <vtkCamera.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

// shapeworks
#include <Visualization/SliceView.h>
#include <Visualization/Viewer.h>

namespace shapeworks {
//-----------------------------------------------------------------------------
SliceView::SliceView(Viewer *viewer) : viewer_(viewer) {
  image_slice_ = vtkSmartPointer<vtkImageSlice>::New();
  slice_mapper_ = vtkSmartPointer<vtkImageSliceMapper>::New();
}

//-----------------------------------------------------------------------------
void SliceView::set_volume(vtkSmartPointer<vtkImageData> volume) {
  volume_ = volume;
  slice_mapper_->SetInputData(volume_);
  image_slice_->SetMapper(slice_mapper_);

  auto transform = viewer_->get_image_transform();
  image_slice_->SetUserTransform(transform);
}

//-----------------------------------------------------------------------------
bool SliceView::is_image_loaded() { return volume_ != nullptr; }

//-----------------------------------------------------------------------------
void SliceView::update_renderer() {
  auto renderer = viewer_->get_renderer();
  renderer->RemoveViewProp(image_slice_);
  if (is_image_loaded()) {
    renderer->AddActor(image_slice_);
  }
}

//-----------------------------------------------------------------------------
void SliceView::update_camera() {
  if (!is_image_loaded()) {
    return;
  }
  auto renderer = viewer_->get_renderer();

  double origin[3];
  volume_->GetOrigin(origin);
  int dims[3];
  volume_->GetDimensions(dims);
  std::cout << "dims: " << dims[0] << "\t" << dims[1] << "\t" << dims[2] << "\n";
  double spaces[3];
  int max_slice_num = dims[2];
  volume_->GetSpacing(spaces);
  std::cout << "spaces: " << spaces[0] << "\t" << spaces[1] << "\t" << spaces[2] << "\n";
  std::cout << "max slice number: " << max_slice_num << "\n";

  double center_x = (spaces[0] * dims[0] / 2) + origin[0];
  double center_y = (spaces[1] * dims[1] / 2) + origin[1];

  renderer->GetActiveCamera()->SetPosition(center_x, center_y, spaces[2] * (max_slice_num + 1));
  renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
  renderer->GetActiveCamera()->SetFocalPoint(center_x, center_y, 0);
  renderer->GetActiveCamera()->SetParallelScale(spaces[1] * dims[1]);
  renderer->GetActiveCamera()->SetParallelProjection(1);
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
