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
| MeshList & | **[meshes](../Classes/classshapeworks_1_1MeshGroup.md#function-meshes)**() |

## Detailed Description

```cpp
class shapeworks::MeshGroup;
```

Representation of a group of meshes. 

The [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) class encapsulates a group of meshes (e.g. from a single subject) 

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


-------------------------------

Updated on 2021-12-07 at 17:10:24 +0000