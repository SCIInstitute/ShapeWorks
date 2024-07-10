---
title: Libs/Mesh/MeshUtils.h

---

# Libs/Mesh/MeshUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshUtils](../Classes/classshapeworks_1_1MeshUtils.md)**  |




## Source code

```cpp
#pragma once

#include "Eigen/Core"
#include "Eigen/Dense"
#include "Mesh.h"
#include "ShapeworksUtils.h"

class vtkActor;

namespace shapeworks {

class MeshUtils {
 public:
  static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const Mesh source, const Mesh target,
                                                                Mesh::AlignmentType align,
                                                                const unsigned iterations = 20,
                                                                bool meshTransform = false);

  static Mesh create_mesh_from_file(std::string filename, double iso_value = 0.5);

  static Mesh threadSafeReadMesh(std::string filename);

  static void threadSafeWriteMesh(std::string filename, Mesh mesh);

  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, bool center = false);

  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh>>& meshes, bool center = false);

  static PhysicalRegion boundingBox(const std::vector<Mesh>& meshes, bool center = false);

  static int findReferenceMesh(std::vector<Mesh>& meshes, int random_subset_size = -1);

  static Mesh boundaryLoopExtractor(Mesh mesh);

  static std::array<Mesh, 3> sharedBoundaryExtractor(const Mesh& mesh_l, const Mesh& mesh_r, double tol);

  static void generateNormals(const std::vector<std::reference_wrapper<Mesh>>& meshes, bool forceRegen = false);

  static Field computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals = true);

  static Field computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh>>& meshes);

  void visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh);

  vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-10 at 20:24:03 +0000
