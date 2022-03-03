---
title: Studio/src/Visualization/LandmarkWidget.h

---

# Studio/src/Visualization/LandmarkWidget.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LandmarkWidget](../Classes/classshapeworks_1_1LandmarkWidget.md)** <br>[LandmarkWidget]().  |




## Source code

```cpp

#include <vtkSmartPointer.h>

#include <vector>

class vtkHandleWidget;
class vtkSphereSource;

namespace shapeworks {

class Viewer;
class LandmarkCallback;

class LandmarkWidget {
 public:
  LandmarkWidget(Viewer* viewer);
  ~LandmarkWidget();

  void update_landmarks();

  void update_positions();

  void update_glyph_properties();

  void clear_landmarks();

 private:
  vtkSmartPointer<vtkHandleWidget> create_handle();

  void assign_handle_to_domain(vtkSmartPointer<vtkHandleWidget> handle, int domain_id);

  Viewer* viewer_ = nullptr;

  std::vector<vtkSmartPointer<vtkHandleWidget>> handles_;

  vtkSmartPointer<vtkSphereSource> sphere_;
  vtkSmartPointer<LandmarkCallback> callback_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-03-03 at 07:50:37 +0000
