---
title: Studio/src/Visualization/SliceView.h

---

# Studio/src/Visualization/SliceView.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::SliceView](../Classes/classshapeworks_1_1SliceView.md)** <br>[SliceView]().  |




## Source code

```cpp
#pragma once

#include <vtkSmartPointer.h>

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
  SliceView(Viewer* viewer);

  void set_volume(vtkSmartPointer<vtkImageData> volume);

  void set_mesh(vtkSmartPointer<vtkPolyData> poly_data);

  void set_orientation(int orientation);

  bool is_image_loaded();

  void update_renderer();

  void update_camera();

  void handle_key(std::string key);

  void set_window_and_level(double window, double level);

  double get_spacing();

  int get_orientation_index();

  void update_particles();

  Viewer* get_viewer();

  bool should_point_show(double x, double y, double z);

 private:
  void update_extent();

  Viewer* viewer_;

  vtkSmartPointer<vtkImageActor> image_slice_;
  vtkSmartPointer<vtkImageSliceMapper> slice_mapper_;
  vtkSmartPointer<vtkImageData> volume_;
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

Updated on 2022-03-05 at 23:20:35 +0000
