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
| void | **[set_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-set-reference-mesh)**(vtkSmartPointer< vtkPolyData > reference_mesh, const Eigen::MatrixXd & reference_particles, const Eigen::MatrixXd & landmarks ={})<br>Set the reference mesh and particles.  |
| bool | **[generate_warp](../Classes/classshapeworks_1_1MeshWarper.md#function-generate-warp)**()<br>Generate warp, return true on success.  |
| bool | **[get_warp_available](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-available)**()<br>Return if the warp is available.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**() const |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-build-mesh)**(const Eigen::MatrixXd & particles)<br>Build a mesh for a given set of particles.  |
| Eigen::MatrixXd | **[extract_landmarks](../Classes/classshapeworks_1_1MeshWarper.md#function-extract-landmarks)**(vtkSmartPointer< vtkPolyData > warped_mesh)<br>Return the landmarks (matrix [Nx3]) from the warped builded mesh.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**()<br>Return if set as a contour.  |
| std::map< int, int > | **[get_landmarks_map](../Classes/classshapeworks_1_1MeshWarper.md#function-get-landmarks-map)**()<br>Return the map of landmarks to vertices.  |
| const Eigen::MatrixXd & | **[get_warp_matrix](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-matrix)**() const<br>Return the warp matrix.  |
| bool | **[has_bad_particles](../Classes/classshapeworks_1_1MeshWarper.md#function-has-bad-particles)**() const<br>Return true if warping has removed any bad particle(s)  |
| vtkSmartPointer< vtkPolyData > | **[get_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-get-reference-mesh)**()<br>Return the reference mesh which has been cleaned and vertices added.  |
| const Eigen::MatrixXd & | **[get_reference_particles](../Classes/classshapeworks_1_1MeshWarper.md#function-get-reference-particles)**() const<br>Return the reference particles.  |
| vtkSmartPointer< vtkPolyData > | **[prep_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-prep-mesh)**(vtkSmartPointer< vtkPolyData > mesh)<br>Prep incoming mesh.  |


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

Updated on 2022-04-08 at 15:18:25 +0000