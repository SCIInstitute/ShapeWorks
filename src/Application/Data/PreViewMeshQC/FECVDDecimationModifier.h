#pragma once
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
		Cluster() : m_sgamma(vec3d(0,0,0)), m_srho(0.0), m_nf(0) {}

	public:
		vec3d	m_sgamma;	// sum of "gamma's"
		double	m_srho;		// sum of "rho's"
		int		m_nf;		// number of faces contributing to this cluster
		vector<int> m_fid; //face ids of the faces in this cluster
		
	};

	struct EDGE
	{
		int n[2];	// the indices of the two faces sharing this edge
		int m[2];	// the nodes of the edge
	};

	// a node which contains a list of clusters it belongs to
	class NODE
	{
	public:
		enum {MAX_CLUSTERS=10};
	public:
		NODE() : nc(0){}

		void AttachToCluster(int n);
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
	void Initialize();

	//! Minimize the energy
	bool Minimize();

	//! Triangulate
	FEMesh* Triangulate();
	FEMesh* Triangulate2();	// uses hole-filling algorithm

	FEMesh* CalculateCVD();
	FEMesh* InterpolateShellThickness(); // to interpolate shell thickness

	//! swap triangles
	void Swap(int nface, int ncluster);

public:
	double	m_pct;	//!< percentage of target number of clusters/vertices
	bool	m_bcvd;
	double  m_sel_pct; // percentage of clusters in selected region
	double	m_gradient; //gradiant for curvature

private:
	FEMesh*			m_pm;
	vector<Cluster>	m_Cluster;
	vector<int>		m_tag;
	vector<double>	m_rho;		// rho for all triangles
	vector<vec3d>	m_gamma;	// centroids of all triangles
	list<EDGE>		m_Edge;		// edge list
	vector<double>	m_curvature1;
	vector<double>	m_curvature2;
	vector<double> m_st;//each node shell thickness
	//vector<int> hashmap;		//hashmap for selected faces
};
