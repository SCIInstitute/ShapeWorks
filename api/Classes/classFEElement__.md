---
title: FEElement_

---

# FEElement_





Inherits from [FEItem](../Classes/classFEItem.md)

Inherited by [FEElement](../Classes/classFEElement.md), [FETri3](../Classes/classFETri3.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEElement_](../Classes/classFEElement__.md#function-feelement-)**()<br>constructor  |
| bool | **[is_equal](../Classes/classFEElement__.md#function-is-equal)**([FEElement_](../Classes/classFEElement__.md) & e) |
| int | **[GetType](../Classes/classFEElement__.md#function-gettype)**() const<br>return the element type  |
| bool | **[IsType](../Classes/classFEElement__.md#function-istype)**(int ntype) const<br>Is the element of this type.  |
| int | **[Nodes](../Classes/classFEElement__.md#function-nodes)**() const<br>number of nodes  |
| int | **[Faces](../Classes/classFEElement__.md#function-faces)**() const<br>Number of faces (shells have no faces)  |
| int | **[Edges](../Classes/classFEElement__.md#function-edges)**() const<br>Number of edges (solids have no edges)  |
| int | **[GetFace](../Classes/classFEElement__.md#function-getface)**(int i, int * n)<br>Get only the nodes of face i (only solids have faces)  |
| [FEFace](../Classes/classFEFace.md) | **[GetFace](../Classes/classFEElement__.md#function-getface)**(int i)<br>Get the face i (only solids have faces)  |
| [FEFace](../Classes/classFEFace.md) | **[GetShellFace](../Classes/classFEElement__.md#function-getshellface)**()<br>Get the face of a shell.  |
| [FEEdge](../Classes/classFEEdge.md) | **[GetEdge](../Classes/classFEElement__.md#function-getedge)**(int i)<br>Get the edge.  |
| bool | **[IsExterior](../Classes/classFEElement__.md#function-isexterior)**()<br>Is this an exterior element.  |
| bool | **[IsSolid](../Classes/classFEElement__.md#function-issolid)**() |
| bool | **[IsShell](../Classes/classFEElement__.md#function-isshell)**() |
| bool | **[IsBeam](../Classes/classFEElement__.md#function-isbeam)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[copy](../Classes/classFEElement__.md#function-copy)**(const [FEElement_](../Classes/classFEElement__.md) & el) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| int * | **[m_node](../Classes/classFEElement__.md#variable-m-node)** <br>pointer to node data  |
| int * | **[m_nbr](../Classes/classFEElement__.md#variable-m-nbr)** <br>neighbour elements  |
| int * | **[m_face](../Classes/classFEElement__.md#variable-m-face)** <br>faces (-1 for interior faces)  |
| double * | **[m_h](../Classes/classFEElement__.md#variable-m-h)** <br>element thickness (only used by shells)  |
| [vec3d](../Classes/classvec3d.md) | **[m_fiber](../Classes/classFEElement__.md#variable-m-fiber)** <br>fiber orientation  |
| [mat3d](../Classes/classmat3d.md) | **[m_Q](../Classes/classFEElement__.md#variable-m-q)** <br>local material orientation  |
| bool | **[m_Qactive](../Classes/classFEElement__.md#variable-m-qactive)** <br>active local material orientation  |
| double | **[m_a0](../Classes/classFEElement__.md#variable-m-a0)** <br>cross-sectional area (only used by truss elements)  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_ntype](../Classes/classFEElement__.md#variable-m-ntype)** <br>type of element  |
| int | **[m_nodes](../Classes/classFEElement__.md#variable-m-nodes)** <br>nr of nodes  |
| int | **[m_nfaces](../Classes/classFEElement__.md#variable-m-nfaces)** <br>nr of faces ( 0 for shells)  |
| int | **[m_nedges](../Classes/classFEElement__.md#variable-m-nedges)** <br>nr of edges ( 0 for solids)  |

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


## Public Functions Documentation

### function FEElement_

```cpp
FEElement_()
```

constructor 

### function is_equal

```cpp
bool is_equal(
    FEElement_ & e
)
```


### function GetType

```cpp
inline int GetType() const
```

return the element type 

### function IsType

```cpp
inline bool IsType(
    int ntype
) const
```

Is the element of this type. 

### function Nodes

```cpp
inline int Nodes() const
```

number of nodes 

### function Faces

```cpp
inline int Faces() const
```

Number of faces (shells have no faces) 

### function Edges

```cpp
inline int Edges() const
```

Number of edges (solids have no edges) 

### function GetFace

```cpp
int GetFace(
    int i,
    int * n
)
```

Get only the nodes of face i (only solids have faces) 

### function GetFace

```cpp
FEFace GetFace(
    int i
)
```

Get the face i (only solids have faces) 

### function GetShellFace

```cpp
FEFace GetShellFace()
```

Get the face of a shell. 

### function GetEdge

```cpp
FEEdge GetEdge(
    int i
)
```

Get the edge. 

### function IsExterior

```cpp
bool IsExterior()
```

Is this an exterior element. 

### function IsSolid

```cpp
inline bool IsSolid()
```


### function IsShell

```cpp
inline bool IsShell()
```


### function IsBeam

```cpp
inline bool IsBeam()
```


## Protected Functions Documentation

### function copy

```cpp
void copy(
    const FEElement_ & el
)
```


## Public Attributes Documentation

### variable m_node

```cpp
int * m_node;
```

pointer to node data 

### variable m_nbr

```cpp
int * m_nbr;
```

neighbour elements 

### variable m_face

```cpp
int * m_face;
```

faces (-1 for interior faces) 

### variable m_h

```cpp
double * m_h;
```

element thickness (only used by shells) 

### variable m_fiber

```cpp
vec3d m_fiber;
```

fiber orientation 

**Todo**: maybe I can add an element attribute section 

### variable m_Q

```cpp
mat3d m_Q;
```

local material orientation 

### variable m_Qactive

```cpp
bool m_Qactive;
```

active local material orientation 

### variable m_a0

```cpp
double m_a0;
```

cross-sectional area (only used by truss elements) 

## Protected Attributes Documentation

### variable m_ntype

```cpp
int m_ntype;
```

type of element 

### variable m_nodes

```cpp
int m_nodes;
```

nr of nodes 

### variable m_nfaces

```cpp
int m_nfaces;
```

nr of faces ( 0 for shells) 

### variable m_nedges

```cpp
int m_nedges;
```

nr of edges ( 0 for solids) 

-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000