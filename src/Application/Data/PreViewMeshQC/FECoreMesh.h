#pragma once
#include "FEElement.h"
#include <vector>

//-----------------------------------------------------------------------------
//! This class defines a simple mesh structure that provides basic container
//! services for storing mesh data.
class FECoreMesh
{
public:
	//! constructor
	FECoreMesh();

	//! destructor
	virtual ~FECoreMesh();

	//! allocate space for mesh
	virtual void Create(int nodes, int elems, int faces = 0, int edges = 0) = 0;

	//! check the type of the mesh
	bool IsType(int ntype);

public: // interface for accessing elements

	//! total number of elements
	virtual int Elements() const = 0;

	//! return reference to element
	virtual FEElement_& ElementRef(int n) = 0;

	//! return pointer to element
	virtual  FEElement* ElementPtr(int n=0) = 0;

public: // interface for accessing mesh items (except elements)

	int Nodes   () const { return m_Node.size(); }
	int Edges   () const { return m_Edge.size(); }
	int Faces   () const { return m_Face.size(); }

	FENode&    Node   (int n) { return m_Node[n]; }
	FEEdge&	   Edge   (int n) { return m_Edge[n]; }
	FEFace&	   Face   (int n) { return m_Face[n]; }

	FENode*    NodePtr   (int n=0) { return ((n>=0) && (n<(int)m_Node.size())? &m_Node[n] : 0); }
	FEEdge*    EdgePtr   (int n=0) { return ((n>=0) && (n<(int)m_Edge.size())? &m_Edge[n] : 0); }
	FEFace*	   FacePtr   (int n=0) { return ((n>=0) && (n<(int)m_Face.size())? &m_Face[n] : 0); }

protected:
	std::vector<FENode>		m_Node;	//!< FE nodes
	std::vector<FEEdge>		m_Edge;	//!< FE edges
	std::vector<FEFace>		m_Face;	//!< FE faces
};
