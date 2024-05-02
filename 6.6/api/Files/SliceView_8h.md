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

#include <Shapeworks.h>
#include <vtkSmartPointer.h>

#include "qcolor.h"

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

class Viewer;

class SliceView {
 public:
  enum SliceChange { Down, Up };

  SliceView(Viewer* viewer);

  void set_volume(std::shared_ptr<Image> volume);

  void add_mesh(vtkSmartPointer<vtkPolyData> poly_data);
  void clear_meshes();

  void set_orientation(int orientation);

  bool is_image_loaded();

  void update_renderer();

  void update_camera();

  Point handle_key(std::string key);

  void change_slice(SliceChange change);

  Point get_slice_position();

  void set_slice_position(Point point);
  
  void set_brightness_and_contrast(double brightness, double contrast);

  double get_spacing();

  int get_orientation_index();

  void update_particles();

  Viewer* get_viewer();

  bool should_point_show(double x, double y, double z);

  int get_slice_number();

 private:
  void set_slice_number(int slice);

  void update_extent();

  vtkSmartPointer<vtkActor> create_shape_actor(vtkSmartPointer<vtkPolyData> poly_data, QColor color);

  Viewer* viewer_{nullptr};

  vtkSmartPointer<vtkImageActor> image_slice_;
  vtkSmartPointer<vtkImageSliceMapper> slice_mapper_;
  std::shared_ptr<Image> volume_;
  vtkSmartPointer<vtkImageData> vtk_volume_;
  vtkSmartPointer<vtkImageActorPointPlacer> placer_;

  int current_slice_number_ = 0;

  std::vector<vtkSmartPointer<vtkActor>> cut_actors_;

  std::vector<vtkSmartPointer<vtkPolyData>> poly_datas_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-02 at 14:18:14 +0000
