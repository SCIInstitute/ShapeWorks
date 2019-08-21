// FEMesh.h: interface for the FEMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEMESH_H__CB7EC714_E3FD_46A4_A397_FEFB23429520__INCLUDED_)
#define AFX_FEMESH_H__CB7EC714_E3FD_46A4_A397_FEFB23429520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FECoreMesh.h"
#include <vector>
#include <set>
using namespace std;

//-----------------------------------------------------------------------------
class FEMesh;

//-----------------------------------------------------------------------------
// Element data field
class FEElementData
{
public:
	FEElementData();
	FEElementData(const FEElementData& d);
	FEElementData& operator = (const FEElementData& d);

	// create a data field
	void Create(FEMesh* pm, double v = 0.0);

	// size of data field
	int Size() { return (int) m_data.size(); }

	// get/set
	double get(int i) { return m_data[i]; }
	void set(int i, double v) { m_data[i] = v; }

	// access operator
	double& operator [] (int i) { return m_data[i]; }

	// get/set name
	void SetName(const char* sz);
	const char* GetName() { return m_szname; }

	void FillRandomBox(double fmin, double fmax);

private:
	char			m_szname[256];	//!< name of data field
	FEMesh*			m_pMesh;		//!< parent mesh
	vector<double>	m_data;			//!< data values
};

//-----------------------------------------------------------------------------
// This class describes a finite element mesh. Every FEMesh must be owned by a
// GObject class. 
class FEMesh : public FECoreMesh
{
public:
	// --- C O N S T R U C T I O N ---
	FEMesh();
	FEMesh(FEMesh& m);
	virtual ~FEMesh();

	// allocate space for mesh
	void Create(int nodes, int elems, int faces = 0, int edges = 0);

	// copy part of the mesh
	void ShallowCopy(FEMesh* pm);

	/// --- C L E A N U P ---
	void Clear();
	void DeleteEdges() { if (!m_Edge.empty()) m_Edge.clear(); }

public: // --- E L E M E N T   A C C E S S ---

	//! return number of elements
	int Elements() const { return m_Elem.size(); }

	//! return element
	FEElement& Element(int n) { return m_Elem[n]; }

	//! return reference to element
	FEElement_& ElementRef(int n) { return m_Elem[n]; }

	//! return pointer to element
	FEElement* ElementPtr(int n=0) { return ((n>=0) && (n<(int)m_Elem.size())? &m_Elem[n] : 0); }

public:
	// --- S U B M E S H ---
	int FindFace(FEElement* pe, FEFace& f, FEFace& fe);

	void FindNodesFromPart(int gid, vector<int>& node);

	//! Get the node array
	//! \todo Maybe I should delete these
	std::vector<FENode>&    NodeArray   () { return m_Node; }
	std::vector<FEFace>&    FaceArray   () { return m_Face; }
	std::vector<FEElement>& ElementArray() { return m_Elem; }

	// --- E V A L U A T I O N ---

	// get the current element value
	double GetElementValue(int n) { return m_data[n]; }

	// set the element value
	void SetElementValue(int n, double v) { m_data[n] = v; }

	// update the range of values
	void UpdateValueRange();

	// get the value range
	void GetValueRange(double& vmin, double& vmax);

	vec3d ProjectToSurface(vec3d r, vec3d t);
	vec3d ProjectToFace(vec3d p, FEFace& f, double& r, double& s);
	vec3d ProjectToEdge(vec3d e1, vec3d e2, vec3d p, double& r);

	bool FindIntersection(FEFace& f, vec3d x, vec3d n, vec3d& q, double& g);

	// --- U P D A T E ---
	void Update();
	void UpdateElementNeighbors();
	void UpdateFaces();
	void UpdateEdges();
	void UpdateNodes();
	void AutoSmooth(double w);
	void UpdateNormals();
	void PartitionSelection();

	// --- O P E R A T I O N S ---
	void RemoveIsolatedNodes();

	void AddNode(FENode& n) { m_Node.push_back(n); }

	// detach the selected elements and create a new mesh
	FEMesh* DetachSelectedMesh();

	// detach selected elements and create a new part
	void DetachSelectedPart();

	FEMesh* ExtractSelectedFaces();

	void DeleteTaggedElements(int tag);
	void DeleteTaggedFaces   (int tag);
	void DeleteTaggedEdges   (int tag);

	void FindDuplicateFaces(vector<int>& l);
	void FindDuplicateEdges(vector<int>& l);

	void DeleteSelectedElements();
	void DeleteSelectedFaces();
	void DeleteSelectedNodes();

	void InvertTaggedElements(int ntag);
	void InvertSelectedElements();

	double ShellJacobian(FEElement& el);

	// Remove duplicated elements
	void RemoveDuplicateElements();
	//Fix inverted elements
	void FixinvertedElements();
	//Fix the reference surface
	void FixReferenceSurface();
	//Interpolate shell thickness for elements whose thickness is unknown
	void InterpolateShellThickness(double);
	// Remove non-manifold elements
	void RemoveNonManifoldElements();

	// fix element winding
	void FixElementWinding();
	void FixElementWinding2();

	// tag all elements
	void TagAllElements(int ntag);

public:
	int DataFields() { return (int) m_map.size(); }
	FEElementData* AddDataField(const char* szname, double v = 0.0);
	FEElementData& GetDataField(int i) { return m_map[i]; }

public:
	double ShortestEdge();

protected:
	bool IntersectTri (vec3d* y, vec3d x, vec3d n, vec3d& q, double& g);
	bool IntersectQuad(vec3d* y, vec3d x, vec3d n, vec3d& q, double& g);

public:
	void BuildNodeElementTable(vector< vector<int> >& NET);
	void BuildNodeFaceTable(vector< vector<int> >& NFT);
	void BuildNodeEdgeTable(vector< vector<int> >& NET);
	void BuildEdgeTable(vector< pair<int, int> >& ET);
	void BuildNodeNodeTable(vector< set<int> >& NNT);
	void BuildSurfaceNodeNodeTable(vector< set<int> >& NNT);
	void BuildElementEdgeTable(vector< vector<int> >& EET, vector< pair<int, int> >& ET);
	void BuildFaceTable(vector<FEFace>& FT);
	void BuildElementFaceTable(vector< vector<int> >& EFT, vector<FEFace>& FT);
	void BuildFaceEdgeTable(vector< vector<int> >& FET, vector< pair<int, int> >& ET);
	void BuildFaceFaceTable(vector<int>& FFT, vector<FEFace>& FT);
	void BuildEdgeEdgeTable(vector<int>& EET, vector< pair<int,int> >& ET);

protected:
	// element data
	std::vector<FEElement>	m_Elem;	//!< FE elements
	std::vector<double>		m_data;	//!< element values
	double	m_min, m_max;		//!< value range of element data

	// data fields (currently used as material maps)
	vector<FEElementData>	m_map;
};

extern int FTHEX8[6][4];
extern int FTHEX20[6][8];
extern int FTHEX27[6][9];
extern int FTPENTA[5][4];
extern int FTTET[4][3];
extern int FTTET10[4][6];
extern int FTTET15[4][7];

double bias(double b, double x);
double gain(double g, double x);

double area_triangle(vec3d r[3]);

#endif // !defined(AFX_FEMESH_H__CB7EC714_E3FD_46A4_A397_FEFB23429520__INCLUDED_)
