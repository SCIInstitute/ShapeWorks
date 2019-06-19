// FEElement.h: interface for the FEElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_)
#define AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "math3d.h"

//-----------------------------------------------------------------------------
// FE State Flags
#define FE_VISIBLE	0x01
#define FE_SELECTED	0x02

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
	int	m_ntag;	// tag of item
	int	m_gid;	// group id
	int	m_nid;	// item id (used e.g. in export).
	//double	m_ndata;		//scalar data associated with a node
private:
	unsigned int m_state;	// the state flag of the mesh(-item)
};

//-----------------------------------------------------------------------------
// The FENode class stores the nodal data.
//
class FENode : public FEItem
{
public:
	FENode(){ m_bext = false; }

public:
	vec3d	r;				// nodal position
	bool	m_bext;			// exterior node or not?
	double	m_ndata;
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
	//! constructor
	FEEdge();

	//! edge comparison
	bool operator == (const FEEdge& e);

	//! return number of nodes
	int Nodes() { return (n[2] == -1? 2 : 3); }

	//! find a node
	int FindNode(int node);

public:
	int n[MAX_NODES];	//!< edge nodes
	int	m_elem;			//!< the element to which this edge belongs (used only by beams)
	int	m_nbr[2];		//!< the two adjacent edges (if there are more edges incident to a node, the neighbour is set to -1)
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
	//! constructor
	FEFace();

	//! comparison operator
	bool operator == (const FEFace& f);

	//! return number of nodes
	int Nodes()	{ return m_nodes; }

	//! return number of edges
	int Edges();

	//! get the edge node numbers
	void GetEdgeNodes(int i, int* n);

	//! See if this face has an edge
	bool HasEdge(int n1, int n2);

	//! See if this face has node with ID i
	bool HasNode(int i);

	//! Fine the array index of node with ID i
	int FindNode(int i);

	//! Is this face internal or external
	bool IsExternal() { return (m_elem[1] == -1); }

public:
	int	n[MAX_NODES];	//!< nodal ID's
	int	m_nodes;		//!< number of nodes

	int		m_nbr[4];	//!< neighbour faces

	vec3d	m_fn;				//!< face normal
	vec3d	m_nn[MAX_NODES];	//!< node normals
	int		m_sid;				//!< smoothing ID

	int	m_elem[2];	//!< the elements to which this face belongs
};

//-----------------------------------------------------------------------------
// element types
// NOTE: do not change the order or values of these macros.
#define	FE_HEX8		1
#define	FE_TET4		2
#define FE_PENTA6	3
#define FE_QUAD4	4
#define FE_TRI3		5
#define FE_BEAM2	6
#define FE_HEX20	7
#define FE_QUAD8	8
#define FE_BEAM3	9
#define FE_TET10	10
#define FE_TRI6 	11
#define FE_TET15	12
#define FE_HEX27	13

#define MAX_ELEM	13

//-----------------------------------------------------------------------------
// The FEElement_ class defines the data interface to the element data. 
// Specialized element classes are then defined by deriving from this base class.
class FEElement_ : public FEItem
{
public:
	//! constructor
	FEElement_();

	// comparison operator
	bool is_equal(FEElement_& e);

	//! return the element type
	int GetType() const { return m_ntype; }

	//! Is the element of this type
	bool IsType(int ntype) const { return m_ntype == ntype; }

	//! number of nodes
	int Nodes() const { return m_nodes; }

	//! Number of faces (shells have no faces)
	int Faces() const { return m_nfaces; }

	//! Number of edges (solids have no edges)
	int Edges() const { return m_nedges; }

	//! Get only the nodes of face i (only solids have faces)
	int GetFace(int i, int* n);

	//! Get the face i (only solids have faces)
	FEFace GetFace(int i);

	//! Get the face of a shell
	FEFace GetShellFace();

	//! Get the edge
	FEEdge GetEdge(int i);

	//! Is this an exterior element
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
	int*		m_node;		//!< pointer to node data
	int*		m_nbr;		//!< neighbour elements
	int*		m_face;		//!< faces (-1 for interior faces)
	double* 	m_h;		//!< element thickness (only used by shells)
	
public:
	vec3d	m_fiber;	//!< fiber orientation \todo maybe I can add an element attribute section
	mat3d	m_Q;		//!< local material orientation
	bool	m_Qactive;	//!< active local material orientation
	double	m_a0;		//!< cross-sectional area (only used by truss elements)
	
protected:
	int		m_ntype; 	//!< type of element
	int		m_nodes;	//!< nr of nodes
	int		m_nfaces;	//!< nr of faces	( 0 for shells)
	int		m_nedges;	//!< nr of edges	( 0 for solids)
};

//-----------------------------------------------------------------------------
// Class for 3-node triangular elements
class FETri3 : public FEElement_
{
public:
	//! Constructor
	FETri3();

	//! copy constructor
	FETri3(FETri3& el);

	//! assignment operator
	FETri3& operator = (FETri3& el);

private:
	int		_node[3];
	int		_nbr[3];
	int		_face[1];
	double	_h[3];
};

//-----------------------------------------------------------------------------
// The FEElement class can be used to represent a general purpose element. 
// This class can represent an element of all different types. 
class FEElement : public FEElement_
{
public:
	enum { MAX_NODES = 27 };

public:
	//! constructor
	FEElement();

	//! copy constructor
	FEElement(const FEElement& el);

	//! assignment operator
	FEElement& operator = (const FEElement& el);

	//! Set the element type
	void SetType(int ntype);

private:
	int		_node[MAX_NODES];	//!< nodal id's
	int		_nbr[6];			//!< neighbour elements
	int		_face[6];			//!< faces (-1 for interior faces)
	double 	_h[9];				//!< element thickness (only used by shells)
};

#endif // !defined(AFX_FEELEMENT_H__4A869671_045F_4EDD_AC13_25C849922373__INCLUDED_)
