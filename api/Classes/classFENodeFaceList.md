---
title: FENodeFaceList

---

# FENodeFaceList





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FENodeFaceList](../Classes/classFENodeFaceList.md#function-fenodefacelist)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| | **[~FENodeFaceList](../Classes/classFENodeFaceList.md#function-~fenodefacelist)**(void ) |
| void | **[Build](../Classes/classFENodeFaceList.md#function-build)**() |
| void | **[BuildSorted](../Classes/classFENodeFaceList.md#function-buildsorted)**() |
| int | **[Nodes](../Classes/classFENodeFaceList.md#function-nodes)**() |
| [FENode](../Classes/classFENode.md) & | **[Node](../Classes/classFENodeFaceList.md#function-node)**(int i) |
| int | **[Valence](../Classes/classFENodeFaceList.md#function-valence)**(int i) |
| [FEFace](../Classes/classFEFace.md) * | **[Face](../Classes/classFENodeFaceList.md#function-face)**(int n, int i) |
| bool | **[HasFace](../Classes/classFENodeFaceList.md#function-hasface)**(int n, [FEFace](../Classes/classFEFace.md) * pf) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[Sort](../Classes/classFENodeFaceList.md#function-sort)**(int node) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [FEMesh](../Classes/classFEMesh.md) * | **[m_pm](../Classes/classFENodeFaceList.md#variable-m-pm)**  |
| vector< int > | **[m_node](../Classes/classFENodeFaceList.md#variable-m-node)**  |
| vector< int > | **[m_val](../Classes/classFENodeFaceList.md#variable-m-val)**  |
| vector< [FEFace](../Classes/classFEFace.md) * > | **[m_pface](../Classes/classFENodeFaceList.md#variable-m-pface)**  |
| vector< int > | **[m_off](../Classes/classFENodeFaceList.md#variable-m-off)**  |

## Public Functions Documentation

### function FENodeFaceList

```cpp
FENodeFaceList(
    FEMesh * pm
)
```


### function ~FENodeFaceList

```cpp
~FENodeFaceList(
    void 
)
```


### function Build

```cpp
void Build()
```


### function BuildSorted

```cpp
void BuildSorted()
```


### function Nodes

```cpp
inline int Nodes()
```


### function Node

```cpp
inline FENode & Node(
    int i
)
```


### function Valence

```cpp
inline int Valence(
    int i
)
```


### function Face

```cpp
inline FEFace * Face(
    int n,
    int i
)
```


### function HasFace

```cpp
bool HasFace(
    int n,
    FEFace * pf
)
```


## Protected Functions Documentation

### function Sort

```cpp
void Sort(
    int node
)
```


## Protected Attributes Documentation

### variable m_pm

```cpp
FEMesh * m_pm;
```


### variable m_node

```cpp
vector< int > m_node;
```


### variable m_val

```cpp
vector< int > m_val;
```


### variable m_pface

```cpp
vector< FEFace * > m_pface;
```


### variable m_off

```cpp
vector< int > m_off;
```


-------------------------------

Updated on 2022-01-01 at 17:59:58 +0000