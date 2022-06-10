---
title: Studio/src/Data/MeshGroup.h

---

# Studio/src/Data/MeshGroup.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md)** <br>Representation of a group of meshes.  |




## Source code

```cpp
#pragma once

#include <Data/StudioMesh.h>

namespace shapeworks {


class MeshGroup {
 public:
  MeshGroup();

  MeshGroup(unsigned long num_meshes);

  ~MeshGroup();

  void set_number_of_meshes(int n);

  bool valid();

  void set_mesh(int i, MeshHandle mesh);

  MeshList& meshes();

  vtkSmartPointer<vtkPolyData> get_combined_poly_data();

 private:
  MeshList meshes_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-06-10 at 06:10:26 +0000
