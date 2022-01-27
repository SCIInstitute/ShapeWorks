---
title: FETri3

---

# FETri3





Inherits from [FEElement_](../Classes/classFEElement__.md), [FEItem](../Classes/classFEItem.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FETri3](../Classes/classFETri3.md#function-fetri3)**()<br>Constructor.  |
| | **[FETri3](../Classes/classFETri3.md#function-fetri3)**([FETri3](../Classes/classFETri3.md) & el)<br>copy constructor  |
| [FETri3](../Classes/classFETri3.md) & | **[operator=](../Classes/classFETri3.md#function-operator=)**([FETri3](../Classes/classFETri3.md) & el)<br>assignment operator  |

## Additional inherited members

**Public Functions inherited from [FEElement_](../Classes/classFEElement__.md)**

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

**Protected Functions inherited from [FEElement_](../Classes/classFEElement__.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[copy](../Classes/classFEElement__.md#function-copy)**(const [FEElement_](../Classes/classFEElement__.md) & el) |

**Public Attributes inherited from [FEElement_](../Classes/classFEElement__.md)**

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

**Protected Attributes inherited from [FEElement_](../Classes/classFEElement__.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[m_ntype](../Classes/classFEElement__.md#variable-m-ntype)** <br>type of element  |
| int | **[m_nodes](../Classes/classFEElement__.md#variable-m-nodes)** <br>nr of nodes  |
| int | **[m_nfaces](../Classes/classFEElement__.md#variable-m-nfaces)** <br>nr of faces ( 0 for shells)  |
| int | **[m_nedges](../Classes/classFEElement__.md#variable-m-nedges)** <br>nr of edges ( 0 for solids)  |

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

### function FETri3

```cpp
FETri3()
```

Constructor. 

### function FETri3

```cpp
FETri3(
    FETri3 & el
)
```

copy constructor 

### function operator=

```cpp
FETri3 & operator=(
    FETri3 & el
)
```

assignment operator 

-------------------------------

Updated on 2022-01-27 at 02:24:32 +0000