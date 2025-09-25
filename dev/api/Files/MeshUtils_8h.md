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
    static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const Mesh source,
                                                                  const Mesh target,
                                                                  Mesh::AlignmentType align,
                                                                  const unsigned iterations = 20,
                                                                  bool meshTransform = false);

    static Mesh create_mesh_from_file(std::string filename, double iso_value = 0.5);

    static Mesh threadSafeReadMesh(std::string filename);

    static void threadSafeWriteMesh(std::string filename, Mesh mesh);

    static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, bool center = false);

    static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh> >& meshes,
                                      bool center = false);

    static PhysicalRegion boundingBox(const std::vector<Mesh>& meshes, bool center = false);

    static int findReferenceMesh(std::vector<Mesh>& meshes, int random_subset_size = -1);

    static Mesh extract_boundary_loop(Mesh mesh);

    static std::array<Mesh, 3> shared_boundary_extractor(const Mesh& mesh_l, const Mesh& mesh_r, double tol);

    static void generateNormals(const std::vector<std::reference_wrapper<Mesh> >& meshes, bool forceRegen = false);

    static Field computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals = true);

    static Field computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh> >& meshes);

    void visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh);

    vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);

    static int evaluate_triangle_position(const double x[3],
                                          double closestPoint[3],
                                          int& subId,
                                          double pcoords[3],
                                          double& dist2,
                                          double weights[],
                                          double pt3[3],
                                          double pt1[3],
                                          double pt2[3]);
};
} // namespace shapeworks
```


-------------------------------

Updated on 2025-09-25 at 21:39:36 +0000
