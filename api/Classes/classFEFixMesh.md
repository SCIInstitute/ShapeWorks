---
title: FEFixMesh

---

# FEFixMesh





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEFixMesh](../Classes/classFEFixMesh.md#function-fefixmesh)**() |
| [FEMesh](../Classes/classFEMesh.md) * | **[Apply](../Classes/classFEFixMesh.md#function-apply)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| void | **[SetTask](../Classes/classFEFixMesh.md#function-settask)**(int n) |
| void | **[SetThickness](../Classes/classFEFixMesh.md#function-setthickness)**(double m) |
| [FEMesh](../Classes/classFEMesh.md) * | **[RemoveDuplicateElements](../Classes/classFEFixMesh.md#function-removeduplicateelements)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[RemoveNonManifoldElements](../Classes/classFEFixMesh.md#function-removenonmanifoldelements)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[FixElementWinding](../Classes/classFEFixMesh.md#function-fixelementwinding)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[FillAllHoles](../Classes/classFEFixMesh.md#function-fillallholes)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[FixinvertedElements](../Classes/classFEFixMesh.md#function-fixinvertedelements)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[FixReferenceSurface](../Classes/classFEFixMesh.md#function-fixreferencesurface)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| [FEMesh](../Classes/classFEMesh.md) * | **[InterpolateShellThickness](../Classes/classFEFixMesh.md#function-interpolateshellthickness)**([FEMesh](../Classes/classFEMesh.md) * pm) |

## Public Functions Documentation

### function FEFixMesh

```cpp
inline FEFixMesh()
```


### function Apply

```cpp
FEMesh * Apply(
    FEMesh * pm
)
```


### function SetTask

```cpp
void SetTask(
    int n
)
```


### function SetThickness

```cpp
void SetThickness(
    double m
)
```


### function RemoveDuplicateElements

```cpp
FEMesh * RemoveDuplicateElements(
    FEMesh * pm
)
```


### function RemoveNonManifoldElements

```cpp
FEMesh * RemoveNonManifoldElements(
    FEMesh * pm
)
```


### function FixElementWinding

```cpp
FEMesh * FixElementWinding(
    FEMesh * pm
)
```


### function FillAllHoles

```cpp
FEMesh * FillAllHoles(
    FEMesh * pm
)
```


### function FixinvertedElements

```cpp
FEMesh * FixinvertedElements(
    FEMesh * pm
)
```


### function FixReferenceSurface

```cpp
FEMesh * FixReferenceSurface(
    FEMesh * pm
)
```


### function InterpolateShellThickness

```cpp
FEMesh * InterpolateShellThickness(
    FEMesh * pm
)
```


-------------------------------

Updated on 2022-03-03 at 07:50:36 +0000