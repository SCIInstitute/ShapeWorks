---
title: FEMeshSmoothingModifier

---

# FEMeshSmoothingModifier



 [More...](#detailed-description)


`#include <FEMeshSmoothingModifier.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEMeshSmoothingModifier](../Classes/classFEMeshSmoothingModifier.md#function-femeshsmoothingmodifier)**()<br>Constructor.  |
| [FEMesh](../Classes/classFEMesh.md) * | **[Apply](../Classes/classFEMeshSmoothingModifier.md#function-apply)**([FEMesh](../Classes/classFEMesh.md) * pm)<br>Apply the smoothing modifier.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| double | **[area_triangle](../Classes/classFEMeshSmoothingModifier.md#function-area-triangle)**([vec3d](../Classes/classvec3d.md) r[3]) |
| double | **[distance](../Classes/classFEMeshSmoothingModifier.md#function-distance)**([vec3d](../Classes/classvec3d.md) x, [vec3d](../Classes/classvec3d.md) y) |
| double | **[random](../Classes/classFEMeshSmoothingModifier.md#function-random)**() |
| void | **[Laplacian_Smoothing](../Classes/classFEMeshSmoothingModifier.md#function-laplacian-smoothing)**([FEMesh](../Classes/classFEMesh.md) * pm, vector< int > hashmap) |
| void | **[Laplacian_Smoothing2](../Classes/classFEMeshSmoothingModifier.md#function-laplacian-smoothing2)**([FEMesh](../Classes/classFEMesh.md) * pm, vector< int > hashmap) |
| void | **[Taubin_Smoothing](../Classes/classFEMeshSmoothingModifier.md#function-taubin-smoothing)**([FEMesh](../Classes/classFEMesh.md) * pm, vector< int > hashmap) |
| void | **[Crease_Enhancing_Diffusion](../Classes/classFEMeshSmoothingModifier.md#function-crease-enhancing-diffusion)**([FEMesh](../Classes/classFEMesh.md) * pm, vector< int > hashmap) |
| void | **[Add_Noise](../Classes/classFEMeshSmoothingModifier.md#function-add-noise)**([FEMesh](../Classes/classFEMesh.md) * pm, vector< int > hashmap) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_threshold1](../Classes/classFEMeshSmoothingModifier.md#variable-m-threshold1)**  |
| double | **[m_threshold2](../Classes/classFEMeshSmoothingModifier.md#variable-m-threshold2)**  |
| double | **[m_iteration](../Classes/classFEMeshSmoothingModifier.md#variable-m-iteration)**  |
| int | **[m_method](../Classes/classFEMeshSmoothingModifier.md#variable-m-method)**  |

## Detailed Description

```cpp
class FEMeshSmoothingModifier;
```


This class implements a modifier that coarses a plygonal mesh using an approximated centroidal voronoi diagram. 

## Public Functions Documentation

### function FEMeshSmoothingModifier

```cpp
FEMeshSmoothingModifier()
```

Constructor. 

### function Apply

```cpp
FEMesh * Apply(
    FEMesh * pm
)
```

Apply the smoothing modifier. 

## Protected Functions Documentation

### function area_triangle

```cpp
double area_triangle(
    vec3d r[3]
)
```


### function distance

```cpp
double distance(
    vec3d x,
    vec3d y
)
```


### function random

```cpp
double random()
```


### function Laplacian_Smoothing

```cpp
void Laplacian_Smoothing(
    FEMesh * pm,
    vector< int > hashmap
)
```


### function Laplacian_Smoothing2

```cpp
void Laplacian_Smoothing2(
    FEMesh * pm,
    vector< int > hashmap
)
```


### function Taubin_Smoothing

```cpp
void Taubin_Smoothing(
    FEMesh * pm,
    vector< int > hashmap
)
```


### function Crease_Enhancing_Diffusion

```cpp
void Crease_Enhancing_Diffusion(
    FEMesh * pm,
    vector< int > hashmap
)
```


### function Add_Noise

```cpp
void Add_Noise(
    FEMesh * pm,
    vector< int > hashmap
)
```


## Public Attributes Documentation

### variable m_threshold1

```cpp
double m_threshold1;
```


### variable m_threshold2

```cpp
double m_threshold2;
```


### variable m_iteration

```cpp
double m_iteration;
```


### variable m_method

```cpp
int m_method;
```


-------------------------------

Updated on 2021-12-07 at 17:10:24 +0000