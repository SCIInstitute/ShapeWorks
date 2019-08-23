#pragma once
#include "FEMesh.h"

//-----------------------------------------------------------------------------
// This class implements a modifier that fills a hole in a mesh. 
class FEFillHole
{
public:
	// helper class for defining a closed loop of edges
	class EdgeRing
	{
	public:
		EdgeRing(){ m_winding = 1; }
		EdgeRing(const EdgeRing& ring) { m_node = ring.m_node; m_r = ring.m_r; m_winding = ring.m_winding; m_normal = ring.m_normal; }
		void operator = (const EdgeRing& ring) { m_node = ring.m_node; m_r = ring.m_r; m_winding = ring.m_winding;  m_normal = ring.m_normal; }

		bool empty() { return m_node.empty(); }

		void add(int n, const vec3d& r, const vec3d& nn) { m_node.push_back(n); m_r.push_back(r); m_normal.push_back(nn); }

		void clear() { m_node.clear(); m_r.clear(); m_normal.clear(); }

		int size() { return (int)m_node.size(); }

		int operator [] (int i) { return m_node[i]; }

		void GetLeftEar(int n0, int n1, EdgeRing& ear);
		void GetRightEar(int n0, int n1, EdgeRing & ear);

	public:
		vector<vec3d>	m_r;		// nodal positions
		vector<int>		m_node;		// sorted list of nodes defining the closed loop
		int				m_winding;	// +1 or -1 depending on the winding
		vector<vec3d>	m_normal;	//node normals
	};

	// helper class for representing a new face
	struct FACE
	{
		vec3d	r[3];	// nodal positions
		int		n[3];	// the three nodes of the face
	};

public:
	FEFillHole(){}
	FEMesh* Apply(FEMesh* pm);

	bool optimize;
	bool insertNodes;
	//AFM
	bool AFM(FEMesh& mesh, EdgeRing& ring, vector<FACE>& tri_list, vector<vec3d> &node_list);
	//Addd node
	vec3d newNode(vec3d current_node, vec3d next_node, vec3d prev_node, vec3d node_normal, double scale, bool concave);
	// divide a ring
	bool DivideRing(EdgeRing& ring, vector<FACE>& tri_list);

	// divide a ring
	bool DivideRing1(EdgeRing& ring, vector<FACE>& tri_list);

	// fill all holes
	void FillAllHoles(FEMesh* pm);


private:
	// Find the ring based on a node (TODO: should I move this to the FEMesh class?)
	bool FindEdgeRing(FEMesh& mesh, int node, EdgeRing& ring);

	// Find the approximate normal of a ring
	vec3d RingNormal(EdgeRing& ring);

	// check if the split is valid
	bool IsValidSplit(EdgeRing& left, EdgeRing& right, const vec3d& p, const vec3d& t);

	// get location sign (+1 on positive side, -1 on negative side, 0 = on both sides)
	int GetPlaneOrientation(EdgeRing& ring, const vec3d& p, const vec3d& t);

	// get the area of the smallest triangle
	double min_tri_area(vector<FACE>& tri);

	// get the quality of the worst triangle
	double min_tri_quality(vector<FACE>& tri);

	// get the triangle quality
	double tri_quality(vec3d r[3]);
};
