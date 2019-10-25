// FEMesh.cpp: implementation of the FEMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEMesh.h"
#include <climits>
#include <stack>
#include <string.h>
#include <stdlib.h>

#define ROTATE(a, i, j, k, l) g=a[i][j]; h=a[k][l];a[i][j]=g-s*(h+g*tau); a[k][l] = h + s*(g - h*tau);

int FTHEX8[6][4] = {
	{ 0, 1, 5, 4 },
	{ 1, 2, 6, 5 },
	{ 2, 3, 7, 6 },
	{ 3, 0, 4, 7 },
	{ 3, 2, 1, 0 },
	{ 4, 5, 6, 7 }};

int FTHEX20[6][8] = {
	{ 0, 1, 5, 4,  8, 17, 12, 16},
	{ 1, 2, 6, 5,  9, 18, 13, 17},
	{ 2, 3, 7, 6, 10, 19, 14, 18},
	{ 0, 4, 7, 3, 16, 15, 19, 11},
	{ 0, 3, 2, 1, 11, 10,  9,  8},
	{ 4, 5, 6, 7, 12, 13, 14, 15}};

int FTHEX27[6][9] = {
	{ 0, 1, 5, 4,  8, 17, 12, 16, 20},
	{ 1, 2, 6, 5,  9, 18, 13, 17, 21},
	{ 2, 3, 7, 6, 10, 19, 14, 18, 22},
	{ 0, 4, 7, 3, 16, 15, 19, 11, 23},
	{ 0, 3, 2, 1, 11, 10,  9,  8, 24},
	{ 4, 5, 6, 7, 12, 13, 14, 15, 25}};

int FTPENTA[5][4] = {
	{ 0, 1, 4, 3 },
	{ 1, 2, 5, 4 },
	{ 0, 3, 5, 2 },
	{ 0, 2, 1,-1 },
	{ 3, 4, 5,-1 }};

int FTTET[4][3] = {
	{ 0, 1, 3 },
	{ 1, 2, 3 },
	{ 0, 3, 2 },
	{ 0, 2, 1 }};

int FTTET10[4][6] = {
	{ 0, 1, 3, 4, 8, 7 },
	{ 1, 2, 3, 5, 9, 8 },
	{ 2, 0, 3, 6, 7, 9 },
	{ 2, 1, 0, 5, 4, 6 }};

int FTTET15[4][7] = {
	{ 0, 1, 3, 4, 8, 7, 11 },
	{ 1, 2, 3, 5, 9, 8, 12 },
	{ 2, 0, 3, 6, 7, 9, 13 },
	{ 2, 1, 0, 5, 4, 6, 10 }};

// first index = integration, second = node
double GHEX8[8][8][3] = {
{{-0.5,-0.5,-0.5},{0.5,0,0},{ 0, 0,-0},{-0,0.5,-0},{-0,-0,0.5},{0,-0,0},{0,0,0},{-0,0,0}},
{{-0.5, 0, 0},{0.5,-0.5,-0.5},{0,0.5,-0},{-0,0,-0},{-0,-0,0},{0,-0,0.5},{0,0,0},{-0,0,0}},
{{ 0, 0, 0},{ 0,-0.5,0},{0.5,0.5,-0.5},{-0.5,0,-0},{-0,-0,0},{0,-0,0},{0,0,0.5},{-0,0,0}},
{{-0,-0.5,-0},{0,-0,-0},{0.5,0,-0},{-0.5,0.5,-0.5},{-0,-0,0},{0,-0,0},{0,0,0},{-0,0,0.5}},
{{-0,-0,-0.5},{0,-0,-0},{0,0,-0},{-0,0,-0},{-0.5,-0.5,0.5},{0.5,-0,0},{0,0,0},{-0,0.5,0}},
{{-0,-0,-0},{0,-0,-0.5},{0,0,-0},{-0,0,-0},{-0.5,-0,0},{0.5,-0.5,0.5},{0,0.5,0},{-0,0,0}},
{{-0,-0,-0},{0,-0,-0},{0,0,-0.5},{-0,0,-0},{-0,-0,0},{0,-0.5,0},{0.5,0.5,0.5},{-0.5,0,0}},
{{-0,-0,-0},{0,-0,-0},{0,0,-0},{-0,0,-0.5},{-0,-0.5,0},{0,-0,0},{0.5,0,0},{-0.5,0.5,0.5}}
};

const double GWEDGE[6][6][3] = {
{{-1.0,-1.0,-0.5},{ 1.0, 0.0,-0.0},{ 0.0, 1.0,-0.0},{-0.0,-0.0, 0.5},{ 0.0, 0.0, 0.0},{ 0.0, 0.0, 0.0}},
{{-1.0,-1.0,-0.0},{ 1.0, 0.0,-0.5},{ 0.0, 1.0,-0.0},{-0.0,-0.0, 0.0},{ 0.0, 0.0, 0.5},{ 0.0, 0.0, 0.0}},
{{-1.0,-1.0,-0.0},{ 1.0, 0.0,-0.0},{ 0.0, 1.0,-0.5},{-0.0,-0.0, 0.0},{ 0.0, 0.0, 0.0},{ 0.0, 0.0, 0.5}},
{{-0.0,-0.0,-0.5},{ 0.0, 0.0,-0.0},{ 0.0, 0.0,-0.0},{-1.0,-1.0, 0.5},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.0}},
{{-0.0,-0.0,-0.0},{ 0.0, 0.0,-0.5},{ 0.0, 0.0,-0.0},{-1.0,-1.0, 0.0},{ 1.0, 0.0, 0.5},{ 0.0, 1.0, 0.0}},
{{-0.0,-0.0,-0.0},{ 0.0, 0.0,-0.0},{ 0.0, 0.0,-0.5},{-1.0,-1.0, 0.0},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.5}}
};

const double GTET[4][4][3] = {
{{-1.0,-1.0,-1.0},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.0},{ 0.0, 0.0, 1.0}},
{{-1.0,-1.0,-1.0},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.0},{ 0.0, 0.0, 1.0}},
{{-1.0,-1.0,-1.0},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.0},{ 0.0, 0.0, 1.0}},
{{-1.0,-1.0,-1.0},{ 1.0, 0.0, 0.0},{ 0.0, 1.0, 0.0},{ 0.0, 0.0, 1.0}}
};

/*
double GTET[4][4][3] = {
{{-1,-1,-1},{1,0,0},{0,1,0},{0,0,1}},
{{-1,-1,-1},{1,0,0},{0,1,0},{0,0,1}},
{{-1,-1,-1},{1,0,0},{0,1,0},{0,0,1}},
{{-1,-1,-1},{1,0,0},{0,1,0},{0,0,1}}
};
*/

double GQUAD[4][4][2] = {
{{-0.5,-0.5},{0.5,-0},{0,0},{-0,0.5}},
{{-0.5,-0},{0.5,-0.5},{0,0.5},{-0,0}},
{{-0,-0},{0,-0.5},{0.5,0.5},{-0.5,0}},
{{-0,-0.5},{0,-0},{0.5,0},{-0.5,0.5}}
};

double GTRI[3][3][2] = {
{{-1,-1},{1,0},{0,1}},
{{-1,-1},{1,0},{0,1}},
{{-1,-1},{1,0},{0,1}}
};

double bias(double b, double x)
{
	const double f = 1.f / (double) log(0.5);
	return (double) pow(x, log(b)*f);
}

double gain(double g, double x)
{
	if (x < 0.5f)
		return bias(1.f-g, 2.f*x)*0.5f;
	else
		return 1.f - bias(1.f-g, 2.f - 2.f*x)*0.5f;
}

//-----------------------------------------------------------------------------
// calculate are of a triangle
double area_triangle(vec3d r[3])
{
	vec3d e1 = r[1] - r[0];
	vec3d e2 = r[2] - r[0];

	return (e1 ^ e2).Length()*0.5;
}

//-----------------------------------------------------------------------------
// default constructor
FEMesh::FEMesh()
{
	m_min = m_max = 0.0;
}

//=============================================================================
//								F E E L E M E N T D A T A
//=============================================================================

//-----------------------------------------------------------------------------
FEElementData::FEElementData()
{
	m_pMesh = 0;
}

//-----------------------------------------------------------------------------
void FEElementData::Create(FEMesh* pm, double v)
{
	m_pMesh = pm;
	m_data.assign(pm->Elements(), v);
}

//-----------------------------------------------------------------------------
FEElementData::FEElementData(const FEElementData& d)
{
	m_pMesh = d.m_pMesh;
	m_data = d.m_data;
	strcpy(m_szname, d.m_szname);
}

//-----------------------------------------------------------------------------
FEElementData& FEElementData::operator = (const FEElementData& d)
{
	m_pMesh = d.m_pMesh;
	m_data = d.m_data;
	strcpy(m_szname, d.m_szname);
	return (*this);
}

//-----------------------------------------------------------------------------
void FEElementData::SetName(const char* sz)
{
	strcpy(m_szname, sz);
}

//-----------------------------------------------------------------------------
void FEElementData::FillRandomBox(double fmin, double fmax)
{
	int N = (int)m_data.size();
	for (int i=0; i<N; ++i)
	{
		double f = (double) rand() / (double) RAND_MAX;
		double v = fmin + (fmax - fmin)*f;
		m_data[i] = v;
	}
}

//=============================================================================
//								F E M E S H
//=============================================================================

//-----------------------------------------------------------------------------
// copy constructor
FEMesh::FEMesh(FEMesh& m)
{
	int i;

	// create the nodes
	m_Node.resize(m.Nodes());
	for (i=0; i<Nodes(); ++i) m_Node[i] = m.m_Node[i];

	// create the elements
	m_Elem.resize(m.Elements());
	for (i=0; i<Elements(); ++i) m_Elem[i] = m.m_Elem[i];

	// create the faces
	m_Face.resize(m.Faces());
	for (i=0; i<Faces(); ++i) m_Face[i] = m.m_Face[i];

	// create the edges
	m_Edge.resize(m.Edges());
	for (i=0; i<Edges(); ++i) m_Edge[i] = m.m_Edge[i];

	// copy element data
	m_data = m.m_data;
	m_min = m.m_min;
	m_max = m.m_max;
	m_map = m.m_map;
}

//-----------------------------------------------------------------------------
// destructor
FEMesh::~FEMesh()
{
	Clear();
}

