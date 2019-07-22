#include "stdafx.h"
#include "FEAutoMesher.h"
#include "FEMesh.h"
#include <list>
#include <stack>
using namespace std;

//-----------------------------------------------------------------------------
// FEAutoMesher
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FEAutoMesher::FEAutoMesher()
{
	m_pm = 0;
	m_wsmooth = 60.0;
}

//-----------------------------------------------------------------------------
// The automesher is used with the GMeshObject class. Unlike most mesher, this
// mesher does not actually build a new mesh. Instead, it modifies the mesh
// stored in the GMeshObject class so that all faces and edges are properly
// initialized. It partitions the elements into parts based on connectivity
// and material ID's. The surfaces are partitioned based on the parts and a
// smoothing angle.

void FEAutoMesher::BuildMesh(FEMesh* pm)
{
	// keep a pointer to this mesh
	m_pm = pm;

	// update the element neighbours
	pm->UpdateElementNeighbors();

	// build the faces
	BuildFaces(*pm);

	// partition the surface
	AutoPartitionSurface(*pm);

	// build the edges
	BuildEdges(*pm);

	// partition the edges
	AutoPartitionEdges(*pm);

	// partition the nodes
	AutoPartitionNodes(*pm);

	// update the mesh
	pm->UpdateNodes();
	pm->UpdateNormals();

	// we're done with this mesh
	m_pm = 0;
}

//-----------------------------------------------------------------------------

void FEAutoMesher::BuildFaces(FEMesh &m)
{
	int i, j, n;

	int elems = m.Elements();

	FEElement* pe = m.ElementPtr();
	for (i=0; i<elems; i++, pe++) pe->m_ntag = i;

	// let's count them first
	int faces = 0;
	pe = m.ElementPtr();
	for (i=0; i<elems; i++, pe++)
	{
		// solid elements
		// we create a face if an element does not have a neighbor
		// or if the neighbor has a different gid. Note that we need
		// to make sure we don't double-count
		n = pe->Faces();
		for (j=0; j<n; ++j)
		{
			pe->m_face[j] = -1;
			if (pe->m_nbr[j] == -1) ++faces;
			else
			{
				FEElement* pen = m.ElementPtr(pe->m_nbr[j]);
				if (pe->m_gid < pen->m_gid) ++faces;
			}
		}
		
		// shell elements
		n = pe->Edges();
		if (n > 0) ++faces;
	}

	// allocate storage
	m.Create(0,0,faces);

	// assign the faces
	pe = m.ElementPtr();
	FEFace* pf = m.FacePtr();
	for (i=0; i<elems; i++, pe++)
	{
		// solid elements
		n = pe->Faces();
		for (j=0; j<n; j++)
		{
			int nbid = pe->m_nbr[j];
			FEElement* pen = (nbid == -1 ? 0 : m.ElementPtr(nbid));
			if (pen == 0)
			{
				*pf = pe->GetFace(j);
				pf->m_elem[0] = i;
				pf->m_elem[1] = -1;
				++pf;
			}
			else if (pe->m_gid < pen->m_gid)
			{
				*pf = pe->GetFace(j);
				pf->m_elem[0] = i;
				pf->m_elem[1] = pen->m_ntag;
				++pf;
			}
		}

		// shell elements
		n = pe->Edges();
		if (n>0)
		{
			pf->m_nodes = pe->Nodes();
			pf->n[0] = pe->m_node[0];
			pf->n[1] = pe->m_node[1];
			pf->n[2] = pe->m_node[2];
			if (pf->m_nodes > 3) pf->n[3] = pe->m_node[3]; else pf->n[3] = pf->n[2];
			pf->m_elem[0] = i;
			pf->m_elem[1] = -1;
			++pf;
		}
	}

	// update face neighbours
	m.UpdateFaces();

	// calculate auto GID's
	m.AutoSmooth(m_wsmooth);

	// update normals
	m.UpdateNormals();
}

//-----------------------------------------------------------------------------

void FEAutoMesher::BuildEdges(FEMesh &m)
{
	int i, j, n;
	int NF = m.Faces();

	FEFace* pfn;

	// let's count the edges
	int nedges = 0;
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m.Face(i);
		if (f.IsExternal())
		{
			f.m_ntag = 1;
			int nf = f.Nodes();
			n = f.Edges();
			for (j=0; j<n; ++j)
			{
				pfn = m.FacePtr(f.m_nbr[j]);
				if ((pfn == 0) || (f.m_gid < pfn->m_gid)) ++nedges;
			}
		}
	}

	// allocate edges
	m.DeleteEdges();
	m.Create(0,0,0,nedges);

	// assign edges
	nedges = 0;
	int n1, n2;
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m.Face(i);
		if (f.IsExternal())
		{
			f.m_ntag = 1;
			int nf = f.Nodes();
			if ((nf==3)||(nf==6)) n = 3;
			if ((nf==4)||(nf==8)) n = 4;
			for (j=0; j<n; ++j)
			{
				pfn = m.FacePtr(f.m_nbr[j]);
				if ((pfn == 0) || (f.m_gid < pfn->m_gid))
				{
					n1 = f.n[j];
					n2 = f.n[(j+1)%n];

					FEEdge& e = m.Edge(nedges);

					e.n[0] = n1;
					e.n[1] = n2;
					e.m_gid = -1;

					++nedges;
				}
			}
		}
	}
	assert(nedges == m.Edges());

	m.UpdateEdges();
}

