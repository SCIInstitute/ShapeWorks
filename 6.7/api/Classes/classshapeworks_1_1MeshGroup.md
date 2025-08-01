---
title: shapeworks::MeshGroup
summary: Representation of a group of meshes. 

---

# shapeworks::MeshGroup



Representation of a group of meshes.  [More...](#detailed-description)


`#include <MeshGroup.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md#function-meshgroup)**() |
| | **[MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md#function-meshgroup)**(unsigned long num_meshes) |
| | **[~MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md#function-~meshgroup)**() |
| void | **[set_number_of_meshes](../Classes/classshapeworks_1_1MeshGroup.md#function-set-number-of-meshes)**(int n) |
| bool | **[valid](../Classes/classshapeworks_1_1MeshGroup.md#function-valid)**() |
| void | **[set_mesh](../Classes/classshapeworks_1_1MeshGroup.md#function-set-mesh)**(int i, MeshHandle mesh) |
| MeshList & | **[meshes](../Classes/classshapeworks_1_1MeshGroup.md#function-meshes)**()<br>Return a reference to the list of meshes.  |
| vtkSmartPointer< vtkPolyData > | **[get_combined_poly_data](../Classes/classshapeworks_1_1MeshGroup.md#function-get-combined-poly-data)**()<br>Return polydata of all meshes, combined.  |

## Detailed Description

```cpp
class shapeworks::MeshGroup;
```

Representation of a group of meshes. 

The [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) class encapsulates a group of meshes (e.g. from a single subject) A subject will have one mesh for each anatomy/domain 

## Public Functions Documentation

### function MeshGroup

```cpp
MeshGroup()
```


### function MeshGroup

```cpp
MeshGroup(
    unsigned long num_meshes
)
```


### function ~MeshGroup

```cpp
~MeshGroup()
```


### function set_number_of_meshes

```cpp
void set_number_of_meshes(
    int n
)
```


### function valid

```cpp
bool valid()
```


### function set_mesh

```cpp
void set_mesh(
    int i,
    MeshHandle mesh
)
```


### function meshes

```cpp
MeshList & meshes()
```

Return a reference to the list of meshes. 

### function get_combined_poly_data

```cpp
vtkSmartPointer< vtkPolyData > get_combined_poly_data()
```

Return polydata of all meshes, combined. 

-------------------------------

Updated on 2025-08-01 at 02:40:46 +0000