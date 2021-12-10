---
title: Libs/Mesh/PreviewMeshQC/FEElement.h

---

# Libs/Mesh/PreviewMeshQC/FEElement.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FEItem](../Classes/classFEItem.md)**  |
| class | **[FENode](../Classes/classFENode.md)**  |
| class | **[FEEdge](../Classes/classFEEdge.md)**  |
| class | **[FEFace](../Classes/classFEFace.md)**  |
| class | **[FEElement_](../Classes/classFEElement__.md)**  |
| class | **[FETri3](../Classes/classFETri3.md)**  |
| class | **[FEElement](../Classes/classFEElement.md)**  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[FE_VISIBLE](../Files/FEElement_8h.md#define-fe-visible)**  |
|  | **[FE_SELECTED](../Files/FEElement_8h.md#define-fe-selected)**  |
|  | **[FE_HEX8](../Files/FEElement_8h.md#define-fe-hex8)**  |
|  | **[FE_TET4](../Files/FEElement_8h.md#define-fe-tet4)**  |
|  | **[FE_PENTA6](../Files/FEElement_8h.md#define-fe-penta6)**  |
|  | **[FE_QUAD4](../Files/FEElement_8h.md#define-fe-quad4)**  |
|  | **[FE_TRI3](../Files/FEElement_8h.md#define-fe-tri3)**  |
|  | **[FE_BEAM2](../Files/FEElement_8h.md#define-fe-beam2)**  |
|  | **[FE_HEX20](../Files/FEElement_8h.md#define-fe-hex20)**  |
|  | **[FE_QUAD8](../Files/FEElement_8h.md#define-fe-quad8)**  |
|  | **[FE_BEAM3](../Files/FEElement_8h.md#define-fe-beam3)**  |
|  | **[FE_TET10](../Files/FEElement_8h.md#define-fe-tet10)**  |
|  | **[FE_TRI6](../Files/FEElement_8h.md#define-fe-tri6)**  |
|  | **[FE_TET15](../Files/FEElement_8h.md#define-fe-tet15)**  |
|  | **[FE_HEX27](../Files/FEElement_8h.md#define-fe-hex27)**  |
|  | **[MAX_ELEM](../Files/FEElement_8h.md#define-max-elem)**  |




## Macros Documentation

### define FE_VISIBLE

```cpp
#define FE_VISIBLE 0x01
```


### define FE_SELECTED

```cpp
#define FE_SELECTED 0x02
```


### define FE_HEX8

```cpp
#define FE_HEX8 1
```


### define FE_TET4

```cpp
#define FE_TET4 2
```


### define FE_PENTA6

```cpp
#define FE_PENTA6 3
```


### define FE_QUAD4

```cpp
#define FE_QUAD4 4
```


### define FE_TRI3

```cpp
#define FE_TRI3 5
```


### define FE_BEAM2

```cpp
#define FE_BEAM2 6
```


### define FE_HEX20

```cpp
#define FE_HEX20 7
```


### define FE_QUAD8

```cpp
#define FE_QUAD8 8
```


### define FE_BEAM3

```cpp
#define FE_BEAM3 9
```


### define FE_TET10

```cpp
#define FE_TET10 10
```


### define FE_TRI6

```cpp
#define FE_TRI6 11
```


### define FE_TET15

```cpp
#define FE_TET15 12
```


### define FE_HEX27

```cpp
#define FE_HEX27 13
```


### define MAX_ELEM

```cpp
#define MAX_ELEM 13
```


## Source code

```cpp
// FEElement.h: interface for the FEElement class.
//

#if !defined(AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_)
#define AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "math3d.h"

//-----------------------------------------------------------------------------
// FE State Flags
#define FE_VISIBLE  0x01
#define FE_SELECTED 0x02

//-----------------------------------------------------------------------------
// The FEItem class is the base class for all FEMesh items. It stores the 
// common attributes such as the state flags and the group ID.
//
class FEItem
{
public:
    FEItem() { m_state = FE_VISIBLE; m_gid = -1; }

    bool IsVisible () const { return ((m_state & FE_VISIBLE ) != 0); }
    bool IsSelected() const { return ((m_state & FE_SELECTED) != 0); }

    void Select()   { m_state = m_state | FE_SELECTED; }
    void UnSelect() { m_state = m_state & ~FE_SELECTED; }
    void Show  () { m_state = m_state | FE_VISIBLE;  }
    void Hide  () { m_state = 0;  }

    unsigned int GetFEState() const { return m_state; }
    void SetFEState(unsigned int state) { m_state = state; }

public:
    int m_ntag; // tag of item
    int m_gid;  // group id
    int m_nid;  // item id (used e.g. in export).
    //double    m_ndata;        //scalar data associated with a node
private:
    unsigned int m_state;   // the state flag of the mesh(-item)
};

//-----------------------------------------------------------------------------
// The FENode class stores the nodal data.
//
class FENode : public FEItem
{
public:
    FENode(){ m_bext = false; }

public:
    vec3d   r;              // nodal position
    bool    m_bext;         // exterior node or not?
    double  m_ndata;
};

//-----------------------------------------------------------------------------
// The FEEdge class stores the edge data.
// An edge can be either linear or quadratic. In the linear case, only the
// first two nodes are used, the third one is set to -1. In the latter case,
// all three nodes are used, with the first two being the outside nodes and
// the third one being the inside node.
//
//   1         3          2
//   +---------o----------+
//
class FEEdge : public FEItem
{
public:
    enum {MAX_NODES = 3};

public:
    FEEdge();

    bool operator == (const FEEdge& e);

    int Nodes() { return (n[2] == -1? 2 : 3); }

    int FindNode(int node);

public:
    int n[MAX_NODES];   
    int m_elem;         
    int m_nbr[2];       
};

//-----------------------------------------------------------------------------
// FEFace class stores face data. 
// A face can either have 3, 4, 6, 8 or 9 nodes. It is a triangle if it has 3 nodes and
// a quad if it has 4. If it has 6 it is a quadratic triangle. The first three are the
// cornder nodes, the other three are the midpoints of the edges. If it has 8 or 9 it is 
// a quadratic quad. The first four nodes are the corner nodes, the next four the 
// interior edge nodes and in the case of a 9-node quad, the last node is in the center.
//
//   4       7       3      3
//   +-------o-------+      +
//   |               |      |\
//   |               |      | \
//  8o       x9      o6    6o  o5
//   |               |      |   \
//   |               |      |    \
//   +-------o-------+      +--o--+
//   1       5       2      1  4  2
//
class FEFace : public FEItem
{
public:
    enum {MAX_NODES = 9};

public:
    FEFace();

    bool operator == (const FEFace& f);

    int Nodes() { return m_nodes; }

    int Edges();

    void GetEdgeNodes(int i, int* n);

    bool HasEdge(int n1, int n2);

    bool HasNode(int i);

    int FindNode(int i);

    bool IsExternal() { return (m_elem[1] == -1); }

public:
    int n[MAX_NODES];   
    int m_nodes;        

    int     m_nbr[4];   

    vec3d   m_fn;               
    vec3d   m_nn[MAX_NODES];    
    int     m_sid;              

    int m_elem[2];  
};

//-----------------------------------------------------------------------------
// element types
// NOTE: do not change the order or values of these macros.
#define FE_HEX8     1
#define FE_TET4     2
#define FE_PENTA6   3
#define FE_QUAD4    4
#define FE_TRI3     5
#define FE_BEAM2    6
#define FE_HEX20    7
#define FE_QUAD8    8
#define FE_BEAM3    9
#define FE_TET10    10
#define FE_TRI6     11
#define FE_TET15    12
#define FE_HEX27    13

#define MAX_ELEM    13

//-----------------------------------------------------------------------------
// The FEElement_ class defines the data interface to the element data. 
// Specialized element classes are then defined by deriving from this base class.
class FEElement_ : public FEItem
{
public:
    FEElement_();

    // comparison operator
    bool is_equal(FEElement_& e);

    int GetType() const { return m_ntype; }

    bool IsType(int ntype) const { return m_ntype == ntype; }

    int Nodes() const { return m_nodes; }

    int Faces() const { return m_nfaces; }

    int Edges() const { return m_nedges; }

    int GetFace(int i, int* n);

    FEFace GetFace(int i);

    FEFace GetShellFace();

    FEEdge GetEdge(int i);

    bool IsExterior();

protected:
    // help class for copy-ing element data
    void copy(const FEElement_& el);

public:
    // Check the element class
    bool IsSolid() { return (m_ntype == FE_HEX8) || (m_ntype == FE_HEX20) || (m_ntype == FE_HEX27) || (m_ntype == FE_PENTA6) || (m_ntype == FE_TET4) || (m_ntype == FE_TET10) || (m_ntype == FE_TET15); }
    bool IsShell() { return (m_ntype == FE_TRI3) || (m_ntype == FE_QUAD4) || (m_ntype == FE_TRI6); }
    bool IsBeam () { return (m_ntype == FE_BEAM2); }

public:
    int*        m_node;     
    int*        m_nbr;      
    int*        m_face;     
    double*     m_h;        
    
public:
    vec3d   m_fiber;    
    mat3d   m_Q;        
    bool    m_Qactive;  
    double  m_a0;       
    
protected:
    int     m_ntype;    
    int     m_nodes;    
    int     m_nfaces;   
    int     m_nedges;   
};

//-----------------------------------------------------------------------------
// Class for 3-node triangular elements
class FETri3 : public FEElement_
{
public:
    FETri3();

    FETri3(FETri3& el);

    FETri3& operator = (FETri3& el);

private:
    int     _node[3];
    int     _nbr[3];
    int     _face[1];
    double  _h[3];
};

//-----------------------------------------------------------------------------
// The FEElement class can be used to represent a general purpose element. 
// This class can represent an element of all different types. 
class FEElement : public FEElement_
{
public:
    enum { MAX_NODES = 27 };

public:
    FEElement();

    FEElement(const FEElement& el);

    FEElement& operator = (const FEElement& el);

    void SetType(int ntype);

private:
    int     _node[MAX_NODES];   
    int     _nbr[6];            
    int     _face[6];           
    double  _h[9];              
};

#endif // !defined(AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_)
```


-------------------------------

Updated on 2021-12-10 at 00:32:12 +0000
