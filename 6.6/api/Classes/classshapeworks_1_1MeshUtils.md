---
title: shapeworks::MeshUtils

---

# shapeworks::MeshUtils

**Module:** **[Mesh Classes](../Modules/group__Group-Mesh.md)**



 [More...](#detailed-description)


`#include <MeshUtils.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| const vtkSmartPointer< vtkMatrix4x4 > | **[createICPTransform](../Classes/classshapeworks_1_1MeshUtils.md#function-createicptransform)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) source, const [Mesh](../Classes/classshapeworks_1_1Mesh.md) target, Mesh::AlignmentType align, const unsigned iterations =20, bool meshTransform =false)<br>computes a rigid transformation from source to target using vtkIterativeClosestPointTransform  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[create_mesh_from_file](../Classes/classshapeworks_1_1MeshUtils.md#function-create-mesh-from-file)**(std::string filename, double iso_value =0.5)<br>[Mesh](../Classes/classshapeworks_1_1Mesh.md) from mesh or image file.  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[threadSafeReadMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-threadsafereadmesh)**(std::string filename)<br>Thread safe reading of a mesh, uses a lock.  |
| void | **[threadSafeWriteMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-threadsafewritemesh)**(std::string filename, [Mesh](../Classes/classshapeworks_1_1Mesh.md) mesh)<br>Thread safe writing of a mesh, uses a lock.  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1MeshUtils.md#function-boundingbox)**(const std::vector< std::string > & filenames, bool center =false)<br>calculate bounding box incrementally for meshes  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1MeshUtils.md#function-boundingbox)**(const std::vector< std::reference_wrapper< const [Mesh](../Classes/classshapeworks_1_1Mesh.md) > > & meshes, bool center =false)<br>calculate bounding box incrementally for meshes  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1MeshUtils.md#function-boundingbox)**(const std::vector< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > & meshes, bool center =false)<br>calculate bounding box incrementally for meshes  |
| int | **[findReferenceMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-findreferencemesh)**(std::vector< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > & meshes, int random_subset_size =-1)<br>determine the reference mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[boundaryLoopExtractor](../Classes/classshapeworks_1_1MeshUtils.md#function-boundaryloopextractor)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) mesh)<br>boundary loop extractor for a given mesh  |
| std::array< [Mesh](../Classes/classshapeworks_1_1Mesh.md), 3 > | **[sharedBoundaryExtractor](../Classes/classshapeworks_1_1MeshUtils.md#function-sharedboundaryextractor)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh_l, const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh_r, double tol)<br>shared boundary extractor for the left and right mesh  |
| void | **[generateNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-generatenormals)**(const std::vector< std::reference_wrapper< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > > & meshes, bool forceRegen =false)<br>generates and adds normals for points and faces for each mesh in given set of meshes  |
| Field | **[computeMeanNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-computemeannormals)**(const std::vector< std::string > & filenames, bool autoGenerateNormals =true)<br>computes average normals for each point in given set of meshes  |
| Field | **[computeMeanNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-computemeannormals)**(const std::vector< std::reference_wrapper< const [Mesh](../Classes/classshapeworks_1_1Mesh.md) > > & meshes)<br>computes average normals for each point in given set of meshes  |
| int | **[evaluate_triangle_position](../Classes/classshapeworks_1_1MeshUtils.md#function-evaluate-triangle-position)**(const double x[3], double closestPoint[3], int & subId, double pcoords[3], double & dist2, double weights[], double pt3[3], double pt1[3], double pt2[3]) |
| void | **[visualizeVectorFieldForFFCs](../Classes/classshapeworks_1_1MeshUtils.md#function-visualizevectorfieldforffcs)**(std::shared_ptr< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > mesh)<br>This function visualizes vector and scalar fields for FFCs.  |
| vtkSmartPointer< vtkActor > | **[getArrow](../Classes/classshapeworks_1_1MeshUtils.md#function-getarrow)**(Eigen::Vector3d start, Eigen::Vector3d end)<br>Used as an auxiliary function for vector field visualizations.  |

## Detailed Description

```cpp
class shapeworks::MeshUtils;
```


This class provides helper functions for meshes 

## Public Functions Documentation

### function createICPTransform

```cpp
static const vtkSmartPointer< vtkMatrix4x4 > createICPTransform(
    const Mesh source,
    const Mesh target,
    Mesh::AlignmentType align,
    const unsigned iterations =20,
    bool meshTransform =false
)
```

computes a rigid transformation from source to target using vtkIterativeClosestPointTransform 

### function create_mesh_from_file

```cpp
static Mesh create_mesh_from_file(
    std::string filename,
    double iso_value =0.5
)
```

[Mesh](../Classes/classshapeworks_1_1Mesh.md) from mesh or image file. 

### function threadSafeReadMesh

```cpp
static Mesh threadSafeReadMesh(
    std::string filename
)
```

Thread safe reading of a mesh, uses a lock. 

### function threadSafeWriteMesh

```cpp
static void threadSafeWriteMesh(
    std::string filename,
    Mesh mesh
)
```

Thread safe writing of a mesh, uses a lock. 

### function boundingBox

```cpp
static PhysicalRegion boundingBox(
    const std::vector< std::string > & filenames,
    bool center =false
)
```

calculate bounding box incrementally for meshes 

### function boundingBox

```cpp
static PhysicalRegion boundingBox(
    const std::vector< std::reference_wrapper< const Mesh > > & meshes,
    bool center =false
)
```

calculate bounding box incrementally for meshes 

### function boundingBox

```cpp
static PhysicalRegion boundingBox(
    const std::vector< Mesh > & meshes,
    bool center =false
)
```

calculate bounding box incrementally for meshes 

### function findReferenceMesh

```cpp
static int findReferenceMesh(
    std::vector< Mesh > & meshes,
    int random_subset_size =-1
)
```

determine the reference mesh 

### function boundaryLoopExtractor

```cpp
static Mesh boundaryLoopExtractor(
    Mesh mesh
)
```

boundary loop extractor for a given mesh 

### function sharedBoundaryExtractor

```cpp
static std::array< Mesh, 3 > sharedBoundaryExtractor(
    const Mesh & mesh_l,
    const Mesh & mesh_r,
    double tol
)
```

shared boundary extractor for the left and right mesh 

### function generateNormals

```cpp
static void generateNormals(
    const std::vector< std::reference_wrapper< Mesh > > & meshes,
    bool forceRegen =false
)
```

generates and adds normals for points and faces for each mesh in given set of meshes 

### function computeMeanNormals

```cpp
static Field computeMeanNormals(
    const std::vector< std::string > & filenames,
    bool autoGenerateNormals =true
)
```

computes average normals for each point in given set of meshes 

### function computeMeanNormals

```cpp
static Field computeMeanNormals(
    const std::vector< std::reference_wrapper< const Mesh > > & meshes
)
```

computes average normals for each point in given set of meshes 

### function evaluate_triangle_position

```cpp
static int evaluate_triangle_position(
    const double x[3],
    double closestPoint[3],
    int & subId,
    double pcoords[3],
    double & dist2,
    double weights[],
    double pt3[3],
    double pt1[3],
    double pt2[3]
)
```


### function visualizeVectorFieldForFFCs

```cpp
void visualizeVectorFieldForFFCs(
    std::shared_ptr< Mesh > mesh
)
```

This function visualizes vector and scalar fields for FFCs. 

### function getArrow

```cpp
vtkSmartPointer< vtkActor > getArrow(
    Eigen::Vector3d start,
    Eigen::Vector3d end
)
```

Used as an auxiliary function for vector field visualizations. 

-------------------------------

Updated on 2025-01-28 at 22:52:41 +0000