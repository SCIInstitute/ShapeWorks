---
title: shapeworks::QMeshWarper
summary: Wraps MeshWarper as a QObject. 

---

# shapeworks::QMeshWarper



Wraps [MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md) as a QObject. 


`#include <QMeshWarper.h>`

Inherits from QObject, [shapeworks::MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md)

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1QMeshWarper.md#signal-progress)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[QMeshWarper](../Classes/classshapeworks_1_1QMeshWarper.md#function-qmeshwarper)**(QObject * parent =nullptr) |
| float | **[get_progress](../Classes/classshapeworks_1_1QMeshWarper.md#function-get-progress)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[update_progress](../Classes/classshapeworks_1_1QMeshWarper.md#function-update-progress)**(float p) override<br>For overriding to handle progress updates.  |

## Additional inherited members

**Public Functions inherited from [shapeworks::MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[set_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-set-reference-mesh)**(vtkSmartPointer< vtkPolyData > reference_mesh, const Eigen::MatrixXd & reference_particles)<br>Set the reference mesh and particles.  |
| bool | **[get_warp_available](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-available)**()<br>Return if the warp is available.  |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-build-mesh)**(const Eigen::MatrixXd & particles)<br>Build a mesh for a given set of particles.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**()<br>Return if set as a contour.  |


## Public Signals Documentation

### signal progress

```cpp
void progress()
```


## Public Functions Documentation

### function QMeshWarper

```cpp
QMeshWarper(
    QObject * parent =nullptr
)
```


### function get_progress

```cpp
float get_progress()
```


## Protected Functions Documentation

### function update_progress

```cpp
virtual void update_progress(
    float p
) override
```

For overriding to handle progress updates. 

**Reimplements**: [shapeworks::MeshWarper::update_progress](../Classes/classshapeworks_1_1MeshWarper.md#function-update-progress)


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000