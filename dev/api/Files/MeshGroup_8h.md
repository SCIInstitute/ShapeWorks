---
title: Libs/Analyze/MeshGroup.h

---

# Libs/Analyze/MeshGroup.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md)** <br>Representation of a group of meshes.  |




## Source code

```cpp
#pragma once

#include "StudioMesh.h"

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

Updated on 2023-09-29 at 06:24:12 +0000
