---
title: shapeworks::MeshCache
summary: Thread safe cache for meshes index by shape. 

---

# shapeworks::MeshCache



Thread safe cache for meshes index by shape.  [More...](#detailed-description)


`#include <MeshCache.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshCache](../Classes/classshapeworks_1_1MeshCache.md#function-meshcache)**() |
| void | **[set_cache_enabled](../Classes/classshapeworks_1_1MeshCache.md#function-set-cache-enabled)**(bool enabled) |
| void | **[set_memory_percent](../Classes/classshapeworks_1_1MeshCache.md#function-set-memory-percent)**(int percent) |
| MeshHandle | **[get_mesh](../Classes/classshapeworks_1_1MeshCache.md#function-get-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & vector) |
| void | **[insert_mesh](../Classes/classshapeworks_1_1MeshCache.md#function-insert-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item, MeshHandle mesh) |
| void | **[clear](../Classes/classshapeworks_1_1MeshCache.md#function-clear)**() |

## Detailed Description

```cpp
class shapeworks::MeshCache;
```

Thread safe cache for meshes index by shape. 

The [MeshCache](../Classes/classshapeworks_1_1MeshCache.md) implements a std::map keyed by shape (list of points) with MeshHandle values. It is thread-safe and can be used from any thread. 

## Public Functions Documentation

### function MeshCache

```cpp
MeshCache()
```


### function set_cache_enabled

```cpp
inline void set_cache_enabled(
    bool enabled
)
```


### function set_memory_percent

```cpp
inline void set_memory_percent(
    int percent
)
```


### function get_mesh

```cpp
MeshHandle get_mesh(
    const MeshWorkItem & vector
)
```


### function insert_mesh

```cpp
void insert_mesh(
    const MeshWorkItem & item,
    MeshHandle mesh
)
```


### function clear

```cpp
void clear()
```


-------------------------------

Updated on 2025-03-19 at 04:20:51 +0000