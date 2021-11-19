---
title: FENodeElementList

---

# FENodeElementList





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FENodeElementList](../Classes/classFENodeElementList.md#function-fenodeelementlist)**([FEMesh](../Classes/classFEMesh.md) * pm) |
| | **[~FENodeElementList](../Classes/classFENodeElementList.md#function-~fenodeelementlist)**() |
| void | **[Build](../Classes/classFENodeElementList.md#function-build)**() |
| int | **[Valence](../Classes/classFENodeElementList.md#function-valence)**(int n) |
| [FEElement](../Classes/classFEElement.md) * | **[Element](../Classes/classFENodeElementList.md#function-element)**(int n, int j) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [FEMesh](../Classes/classFEMesh.md) * | **[m_pm](../Classes/classFENodeElementList.md#variable-m-pm)**  |
| vector< int > | **[m_val](../Classes/classFENodeElementList.md#variable-m-val)**  |
| vector< int > | **[m_off](../Classes/classFENodeElementList.md#variable-m-off)**  |
| vector< [FEElement](../Classes/classFEElement.md) * > | **[m_pelem](../Classes/classFENodeElementList.md#variable-m-pelem)**  |

## Public Functions Documentation

### function FENodeElementList

```cpp
FENodeElementList(
    FEMesh * pm
)
```


### function ~FENodeElementList

```cpp
~FENodeElementList()
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


### function Element

```cpp
inline FEElement * Element(
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


### variable m_pelem

```cpp
vector< FEElement * > m_pelem;
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000