//-----------------------------------------------------------------------------
// Clear the mesh data
void FEMesh::Clear()
{
	m_Edge.clear();
	m_Face.clear();
	m_Elem.clear();
	m_Node.clear();

	m_data.clear();
	m_map.clear();
}


//-----------------------------------------------------------------------------
// Allocate storage for the mesh data. If bclear is true (default = true) all 
// existing groups are deleted.
void FEMesh::Create(int nodes, int elems, int faces, int edges)
{
	// allocate storage
	if (nodes > 0) { if (nodes) m_Node.resize(nodes); else m_Node.clear(); }
	if (elems > 0) { if (elems) m_Elem.resize(elems); else m_Elem.clear(); }
	if (faces > 0) { if (faces) m_Face.resize(faces); else m_Face.clear(); }
	if (edges > 0) { if (edges) m_Edge.resize(edges); else m_Edge.clear(); }

	// allocate storage for element data
	if (elems > 0) m_data.assign(elems, 0.0);

	// see if we need to clear the maps
	if (elems > 0) m_map.clear();
}

//-----------------------------------------------------------------------------
// Detach the selection and return it as a new object
// TODO: I want to remove this function and replace it with a modifier who
// does all the work.
FEMesh* FEMesh::DetachSelectedMesh()
{
	int i, j, n;

	// count selected elements
	int elems = 0;
	FEElement* pe = ElementPtr();
	for (i=0; i<Elements(); ++i, ++pe) if (pe->IsSelected()) ++elems;

	// make sure there is a selection
	if (elems == 0) return 0;

	// tag nodes that will be moved to the new mesh
	FENode* pn = NodePtr();
	for (i=0; i<Nodes(); ++i, ++pn) pn->m_ntag = -1;

	pe = ElementPtr();
	for (i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->IsSelected())
		{
			n = pe->Nodes();
			for (j=0; j<n; ++j) Node(pe->m_node[j]).m_ntag = 1;
		}
	}

	// count nodes
	int nodes = 0;
	pn = NodePtr();
	for (i=0; i<Nodes(); ++i, ++pn) if (pn->m_ntag > 0) ++nodes;

	// create a new mesh
	FEMesh* pm = new FEMesh();
	pm->Create(nodes, elems);

	// create the new nodes
	pn = NodePtr();
	n = 0;
	for (i=0; i<Nodes(); ++i, ++pn)
	{
		if (pn->m_ntag > 0)
		{
			FENode& node = pm->Node(n);
			node.r  = pn->r;
			pn->m_ntag = n++;
		}
	}
	assert(n == nodes);

	// create the new elements
	pe = ElementPtr();
	n = 0;
	for (i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->IsSelected())
		{
			FEElement& el = pm->Element(n);
			el.SetType(pe->GetType());
			el.m_h[0] = pe->m_h[0];
			el.m_h[1] = pe->m_h[1];
			el.m_h[2] = pe->m_h[2];
			el.m_h[3] = pe->m_h[3];
			
			for (j=0; j<pe->Nodes(); ++j)
			{
				el.m_node[j] = Node(pe->m_node[j]).m_ntag;
				assert(el.m_node[j] >= 0);
			}
			++n;
		}
	}
	assert(n==elems);

	// update the new mesh (is done later)
//	pm->Update();

	vector<int> ELT;
	ELT.assign(Elements(), -1);

	// the new mesh is created, so let's clean up this mesh
	// delete selected elements
	n = 0;
	for (i=0; i<Elements(); ++i)
	{
		FEElement& e0 = Element(i);
		FEElement& e1 = Element(n);

		if (!e0.IsSelected())
		{
			e1 = e0;
			ELT[i] = n;
			++n;
		}
	}
	m_Elem.resize(n);
	m_data.resize(n);

	// tag nodes which will be kept
	pn = NodePtr();
	for (i=0; i<Nodes(); ++i, ++pn) pn->m_ntag = -1;

	pe = ElementPtr();
	for (i=0; i<Elements(); ++i, ++pe)
	{
		n = pe->Nodes();
		for (j=0; j<n; ++j) Node(pe->m_node[j]).m_ntag = 1;
	}

	// reindex the nodes
	n = 0;
	pn = NodePtr();
	for (i=0; i<Nodes(); ++i, ++pn) if (pn->m_ntag > 0) pn->m_ntag = n++;

	pe = ElementPtr();
	for (i=0; i<Elements(); ++i, ++pe)
	{
		n = pe->Nodes();
		for (j=0; j<n; ++j)	pe->m_node[j] = Node(pe->m_node[j]).m_ntag;
	}

	// delete untagged nodes
	n = 0;
	for (i=0; i<Nodes(); ++i)
	{
		FENode& n0 = Node(i);
		FENode& n1 = Node(n);

		if (n0.m_ntag >= 0)
		{
			n1 = n0;
			++n;
		}
	}
	m_Node.resize(n);

	// update the mesh (is done later)
///	Update();

	// Done!
	return pm;
}

//-----------------------------------------------------------------------------
// Detach the selection and create a new part
// TODO: I want to remove this function and replace it with a modifier that
// does all the work.
void FEMesh::DetachSelectedPart()
{
	// count selected elements
	int elems = 0;
	FEElement* pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe) if (pe->IsSelected()) ++elems;

	// make sure there is a selection
	if (elems == 0) return;

	// tag nodes that will be copied
	FENode* pn = NodePtr();
	for (int i=0; i<Nodes(); ++i, ++pn) pn->m_ntag = -1;

	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->IsSelected())
		{
			int ne = pe->Nodes();
			for (int j=0; j<ne; ++j) Node(pe->m_node[j]).m_ntag = -2;
		}
	}

	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->IsSelected() == false)
		{
			int ne = pe->Nodes();
			for (int j=0; j<ne; ++j) 
			{
				FENode& nr = Node(pe->m_node[j]);
				if (nr.m_ntag == -2) nr.m_ntag = 0;
			}
		}
	}


	// count nodes
	int N1 = 0;
	pn = NodePtr();
	for (int i=0; i<Nodes(); ++i, ++pn) if (pn->m_ntag == 0) ++N1;

	// make sure we need to duplicate anything
	if (N1 == 0) return;

	// allocate more nodes
	int N0 = Nodes();
	Create(N0+N1,0);

	// create the new nodes
	pn = NodePtr();
	int n = N0;
	for (int i=0; i<N0; ++i, ++pn)
	{
		if (pn->m_ntag == 0)
		{
			FENode& node = Node(n);
			node.r  = pn->r;
			pn->m_ntag = n++;
		}
	}

	// count groups
	int ng = 0;
	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->m_gid > ng) ng = pe->m_gid;
	}
	++ng;

	// create the new elements
	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		if (pe->IsSelected())
		{
			pe->m_gid = ng;
			int ne = pe->Nodes();
			for (int j=0; j<ne; ++j)
			{
				FENode& nr = Node(pe->m_node[j]);
				if (nr.m_ntag >= 0) pe->m_node[j] = nr.m_ntag;
			}
		}
	}

	// reindex the groups
	vector<int> G; G.assign(ng+1,0);
	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe) G[pe->m_gid]++;

	ng = 0;
	for (int i=0; i<(int) G.size(); ++i)
	{
		if (G[i] > 0) G[i] = ng++; else G[i] = -1;
	}

	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		pe->m_gid = G[pe->m_gid];
		assert(G[pe->m_gid != -1]);
	}
}

//-----------------------------------------------------------------------------
// Tag all elements
void FEMesh::TagAllElements(int ntag)
{
	const int NE = Elements();
	for (int i=0; i<NE; ++i) Element(i).m_ntag = ntag;
}

//-----------------------------------------------------------------------------
// NOTE: Assumes triangle mesh.
void FEMesh::FixElementWinding()
{
	// assume triangle mesh
	if (IsType(FE_TRI3) == false) return;

	// clear tags
	TagAllElements(0);

	// loop over all elements.
	// we assume an element is inverted if it is surrounded by elements
	// that are wound in the opposite direction
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& ei = Element(i);
		bool bok = false;
		for (int j=0; j<3; ++j)
		{
			if (ei.m_nbr[j] >= 0)
			{
				int n0 = ei.m_node[j];
				int n1 = ei.m_node[(j+1)%3];
				FEElement& ej = Element(ei.m_nbr[j]);
				int* m = ej.m_node;
				if (((m[0]==n1)&&(m[1]==n0))||
					((m[1]==n1)&&(m[2]==n0))||
					((m[2]==n1)&&(m[0]==n0)))
				{
					bok = true;
					break;
				}
			}
		}
		if (bok == false) ei.m_ntag = 1;
	}

	// invert the tagged elements.
	InvertTaggedElements(1);
}

