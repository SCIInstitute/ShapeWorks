---
title: FECoreMesh

---

# FECoreMesh



 [More...](#detailed-description)


`#include <FECoreMesh.h>`

Inherited by [FEMesh](../Classes/classFEMesh.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FECoreMesh](../Classes/classFECoreMesh.md#function-fecoremesh)**()<br>constructor  |
| virtual | **[~FECoreMesh](../Classes/classFECoreMesh.md#function-~fecoremesh)**()<br>destructor  |
| virtual void | **[Create](../Classes/classFECoreMesh.md#function-create)**(int nodes, int elems, int faces =0, int edges =0) =0<br>allocate space for mesh  |
| bool | **[IsType](../Classes/classFECoreMesh.md#function-istype)**(int ntype)<br>check the type of the mesh  |
| virtual int | **[Elements](../Classes/classFECoreMesh.md#function-elements)**() const =0<br>total number of elements  |
| virtual [FEElement_](../Classes/classFEElement__.md) & | **[ElementRef](../Classes/classFECoreMesh.md#function-elementref)**(int n) =0<br>return reference to element  |
| virtual [FEElement](../Classes/classFEElement.md) * | **[ElementPtr](../Classes/classFECoreMesh.md#function-elementptr)**(int n =0) =0<br>return pointer to element  |
| int | **[Nodes](../Classes/classFECoreMesh.md#function-nodes)**() const |
| int | **[Edges](../Classes/classFECoreMesh.md#function-edges)**() const |
| int | **[Faces](../Classes/classFECoreMesh.md#function-faces)**() const |
| [FENode](../Classes/classFENode.md) & | **[Node](../Classes/classFECoreMesh.md#function-node)**(int n) |
| [FEEdge](../Classes/classFEEdge.md) & | **[Edge](../Classes/classFECoreMesh.md#function-edge)**(int n) |
| [FEFace](../Classes/classFEFace.md) & | **[Face](../Classes/classFECoreMesh.md#function-face)**(int n) |
| [FENode](../Classes/classFENode.md) * | **[NodePtr](../Classes/classFECoreMesh.md#function-nodeptr)**(int n =0) |
| [FEEdge](../Classes/classFEEdge.md) * | **[EdgePtr](../Classes/classFECoreMesh.md#function-edgeptr)**(int n =0) |
| [FEFace](../Classes/classFEFace.md) * | **[FacePtr](../Classes/classFECoreMesh.md#function-faceptr)**(int n =0) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< [FENode](../Classes/classFENode.md) > | **[m_Node](../Classes/classFECoreMesh.md#variable-m-node)** <br>FE nodes.  |
| std::vector< [FEEdge](../Classes/classFEEdge.md) > | **[m_Edge](../Classes/classFECoreMesh.md#variable-m-edge)** <br>FE edges.  |
| std::vector< [FEFace](../Classes/classFEFace.md) > | **[m_Face](../Classes/classFECoreMesh.md#variable-m-face)** <br>FE faces.  |

## Detailed Description

```cpp
class FECoreMesh;
```


This class defines a simple mesh structure that provides basic container services for storing mesh data. 

## Public Functions Documentation

### function FECoreMesh

```cpp
FECoreMesh()
```

constructor 

### function ~FECoreMesh

```cpp
virtual ~FECoreMesh()
```

destructor 

### function Create

```cpp
virtual void Create(
    int nodes,
    int elems,
    int faces =0,
    int edges =0
) =0
```

allocate space for mesh 

**Reimplemented by**: [FEMesh::Create](../Classes/classFEMesh.md#function-create)


### function IsType

```cpp
bool IsType(
    int ntype
)
```

check the type of the mesh 

### function Elements

```cpp
virtual int Elements() const =0
```

total number of elements 

**Reimplemented by**: [FEMesh::Elements](../Classes/classFEMesh.md#function-elements)


### function ElementRef

```cpp
virtual FEElement_ & ElementRef(
    int n
) =0
```

return reference to element 

**Reimplemented by**: [FEMesh::ElementRef](../Classes/classFEMesh.md#function-elementref)


### function ElementPtr

```cpp
virtual FEElement * ElementPtr(
    int n =0
) =0
```

return pointer to element 

**Reimplemented by**: [FEMesh::ElementPtr](../Classes/classFEMesh.md#function-elementptr)


### function Nodes

```cpp
inline int Nodes() const
```


### function Edges

```cpp
inline int Edges() const
```


### function Faces

```cpp
inline int Faces() const
```


### function Node

```cpp
inline FENode & Node(
    int n
)
```


### function Edge

```cpp
inline FEEdge & Edge(
    int n
)
```


### function Face

```cpp
inline FEFace & Face(
    int n
)
```


### function NodePtr

```cpp
inline FENode * NodePtr(
    int n =0
)
```


### function EdgePtr

```cpp
inline FEEdge * EdgePtr(
    int n =0
)
```


### function FacePtr

```cpp
inline FEFace * FacePtr(
    int n =0
)
```


## Protected Attributes Documentation

### variable m_Node

```cpp
std::vector< FENode > m_Node;
```

FE nodes. 

### variable m_Edge

```cpp
std::vector< FEEdge > m_Edge;
```

FE edges. 

### variable m_Face

```cpp
std::vector< FEFace > m_Face;
```

FE faces. 

-------------------------------

Updated on 2021-11-15 at 18:13:24 +0000