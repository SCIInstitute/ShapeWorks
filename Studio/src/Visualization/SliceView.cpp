// vtk
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCutter.h>
#include <vtkImageActor.h>
#include <vtkImageActorPointPlacer.h>
#include <vtkImageProperty.h>
#include <vtkImageSliceMapper.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>
#include <vtkTransformPolyDataFilter.h>

// shapeworks
#include <Visualization/SliceView.h>
#include <Visualization/Viewer.h>

namespace shapeworks {
//-----------------------------------------------------------------------------
SliceView::SliceView(Viewer *viewer) : viewer_(viewer) {
  image_slice_ = vtkSmartPointer<vtkImageActor>::New();

  slice_mapper_ = vtkSmartPointer<vtkImageSliceMapper>::New();

  cut_transform_filter_ = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  cutter_ = vtkSmartPointer<vtkCutter>::New();
  stripper_ = vtkSmartPointer<vtkStripper>::New();
  cut_mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  cut_actor_ = vtkSmartPointer<vtkActor>::New();

  stripper_->PassCellDataAsFieldDataOn();
  cut_mapper_->SetScalarVisibility(false);
  cut_mapper_->SetResolveCoincidentTopologyToPolygonOffset();
  cut_actor_->GetProperty()->SetColor(1, 1, 0.25);
  cut_actor_->GetProperty()->SetLineWidth(3);
  cut_actor_->GetProperty()->SetAmbient(1.0);
  cut_actor_->GetProperty()->SetDiffuse(0.0);

  placer_ = vtkSmartPointer<vtkImageActorPointPlacer>::New();
  placer_->SetImageActor(image_slice_);
}

//-----------------------------------------------------------------------------
void SliceView::set_volume(vtkSmartPointer<vtkImageData> volume) {
  volume_ = volume;
  slice_mapper_->SetInputData(volume_);

  image_slice_->SetMapper(slice_mapper_);

  auto transform = viewer_->get_image_transform();
  image_slice_->SetUserTransform(transform);

  update_extent();
}

//-----------------------------------------------------------------------------
void SliceView::set_mesh(vtkSmartPointer<vtkPolyData> poly_data) {
  if (poly_data == current_poly_data_) {
    return;
  }
  if (poly_data->GetNumberOfCells() == 0) {
    return;
  }

  current_poly_data_ = poly_data;
  auto transform = viewer_->get_image_transform();
  cut_transform_filter_->SetInputData(poly_data);
  cut_transform_filter_->SetTransform(transform);

  cutter_->SetInputConnection(cut_transform_filter_->GetOutputPort());
  cutter_->SetCutFunction(slice_mapper_->GetSlicePlane());

  stripper_->SetInputConnection(cutter_->GetOutputPort());
  stripper_->Update();

  cut_mapper_->SetInputConnection(stripper_->GetOutputPort());

  cut_actor_->SetMapper(cut_mapper_);
}

//-----------------------------------------------------------------------------
void SliceView::set_orientation(int orientation) {
  if (orientation == slice_mapper_->GetOrientation()) {
    return;
  }
  slice_mapper_->SetOrientation(orientation);

  update_camera();
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

  int orientation = slice_mapper_->GetOrientation();
  double origin[3];
  int dims[3];
  double spacing[3];
  volume_->GetOrigin(origin);
  volume_->GetDimensions(dims);
  volume_->GetSpacing(spacing);
  // int max_slice_num = slice_mapper_->GetSliceNumberMaxValue();
  //  std::cout << "dims: " << dims[0] << "\t" << dims[1] << "\t" << dims[2] << "\n";
  //  std::cout << "spaces: " << spacing[0] << "\t" << spacing[1] << "\t" << spacing[2] << "\n";
  //  std::cout << "slice range: " << slice_mapper_->GetSliceNumberMinValue() << " to "
  //            << slice_mapper_->GetSliceNumberMaxValue() << "\n";

  int max_slice_num = slice_mapper_->GetSliceNumberMinValue();
  current_slice_number_ = (slice_mapper_->GetSliceNumberMaxValue() - slice_mapper_->GetSliceNumberMinValue()) / 2;
  slice_mapper_->SetSliceNumber(current_slice_number_);

  //  std::cerr << "orientation = " << orientation << "\n";
  if (orientation == 0) {
    renderer->GetActiveCamera()->SetPosition(spacing[0] * (max_slice_num + 1), spacing[1] * dims[1] / 2,
                                             spacing[2] * dims[2] / 2);
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer->GetActiveCamera()->SetFocalPoint(0, spacing[1] * dims[1] / 2, spacing[2] * dims[2] / 2);
    renderer->GetActiveCamera()->SetParallelScale(spacing[2] * dims[2]);
  } else if (orientation == 1) {
    renderer->GetActiveCamera()->SetPosition(spacing[0] * dims[0] / 2, spacing[1] * (max_slice_num + 1),
                                             spacing[2] * dims[2] / 2);
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer->GetActiveCamera()->SetFocalPoint(spacing[0] * dims[0] / 2, 0, spacing[2] * dims[2] / 2);
    renderer->GetActiveCamera()->SetParallelScale(spacing[2] * dims[2]);
  } else {
    renderer->GetActiveCamera()->SetPosition(spacing[0] * dims[0] / 2, spacing[1] * dims[1] / 2,
                                             spacing[2] * (max_slice_num + 1));
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->GetActiveCamera()->SetFocalPoint(spacing[0] * dims[0] / 2, spacing[1] * dims[1] / 2, 0);
    renderer->GetActiveCamera()->SetParallelScale(spacing[1] * dims[1]);
  }

  renderer->GetActiveCamera()->SetParallelProjection(1);
  renderer->ResetCameraClippingRange();
  renderer->ResetCamera();
  update_extent();
}

//-----------------------------------------------------------------------------
void SliceView::handle_key(std::string key) {
  if (!is_image_loaded()) {
    return;
  }

  if (key == "Up") {
    current_slice_number_++;
    current_slice_number_ = std::min(current_slice_number_, slice_mapper_->GetSliceNumberMaxValue());
    slice_mapper_->SetSliceNumber(current_slice_number_);
    viewer_->get_renderer()->GetRenderWindow()->Render();
  } else if (key == "Down") {
    current_slice_number_--;
    current_slice_number_ = std::max(current_slice_number_, slice_mapper_->GetSliceNumberMinValue());
    slice_mapper_->SetSliceNumber(current_slice_number_);
    viewer_->get_renderer()->GetRenderWindow()->Render();
  }
  update_extent();

  viewer_->update_points();
}

//-----------------------------------------------------------------------------
void SliceView::set_window_and_level(double window, double level) {
  image_slice_->GetProperty()->SetColorWindow(window);
  image_slice_->GetProperty()->SetColorLevel(level);
}

//-----------------------------------------------------------------------------
double SliceView::get_spacing() {
  if (!volume_) {
    return 1.0;
  }
  int index = get_orientation_index();
  double *spacing = volume_->GetSpacing();
  return spacing[index];
}

//-----------------------------------------------------------------------------
int SliceView::get_orientation_index() { return slice_mapper_->GetOrientation(); }

//-----------------------------------------------------------------------------
void SliceView::update_particles() {}

//-----------------------------------------------------------------------------
Viewer *SliceView::get_viewer() { return viewer_; }

//-----------------------------------------------------------------------------
bool SliceView::should_point_show(double x, double y, double z) {
  if (!is_image_loaded()) {
    return true;
  }
  vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();

  double bounds[6];

  slice_mapper_->GetBounds(bounds);

  int orientation = this->get_orientation_index();

  if (orientation == 2) {
    plane->SetNormal(0, 0, 1);
    plane->SetOrigin(0, 0, bounds[4]);
  } else if (orientation == 1) {
    plane->SetNormal(0, 1, 0);
    plane->SetOrigin(0, bounds[2], 0);
  } else {
    plane->SetNormal(1, 0, 0);
    plane->SetOrigin(bounds[0], 0, 0);
  }

  double point[3];
  point[0] = x;
  point[1] = y;
  point[2] = z;
  double distance = plane->DistanceToPlane(point);
  double spacing = get_spacing();
  int slice_number = (distance + (spacing / 2.0)) / spacing;
  if (slice_number == current_slice_number_) {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
void SliceView::update_extent() {
  if (!slice_mapper_ || !volume_) {
    return;
  }
  int orientation = slice_mapper_->GetOrientation();

  int *extent = volume_->GetExtent();

  int slice = current_slice_number_;
  if (orientation == 2) {
    image_slice_->SetDisplayExtent(extent[0], extent[1], extent[2], extent[3], slice, slice);
  } else if (orientation == 1) {
    image_slice_->SetDisplayExtent(extent[0], extent[1], slice, slice, extent[4], extent[5]);
  } else if (orientation == 0) {
    image_slice_->SetDisplayExtent(slice, slice, extent[2], extent[3], extent[4], extent[5]);
  }

  // Figure out the correct clipping range
  vtkCamera *cam = viewer_->get_renderer()->GetActiveCamera();
  if (cam) {
    double bounds[6];
    image_slice_->GetBounds(bounds);

    int index = get_orientation_index();
    double spos = bounds[index * 2];
    double cpos = cam->GetPosition()[index];

    double range = fabs(spos - cpos);

    double spacing = this->get_spacing();

    double near_clip = range - spacing / 2.0 + 0.001;
    double far_clip = range + spacing / 2.0 - 0.001;

    // cam->SetClippingRange( near_clip, far_clip );
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}  // namespace shapeworks
