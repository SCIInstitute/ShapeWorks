---
title: FEAreaCoverage::Surface

---

# FEAreaCoverage::Surface





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Surface](../Classes/classFEAreaCoverage_1_1Surface.md#function-surface)**() |
| int | **[Faces](../Classes/classFEAreaCoverage_1_1Surface.md#function-faces)**() |
| void | **[Create](../Classes/classFEAreaCoverage_1_1Surface.md#function-create)**(std::shared_ptr< [FEMesh](../Classes/classFEMesh.md) > m) |
| int | **[Nodes](../Classes/classFEAreaCoverage_1_1Surface.md#function-nodes)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| std::shared_ptr< [FEMesh](../Classes/classFEMesh.md) > | **[m_mesh](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-mesh)**  |
| vector< int > | **[m_face](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-face)**  |
| vector< int > | **[m_node](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-node)**  |
| vector< [vec3d](../Classes/classvec3d.md) > | **[m_pos](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-pos)**  |
| vector< int > | **[m_lnode](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-lnode)**  |
| vector< [vec3d](../Classes/classvec3d.md) > | **[m_norm](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-norm)**  |
| vector< [vec3d](../Classes/classvec3d.md) > | **[m_fnorm](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-fnorm)**  |
| vector< vector< int > > | **[m_NLT](../Classes/classFEAreaCoverage_1_1Surface.md#variable-m-nlt)**  |

## Public Functions Documentation

### function Surface

```cpp
inline Surface()
```


### function Faces

```cpp
inline int Faces()
```


### function Create

```cpp
void Create(
    std::shared_ptr< FEMesh > m
)
```


### function Nodes

```cpp
inline int Nodes()
```


## Public Attributes Documentation

### variable m_mesh

```cpp
std::shared_ptr< FEMesh > m_mesh;
```


### variable m_face

```cpp
vector< int > m_face;
```


### variable m_node

```cpp
vector< int > m_node;
```


### variable m_pos

```cpp
vector< vec3d > m_pos;
```


### variable m_lnode

```cpp
vector< int > m_lnode;
```


### variable m_norm

```cpp
vector< vec3d > m_norm;
```


### variable m_fnorm

```cpp
vector< vec3d > m_fnorm;
```


### variable m_NLT

```cpp
vector< vector< int > > m_NLT;
```


-------------------------------

Updated on 2022-01-01 at 17:59:58 +0000