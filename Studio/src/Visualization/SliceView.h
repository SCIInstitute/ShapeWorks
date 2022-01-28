#pragma once

#include <vtkSmartPointer.h>

class vtkImageSlice;
class vtkImageSliceMapper;
class vtkImageData;

namespace shapeworks {

class Viewer;
//! SliceView
/*!
 * Provide slice viewing to the Viewer
 *
 *
 */
class SliceView {
 public:
  SliceView(Viewer* viewer);

  void set_volume(vtkSmartPointer<vtkImageData> volume);

  bool is_image_loaded();

  void update_renderer();

  void update_camera();

  void handle_key(std::string key);

 private:
  Viewer *viewer_;

  vtkSmartPointer<vtkImageSlice> image_slice_;
  vtkSmartPointer<vtkImageSliceMapper> slice_mapper_;
  vtkSmartPointer<vtkImageData> volume_;
  int image_slice_number_ = 0;

};

}  // namespace shapeworks
