#pragma once
#include "FEMesh.h"
#include "Intersect.h"
#include <vector>
#include <string>
using namespace std;

class FEModel;

class FEAreaCoverage
{
  class Surface
  {
public:
    Surface() : m_mesh(nullptr) {}
    int Faces() { return (int)m_face.size(); }

    void Create(FEMesh& m);

    int Nodes() { return (int)m_node.size(); }

public:
    FEMesh*       m_mesh;
    vector<int>   m_face;                     // face list
    vector<int>   m_node;                     // node list
    vector<vec3d> m_pos;                      // node positions
    vector<int>   m_lnode;                    // local node list
    vector<vec3d> m_norm;                     // node normals
    vector<vec3d> m_fnorm;                    // face normals

    vector<vector<int>> m_NLT;                // node-facet look-up table
  };

public:
  FEAreaCoverage();

  // assign selections
  void SetSelection1(vector<int>& s) { m_surf1.m_face = s; }
  void SetSelection2(vector<int>& s) { m_surf2.m_face = s; }

  // apply the map
  // returns one value per node
  vector<double> Apply(FEMesh& mesh1, FEMesh& mesh2);

  // get/set back intersection flag
	void AllowBackIntersection(bool b);
	bool AllowBackIntersection() const;

  // set/get the angle threshold (w between 0 and 1)
	void SetAngleThreshold(double w);
	double GetAngleThreshold() const;

  // set/get back-intersection search radius (0 ignores the search radius)
	void SetBackSearchRadius(double R);
	double GetBackSearchRadius() const;

protected:
  // build node normal list
  void UpdateSurface(FEAreaCoverage::Surface& s);

  // see if a ray intersects with a surface
  bool intersect(const vec3d& r, const vec3d& N, FEAreaCoverage::Surface& surf, Intersection& q);
  bool faceIntersect(FEAreaCoverage::Surface& surf, const Ray& ray, int nface, Intersection& q);

protected:
  Surface m_surf1;
  Surface m_surf2;

	bool		  m_ballowBackIntersections;	// include back intersections
  double		m_angleThreshold;			      // angular threshold (between 0 and 1)
	double		m_backSearchRadius;			    // search radius for back intersections (set to 0 to ignore)
};
