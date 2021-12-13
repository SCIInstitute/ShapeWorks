---
title: FEVTKimport

---

# FEVTKimport





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEVTKimport](../Classes/classFEVTKimport.md#function-fevtkimport)**() |
| | **[~FEVTKimport](../Classes/classFEVTKimport.md#function-~fevtkimport)**(void ) |
| [FEMesh](../Classes/classFEMesh.md) * | **[Load](../Classes/classFEVTKimport.md#function-load)**(const char * szfile) |
| [FEMesh](../Classes/classFEMesh.md) * | **[Load](../Classes/classFEVTKimport.md#function-load)**(istream & stream) |
| [FEMesh](../Classes/classFEMesh.md) * | **[Load](../Classes/classFEVTKimport.md#function-load)**(vtkPolyData * polydata) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[BuildMesh](../Classes/classFEVTKimport.md#function-buildmesh)**() |
| [FEMesh](../Classes/classFEMesh.md) * | **[errf](../Classes/classFEVTKimport.md#function-errf)**(const char * sz, ... ) |
| void | **[Close](../Classes/classFEVTKimport.md#function-close)**() |

## Public Functions Documentation

### function FEVTKimport

```cpp
FEVTKimport()
```


### function ~FEVTKimport

```cpp
~FEVTKimport(
    void 
)
```


### function Load

```cpp
FEMesh * Load(
    const char * szfile
)
```


### function Load

```cpp
FEMesh * Load(
    istream & stream
)
```


### function Load

```cpp
FEMesh * Load(
    vtkPolyData * polydata
)
```


## Protected Functions Documentation

### function BuildMesh

```cpp
bool BuildMesh()
```


### function errf

```cpp
FEMesh * errf(
    const char * sz,
    ... 
)
```


### function Close

```cpp
void Close()
```


-------------------------------

Updated on 2021-12-13 at 17:35:58 +0000