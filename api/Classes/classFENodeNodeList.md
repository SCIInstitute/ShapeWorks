---
title: FENodeNodeList

---

# FENodeNodeList





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FENodeNodeList](../Classes/classFENodeNodeList.md#function-fenodenodelist)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| | **[~FENodeNodeList](../Classes/classFENodeNodeList.md#function-~fenodenodelist)**() |
| void | **[Build](../Classes/classFENodeNodeList.md#function-build)**() |
| int | **[Valence](../Classes/classFENodeNodeList.md#function-valence)**(int n) |
| int | **[Node](../Classes/classFENodeNodeList.md#function-node)**(int n, int j) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [FEMesh](../Classes/classFEMesh.md) * | **[m_pm](../Classes/classFENodeNodeList.md#variable-m-pm)**  |
| vector< int > | **[m_val](../Classes/classFENodeNodeList.md#variable-m-val)**  |
| vector< int > | **[m_off](../Classes/classFENodeNodeList.md#variable-m-off)**  |
| vector< int > | **[m_node](../Classes/classFENodeNodeList.md#variable-m-node)**  |

## Public Functions Documentation

### function FENodeNodeList

```cpp
FENodeNodeList(
    FEMesh * pm
)
```


### function ~FENodeNodeList

```cpp
~FENodeNodeList()
```


### function Build

```cpp
void Build()
```


### function Valence

```cpp
inline int Valence(
    int n
)
```


### function Node

```cpp
inline int Node(
    int n,
    int j
)
```


## Protected Attributes Documentation

### variable m_pm

```cpp
FEMesh * m_pm;
```


### variable m_val

```cpp
vector< int > m_val;
```


### variable m_off

```cpp
vector< int > m_off;
```


### variable m_node

```cpp
vector< int > m_node;
```


-------------------------------

Updated on 2021-11-15 at 18:13:25 +0000