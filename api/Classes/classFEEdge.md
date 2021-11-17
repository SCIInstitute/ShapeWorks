---
title: FEEdge

---

# FEEdge





Inherits from [FEItem](../Classes/classFEItem.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[@0](../Classes/classFEEdge.md#enum-@0)** { MAX_NODES = 3} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEEdge](../Classes/classFEEdge.md#function-feedge)**()<br>constructor  |
| bool | **[operator==](../Classes/classFEEdge.md#function-operator==)**(const [FEEdge](../Classes/classFEEdge.md) & e)<br>edge comparison  |
| int | **[Nodes](../Classes/classFEEdge.md#function-nodes)**()<br>return number of nodes  |
| int | **[FindNode](../Classes/classFEEdge.md#function-findnode)**(int node)<br>find a node  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[n](../Classes/classFEEdge.md#variable-n)** <br>edge nodes  |
| int | **[m_elem](../Classes/classFEEdge.md#variable-m-elem)** <br>the element to which this edge belongs (used only by beams)  |
| int | **[m_nbr](../Classes/classFEEdge.md#variable-m-nbr)** <br>the two adjacent edges (if there are more edges incident to a node, the neighbour is set to -1)  |

## Additional inherited members

**Public Functions inherited from [FEItem](../Classes/classFEItem.md)**

|                | Name           |
| -------------- | -------------- |
| | **[FEItem](../Classes/classFEItem.md#function-feitem)**() |
| bool | **[IsVisible](../Classes/classFEItem.md#function-isvisible)**() const |
| bool | **[IsSelected](../Classes/classFEItem.md#function-isselected)**() const |
| void | **[Select](../Classes/classFEItem.md#function-select)**() |
| void | **[UnSelect](../Classes/classFEItem.md#function-unselect)**() |
| void | **[Show](../Classes/classFEItem.md#function-show)**() |
| void | **[Hide](../Classes/classFEItem.md#function-hide)**() |
| unsigned int | **[GetFEState](../Classes/classFEItem.md#function-getfestate)**() const |
| void | **[SetFEState](../Classes/classFEItem.md#function-setfestate)**(unsigned int state) |

**Public Attributes inherited from [FEItem](../Classes/classFEItem.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[m_ntag](../Classes/classFEItem.md#variable-m-ntag)**  |
| int | **[m_gid](../Classes/classFEItem.md#variable-m-gid)**  |
| int | **[m_nid](../Classes/classFEItem.md#variable-m-nid)**  |


## Public Types Documentation

### enum @0

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| MAX_NODES | 3|   |




## Public Functions Documentation

### function FEEdge

```cpp
FEEdge()
```

constructor 

### function operator==

```cpp
bool operator==(
    const FEEdge & e
)
```

edge comparison 

### function Nodes

```cpp
inline int Nodes()
```

return number of nodes 

### function FindNode

```cpp
int FindNode(
    int node
)
```

find a node 

## Public Attributes Documentation

### variable n

```cpp
int n;
```

edge nodes 

### variable m_elem

```cpp
int m_elem;
```

the element to which this edge belongs (used only by beams) 

### variable m_nbr

```cpp
int m_nbr;
```

the two adjacent edges (if there are more edges incident to a node, the neighbour is set to -1) 

-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000