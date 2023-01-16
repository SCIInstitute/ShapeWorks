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
| void | **[set_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-set-reference-mesh)**(vtkSmartPointer< vtkPolyData > reference_mesh, const Eigen::MatrixXd & reference_particles, const Eigen::MatrixXd & landmarks ={})<br>Set the reference mesh and particles.  |
| bool | **[generate_warp](../Classes/classshapeworks_1_1MeshWarper.md#function-generate-warp)**()<br>Generate warp, return true on success.  |
| bool | **[get_warp_available](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-available)**()<br>Return if the warp is available.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**() const |
| vtkSmartPointer< vtkPolyData > | **[build_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-build-mesh)**(const Eigen::MatrixXd & particles)<br>Build a mesh for a given set of particles.  |
| Eigen::MatrixXd | **[extract_landmarks](../Classes/classshapeworks_1_1MeshWarper.md#function-extract-landmarks)**(vtkSmartPointer< vtkPolyData > warped_mesh)<br>Return the landmarks (matrix [Nx3]) from the warped builded mesh.  |
| bool | **[is_contour](../Classes/classshapeworks_1_1MeshWarper.md#function-is-contour)**()<br>Return if set as a contour.  |
| std::map< int, int > | **[get_landmarks_map](../Classes/classshapeworks_1_1MeshWarper.md#function-get-landmarks-map)**() const<br>Return the map of landmarks id (Key) to vertice index (Value)  |
| std::vector< int > | **[get_good_particle_indices](../Classes/classshapeworks_1_1MeshWarper.md#function-get-good-particle-indices)**() const<br>Return the indexes of good particles (those that really control the warping)  |
| const Eigen::MatrixXd & | **[get_warp_matrix](../Classes/classshapeworks_1_1MeshWarper.md#function-get-warp-matrix)**() const<br>Return the warp matrix.  |
| bool | **[has_bad_particles](../Classes/classshapeworks_1_1MeshWarper.md#function-has-bad-particles)**() const<br>Return true if warping has removed any bad particle(s)  |
| vtkSmartPointer< vtkPolyData > | **[get_reference_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-get-reference-mesh)**()<br>Return the reference mesh which has been cleaned and vertices added.  |
| const Eigen::MatrixXd & | **[get_reference_particles](../Classes/classshapeworks_1_1MeshWarper.md#function-get-reference-particles)**() const<br>Return the reference particles.  |
| vtkSmartPointer< vtkPolyData > | **[prep_mesh](../Classes/classshapeworks_1_1MeshWarper.md#function-prep-mesh)**(vtkSmartPointer< vtkPolyData > mesh)<br>Prep incoming mesh.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[update_progress](../Classes/classshapeworks_1_1MeshWarper.md#function-update-progress)**(float p)<br>For overriding to handle progress updates.  |

## Detailed Description

```cpp
class shapeworks::MeshWarper;
```


This class implements mesh warping based on correspondence particles. Correspondence points are embedded into the mesh as new vertices (traingles split). Then a biharmonic deformation is used to warp the mesh to new sets of correspondence particles.

It can optionally be used to warp landmarks along with the mesh by embedding them as vertices 

## Public Functions Documentation

### function set_reference_mesh

```cpp
void set_reference_mesh(
    vtkSmartPointer< vtkPolyData > reference_mesh,
    const Eigen::MatrixXd & reference_particles,
    const Eigen::MatrixXd & landmarks ={}
)
```

Set the reference mesh and particles. 

### function generate_warp

```cpp
bool generate_warp()
```

Generate warp, return true on success. 

### function get_warp_available

```cpp
bool get_warp_available()
```

Return if the warp is available. 

### function is_contour

```cpp
inline bool is_contour() const
```


### function build_mesh

```cpp
vtkSmartPointer< vtkPolyData > build_mesh(
    const Eigen::MatrixXd & particles
)
```

Build a mesh for a given set of particles. 

### function extract_landmarks

```cpp
Eigen::MatrixXd extract_landmarks(
    vtkSmartPointer< vtkPolyData > warped_mesh
)
```

Return the landmarks (matrix [Nx3]) from the warped builded mesh. 

### function is_contour

```cpp
inline bool is_contour()
```

Return if set as a contour. 

### function get_landmarks_map

```cpp
inline std::map< int, int > get_landmarks_map() const
```

Return the map of landmarks id (Key) to vertice index (Value) 

### function get_good_particle_indices

```cpp
inline std::vector< int > get_good_particle_indices() const
```

Return the indexes of good particles (those that really control the warping) 

### function get_warp_matrix

```cpp
inline const Eigen::MatrixXd & get_warp_matrix() const
```

Return the warp matrix. 

### function has_bad_particles

```cpp
inline bool has_bad_particles() const
```

Return true if warping has removed any bad particle(s) 

### function get_reference_mesh

```cpp
inline vtkSmartPointer< vtkPolyData > get_reference_mesh()
```

Return the reference mesh which has been cleaned and vertices added. 

### function get_reference_particles

```cpp
inline const Eigen::MatrixXd & get_reference_particles() const
```

Return the reference particles. 

### function prep_mesh

```cpp
static vtkSmartPointer< vtkPolyData > prep_mesh(
    vtkSmartPointer< vtkPolyData > mesh
)
```

Prep incoming mesh. 

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

Updated on 2023-01-16 at 19:53:05 +0000