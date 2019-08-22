#pragma once
#include "math3d.h"
#include "FEMesh.h"
#include <list>
using namespace std;

//-----------------------------------------------------------------------------
//! This class implements a modifier that coarses a plygonal mesh using an
//! approximated centroidal voronoi diagram.
class FECVDDecimationModifier
{
private:
	class Cluster
	{
	public:
		Cluster() : m_sgamma(vec3d(0,0,0)), m_srho(0.0) {}

		int faces() const { return (int) m_fid.size(); }

	public:
		vec3d	m_sgamma;	// sum of "gamma's"
		double	m_srho;		// sum of "rho's"
		vector<int> m_fid; //face ids of the faces in this cluster
		
	};

	struct EDGE
	{
		int face[2];	// the indices of the two faces sharing this edge
		int node[2];	// the nodes of the edge
	};

	// a node which contains a list of clusters it belongs to
	class NODE
	{
	public:
		enum {MAX_CLUSTERS=23};
	public:
		NODE() : nc(0){}

		bool AttachToCluster(int n);
	public:
		int	c[MAX_CLUSTERS];	// cluster ID's
		int	nc;					// nr of clusters
	};

public:
	//! Constructor
	FECVDDecimationModifier();

	//! Apply the decimation modifier
	FEMesh* Apply(FEMesh* pm);

private:
	//! Initialize data structures
	bool Initialize(FEMesh* pm);

	//! Minimize the energy
	bool Minimize(FEMesh* pm);

	//! Triangulate
	FEMesh* Triangulate(FEMesh* pm);
	FEMesh* Triangulate2(FEMesh* pm);	// uses hole-filling algorithm

	FEMesh* CalculateCVD(FEMesh* pm);
	void InterpolateShellThickness(FEMesh* pm); // to interpolate shell thickness

	//! swap triangles
	bool Swap(FEFace& face, int nface, int ncluster);

public:
	double	m_pct;	//!< percentage of target number of clusters/vertices
	bool	m_bcvd;
	double  m_sel_pct; // percentage of clusters in selected region
	double	m_gradient; //gradiant for curvature

private:
	vector<Cluster>	m_Cluster;
	vector<int>		m_tag;
	vector<double>	m_rho;		// rho for all triangles
	vector<vec3d>	m_gamma;	// centroids of all triangles
	list<EDGE>		m_Edge;		// edge list
	vector<double> m_st;//each node shell thickness
	//vector<int> hashmap;		//hashmap for selected faces
};
