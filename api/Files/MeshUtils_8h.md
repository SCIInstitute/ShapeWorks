---
title: Libs/Mesh/MeshUtils.h

---

# Libs/Mesh/MeshUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshUtils](../Classes/classshapeworks_1_1MeshUtils.md)**  |




## Source code

```cpp
#pragma once

#include "ShapeworksUtils.h"
#include "Mesh.h"
#include "Eigen/Core"
#include "Eigen/Dense"

class vtkActor;

namespace shapeworks {

class MeshUtils
{
public:

  static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const Mesh source,
                                                                const Mesh target,
                                                                Mesh::AlignmentType align,
                                                                const unsigned iterations = 20,
                                                                bool meshTransform = false);

  static Mesh threadSafeReadMesh(std::string filename);

  static void threadSafeWriteMesh(std::string filename, Mesh mesh);

  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, bool center = false);

  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh>>& meshes, bool center = false);

  static size_t findReferenceMesh(std::vector<Mesh> &meshes);

  static void generateNormals(const std::vector<std::reference_wrapper<Mesh>>& meshes, bool forceRegen = false);

  static Field computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals = true);

  static Field computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh>>& meshes);

  void visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh);

  vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);
};

} // shapeworks
```


-------------------------------

Updated on 2022-01-12 at 19:06:15 +0000
