---
title: shapeworks::MeshManager

---

# shapeworks::MeshManager





Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[handle_thread_complete](../Classes/classshapeworks_1_1MeshManager.md#slot-handle-thread-complete)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item, MeshHandle mesh) |
| void | **[handle_warper_progress](../Classes/classshapeworks_1_1MeshManager.md#slot-handle-warper-progress)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[new_mesh](../Classes/classshapeworks_1_1MeshManager.md#signal-new-mesh)**() |
| void | **[error_encountered](../Classes/classshapeworks_1_1MeshManager.md#signal-error-encountered)**(QString message) |
| void | **[progress](../Classes/classshapeworks_1_1MeshManager.md#signal-progress)**(int ) |
| void | **[status](../Classes/classshapeworks_1_1MeshManager.md#signal-status)**(QString ) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshManager](../Classes/classshapeworks_1_1MeshManager.md#function-meshmanager)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~MeshManager](../Classes/classshapeworks_1_1MeshManager.md#function-~meshmanager)**() |
| void | **[generate_mesh](../Classes/classshapeworks_1_1MeshManager.md#function-generate-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) item)<br>generate and cache a mesh for this shape in a different thread  |
| MeshHandle | **[get_mesh](../Classes/classshapeworks_1_1MeshManager.md#function-get-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item, bool wait =false)<br>get a mesh for a [MeshWorkItem]() |
| MeshHandle | **[get_mesh](../Classes/classshapeworks_1_1MeshManager.md#function-get-mesh)**(const vnl_vector< double > & points, int domain)<br>get a mesh for a set of points  |
| std::shared_ptr< [SurfaceReconstructor](../Classes/classSurfaceReconstructor.md) > | **[get_surface_reconstructor](../Classes/classshapeworks_1_1MeshManager.md#function-get-surface-reconstructor)**(int domain)<br>return the surface reconstructor for a given domain  |
| std::shared_ptr< [MeshWarper](../Classes/classshapeworks_1_1MeshWarper.md) > | **[get_mesh_warper](../Classes/classshapeworks_1_1MeshManager.md#function-get-mesh-warper)**(int domain)<br>return the mesh warper for a given domain  |
| std::shared_ptr< [MeshGenerator](../Classes/classshapeworks_1_1MeshGenerator.md) > | **[get_mesh_generator](../Classes/classshapeworks_1_1MeshManager.md#function-get-mesh-generator)**()<br>return the mesh generator  |
| void | **[clear_cache](../Classes/classshapeworks_1_1MeshManager.md#function-clear-cache)**()<br>clear the cache  |

## Public Slots Documentation

### slot handle_thread_complete

```cpp
void handle_thread_complete(
    const MeshWorkItem & item,
    MeshHandle mesh
)
```


### slot handle_warper_progress

```cpp
void handle_warper_progress()
```


## Public Signals Documentation

### signal new_mesh

```cpp
void new_mesh()
```


### signal error_encountered

```cpp
void error_encountered(
    QString message
)
```


### signal progress

```cpp
void progress(
    int 
)
```


### signal status

```cpp
void status(
    QString 
)
```


## Public Functions Documentation

### function MeshManager

```cpp
MeshManager(
    Preferences & prefs
)
```


### function ~MeshManager

```cpp
~MeshManager()
```


### function generate_mesh

```cpp
void generate_mesh(
    const MeshWorkItem item
)
```

generate and cache a mesh for this shape in a different thread 

### function get_mesh

```cpp
MeshHandle get_mesh(
    const MeshWorkItem & item,
    bool wait =false
)
```

get a mesh for a [MeshWorkItem]()

### function get_mesh

```cpp
MeshHandle get_mesh(
    const vnl_vector< double > & points,
    int domain
)
```

get a mesh for a set of points 

### function get_surface_reconstructor

```cpp
std::shared_ptr< SurfaceReconstructor > get_surface_reconstructor(
    int domain
)
```

return the surface reconstructor for a given domain 

### function get_mesh_warper

```cpp
std::shared_ptr< MeshWarper > get_mesh_warper(
    int domain
)
```

return the mesh warper for a given domain 

### function get_mesh_generator

```cpp
inline std::shared_ptr< MeshGenerator > get_mesh_generator()
```

return the mesh generator 

### function clear_cache

```cpp
void clear_cache()
```

clear the cache 

-------------------------------

Updated on 2021-12-13 at 17:35:57 +0000