//-----------------------------------------------------------------------------
void FEAutoMesher::AutoPartitionSurface(FEMesh& m)
{
	// Get the mesh and number of faces
	int NF = m.Faces();

	// face that still require processing 
	// will be placed on a stack
	// The partitioning is done when the stack is empty
	vector<FEFace*> stack(NF);
	int ns = 0;

	// reset face ID's 
	for (int i=0; i<NF; ++i) m.Face(i).m_gid = -1;

	// let's get to work
	int ngid = 0;
	for (int i=0; i<NF; ++i)
	{
		FEFace* pf = m.FacePtr(i);
		if (pf->m_gid == -1)
		{
			stack[ns++] = pf;
			while (ns > 0)
			{
				// pop a face
				pf = stack[--ns];

				// mark as processed
				pf->m_gid = ngid;

				// get the element part ID's
				assert(pf->m_elem[0] >= 0);
				FEElement* pe11 = m.ElementPtr(pf->m_elem[0]);
				FEElement* pe12 = m.ElementPtr(pf->m_elem[1]);
				int gid11 = (pe11?pe11->m_gid:-1);
				int gid12 = (pe12?pe12->m_gid:-1);

				int n = pf->Edges();
				for (int j=0; j<n; ++j)
				{
					FEFace* pf2 = m.FacePtr(pf->m_nbr[j]);
					if (pf2)
					{
						assert(pf2->m_elem[0] >= 0);
						FEElement* pe21 = m.ElementPtr(pf2->m_elem[0]);
						FEElement* pe22 = m.ElementPtr(pf2->m_elem[1]);

						int gid21 = (pe21?pe21->m_gid:-1);
						int gid22 = (pe22?pe22->m_gid:-1);

						if ((pf2->m_gid == -1) && (pf->m_sid == pf2->m_sid) && (gid11 == gid21) && (gid12 == gid22)) 
						{
							pf2->m_gid = -2;
							stack[ns++] = pf2;
						}
					}
				}
			}
			++ngid;
		}
	}
}

//-----------------------------------------------------------------------------
void FEAutoMesher::AutoPartitionEdges(FEMesh& m)
{
	// get the number of edges
	int NE = m.Edges();

	// get the edge pointer
	FEEdge* pe = m.EdgePtr();
	m.UpdateEdges();

	// intialize all group ID's
	int i, j;
	for (i=0; i<NE; ++i, ++pe) pe->m_gid = -1;

	// build a stack
	stack<FEEdge*> se;

	int ue = 0;
	int ng = 0;
	vec3d n1, n2;
	while (true)
	{
		// find an unassigned edge
		if (se.empty())
		{
			pe = 0;
			for (i=ue; i<NE; ++i, ++ue)
			{
				pe = m.EdgePtr(i);
				if (pe->m_gid == -1)
				{
					ng++;
					se.push(pe);
					break;
				}
				else pe = 0;
			}

			if (pe == 0) break;
		}
		else
		{
			pe = se.top(); se.pop();
			pe->m_gid = ng-1;

			// assign all unassigned neighbours
			for (j=0; j<2; ++j)
			{
				FEEdge* pnb = m.EdgePtr(pe->m_nbr[j]);
				if (pnb && (pnb->m_gid == -1)) 
				{
					pnb->m_gid = -2;
					se.push(pnb);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Assign gids to the nodes based on the edge gids.
void FEAutoMesher::AutoPartitionNodes(FEMesh& m)
{
	// get the mesh
	int NE = m.Edges();

	// reset node tags
	for (int i=0; i<m.Nodes(); ++i) m.Node(i).m_gid = -1;

	// get the edge pointer
	FEEdge* pe = m.EdgePtr();

	// loop over all edges
	int nn = 0;
	for (int i=0; i<NE; ++i, ++pe)
	{
		if ((pe->m_nbr[0] == -1) || (m.EdgePtr(pe->m_nbr[0])->m_gid != pe->m_gid))
		{
			FENode& node = m.Node(pe->n[0]);
			if (node.m_gid == -1) { node.m_gid = nn++; }
		}
		if ((pe->m_nbr[1] == -1) || (m.EdgePtr(pe->m_nbr[1])->m_gid != pe->m_gid))
		{
			FENode& node = m.Node(pe->n[1]);
			if (node.m_gid == -1) { node.m_gid = nn++; }
		}
	}
}

//-----------------------------------------------------------------------------
// This function is called when element ID's were reassigned (e.g. after loading
// the mesh or after the user created a new partition. This requires that we rebuild the faces
void FEAutoMesher::Repartition(FEMesh& m)
{
	BuildFaces(m);

	// repartition the surface
	AutoPartitionSurface(m);

	// rebuild the edges since the number of edges may have changed
	BuildEdges(m);

	// partition the edges
	AutoPartitionEdges(m);

	// partition the nodes
	AutoPartitionNodes(m);

	// update the mesh
	m.UpdateNodes();
}

