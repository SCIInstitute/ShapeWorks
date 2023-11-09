---
title: Studio/Visualization/SliceView.h

---

# Studio/Visualization/SliceView.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::SliceView](../Classes/classshapeworks_1_1SliceView.md)** <br>[SliceView]().  |




## Source code

```cpp
#pragma once

#include <vtkSmartPointer.h>
#include <Shapeworks.h>

class vtkImageActor;
class vtkImageSliceMapper;
class vtkImageData;

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkTransformPolyDataFilter;
class vtkCutter;
class vtkStripper;
class vtkImageActorPointPlacer;

namespace shapeworks {

class SeedWidget;

class Viewer;

class SliceView {
 public:

  enum SliceChange { Down, Up };

  SliceView(Viewer* viewer);

  void set_volume(std::shared_ptr<Image> volume);

  void set_mesh(vtkSmartPointer<vtkPolyData> poly_data);

  void set_orientation(int orientation);

  bool is_image_loaded();

  void update_renderer();

  void update_camera();

  Point handle_key(std::string key);

  void change_slice(SliceChange change);

  Point get_slice_position();

  void set_slice_position(Point point);

  void set_window_and_level(double window, double level);

  double get_spacing();

  int get_orientation_index();

  void update_particles();

  Viewer* get_viewer();

  bool should_point_show(double x, double y, double z);

 private:

  void set_slice_number(int slice);

  void update_extent();

  Viewer* viewer_;

  vtkSmartPointer<vtkImageActor> image_slice_;
  vtkSmartPointer<vtkImageSliceMapper> slice_mapper_;
  std::shared_ptr<Image> volume_;
  vtkSmartPointer<vtkImageData> vtk_volume_;
  vtkSmartPointer<vtkImageActorPointPlacer> placer_;

  int current_slice_number_ = 0;

  vtkSmartPointer<vtkPolyDataMapper> cut_mapper_;
  vtkSmartPointer<vtkActor> cut_actor_;
  vtkSmartPointer<vtkTransformPolyDataFilter> cut_transform_filter_;
  vtkSmartPointer<vtkCutter> cutter_;
  vtkSmartPointer<vtkStripper> stripper_;

  vtkSmartPointer<vtkPolyData> current_poly_data_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-11-09 at 17:05:58 +0000
