---
title: shapeworks::MeshCache

---

# shapeworks::MeshCache





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshCache](../Classes/classshapeworks_1_1MeshCache.md#function-meshcache)**([Preferences](../Classes/classPreferences.md) & prefs) |
| MeshHandle | **[get_mesh](../Classes/classshapeworks_1_1MeshCache.md#function-get-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & vector) |
| void | **[insert_mesh](../Classes/classshapeworks_1_1MeshCache.md#function-insert-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item, MeshHandle mesh) |
| void | **[clear](../Classes/classshapeworks_1_1MeshCache.md#function-clear)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| [Preferences](../Classes/classPreferences.md) * | **[pref_ref_](../Classes/classshapeworks_1_1MeshCache.md#variable-pref-ref-)**  |

## Public Functions Documentation

### function MeshCache

```cpp
MeshCache(
    Preferences & prefs
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


## Public Attributes Documentation

### variable pref_ref_

```cpp
static Preferences * pref_ref_;
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000