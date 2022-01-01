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
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[threadSafeReadMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-threadsafereadmesh)**(std::string filename)<br>Thread safe reading of a mesh, uses a lock.  |
| void | **[threadSafeWriteMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-threadsafewritemesh)**(std::string filename, [Mesh](../Classes/classshapeworks_1_1Mesh.md) mesh)<br>Thread safe writing of a mesh, uses a lock.  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1MeshUtils.md#function-boundingbox)**(const std::vector< std::string > & filenames, bool center =false)<br>calculate bounding box incrementally for meshes  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1MeshUtils.md#function-boundingbox)**(const std::vector< std::reference_wrapper< const [Mesh](../Classes/classshapeworks_1_1Mesh.md) >> & meshes, bool center =false)<br>calculate bounding box incrementally for meshes  |
| size_t | **[findReferenceMesh](../Classes/classshapeworks_1_1MeshUtils.md#function-findreferencemesh)**(std::vector< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > & meshes)<br>determine the reference mesh  |
| void | **[generateNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-generatenormals)**(const std::vector< std::reference_wrapper< [Mesh](../Classes/classshapeworks_1_1Mesh.md) >> & meshes, bool forceRegen =false)<br>generates and adds normals for points and faces for each mesh in given set of meshes  |
| Field | **[computeMeanNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-computemeannormals)**(const std::vector< std::string > & filenames, bool autoGenerateNormals =true)<br>computes average normals for each point in given set of meshes  |
| Field | **[computeMeanNormals](../Classes/classshapeworks_1_1MeshUtils.md#function-computemeannormals)**(const std::vector< std::reference_wrapper< const [Mesh](../Classes/classshapeworks_1_1Mesh.md) >> & meshes)<br>computes average normals for each point in given set of meshes  |
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
    const std::vector< std::reference_wrapper< const Mesh >> & meshes,
    bool center =false
)
```

calculate bounding box incrementally for meshes 

### function findReferenceMesh

```cpp
static size_t findReferenceMesh(
    std::vector< Mesh > & meshes
)
```

determine the reference mesh 

### function generateNormals

```cpp
static void generateNormals(
    const std::vector< std::reference_wrapper< Mesh >> & meshes,
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
    const std::vector< std::reference_wrapper< const Mesh >> & meshes
)
```

computes average normals for each point in given set of meshes 

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

Updated on 2022-01-01 at 17:33:55 +0000