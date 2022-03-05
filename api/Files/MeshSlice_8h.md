---
title: Studio/src/Visualization/MeshSlice.h

---

# Studio/src/Visualization/MeshSlice.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshSlice](../Classes/classshapeworks_1_1MeshSlice.md)** <br>[MeshSlice]().  |




## Source code

```cpp
#pragma once

#include <vtkSmartPointer.h>

class vtkImageSlice;
class vtkImageSliceMapper;
class vtkImageData;

namespace shapeworks {

class Viewer;

class MeshSlice {
 public:
  MeshSlice(Viewer* viewer);

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
```


-------------------------------

Updated on 2022-03-05 at 23:20:35 +0000
