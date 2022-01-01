---
title: Studio/src/Data/MeshGenerator.h
summary: Mesh generation. 

---

# Studio/src/Data/MeshGenerator.h

Mesh generation.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshReconstructors](../Classes/classshapeworks_1_1MeshReconstructors.md)**  |
| class | **[shapeworks::MeshGenerator](../Classes/classshapeworks_1_1MeshGenerator.md)**  |

## Detailed Description

Mesh generation. 

The MeshGenerator performs the actual work of reconstructing a mesh from a shape (list of points). 




## Source code

```cpp
#pragma once

#include <QSharedPointer>

#include "vnl/vnl_vector.h"

#include <Libs/Mesh/MeshWarper.h>

#include <Data/StudioMesh.h>
#include <Data/QMeshWarper.h>
#include <Data/Preferences.h>
#include <Data/SurfaceReconstructor.h>
#include <Data/MeshWorkQueue.h>

namespace shapeworks {

class MeshReconstructors {
public:
  std::vector<std::shared_ptr<SurfaceReconstructor>> surface_reconstructors_;
  std::vector<std::shared_ptr<QMeshWarper>> mesh_warpers_;
};

class MeshGenerator {
public:

  MeshGenerator();

  ~MeshGenerator();

  MeshHandle build_mesh(const MeshWorkItem& item);

  MeshHandle build_mesh_from_points(const vnl_vector<double>& shape, int domain);

  MeshHandle build_mesh_from_image(ImageType::Pointer image, float iso_value = 0.0001);

  MeshHandle build_mesh_from_file(std::string filename, float iso_value = 0.0001);

  void set_mesh_reconstructors(std::shared_ptr<MeshReconstructors> reconstructors);

  void set_reconstruction_method(std::string method);
  std::string get_reconstruction_method();

  static const std::string RECONSTRUCTION_LEGACY_C;
  static const std::string RECONSTRUCTION_DISTANCE_TRANSFORM_C;
  static const std::string RECONSTRUCTION_MESH_WARPER_C;

private:

  std::shared_ptr<MeshReconstructors> reconstructors_ = std::make_shared<MeshReconstructors>();

  std::string reconstruction_method_ = RECONSTRUCTION_MESH_WARPER_C;

};
}
```


-------------------------------

Updated on 2022-01-01 at 17:29:14 +0000
