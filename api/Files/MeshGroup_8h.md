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

private:
  MeshList meshes_;

};
}
```


-------------------------------

Updated on 2022-01-14 at 16:56:17 +0000
