---
title: FEFillHole

---

# FEFillHole





## Public Classes

|                | Name           |
| -------------- | -------------- |
| class | **[EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md)**  |
| struct | **[FACE](../Classes/structFEFillHole_1_1FACE.md)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEFillHole](../Classes/classFEFillHole.md#function-fefillhole)**() |
| [FEMesh](../Classes/classFEMesh.md) * | **[Apply](../Classes/classFEFillHole.md#function-apply)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| bool | **[AFM](../Classes/classFEFillHole.md#function-afm)**([FEMesh](../Classes/classFEMesh.md) & mesh, [EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ring, vector< [FACE](../Classes/structFEFillHole_1_1FACE.md) > & tri_list, vector< [vec3d](../Classes/classvec3d.md) > & node_list) |
| [vec3d](../Classes/classvec3d.md) | **[newNode](../Classes/classFEFillHole.md#function-newnode)**([vec3d](../Classes/classvec3d.md) current_node, [vec3d](../Classes/classvec3d.md) next_node, [vec3d](../Classes/classvec3d.md) prev_node, [vec3d](../Classes/classvec3d.md) node_normal, double scale, bool concave) |
| bool | **[DivideRing](../Classes/classFEFillHole.md#function-dividering)**([EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ring, vector< [FACE](../Classes/structFEFillHole_1_1FACE.md) > & tri_list) |
| bool | **[DivideRing1](../Classes/classFEFillHole.md#function-dividering1)**([EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ring, vector< [FACE](../Classes/structFEFillHole_1_1FACE.md) > & tri_list) |
| void | **[FillAllHoles](../Classes/classFEFillHole.md#function-fillallholes)**([FEMesh](../Classes/classFEMesh.md) * pm) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[optimize](../Classes/classFEFillHole.md#variable-optimize)**  |
| bool | **[insertNodes](../Classes/classFEFillHole.md#variable-insertnodes)**  |

## Public Functions Documentation

### function FEFillHole

```cpp
inline FEFillHole()
```


### function Apply

```cpp
FEMesh * Apply(
    FEMesh * pm
)
```


### function AFM

```cpp
bool AFM(
    FEMesh & mesh,
    EdgeRing & ring,
    vector< FACE > & tri_list,
    vector< vec3d > & node_list
)
```


### function newNode

```cpp
vec3d newNode(
    vec3d current_node,
    vec3d next_node,
    vec3d prev_node,
    vec3d node_normal,
    double scale,
    bool concave
)
```


### function DivideRing

```cpp
bool DivideRing(
    EdgeRing & ring,
    vector< FACE > & tri_list
)
```


### function DivideRing1

```cpp
bool DivideRing1(
    EdgeRing & ring,
    vector< FACE > & tri_list
)
```


### function FillAllHoles

```cpp
void FillAllHoles(
    FEMesh * pm
)
```


## Public Attributes Documentation

### variable optimize

```cpp
bool optimize;
```


### variable insertNodes

```cpp
bool insertNodes;
```


-------------------------------

Updated on 2022-02-02 at 18:18:09 +0000