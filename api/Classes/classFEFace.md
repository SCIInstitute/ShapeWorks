---
title: FEFace

---

# FEFace





Inherits from [FEItem](../Classes/classFEItem.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[@1](../Classes/classFEFace.md#enum-@1)** { MAX_NODES = 9} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEFace](../Classes/classFEFace.md#function-feface)**()<br>constructor  |
| bool | **[operator==](../Classes/classFEFace.md#function-operator==)**(const [FEFace](../Classes/classFEFace.md) & f)<br>comparison operator  |
| int | **[Nodes](../Classes/classFEFace.md#function-nodes)**()<br>return number of nodes  |
| int | **[Edges](../Classes/classFEFace.md#function-edges)**()<br>return number of edges  |
| void | **[GetEdgeNodes](../Classes/classFEFace.md#function-getedgenodes)**(int i, int * n)<br>get the edge node numbers  |
| bool | **[HasEdge](../Classes/classFEFace.md#function-hasedge)**(int n1, int n2)<br>See if this face has an edge.  |
| bool | **[HasNode](../Classes/classFEFace.md#function-hasnode)**(int i)<br>See if this face has node with ID i.  |
| int | **[FindNode](../Classes/classFEFace.md#function-findnode)**(int i)<br>Fine the array index of node with ID i.  |
| bool | **[IsExternal](../Classes/classFEFace.md#function-isexternal)**()<br>Is this face internal or external.  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[n](../Classes/classFEFace.md#variable-n)** <br>nodal ID's  |
| int | **[m_nodes](../Classes/classFEFace.md#variable-m-nodes)** <br>number of nodes  |
| int | **[m_nbr](../Classes/classFEFace.md#variable-m-nbr)** <br>neighbour faces  |
| [vec3d](../Classes/classvec3d.md) | **[m_fn](../Classes/classFEFace.md#variable-m-fn)** <br>face normal  |
| [vec3d](../Classes/classvec3d.md) | **[m_nn](../Classes/classFEFace.md#variable-m-nn)** <br>node normals  |
| int | **[m_sid](../Classes/classFEFace.md#variable-m-sid)** <br>smoothing ID  |
| int | **[m_elem](../Classes/classFEFace.md#variable-m-elem)** <br>the elements to which this face belongs  |

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

### enum @1

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| MAX_NODES | 9|   |




## Public Functions Documentation

### function FEFace

```cpp
FEFace()
```

constructor 

### function operator==

```cpp
bool operator==(
    const FEFace & f
)
```

comparison operator 

### function Nodes

```cpp
inline int Nodes()
```

return number of nodes 

### function Edges

```cpp
int Edges()
```

return number of edges 

### function GetEdgeNodes

```cpp
void GetEdgeNodes(
    int i,
    int * n
)
```

get the edge node numbers 

### function HasEdge

```cpp
bool HasEdge(
    int n1,
    int n2
)
```

See if this face has an edge. 

### function HasNode

```cpp
bool HasNode(
    int i
)
```

See if this face has node with ID i. 

### function FindNode

```cpp
int FindNode(
    int i
)
```

Fine the array index of node with ID i. 

### function IsExternal

```cpp
inline bool IsExternal()
```

Is this face internal or external. 

## Public Attributes Documentation

### variable n

```cpp
int n;
```

nodal ID's 

### variable m_nodes

```cpp
int m_nodes;
```

number of nodes 

### variable m_nbr

```cpp
int m_nbr;
```

neighbour faces 

### variable m_fn

```cpp
vec3d m_fn;
```

face normal 

### variable m_nn

```cpp
vec3d m_nn;
```

node normals 

### variable m_sid

```cpp
int m_sid;
```

smoothing ID 

### variable m_elem

```cpp
int m_elem;
```

the elements to which this face belongs 

-------------------------------

Updated on 2021-11-18 at 01:30:45 +0000