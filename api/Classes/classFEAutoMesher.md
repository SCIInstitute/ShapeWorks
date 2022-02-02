---
title: FEAutoMesher

---

# FEAutoMesher





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEAutoMesher](../Classes/classFEAutoMesher.md#function-feautomesher)**() |
| void | **[BuildMesh](../Classes/classFEAutoMesher.md#function-buildmesh)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| void | **[AutoPartitionSurface](../Classes/classFEAutoMesher.md#function-autopartitionsurface)**([FEMesh](../Classes/classFEMesh.md) & m) |
| void | **[AutoPartitionEdges](../Classes/classFEAutoMesher.md#function-autopartitionedges)**([FEMesh](../Classes/classFEMesh.md) & m) |
| void | **[AutoPartitionNodes](../Classes/classFEAutoMesher.md#function-autopartitionnodes)**([FEMesh](../Classes/classFEMesh.md) & m) |
| void | **[Repartition](../Classes/classFEAutoMesher.md#function-repartition)**([FEMesh](../Classes/classFEMesh.md) & m) |
| void | **[SetSmoothingAngle](../Classes/classFEAutoMesher.md#function-setsmoothingangle)**(double w) |
| double | **[GetSmoothingAngle](../Classes/classFEAutoMesher.md#function-getsmoothingangle)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[BuildFaces](../Classes/classFEAutoMesher.md#function-buildfaces)**([FEMesh](../Classes/classFEMesh.md) & m) |
| void | **[BuildEdges](../Classes/classFEAutoMesher.md#function-buildedges)**([FEMesh](../Classes/classFEMesh.md) & m) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [FEMesh](../Classes/classFEMesh.md) * | **[m_pm](../Classes/classFEAutoMesher.md#variable-m-pm)**  |
| double | **[m_wsmooth](../Classes/classFEAutoMesher.md#variable-m-wsmooth)**  |

## Public Functions Documentation

### function FEAutoMesher

```cpp
FEAutoMesher()
```


### function BuildMesh

```cpp
void BuildMesh(
    FEMesh * pm
)
```


### function AutoPartitionSurface

```cpp
void AutoPartitionSurface(
    FEMesh & m
)
```


### function AutoPartitionEdges

```cpp
void AutoPartitionEdges(
    FEMesh & m
)
```


### function AutoPartitionNodes

```cpp
void AutoPartitionNodes(
    FEMesh & m
)
```


### function Repartition

```cpp
void Repartition(
    FEMesh & m
)
```


### function SetSmoothingAngle

```cpp
inline void SetSmoothingAngle(
    double w
)
```


### function GetSmoothingAngle

```cpp
inline double GetSmoothingAngle()
```


## Protected Functions Documentation

### function BuildFaces

```cpp
void BuildFaces(
    FEMesh & m
)
```


### function BuildEdges

```cpp
void BuildEdges(
    FEMesh & m
)
```


## Protected Attributes Documentation

### variable m_pm

```cpp
FEMesh * m_pm;
```


### variable m_wsmooth

```cpp
double m_wsmooth;
```


-------------------------------

Updated on 2022-02-02 at 18:18:09 +0000