//-----------------------------------------------------------------------------
// This function tries to wind all the triangles in the same direction. 
void FEMesh::FixElementWinding2()
{
	// assume triangle mesh
	if (IsType(FE_TRI3) == false) return;

	// clear tags
	TagAllElements(0);

	// loop over all triangles
	int NF = Elements();
	for (int i=0; i<NF; ++i)
	{
		// get an element
		FEElement& e0 = Element(i);

		// proceed if it has not been processed
		if (e0.m_ntag == 0)
		{
			// this element will be the starter element
			// all elements connected to this element will now be wound
			// in the same direction.
			e0.m_ntag = 1;
			stack<FEElement*> S;
			S.push(&e0);
			while (S.empty() == false)
			{
				// pop an element
				FEElement* pe = S.top(); S.pop();

				// loop over the neighbors
				for (int j=0; j<3; ++j)
				{
					int n0 = pe->m_node[j];
					int n1 = pe->m_node[(j+1)%3];
					int ej = pe->m_nbr[j];
					if (ej >= 0)
					{
						FEElement* pej = &Element(ej);
						if (pej->m_ntag == 0)
						{
							// find the shared edge
							for (int k=0; k<3; ++k)
							{
								int k0 = pej->m_node[k];
								int k1 = pej->m_node[(k+1)%3];

								if ((k0==n0)&&(k1==n1))
								{
									// winding is wrong, so flip the element
									// flip nodes
									int ntmp = pej->m_node[1];
									pej->m_node[1] = pej->m_node[2];
									pej->m_node[2] = ntmp;

									// flip neighbors
									ntmp = pej->m_nbr[0];
									pej->m_nbr[0] = pej->m_nbr[2];
									pej->m_nbr[2] = ntmp;

									pej->m_ntag = 1;
									S.push(pej);
									break;
								}
								else if ((k0==n1)&&(k1==n0))
								{
									// winding is correct, just push it
									pej->m_ntag = 1;
									S.push(pej);
									break;
								}
							}
							assert(pej->m_ntag == 1);
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Remove nodes that are not referenced by any elements.
void FEMesh::RemoveIsolatedNodes()
{
	// find the isolated nodes
	FENode* pn = NodePtr();
	for (int i=0; i<Nodes(); ++i, ++pn) pn->m_ntag = -1;

	FEElement* pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		int n = pe->Nodes();
		for (int j=0; j<n; ++j) Node(pe->m_node[j]).m_ntag = 1;
	}

	// reindex the nodes
	int n = 0;
	pn = NodePtr();
	for (int i=0; i<Nodes(); ++i, ++pn)
		if (pn->m_ntag > 0)
			pn->m_ntag = n++;

	// fix element node numbering
	pe = ElementPtr();
	for (int i=0; i<Elements(); ++i, ++pe)
	{
		int n = pe->Nodes();
		for (int j=0; j<n; ++j) pe->m_node[j] = Node(pe->m_node[j]).m_ntag;
	}

	// fix face node numbering
	FEFace* pf = FacePtr();
	for (int i=0; i<Faces(); ++i, ++pf)
	{
		int n = pf->Nodes();
		for (int j=0; j<n; ++j) pf->n[j] = Node(pf->n[j]).m_ntag;
	}

	// fix edge node numbering
	FEEdge* pc = EdgePtr();
	for (int i=0; i<Edges(); ++i, ++pc)
	{
		int n = pc->Nodes();
		for (int j=0; j<n; ++j) pc->n[j] = Node(pc->n[j]).m_ntag;
	}

	// remove the isolated nodes
	n = 0;
	for (int i=0; i<Nodes(); ++i)
	{
		FENode& n1 = Node(i);
		FENode& n2 = Node(n);

		if (n1.m_ntag >= 0)
		{
			n2 = n1;
			++n;
		}
	}

	// adjust the node container size
	m_Node.resize(n);
}

//-----------------------------------------------------------------------------
// Delete selected elements
void FEMesh::DeleteSelectedElements()
{
	int i, n;

	// remove selected elements
	n = 0;
	for (i=0; i<Elements(); ++i)
	{
		FEElement& e1 = Element(i);
		FEElement& e2 = Element(n);

		if (!e1.IsSelected())
		{
			e2 = e1;
			++n;
		}
	}
	m_Elem.resize(n);
	m_data.resize(n);

	// remove isolated nodes
	RemoveIsolatedNodes();

	// update the mesh
	// is already called in RemoveIsolatedNodes
//	Update();
}

//-----------------------------------------------------------------------------
void FEMesh::RemoveDuplicateElements()
{
	// clear all tags
	for (int i=0; i<Elements(); ++i) Element(i).m_ntag = 0;

	// build the node-element table
	vector<vector<int> > NET;
	BuildNodeElementTable(NET);

	// loop over all elements
	for (int i=0; i<Nodes(); ++i)
	{
		vector<int>& NEi = NET[i];
		int n = (int) NEi.size();
		for (int j=0; j<n-1; ++j)
		{
			FEElement& ej = Element(NEi[j]);
			if (ej.m_ntag == 0)
			{
				for (int k=j+1; k<n; ++k)
				{
					FEElement& ek = Element(NEi[k]);
					if (ej.is_equal(ek))
					{
						ek.m_ntag = 1;
					}
				}
			}
		}
	}

	// remove tagged elements
	DeleteTaggedElements(1);
}

//-----------------------------------------------------------------------------
// Calculate the min jacobian of a shell element
//
double FEMesh::ShellJacobian(FEElement& el)
{
	assert(el.IsShell());

	int i, j, k;
	int n = el.Nodes();
	double d, dmin = 1e99;
	vec3d r[4], D[4];
	double h[4];
	for (i = 0; i<n; ++i) r[i] = Node(el.m_node[i]).r;

	FEFace& face = Face(el.m_face[0]);
	for (i = 0; i<n; ++i) D[i] = face.m_nn[i];//normal node
	for (i = 0; i<n; ++i) h[i] = el.m_h[i];//shell thickness

	for (k = 0; k<2; ++k)
	{
		for (i = 0; i<n; ++i)
		{
			double J[3][3] = { 0 };
			double z = (k == 0 ? 0.5*h[i] : -0.5*h[i]);
			if (el.IsType(FE_QUAD4))
			{
				for (j = 0; j<n; ++j)
				{
					J[0][0] += (r[j].x + z*D[j].x)*GQUAD[i][j][0]; J[1][0] += (r[j].y + z*D[j].y)*GQUAD[i][j][0]; J[2][0] += (r[j].z + z*D[j].z)*GQUAD[i][j][0];
					J[0][1] += (r[j].x + z*D[j].x)*GQUAD[i][j][1]; J[1][1] += (r[j].y + z*D[j].y)*GQUAD[i][j][1]; J[2][1] += (r[j].z + z*D[j].z)*GQUAD[i][j][1];
				}
				J[0][2] += 0.5*h[i] * D[i].x; J[1][2] += 0.5*h[i] * D[i].y; J[2][2] += 0.5*h[i] * D[i].z;
			}
			else if (el.IsType(FE_TRI3))
			{
				for (j = 0; j<3; ++j)
				{
					J[0][0] += (r[j].x + z*D[j].x)*GTRI[i][j][0]; J[1][0] += (r[j].y + z*D[j].y)*GTRI[i][j][0]; J[2][0] += (r[j].z + z*D[j].z)*GTRI[i][j][0];
					J[0][1] += (r[j].x + z*D[j].x)*GTRI[i][j][1]; J[1][1] += (r[j].y + z*D[j].y)*GTRI[i][j][1]; J[2][1] += (r[j].z + z*D[j].z)*GTRI[i][j][1];
				}
				J[0][2] += 0.5*h[i] * D[i].x; J[1][2] += 0.5*h[i] * D[i].y; J[2][2] += 0.5*h[i] * D[i].z;
			}

			d = J[0][0] * (J[1][1] * J[2][2] - J[1][2] * J[2][1]) + J[0][1] * (J[1][2] * J[2][0] - J[1][0] * J[2][2]) + J[0][2] * (J[1][0] * J[2][1] - J[1][1] * J[2][0]);
			if (d < dmin) dmin = d;
		}
	}

	return dmin;
}

//-----------------------------------------------------------------------------
void FEMesh::FixinvertedElements()
{
	//vector to save thickness at each node
	vector<double> nodeShellThickness;
	int nodes = Nodes();
	nodeShellThickness.reserve(nodes);
	for (int k = 0; k< nodes; k++)
		nodeShellThickness.push_back(0);
	for (int i = 0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		assert(e.IsShell());
		for (int k = 0; k<e.Nodes(); k++)
		{
			if (nodeShellThickness[e.m_node[k]] != 0)
				nodeShellThickness[e.m_node[k]] = (nodeShellThickness[e.m_node[k]] + e.m_h[k]) / 2;
			else
				nodeShellThickness[e.m_node[k]] = e.m_h[k];
		}
	}

	double start[9], end[9];
	for (int i = 0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		assert(e.IsShell());
		//number of nodes per shell
		int n = e.Nodes();
		//getting thickness from vector
		for (int j = 0; j<n; ++j)
			e.m_h[j] = nodeShellThickness[e.m_node[j]];

		//initial jacobian
		double val = ShellJacobian(e);
		if (val<0)
		{
			for (int j = 0; j<n; ++j)
			{
				end[j] = e.m_h[j];
				start[j] = 0;
			}
			while (val < 0)
			{
				for (int j = 0; j<n; ++j)
					e.m_h[j] = (end[j] + start[j]) / 2;
				val = ShellJacobian(e);
				if (val < 0)
				for (int j = 0; j<n; ++j) end[j] = e.m_h[j];
				else if (val > 0)
				for (int j = 0; j<n; ++j) start[j] = e.m_h[j];
			}

			for (int j = 0; j<n; ++j)
				nodeShellThickness[e.m_node[j]] = e.m_h[j];
		}
	}

	for (int i = 0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		for (int k = 0; k<e.Nodes(); k++)
			e.m_h[k] = nodeShellThickness[e.m_node[k]];
	}

}

//-----------------------------------------------------------------------------
void FEMesh::FixReferenceSurface()
{	
	// clear tags
	TagAllElements(0);
	vector<double> thickness;
	thickness.reserve(Nodes());
	for (int i = 0 ; i< Nodes();i++)
	{
		thickness.push_back(0);
		Node(i).m_ntag = 0;
	}
	//save thickness of each node 
	for(int i =0 ; i< Elements() ; i++)
	{
		FEElement& e = Element(i);
		assert(e.IsShell());
		for(int j=0;j<e.Nodes();++j)
		{
			double current_thickness = e.m_h[j];
			int node_id =  e.m_node[j];
			if (thickness[node_id] == 0)
				thickness[node_id] = current_thickness;
		}	
	}


	for (int i=0; i<Faces(); ++i) 
	{		
		FEFace& f1 = Face(i);
		for(int j=0;j<f1.Nodes();++j)
		{
			int node_id = f1.n[j];	
			vec3d node_normal = f1.m_nn[j];
			FENode &n = Node(node_id);
			if(n.m_ntag == 0)
			{
				n.r -= node_normal * thickness[node_id]/2 ;
				n.m_ntag = 1;
			}
		}
	}


}

//-----------------------------------------------------------------------------
void FEMesh::InterpolateShellThickness(double maxThickness)
{	
	//vector to save thickness at each node
	vector<double> nodeShellThickness; 
	int nodes = Nodes();
	nodeShellThickness.reserve(nodes);
	for (int k = 0 ; k< nodes;k++)
		nodeShellThickness.push_back(0);
	for (int i=0; i<Elements(); ++i) 
	{
		FEElement& e = Element(i);
		assert(e.IsShell());
		for(int k =0;k<e.Nodes();k++)
		{
			if (nodeShellThickness[e.m_node[k]] != 0)
			{
				assert(nodeShellThickness[e.m_node[k]] == e.m_h[k]);
			}
			else
				nodeShellThickness[e.m_node[k]] = e.m_h[k];
		}
	}
	//vector to save node ids of nodes with max-thickness
	vector<int> maxShellThickness; 
	for (int i = 0 ; i< nodes;i++)
	{
		if (nodeShellThickness[i] >= maxThickness )
			maxShellThickness.push_back(i);
	}
	// build the node-node table
	vector< set<int> > NNT;
	BuildNodeNodeTable(NNT);
	while(maxShellThickness.size() != 0)
	{
		for (int i = 0 ; i< maxShellThickness.size();i++)
		{	
			int node_id = maxShellThickness[i];
			set<int>& NL = NNT[node_id];
			set<int>::iterator it;
			double sum = 0;
			int count = 0;
			for (it=NL.begin(); it != NL.end(); ++it)
			{
				int m = *it;
				if (nodeShellThickness[m] < maxThickness)
				{
					sum += nodeShellThickness[m];
					count++ ;
				}			
			}
			if (sum != 0)
			{
				nodeShellThickness[node_id] = sum/count;	
				maxShellThickness.erase(maxShellThickness.begin() + i);
				i--;
			}
		}
	}
	for (int i=0; i<Elements(); ++i) 
	{
		FEElement& e = Element(i);
		assert(e.IsShell());
		for(int k =0;k<e.Nodes();k++)			
			e.m_h[k] = nodeShellThickness[e.m_node[k]];
	}
}
//-----------------------------------------------------------------------------
void FEMesh::RemoveNonManifoldElements()
{
	// clear all tags
	for (int i=0; i<Elements(); ++i) Element(i).m_ntag = 0;

	// loop over all elements
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& el = Element(i);
		int n = el.Edges();
		for (int j=0; j<n; ++j) if (el.m_nbr[j] == -1) { el.m_ntag = 1; break; }
	}

	// remove tagged elements
	DeleteTaggedElements(1);
}

//-----------------------------------------------------------------------------
// Delete tagged elements
void FEMesh::DeleteTaggedElements(int tag)
{
	// remove tagged elements
	int n = 0;
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& e1 = Element(i);
		FEElement& e2 = Element(n);

		if (e1.m_ntag != tag)
		{
			e2 = e1;
			++n;
		}
	}
	m_Elem.resize(n);
	m_data.resize(n);

	// remove isolated nodes
	RemoveIsolatedNodes();

	// update the mesh
	// is already called in RemoveIsolatedNodes
//	Update();
}

//-----------------------------------------------------------------------------
// Delete tagged faces
void FEMesh::DeleteTaggedFaces(int tag)
{
	// remove tagged elements
	int n = 0;
	for (int i=0; i<Faces(); ++i)
	{
		FEFace& f1 = Face(i);
		FEFace& f2 = Face(n);

		if (f1.m_ntag != tag)
		{
			f2 = f1;
			++n;
		}
	}
	m_Face.resize(n);
}

//-----------------------------------------------------------------------------
// Delete tagged edges
void FEMesh::DeleteTaggedEdges(int ntag)
{
	// remove tagged edges
	int n = 0;
	for (int i=0; i<Edges(); ++i)
	{
		FEEdge& e1 = Edge(i);
		FEEdge& e2 = Edge(n);

		if (e1.m_ntag != ntag)
		{
			e2 = e1;
			++n;
		}
	}
	m_Edge.resize(n);

	// reindex the edges
	int ng = 0;
	for (int i=0; i<Edges(); ++i)
		if (Edge(i).m_gid > ng) ng = Edge(i).m_gid;

	vector<int> tag;
	tag.assign(ng+1, -1);
	for (int i=0; i<Edges(); ++i)
		tag[Edge(i).m_gid] = 0;

	n = 0;
	for (int i=0; i<ng+1; ++i)
	{
		if (tag[i] == 0) tag[i] = n++;
	}

	for (int i=0; i<Edges(); ++i)
		Edge(i).m_gid = tag[Edge(i).m_gid];
}

//-----------------------------------------------------------------------------
//! This function identifies duplicate faces and returns a list with the duplicates
void FEMesh::FindDuplicateFaces(vector<int>& l)
{
	l.clear();
	int NF = Faces();
	for (int i=0; i<NF; ++i)
	{
		FEFace& fi = Face(i);
		for (int j=i+1; j<NF; ++j)
		{
			FEFace& fj = Face(j);
			if (fi == fj) l.push_back(j);
		}
	}
}

//-----------------------------------------------------------------------------
//! This function identifies duplicate edges and returns a list with the duplicates
void FEMesh::FindDuplicateEdges(vector<int>& l)
{
	l.clear();
	int NL = Edges();
	for (int i=0; i<NL; ++i)
	{
		FEEdge& ei = Edge(i);
		for (int j=i+1; j<NL; ++j)
		{
			FEEdge& ej = Edge(j);
			if (ei == ej) l.push_back(j);
		}
	}
}


//-----------------------------------------------------------------------------
// Delete selected faces
void FEMesh::DeleteSelectedFaces()
{
	int i;

	// clear element selection
	for (i=0; i<Elements(); ++i) Element(i).UnSelect();

	// select all elements that will be deleted
	for (i=0; i<Faces(); ++i) 
		if (Face(i).IsSelected()) 
			Element( Face(i).m_elem[0] ).Select();

	// delete selected elements
	DeleteSelectedElements();
}

//-----------------------------------------------------------------------------
// Delete selected nodes
void FEMesh::DeleteSelectedNodes()
{
	int i, j, n;

	// select elements that will be deleted
	for (i=0; i<Elements(); ++i)
	{
		FEElement& el = Element(i);
		el.UnSelect();

		n = el.Nodes();
		for (j=0; j<n; ++j) 
			if (Node(el.m_node[j]).IsSelected()) el.Select();
	}

	// delete selected elements
	DeleteSelectedElements();
}

//-----------------------------------------------------------------------------
// Build the node-element table
void FEMesh::BuildNodeElementTable(vector< vector<int> >& NET)
{
	int i, j, n;
	int NN = Nodes();
	int NE = Elements();

	// zero nodal valences
	for (i=0; i<NN; ++i) m_Node[i].m_ntag = 0;

	// calculate nodal valences
	for (i=0; i<NE; ++i)
	{
		FEElement& el = m_Elem[i];
		n = el.Nodes();
		for (j=0; j<n; ++j) m_Node[ el.m_node[j] ].m_ntag++;
	}

	// allocate node-element-table
	NET.resize(NN);
	for (i=0; i<NN; ++i) NET[i].reserve(m_Node[i].m_ntag);

	// fill node element table
	for (i=0; i<NE; ++i)
	{
		FEElement& el = m_Elem[i];
		n = el.Nodes();
		for (j=0; j<n; ++j) NET[el.m_node[j]].push_back(i);
	}
}

//-----------------------------------------------------------------------------
// Build the node-face table
void FEMesh::BuildNodeFaceTable(vector< vector<int> >& NFT)
{
	int i, j, n;
	int NN = Nodes();
	int NF = Faces();

	// zero nodal valences
	for (i=0; i<NN; ++i) m_Node[i].m_ntag = 0;

	// calculate nodal valences
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		n = f.Nodes();
		for (j=0; j<n; ++j) m_Node[ f.n[j] ].m_ntag++;
	}

	// allocate node-face-table
	NFT.resize(NN);
	for (i=0; i<NN; ++i) NFT[i].reserve(m_Node[i].m_ntag);

	// fill node element table
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		n = f.Nodes();
		for (j=0; j<n; ++j) NFT[f.n[j]].push_back(i);
	}
}

//-----------------------------------------------------------------------------
// Build the node-edge table
void FEMesh::BuildNodeEdgeTable(vector< vector<int> >& NET)
{
	int i;
	int NN = Nodes();
	int NE = Edges();
	for (i=0; i<NN; ++i) m_Node[i].m_ntag = 0;
	for (i=0; i<NE; ++i)
	{
		FEEdge& e = Edge(i);
		m_Node[e.n[0]].m_ntag++;
		m_Node[e.n[1]].m_ntag++;
	}

	NET.resize(NN);
	for (i=0; i<NN; ++i) NET[i].reserve(m_Node[i].m_ntag);

	for (i=0; i<NE; ++i)
	{
		FEEdge& e = Edge(i);
		NET[ e.n[0] ].push_back(i);
		NET[ e.n[1] ].push_back(i);
	}
}

//-----------------------------------------------------------------------------
// Builds the internal edge table. That is, the table of all edges
void FEMesh::BuildEdgeTable(vector<pair<int, int> >& ET)
{
	ET.clear();

	// build the node-node table
	vector< set<int> > NNT;
	BuildNodeNodeTable(NNT);

	// add all the edges
	int NN = Nodes();
	for (int i=0; i<NN; ++i)
	{
		pair<int, int> edge;
		set<int>& NL = NNT[i];
		set<int>::iterator it;
		for (it=NL.begin(); it != NL.end(); ++it)
		{
			int m = *it;
			if (m > i)
			{
				edge.first = i;
				edge.second = m;
				ET.push_back(edge);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// TODO: This assumes TET4 elements
void FEMesh::BuildElementEdgeTable(vector<vector<int> >& EET, vector<pair<int, int> >& ET)
{
	const int EN[6][2] = {{0,1},{1,2},{2,0},{0,3},{1,3},{2,3}};

	int NN = Nodes();
	vector<pair<int, int> > NI;
	NI.resize(NN);
	for (int i=0; i<NN; ++i) NI[i].second = 0;
	for (int i=ET.size()-1; i>=0; --i)
	{
		pair<int, int>& et = ET[i];
		NI[et.first].first = i;
		NI[et.first].second++;
	}

	int NE = Elements();
	EET.resize(NE);
	for (int i=0; i<NE; ++i)
	{
		FEElement& el = Element(i);
		assert(el.GetType() == FE_TET4);
		vector<int>& EETi = EET[i];
		EETi.resize(6);
		for (int j=0; j<6; ++j)
		{
			int n0 = el.m_node[EN[j][0]];
			int n1 = el.m_node[EN[j][1]];

			if (n1 < n0) { int nt = n1; n1 = n0; n0 = nt; }

			int l0 = NI[n0].first;
			int ln = NI[n0].second;
			for (int l=0; l<ln; ++l)
			{
				assert(ET[l0+l].first == n0);
				if (ET[l0+l].second == n1)
				{
					EETi[j] = l0+l;
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Build the node-node table. That is table of node indices that each node
// connects to.
void FEMesh::BuildNodeNodeTable(vector<set<int> >& NNT)
{
	// reset node-node table
	int NN = Nodes();
	NNT.resize(NN);
	for (int i=0; i<NN; ++i) NNT[i].clear();

	// loop over all elements
	int NE = Elements();
	for (int i=0; i<NE; ++i)
	{
		FEElement& e = Element(i);
		int ne = e.Nodes();
		for (int j=0; j<ne; ++j)
		{
			int nj = e.m_node[j];
			for (int k=0; k<ne; ++k)
			{
				int nk = e.m_node[k];
				if (nj != nk) NNT[nj].insert(nk);										
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Build the node-node table for surface nodes only. That is table of node indices that each node
// connects to.
void FEMesh::BuildSurfaceNodeNodeTable(vector<set<int> >& NNT)
{
	// reset node-node table
	int NN = Nodes();
	NNT.resize(NN);
	for (int i=0; i<NN; ++i) NNT[i].clear();

	// loop over all faces
	int NF = Faces();
	for (int i=0; i<NF; ++i)
	{
		FEFace& f = Face(i);
		int nf = f.Nodes();
		for (int j=0; j<nf; ++j)
		{
			int nj = f.n[j];
			for (int k=0; k<nf; ++k)
			{
				int nk = f.n[k];
				if (nj != nk) NNT[nj].insert(nk);										
			}
		}
	}
}


//-----------------------------------------------------------------------------
void FEMesh::BuildFaceTable(vector<FEFace>& FT)
{
	FT.clear();
	for (int i=0; i<Elements(); ++i) Element(i).m_ntag = i;

	for (int i=0; i<Elements(); ++i)
	{
		FEElement& ei = Element(i);
		int nf = ei.Faces();
		for (int j=0; j<nf; ++j)
		{
			if ((ei.m_nbr[j] < 0) || (Element(ei.m_nbr[j]).m_ntag < ei.m_ntag))
			{
				FEFace f = ei.GetFace(j);
				FT.push_back(f);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// only works with tet4 meshes
void FEMesh::BuildElementFaceTable(vector<vector<int> >& EFT, vector<FEFace>& FT)
{
	// build a node face table for FT to facilitate searching
	vector<vector<int> > NFT; NFT.resize(Nodes());
	for (int i=0; i<(int)FT.size(); ++i) 
	{
		FEFace& f = FT[i];
		assert(f.m_nodes == 3);
		NFT[f.n[0]].push_back(i);
		NFT[f.n[1]].push_back(i);
		NFT[f.n[2]].push_back(i);
	}

	EFT.resize(Elements());
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& ei = Element(i);
		vector<int>& EFTi = EFT[i];
		EFTi.resize(4);
		assert(ei.GetType() == FE_TET4);
		for (int j=0; j<4; ++j)
		{
			FEFace fj = ei.GetFace(j);
			EFTi[j] = -1;
			vector<int>& nfi = NFT[fj.n[0]];
			for (int k=0; k<(int)nfi.size(); ++k)
			{
				FEFace& fk = FT[nfi[k]];
				if (fj == fk)
				{
					EFTi[j] = nfi[k];
					break;
				}
			}
			assert(EFTi[j] != -1);
		}
	}
}

//-----------------------------------------------------------------------------
void FEMesh::BuildFaceEdgeTable(vector< vector<int> >& FET, vector< pair<int,int> >& ET)
{
	const int EN[3][2] = {{0,1},{1,2},{2,0}};

	int NN = Nodes();
	vector<pair<int, int> > NI;
	NI.resize(NN);
	for (int i=0; i<NN; ++i) NI[i].second = 0;
	for (int i=ET.size()-1; i>=0; --i)
	{
		pair<int, int>& et = ET[i];
		NI[et.first].first = i;
		NI[et.first].second++;
	}

	int NF = Faces();
	FET.resize(NF);
	for (int i=0; i<NF; ++i)
	{
		FEFace& face = Face(i);
		assert(face.Nodes() == 3);
		vector<int>& FETi = FET[i];
		FETi.resize(3);
		for (int j=0; j<3; ++j)
		{
			int n0 = face.n[EN[j][0]];
			int n1 = face.n[EN[j][1]];

			if (n1 < n0) { int nt = n1; n1 = n0; n0 = nt; }

			int l0 = NI[n0].first;
			int ln = NI[n0].second;
			for (int l=0; l<ln; ++l)
			{
				assert(ET[l0+l].first == n0);
				if (ET[l0+l].second == n1)
				{
					FETi[j] = l0+l;
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
void FEMesh::BuildFaceFaceTable(vector<int>& FFT, vector<FEFace>& FT)
{
	// build a node face table for FT to facilitate searching
	vector<vector<int> > NFT; NFT.resize(Nodes());
	for (int i=0; i<(int)FT.size(); ++i) 
	{
		FEFace& f = FT[i];
		assert(f.m_nodes == 3);
		NFT[f.n[0]].push_back(i);
		NFT[f.n[1]].push_back(i);
		NFT[f.n[2]].push_back(i);
	}

	FFT.resize(Faces());
	for (int i=0; i<Faces(); ++i)
	{
		FEFace& fi = Face(i);
		vector<int>& nfi = NFT[fi.n[0]];
		FFT[i] = -1;
		for (int k=0; k<(int)nfi.size(); ++k)
		{
			FEFace& fk = FT[nfi[k]];
			if (fi == fk)
			{
				FFT[i] = nfi[k];
				break;
			}
		}
		assert(FFT[i] != -1);
	}
}

//-----------------------------------------------------------------------------
void FEMesh::BuildEdgeEdgeTable(vector<int>& EET, vector< pair<int,int> >& ET)
{
	// build a node-edge table for ET to facilitate searching
	vector<vector<int> > NET; NET.resize(Nodes());
	for (int i=0; i<(int)ET.size(); ++i) 
	{
		pair<int,int>& edge = ET[i];
		NET[edge.first ].push_back(i);
		NET[edge.second].push_back(i);
	}

	EET.resize(Edges());
	for (int i=0; i<Edges(); ++i)
	{
		FEEdge& ei = Edge(i);
		vector<int>& nei = NET[ei.n[0]];
		EET[i] = -1;
		for (int k=0; k<(int)nei.size(); ++k)
		{
			pair<int,int>& ek = ET[nei[k]];
			if (((ei.n[0] == ek.first)&&(ei.n[1]==ek.second))||
				((ei.n[1] == ek.first)&&(ei.n[0]==ek.second)))
			{
				EET[i] = nei[k];
				break;
			}
		}
		assert(EET[i] != -1);
	}
}

//-----------------------------------------------------------------------------
// Update mesh data
void FEMesh::Update()
{
	// update the element neighbours
	UpdateElementNeighbors();

	// update face neighbours
	UpdateFaces();

	// update the edges
	UpdateEdges();

	// update normals
	UpdateNormals();

	// now we can figure out which nodes are interior and which are exterior
	UpdateNodes();
}

//-----------------------------------------------------------------------------
inline bool cmp_fn(int* n1, int* n2, int m)
{
	switch (m)
	{
	case 3:
		{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2])) return false;
		}
		break;
	case 4:
		{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2]) && (n1[0] != n2[3])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2]) && (n1[1] != n2[3])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2]) && (n1[2] != n2[3])) return false;
			if ((n1[3] != n2[0]) && (n1[3] != n2[1]) && (n1[3] != n2[2]) && (n1[3] != n2[3])) return false;
		}
		break;
	case 6:
		{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2])) return false;

			if ((n1[3] != n2[3]) && (n1[3] != n2[4]) && (n1[3] != n2[5])) return false;
			if ((n1[4] != n2[3]) && (n1[4] != n2[4]) && (n1[4] != n2[5])) return false;
			if ((n1[5] != n2[3]) && (n1[5] != n2[4]) && (n1[5] != n2[5])) return false;
		}
		break;
	case 7:
	{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2])) return false;

			if ((n1[3] != n2[3]) && (n1[3] != n2[4]) && (n1[3] != n2[5])) return false;
			if ((n1[4] != n2[3]) && (n1[4] != n2[4]) && (n1[4] != n2[5])) return false;
			if ((n1[5] != n2[3]) && (n1[5] != n2[4]) && (n1[5] != n2[5])) return false;

			if (n1[6] != n2[6]) return false;
		}
		break;
	case 8:
		{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2]) && (n1[0] != n2[3])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2]) && (n1[1] != n2[3])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2]) && (n1[2] != n2[3])) return false;
			if ((n1[3] != n2[0]) && (n1[3] != n2[1]) && (n1[3] != n2[2]) && (n1[3] != n2[3])) return false;

			if ((n1[4] != n2[4]) && (n1[4] != n2[5]) && (n1[4] != n2[6]) && (n1[4] != n2[7])) return false;
			if ((n1[5] != n2[4]) && (n1[5] != n2[5]) && (n1[5] != n2[6]) && (n1[5] != n2[7])) return false;
			if ((n1[6] != n2[4]) && (n1[6] != n2[5]) && (n1[6] != n2[6]) && (n1[6] != n2[7])) return false;
			if ((n1[7] != n2[4]) && (n1[7] != n2[5]) && (n1[7] != n2[6]) && (n1[7] != n2[7])) return false;
		}
		break;
	case 9:
		{
			if ((n1[0] != n2[0]) && (n1[0] != n2[1]) && (n1[0] != n2[2]) && (n1[0] != n2[3])) return false;
			if ((n1[1] != n2[0]) && (n1[1] != n2[1]) && (n1[1] != n2[2]) && (n1[1] != n2[3])) return false;
			if ((n1[2] != n2[0]) && (n1[2] != n2[1]) && (n1[2] != n2[2]) && (n1[2] != n2[3])) return false;
			if ((n1[3] != n2[0]) && (n1[3] != n2[1]) && (n1[3] != n2[2]) && (n1[3] != n2[3])) return false;

			if ((n1[4] != n2[4]) && (n1[4] != n2[5]) && (n1[4] != n2[6]) && (n1[4] != n2[7])) return false;
			if ((n1[5] != n2[4]) && (n1[5] != n2[5]) && (n1[5] != n2[6]) && (n1[5] != n2[7])) return false;
			if ((n1[6] != n2[4]) && (n1[6] != n2[5]) && (n1[6] != n2[6]) && (n1[6] != n2[7])) return false;
			if ((n1[7] != n2[4]) && (n1[7] != n2[5]) && (n1[7] != n2[6]) && (n1[7] != n2[7])) return false;

			if (n1[8] != n2[8]) return false;
		}
		break;
	default:
		assert(false);
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// This function finds the element neighbours.
//
void FEMesh::UpdateElementNeighbors()
{
	int i, j, l, n;

	// get number of elements
	int elems = Elements();

	// reset all element neighbor and face ptrs
	FEElement* pe = ElementPtr();
	for (i=0; i<elems; i++, pe++)
	{
		pe->m_ntag = i;
		for (j=0; j<6; ++j) 
		{
			pe->m_nbr[j] = -1;
			pe->m_face[j] = -1;
		}
	}

	// calculate the node-element table
	vector< vector<int> > NET;
	BuildNodeElementTable(NET);

	// set up the element's neighbour pointers
	FEEdge edge;
	int nbrf, nbre;
	int fn1[9], fn2[9];

	// assign neighbor elements
	pe = ElementPtr();
	for (i=0; i<elems; i++, pe++)
	{
		// do the solid elements
		n = pe->Faces();
		for (j=0; j<n; j++)
		{
			if (pe->m_nbr[j] == -1)
			{
				int m1 = pe->GetFace(j, fn1);

				// find the neighbour element
				int inode = fn1[0];
				int nval = NET[inode].size();
				bool bfound = false;
				for (int k=0; k < nval; k++)
				{
					int nbe = NET[inode][k];
					FEElement* pne = ElementPtr(nbe);
					if (pne != pe)
					{
						nbrf = pne->Faces();
						for (l=0; l<nbrf; l++) 
						{
							int m2 = pne->GetFace(l, fn2);
							if ((m1 == m2) && (cmp_fn(fn1, fn2,m1)))
							{
								bfound = true;
								break;
							}
						}

						// we must consider the possibility that a shell is 
						// sitting on top of a solid element
						// TODO: This won't work if a shell is sitting on top of a HEX20
						if (pne->IsShell())
						{
							int fn3[4], *n = pne->m_node;
							fn3[0] = n[0];
							fn3[1] = n[1];
							fn3[2] = n[2];
							fn3[3] = (pne->IsType(FE_TRI3)? n[2] : n[3]);
							if (cmp_fn(fn1, fn3, (n[2] == n[3] ? 3 : 4)))
							{
								pe->m_nbr[j] = nbe;
								// don't make the solid a neighbor of the shell!
								break;
							}
						}

						if (bfound)
						{
							pe->m_nbr[j] = nbe;
							pne->m_nbr[l] = i;
							break;
						}
					}
				}
			}
		}

		// do the shell elements
		n = pe->Edges();
		for (j=0; j<n; j++)
		{
			pe->m_nbr[j] = -1;
			pe->m_face[j] = -1;
			edge = pe->GetEdge(j);

			// find the neighbour element
			int inode = edge.n[0];
			int nval = NET[inode].size();
			bool bfound = false;
			for (int k=0; k < nval; k++)
			{
				FEElement* pne = ElementPtr(NET[inode][k]);
				if (pne != pe)
				{
					nbre = pne->Edges();
					for (l=0; l<nbre; l++) 
						if (edge == pne->GetEdge(l))
						{
							bfound = true;
							break;
						}

					if (bfound)
					{
						pe->m_nbr[j] = NET[inode][k];
						break;
					}
				}
			}
		}

		// do the beam elements
		if (pe->IsType(FE_BEAM2))
		{
			for (j=0; j<2; ++j)
			{				
				pe->m_nbr[j] = -1;
				pe->m_face[j] = -1;
				int inode = pe->m_node[j];
				int nval = NET[inode].size();
				for (int k=0; k<nval; ++k)
				{
					FEElement* pne = ElementPtr(NET[inode][k]);
					if (pne != pe)
					{
						if ((pne->IsType(FE_BEAM2)) && ((pne->m_node[0] == pe->m_node[j]) || (pne->m_node[1] == pe->m_node[j])))
						{
							pe->m_nbr[j] = NET[inode][k];
							break;
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//! This function finds the face neighbours. Note that internal faces cannot
//! be neighbours of external faces. 
//!
//! \todo The FEAutoMesher::BuildFaces also assign the FEFace.m_elem variables.
//!       I should probably change this so that only one of these two functions
//!		 does this.
void FEMesh::UpdateFaces()
{
	int i, j, k, n, m, inode, nval;
	int NF = Faces();

	// make sure we have faces
	assert(NF);

	// first we need to figure out which face belongs to which element
	// first build the node element table
	vector< vector<int> > NET;
	BuildNodeElementTable(NET);

	// loop over all faces
	int f2[9];
	for (i=0; i<NF; ++i)
	{
		FEFace& f = Face(i);
		int m1 = f.m_nodes;
		f.m_elem[0] = -1;
		f.m_elem[1] = -1;

		// pick a node on the face
		inode = f.n[0];
		nval = NET[inode].size();
		m = 0;
		for (j=0; j<nval; ++j)
		{
			int ni = NET[inode][j];
			FEElement* pe = ElementPtr(ni);

			// search solid elements
			if (pe->IsSolid())
			{
				n = pe->Faces();
				for (k=0; k<n; ++k)
				{
					int m2 = pe->GetFace(k, f2);
					if ((m1 == m2) && cmp_fn(f.n, f2, m1))
					{
						assert(m<2);
						if (m == 0) f.m_elem[m++] = ni;
						else
						{
							int n0 = f.m_elem[0];
							if (ni < n0) { f.m_elem[0] = ni; f.m_elem[1] = n0; } else { f.m_elem[0] = n0; f.m_elem[1] = ni; }
						}
						pe->m_face[k] = i;
					}
				}
			}
			else if (pe->IsShell())
			{
				FEFace f2 = pe->GetShellFace();
				if (f == f2)
				{
					assert(m<2);
					if (m<2)
					{
						f.m_elem[m++] = ni;
						pe->m_face[0] = i;
					}
				}
			}
		}

		assert(f.m_elem[0] != -1);
	}

	// build the node-face table
	vector< vector<int> > NFT;
	BuildNodeFaceTable(NFT);

	// find all face neighbours
	int n1, n2;
	for (i=0; i<NF; ++i)
	{
		FEFace* pf = FacePtr(i);

		int nf = pf->Nodes();
		n = pf->Edges();
		for (j=0; j<n; ++j)
		{
			n1 = pf->n[j];
			n2 = pf->n[(j+1)%n];
			nval = NFT[n1].size();
			pf->m_nbr[j] = -1;
			for (k=0; k<nval; ++k)
			{
				FEFace* pfn = FacePtr(NFT[n1][k]);
				if (pfn != pf)
				{
					if (pfn->HasEdge(n1, n2))
					{
						if (pf->IsExternal())
						{
							if (pfn->IsExternal()) { pf->m_nbr[j] = NFT[n1][k]; break; }
						}
						else if (!pfn->IsExternal()) { pf->m_nbr[j] = NFT[n1][k]; break; }
					}
				}
			}
		}
	}

#ifdef _DEBUG
	// see if all faces belong to at least one element
	for (i=0; i<NF; ++i)
	{
		FEFace& f = Face(i);
		assert(f.m_elem[0] >= 0);
	}
#endif

}

//-----------------------------------------------------------------------------
// This function finds the edge neighbours
//
void FEMesh::UpdateEdges()
{
	int i, j, k, n, nval;
	int NE = Edges();

	// build the node-edge table
	vector< vector<int> > NET;
	BuildNodeEdgeTable(NET);

	// find neighbours
	for (i=0; i<NE; ++i)
	{
		FEEdge* pe = EdgePtr(i);
		for (j=0; j<2; ++j)
		{
			pe->m_nbr[j] = -1;
			n = pe->n[j];
			nval = NET[n].size();
			if (nval == 2)
			{
				for (k=0; k<2; ++k)
				{
					FEEdge* pen = EdgePtr(NET[n][k]);
					if (pen != pe) pe->m_nbr[j] = NET[n][k];
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// This function finds the interior and exterior nodes
//
void FEMesh::UpdateNodes()
{
	int i, j;

	int nodes = Nodes();
	int faces = Faces();
	int elems = Elements();

	FENode* pn = NodePtr();
	for (i=0; i<nodes; ++i, ++pn) pn->m_bext = false;
	
	FEFace* pf = FacePtr();
	for (i=0; i<faces; ++i, ++pf)
		for (j=0; j<pf->m_nodes; ++j)
			m_Node[pf->n[j]].m_bext = true;

	// mark all nodes attached to beams as exterior
	FEElement* pe = ElementPtr();
	for (i=0; i<elems; ++i, ++pe)
	{
		if (pe->IsType(FE_BEAM2))
		{
			Node(pe->m_node[0]).m_bext = true;
			Node(pe->m_node[1]).m_bext = true;
		}
	}
}

//-----------------------------------------------------------------------------
// This function assignes group ID's to the mesh' faces based on a smoothing
// angle.
//
void FEMesh::AutoSmooth(double w)
{
	int NF = Faces();

	// smoothing threshold
	double eps = (double) cos(w*PI/180);

	// clear face group ID's
	for (int i=0; i<NF; ++i) 
	{
		FEFace* pf = FacePtr(i);
		pf->m_sid = -1;
	}

	// calculate face normals
	for (int i=0; i<NF; ++i) 
	{
		FEFace* pf = FacePtr(i);

		// calculate the face normals
		vec3d& r0 = Node(pf->n[0]).r;
		vec3d& r1 = Node(pf->n[1]).r;
		vec3d& r2 = Node(pf->n[2]).r;

		pf->m_fn = (r1 - r0)^(r2 - r0);
		pf->m_fn.Normalize();
	}


	// stack for tracking unprocessed faces
	vector<FEFace*> stack(NF);
	int ns = 0;

	// process all faces
	int nsg = 0;
	for (int i=0; i<NF; ++i)
	{
		FEFace* pf = FacePtr(i);
		if (pf->m_sid == -1)
		{
			stack[ns++] = pf;
			while (ns > 0)
			{
				// pop a face
				pf = stack[--ns];

				// mark as processed
				pf->m_sid = nsg;

				// loop over neighbors
				int n = pf->Edges();
				for (int j=0; j<n; ++j)
				{
					FEFace* pf2 = FacePtr(pf->m_nbr[j]);

					// push unprocessed neighbour
					if (pf2 && (pf2->m_sid == -1) && (pf->m_fn*pf2->m_fn >= eps))
					{
						pf2->m_sid = -2;
						stack[ns++] = pf2;
					}
				}
			}
			++nsg;
		}
	}
}

//-----------------------------------------------------------------------------
// Calculate normals of the mesh' faces based on smoothing groups
//
void FEMesh::UpdateNormals()
{
	int i;

	int NN = Nodes();
	int NF = Faces();

	// calculate face normals
	for (i=0; i<NF; ++i) 
	{
		FEFace* pf = FacePtr(i);

		// calculate the face normals
		vec3d& r0 = Node(pf->n[0]).r;
		vec3d& r1 = Node(pf->n[1]).r;
		vec3d& r2 = Node(pf->n[2]).r;

		pf->m_fn = (r1 - r0)^(r2 - r0);
		pf->m_fn.Normalize();

		int nf = pf->m_nodes;
		for (int j=0; j<nf; ++j) pf->m_nn[j] = pf->m_fn;
	}

	//calculate the node normals
	vector<vec3d> norm; norm.resize(NN);
	for (i=0; i<NN; ++i) norm[i] = vec3d(0,0,0);

	// reset smoothing id's
	FEFace* pf = FacePtr();
	for (i=0; i<NF; ++i, ++pf) pf->m_ntag = -1;

	// this array keeps track of all faces in a smoothing group
	vector<FEFace*> F(NF);
	int FC = 0;

	// this array is used as a stack when processing neighbors
	vector<FEFace*> stack(NF);
	int ns = 0;

	// loop over all faces
	int nsg = 0;
	for (int i=0; i<NF; ++i)
	{
		FEFace* pf = FacePtr(i);
		if (pf->m_ntag == -1)
		{
			// find all connected faces
			stack[ns++] = pf;
			while (ns > 0)
			{
				// pop a face
				pf = stack[--ns];

				// mark as processed
				pf->m_ntag = nsg;
				F[FC++] = pf;

				// add face normal to node normal
				int n = pf->Nodes();
				for (int j=0; j<n; ++j) norm[pf->n[j]] += pf->m_fn;

				// process neighbors
				n = pf->Edges();
				for (int j=0; j<n; ++j)
				{
					FEFace* pf2 = FacePtr(pf->m_nbr[j]);
					// push unprocessed neighbor
					if (pf2 && (pf2->m_ntag == -1) && (pf->m_sid == pf2->m_sid))
					{
						pf2->m_ntag = -2;
						stack[ns++] = pf2;
					}
				}
			}

			// assign node normals
			for (int j=0; j<FC; ++j)
			{
				pf = F[j];
				assert(pf->m_ntag == nsg);
				int nf = pf->m_nodes;
				for (int k=0; k<nf; ++k) pf->m_nn[k] = norm[pf->n[k]];
			}

			// clear normals
			for (int j=0; j<FC; ++j)
			{
				pf = F[j];
				int nf = pf->m_nodes;
				for (int k=0; k<nf; ++k) norm[pf->n[k]] = vec3d(0,0,0);
			}
			++nsg;
			FC = 0;
		}
	}

	// normalize face normals
	pf = FacePtr();
	for (i=0; i<NF; ++i, ++pf)
	{
		int n = pf->Nodes();
		for (int j=0; j<n; ++j) pf->m_nn[j].Normalize();
	}
}

//-----------------------------------------------------------------------------
// Extract the selected faces as a shell mesh
FEMesh* FEMesh::ExtractSelectedFaces()
{
	int i, j, n;
	// count selected faces
	int faces = 0;
	for (i=0; i<Faces(); ++i) if (Face(i).IsSelected()) ++faces;

	// tag nodes that need to be copied
	for (i=0; i<Nodes(); ++i) Node(i).m_ntag = -1;
	for (i=0; i<Faces(); ++i)
	{
		FEFace& f = Face(i);
		if (f.IsSelected())
		{
			n = f.Nodes();
			for (j=0; j<n; ++j) Node(f.n[j]).m_ntag = 1;
		}
	}

	// count nodes
	int nodes = 0;
	for (i=0; i<Nodes(); ++i) 
	{
		FENode& node = Node(i);
		if (node.m_ntag == 1) 
		{
			node.m_ntag = nodes;
			++nodes;
		}
	}

	assert( (nodes>0) && (faces>0));

	// allocate new mesh
	FEMesh* pm = new FEMesh();
	pm->Create(nodes, faces);

	// create the nodes
	FENode* pn = pm->NodePtr();
	for (i=0; i<Nodes(); ++i)
	{
		FENode& node = Node(i);
		if (node.m_ntag >= 0)
		{
			*pn = node;
			++pn;
		}
	}

	// create the elements
	FEElement* pe = pm->ElementPtr();
	for (i=0; i<Faces(); ++i)
	{
		FEFace& face = Face(i);
		if (face.IsSelected())
		{
			n = face.Nodes();
			if (n == 4)
			{
				pe->SetType(FE_QUAD4);
			}
			else if (n ==3 )
			{
				pe->SetType(FE_TRI3);
			}
			else assert(false);

			for (j=0; j<n; ++j) pe->m_node[j] = Node(face.n[j]).m_ntag;

			++pe;
		}
	}

	return pm;
}

//-----------------------------------------------------------------------------
void FEMesh::UpdateValueRange()
{
	m_min = m_max = m_data[0];
	int N = (int) m_data.size();
	for (int i=1; i<N; ++i)
	{
		if (m_data[i] > m_max) m_max = m_data[i];
		if (m_data[i] < m_min) m_min = m_data[i];
	}
}

//-----------------------------------------------------------------------------
void FEMesh::GetValueRange(double& vmin, double& vmax)
{
	vmin = m_min;
	vmax = m_max;
}

//-----------------------------------------------------------------------------
// Project a point to the surface of a FE mesh
//
vec3d FEMesh::ProjectToFace(vec3d p, FEFace &f, double &r, double &s)
{
	double R[2], u[2], D;

	vec3d q(0,0,0), y[4];

	double gr[4] = {-1, +1, +1, -1};
	double gs[4] = {-1, -1, +1, +1};
	double H[4], Hr[4], Hs[4], Hrs[4];
	double normu;

	int i, j;
	int NMAX = 10, n=0;

	// number of face nodes
	int nf = f.m_nodes;

	// get the elements nodal positions
	for (i=0; i<nf; ++i) y[i] = Node(f.n[i]).r;

	// loop until converged
	do
	{
		if (nf == 4)
		{
			// do quadrilaterals
			for (i=0; i<4; ++i)
			{
				H[i] = 0.25*(1+gr[i]*r)*(1+gs[i]*s);
	
				Hr[i] = 0.25*gr[i]*( 1 + gs[i]*s );
				Hs[i] = 0.25*gs[i]*( 1 + gr[i]*r );

				Hrs[i] = 0.25*gr[i]*gs[i];
			}
		}
		else
		{
			// do triangles
			H[0] = 1 - r - s;
			H[1] = r;
			H[2] = s;
			Hr[0] = -1; Hs[0] = -1;
			Hr[1] =  1; Hs[1] =  0;
			Hr[2] =  0; Hs[2] =  1;
			Hrs[0] = Hrs[1] = Hrs[2] = 0;
		}

		// set up the system of equations
		q = vec3d(0,0,0);
		R[0] = R[1] = 0;
		double A[2][2] = {0};
		for (i=0; i<nf; ++i)
		{
			R[0] -= (p*y[i])*Hr[i];
			R[1] -= (p*y[i])*Hs[i];

			A[0][1] += (p*y[i])*Hrs[i];
			A[1][0] += (p*y[i])*Hrs[i];

			for (j=0; j<nf; ++j)
			{
				R[0] -= -H[j]*Hr[i]*(y[i]*y[j]);
				R[1] -= -H[j]*Hs[i]*(y[i]*y[j]);

				A[0][0] += -(y[i]*y[j])*(Hr[i]*Hr[j]);
				A[1][1] += -(y[i]*y[j])*(Hs[i]*Hs[j]);

				A[0][1] += -(y[i]*y[j])*(Hs[j]*Hr[i]+H[i]*Hrs[j]);
				A[1][0] += -(y[i]*y[j])*(Hr[j]*Hs[i]+H[i]*Hrs[j]);
			}
		
			q += y[i]*H[i];
		}
	
		// determinant of A
		D = A[0][0]*A[1][1] - A[0][1]*A[1][0];

		// solve for u = A^(-1)*R
		u[0] = (A[1][1]*R[0] - A[0][1]*R[1])/D;
		u[1] = (A[0][0]*R[1] - A[1][0]*R[0])/D;

		normu = sqrt(u[0]*u[0]+u[1]*u[1]);
	
		r += u[0];
		s += u[1];

		++n;
	}
	while ((normu > 1e-7) && (n < NMAX));

	if (nf == 4)
	{
		if      (r < -1) q = ProjectToEdge(y[0], y[3], p, s);
		else if (r >  1) q = ProjectToEdge(y[1], y[2], p, s);
		else if (s < -1) q = ProjectToEdge(y[0], y[1], p, s);
		else if (s >  1) q = ProjectToEdge(y[3], y[2], p, s);
	}
	else
	{
		if      (r < 0) q = ProjectToEdge(y[0], y[2], p, s);
		else if (s < 0) q = ProjectToEdge(y[0], y[1], p, s);
		else if (r+s>1) q = ProjectToEdge(y[1], y[2], p, s);
	}

	return q;
}

//-----------------------------------------------------------------------------
// Project a node to an edge
vec3d FEMesh::ProjectToEdge(vec3d e1, vec3d e2, vec3d p, double& r)
{
	vec3d t = e2 - e1;
	r = (p - e1)*t / (t*t);
	if (r < 0) r = 0;
	if (r > 1) r = 1;
	return e1 + t*r;
}

//-----------------------------------------------------------------------------
// Project a point to the mesh
//
vec3d FEMesh::ProjectToSurface(vec3d r, vec3d t)
{
	int i, j, n;
	// tag all surface nodes
	for (i=0; i<Nodes(); ++i) Node(i).m_ntag = 0;
	for (i=0; i<Faces(); ++i)
	{
		// only pick faces that are facing r
		FEFace& f = Face(i);
		if (t*f.m_fn < 0)
		{
			n = f.Nodes();
			for (j=0; j<n; ++j) Node(f.n[j]).m_ntag = 1;
		}
	}

	// find the closest tagged nodes
	double L, Lmin = 1e99;
	vec3d p = r, q;
	for (i=0; i<Nodes(); ++i)
	{
		FENode& node = Node(i);
		if (node.m_ntag)
		{
			q = r + t*((node.r - r)*t);
			L = fabs((node.r - q).Length());
			if (L < Lmin)
			{
				p = node.r;
				Lmin = L;
			}
		}
	}

	return p;
}

//-----------------------------------------------------------------------------
// Partition the selected faces (?)
//
void FEMesh::PartitionSelection()
{
	int i;
	int N = Faces();

	int nsg = 0;

	// tag all selected faces
	for (i=0; i<N; ++i)
	{
		FEFace& f = Face(i);
		if (f.IsSelected()) 
		{
			f.m_ntag = 1; 
			f.m_gid = -1;
		}
		else 
		{
			f.m_ntag = 0;
			if (f.m_gid > nsg) nsg = f.m_gid;
		}
	}

	// reassign smoothing groups
	++nsg;
	for (i=0; i<N; ++i)
	{
		FEFace& f = Face(i);
		if (f.m_ntag == 1) f.m_gid = nsg;
	}

	UpdateNormals();
}

//-----------------------------------------------------------------------------
// Create a shallow-copy of the mesh
void FEMesh::ShallowCopy(FEMesh* pm)
{
	m_Node = pm->m_Node;
	m_Edge = pm->m_Edge;
	m_Face = pm->m_Face;
	m_Elem = pm->m_Elem;

	m_data = pm->m_data;
}

//-----------------------------------------------------------------------------
void FEMesh::InvertSelectedElements()
{
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		if (e.IsSelected()) e.m_ntag = 1;
		else e.m_ntag = 0;
	}

	InvertTaggedElements(1);
}

//-----------------------------------------------------------------------------
// Invert selected elements.
void FEMesh::InvertTaggedElements(int ntag)
{
	// invert selected elements (or all elements if none are selected)
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		if (e.m_ntag == ntag)
		{
			int n = e.Nodes(), m;

			switch (e.GetType())
			{
			case FE_QUAD4:
			case FE_TRI3:
				{
					for (int j=0; j<n/2; ++j) 
					{
						m = e.m_node[j];
						e.m_node[j] = e.m_node[n-j-1];
						e.m_node[n-j-1] = m;
					}
				}
				break;
			case FE_TET4:
				{
					m = e.m_node[0]; e.m_node[0] = e.m_node[3]; e.m_node[3] = m;
				}
				break;
			case FE_PENTA6:
				{
					m = e.m_node[0]; e.m_node[0] = e.m_node[2]; e.m_node[2] = m;
					m = e.m_node[3]; e.m_node[3] = e.m_node[5]; e.m_node[5] = m;
				}
				break;
			case FE_HEX8:
				{
					m = e.m_node[0]; e.m_node[0] = e.m_node[4]; e.m_node[4] = m;
					m = e.m_node[1]; e.m_node[1] = e.m_node[5]; e.m_node[5] = m;
					m = e.m_node[2]; e.m_node[2] = e.m_node[6]; e.m_node[6] = m;
					m = e.m_node[3]; e.m_node[3] = e.m_node[7]; e.m_node[7] = m;
				}
				break;
			default:
				assert(false);
			}
		}
	}

	// mirror the faces
	for (int i=0; i<Faces(); ++i)
	{
		FEFace& f = Face(i);
		FEElement* pe = ElementPtr(f.m_elem[0]);

		if (pe->m_ntag == ntag)
		{
			FEFace g;
			FindFace(pe, f, g);

			f.n[0] = g.n[0];
			f.n[1] = g.n[1];
			f.n[2] = g.n[2];
			f.n[3] = g.n[3];
		}
	}

	UpdateElementNeighbors();
	UpdateFaces();
	UpdateNormals();
}

//-----------------------------------------------------------------------------
void FEMesh::FindNodesFromPart(int gid, vector<int>& node)
{
	for (int i=0; i<Nodes(); ++i) Node(i).m_ntag = 0;
	for (int i=0; i<Elements(); ++i)
	{
		FEElement& e = Element(i);
		if (e.m_gid == gid)
		{
			int ne = e.Nodes();
			for (int j=0; j<ne; ++j) Node(e.m_node[j]).m_ntag = 1;
		}
	}

	int nodes = 0;
	for (int i=0; i<Nodes(); ++i) if (Node(i).m_ntag == 1) nodes++;

	node.resize(nodes);
	nodes = 0;
	for (int i=0; i<Nodes(); ++i)
		if (Node(i).m_ntag == 1) node[nodes++] = i;
}

//-----------------------------------------------------------------------------
// Find a face of an element.
int FEMesh::FindFace(FEElement *pe, FEFace &f, FEFace& fe)
{
	if (pe->IsSolid())
	{
		int n = pe->Faces();
		for (int i=0; i<n; ++i)
		{
			fe = pe->GetFace(i);
			if (fe == f) return i;
		}
	}
	if (pe->IsShell())
	{
		fe = pe->GetShellFace();
		if (fe == f) return 0;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Find the intersection.
//
bool FEMesh::FindIntersection(FEFace &f, vec3d x, vec3d n, vec3d& q, double& g)
{
	int N = f.m_nodes;

	vec3d y[4];
	for (int i=0; i<N; ++i) y[i] = Node(f.n[i]).r;

	// call the correct intersection function
	if      (N == 3) return IntersectTri (y, x, n, q, g);
	else if (N == 4) return IntersectQuad(y, x, n, q, g);

	// if we get here, the ray did not intersect the element
	return false;
}

//-----------------------------------------------------------------------------
// This function calculates the intersection of a ray with a triangle
// and returns true if the ray intersects the triangle.
//
bool FEMesh::IntersectTri(vec3d* y, vec3d r, vec3d n, vec3d& q, double& g)
{
	vec3d e[2], E[2];
	double G[2][2], Gi[2][2], D;

	// create base vectors on triangle
	e[0] = y[1]-y[0];
	e[1] = y[2]-y[0];

	// create triangle normal
	vec3d m = e[0]^e[1]; m.Normalize();

	double eps = 0.01;

	double d = n*m;
	if (d != 0)
	{
		// distance from r to plane of triangle
		g = m*(y[0] - r)/d;

		// intersection point with plane of triangle
		q = r + n*g;

		// next, we decompose q into its components
		// in the triangle basis
		// we need to create the dual basis
		// first, we calculate the metric tensor
		G[0][0] = e[0]*e[0]; G[0][1] = e[0]*e[1];
		G[1][0] = e[1]*e[0]; G[1][1] = e[1]*e[1];

		// and its inverse
		D = G[0][0]*G[1][1] - G[0][1]*G[1][0];
		Gi[0][0] =  1/D*G[1][1]; Gi[0][1] = -1/D*G[0][1];
		Gi[1][0] = -1/D*G[1][0]; Gi[1][1] =  1/D*G[0][0];

		// build dual basis
		E[0] = e[0]*Gi[0][0] + e[1]*Gi[0][1];
		E[1] = e[0]*Gi[1][0] + e[1]*Gi[1][1];

		// get the components
		double rp = E[0]*(q - y[0]);
		double sp = E[1]*(q - y[0]);

		// see if the intersection point is inside the triangle
		if ((rp >= -eps) && (sp >= -eps) && (rp+sp <= 1+eps)) return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//! This function calculates the intersection of a ray with a quad
//! and returns true if the ray intersected.
//!
bool FEMesh::IntersectQuad(vec3d* y, vec3d r, vec3d n, vec3d& q, double& g)
{
	// first we're going to see if the ray intersects the two subtriangles
	vec3d x1[3], x2[3];
	x1[0] = y[0]; x2[0] = y[2];
	x1[1] = y[1]; x2[1] = y[3];
	x1[2] = y[3]; x2[2] = y[1];

	bool b = false;
	double rp, sp;

	double eps = 0.01;

	if (IntersectTri(x1, r, n, q, g))
	{
		// we've intersected the first triangle
		b = true;
		rp = 0;
		sp = 0;
	}
	else if (IntersectTri(x2, r, n, q, g))
	{
		// we've intersected the second triangle
		b = true;
		rp = 0;
		sp = 0;
	}

	// if one of the triangels was intersected,
	// we calculate a more accurate projection
	if (b)
	{
		mat3d A;
		vec3d dx;
		vec3d F, F1, F2, F3;
		double H[4], H1[4], H2[4];

		double l1 = rp;
		double l2 = sp;
		double l3 = g;
		
		int nn = 0;
		int maxn = 5;
		do
		{
			// shape functions of quad
			H[0] = 0.25*(1 - l1)*(1 - l2);
			H[1] = 0.25*(1 + l1)*(1 - l2);
			H[2] = 0.25*(1 + l1)*(1 + l2);
			H[3] = 0.25*(1 - l1)*(1 + l2);
			q = y[0]*H[0] + y[1]*H[1] + y[2]*H[2] + y[3]*H[3];

			// shape function derivatives
			H1[0] = -0.25*(1 - l2); H2[0] = -0.25*(1 - l1);
			H1[1] =  0.25*(1 - l2); H2[1] = -0.25*(1 + l1);
			H1[2] =  0.25*(1 + l2); H2[2] =  0.25*(1 + l1);
			H1[3] = -0.25*(1 + l2); H2[3] =  0.25*(1 - l1);

			// calculate residual
			F = r + n*l3 - q;

			// residual derivatives
			F1 = - y[0]*H1[0] - y[1]*H1[1] - y[2]*H1[2] - y[3]*H1[3];
			F2 = - y[0]*H2[0] - y[1]*H2[1] - y[2]*H2[2] - y[3]*H2[3];
			F3 = n;

			// set up the tangent matrix
			A[0][0] = F1.x; A[0][1] = F2.x; A[0][2] = F3.x;
			A[1][0] = F1.y; A[1][1] = F2.y; A[1][2] = F3.y;
			A[2][0] = F1.z; A[2][1] = F2.z; A[2][2] = F3.z;

			// calculate solution increment
			mat3d Ai(A);
			Ai.Invert();
			dx = -(Ai*F);

			// update solution
			l1 += dx.x;
			l2 += dx.y;
			l3 += dx.z;

			++nn;
		}
		while ((dx.Length() > 1e-7) && (nn < maxn));

		// store results
		rp = l1;
		sp = l2;
		g  = l3;

		// see if the point is inside the quad
		if ((rp >= -1-eps) && (rp <= 1+eps) && 
			(sp >= -1-eps) && (sp <= 1+eps)) return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
FEElementData* FEMesh::AddDataField(const char* sz, double v)
{
	FEElementData map;
	map.Create(this, v);
	map.SetName(sz);
	m_map.push_back(map);
	return &m_map[m_map.size()-1];
}
