---
title: shapeworks::MeshWarper

---

# shapeworks::MeshWarper

**Module:** **[Mesh Classes](../Modules/group__Group-Mesh.md)**



 [More...](#detailed-description)


`#include <MeshWarper.h>`

Inherited by [shapeworks::QMeshWarper](../Classes/classshapeworks_1_1QMeshWarper.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[set_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-set-reference-mesh)**(vtkSmartPointer< vtkPolyData > reference_mesh, const Eigen::MatrixXd & reference_particles)<br>Set the reference mesh and particles.  |
| bool | **[get_warp_available](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-available)**()<br>Return if the warp is available.  |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-build-mesh)**(const Eigen::MatrixXd & particles)<br>Build a mesh for a given set of particles.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**()<br>Return if set as a contour.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[update_progress](../Classes/classshapeworks_1_1MeshWarper.md#function-update-progress)**(float p)<br>For overriding to handle progress updates.  |

## Detailed Description

```cpp
class shapeworks::MeshWarper;
```


This class implements mesh warping based on correspondence particles 

## Public Functions Documentation

### function set_reference_mesh

```cpp
void set_reference_mesh(
    vtkSmartPointer< vtkPolyData > reference_mesh,
    const Eigen::MatrixXd & reference_particles
)
```

Set the reference mesh and particles. 

### function get_warp_available

```cpp
bool get_warp_available()
```

Return if the warp is available. 

### function build_mesh

```cpp
vtkSmartPointer< vtkPolyData > build_mesh(
    const Eigen::MatrixXd & particles
)
```

Build a mesh for a given set of particles. 

### function is_contour

```cpp
inline bool is_contour()
```

Return if set as a contour. 

## Protected Functions Documentation

### function update_progress

```cpp
inline virtual void update_progress(
    float p
)
```

For overriding to handle progress updates. 

**Reimplemented by**: [shapeworks::QMeshWarper::update_progress](../Classes/classshapeworks_1_1QMeshWarper.md#function-update-progress)


-------------------------------

Updated on 2022-02-02 at 18:18:10 +0000