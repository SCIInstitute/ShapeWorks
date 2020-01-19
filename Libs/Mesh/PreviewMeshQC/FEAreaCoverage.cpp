#include "stdafx.h"
#include "FEAreaCoverage.h"
#include "Intersect.h"

//-----------------------------------------------------------------------------
void FEAreaCoverage::Surface::Create(FEMesh& mesh)
{
  m_mesh = &mesh;

  // if the list is empty, just use all faces
  if (m_face.empty()) {
    m_face.resize(mesh.Faces());
    for (int i = 0; i < mesh.Faces(); ++i) {
      m_face[i] = i;
    }
  }

  // this assumes that the m_face member was initialized
  int NF = (int)m_face.size();
  m_fnorm.resize(NF, vec3d(0.f, 0.f, 0.f));

  // tag all nodes that belong to this surface
  int N = mesh.Nodes();
  for (int i = 0; i < N; ++i) {
    mesh.Node(i).m_ntag = -1;
  }
  int nn = 0;
  for (int i = 0; i < Faces(); ++i) {
    FEFace& f = mesh.Face(m_face[i]);
    int nf = f.Nodes();
    for (int j = 0; j < nf; ++j) {
      FENode& node = mesh.Node(f.n[j]);
      if (node.m_ntag == -1) { node.m_ntag = nn++;}
    }
  }

  // create the global node list
  m_node.resize(nn);
  for (int i = 0; i < N; ++i) {
    FENode& node = mesh.Node(i);
    if (node.m_ntag >= 0) { m_node[node.m_ntag] = i;}
  }
  m_pos.resize(nn);

  // create the local node list
  m_lnode.resize(Faces() * 4);
  for (int i = 0; i < Faces(); ++i) {
    FEFace& f = mesh.Face(m_face[i]);
    if (f.Nodes() == 4) {
      m_lnode[4 * i] = mesh.Node(f.n[0]).m_ntag; assert(m_lnode[4 * i] >= 0);
      m_lnode[4 * i + 1] = mesh.Node(f.n[1]).m_ntag; assert(m_lnode[4 * i + 1] >= 0);
      m_lnode[4 * i + 2] = mesh.Node(f.n[2]).m_ntag; assert(m_lnode[4 * i + 2] >= 0);
      m_lnode[4 * i + 3] = mesh.Node(f.n[3]).m_ntag; assert(m_lnode[4 * i + 3] >= 0);
    }
    else if (f.Nodes() == 3) {
      m_lnode[4 * i] = mesh.Node(f.n[0]).m_ntag; assert(m_lnode[4 * i] >= 0);
      m_lnode[4 * i + 1] = mesh.Node(f.n[1]).m_ntag; assert(m_lnode[4 * i + 1] >= 0);
      m_lnode[4 * i + 2] = mesh.Node(f.n[2]).m_ntag; assert(m_lnode[4 * i + 2] >= 0);
      m_lnode[4 * i + 3] = m_lnode[4 * i + 2];
    }
    else { assert(false);}
  }

  // create the node-facet look-up table
  m_NLT.resize(Nodes());
  for (int i = 0; i < Faces(); ++i) {
    FEFace& f = mesh.Face(m_face[i]);
    int nf = f.Nodes();
    for (int j = 0; j < nf; ++j) {
      int inode = m_lnode[4 * i + j];
      m_NLT[inode].push_back(m_face[i]);
    }
  }
}

//-----------------------------------------------------------------------------
FEAreaCoverage::FEAreaCoverage()
{}

//-----------------------------------------------------------------------------
vector<double> FEAreaCoverage::Apply(FEMesh& mesh1, FEMesh& mesh2)
{
  int N1 = mesh1.Nodes();
  vector<double> val(N1, 0.0);

  // build the node lists
  m_surf1.Create(mesh1);
  m_surf2.Create(mesh2);

  // build the normal lists
  UpdateSurface(m_surf1);
  UpdateSurface(m_surf2);

  // repeat over all nodes of surface 1
  vector<float> a(m_surf1.Nodes(), 0.f);
  for (int i = 0; i < m_surf1.Nodes(); ++i) {
    int inode = m_surf1.m_node[i];
    FENode& node = mesh1.Node(inode);
    vec3d ri = node.r;
    vec3d Ni = m_surf1.m_norm[i];

    // see if it intersects the other surface
    if (intersect(ri, Ni, m_surf2)) {
      val[inode] = 1.f;
    }
  }

  return val;
}

//-----------------------------------------------------------------------------
void FEAreaCoverage::UpdateSurface(FEAreaCoverage::Surface& s)
{
  // get the mesh
  FEMesh& mesh = *s.m_mesh;
  int NF = s.Faces();
  int NN = s.Nodes();

  // update nodal positions
  for (int i = 0; i < NN; ++i) {
    s.m_pos[i] = mesh.Node(s.m_node[i]).r;
  }

  // update face normals
  s.m_fnorm.assign(NF, vec3d(0.f, 0.f, 0.f));
  s.m_norm.assign(NN, vec3d(0.f, 0.f, 0.f));
  vec3d r[3];
  for (int i = 0; i < NF; ++i) {
    FEFace& f = mesh.Face(s.m_face[i]);

    r[0] = s.m_pos[s.m_lnode[i * 4    ]];
    r[1] = s.m_pos[s.m_lnode[i * 4 + 1]];
    r[2] = s.m_pos[s.m_lnode[i * 4 + 2]];

    vec3d N = (r[1] - r[0]) ^ (r[2] - r[0]);

    s.m_fnorm[i] = N;
    s.m_fnorm[i].Normalize();

    int nf = f.Nodes();
    for (int j = 0; j < nf; ++j) {
      assert(j < 4);
      int n = s.m_lnode[4 * i + j]; assert(n >= 0);
      s.m_norm[n] += N;
    }
  }
  for (int i = 0; i < (int)s.m_norm.size(); ++i) {
    s.m_norm[i].Normalize();
  }
}

//-----------------------------------------------------------------------------
bool FEAreaCoverage::intersect(const vec3d& r, const vec3d& N, FEAreaCoverage::Surface& surf)
{
  // create the ray
  Ray ray = {r, N};

  // loop over all facets connected to this node
  Intersection q;
  for (int i = 0; i < (int)surf.m_face.size(); ++i) {
    // see if the ray intersects this face
    if (faceIntersect(surf, ray, i)) {
      return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------
bool FEAreaCoverage::faceIntersect(FEAreaCoverage::Surface& surf, const Ray& ray, int nface)
{
  Intersection q;
  q.m_index = -1;
  FEMesh& mesh = *surf.m_mesh;

  vec3d rn[4];
  FEFace& face = mesh.Face(surf.m_face[nface]);

  bool bfound = false;
  switch (face.m_nodes) {
  case 3:
  {
    for (int i = 0; i < 3; ++i) {
      rn[i] = surf.m_pos[surf.m_lnode[4 * nface + i]];
    }

    Triangle tri = { rn[0], rn[1], rn[2], surf.m_fnorm[nface] };
    bfound = IntersectTriangle(ray, tri, q, false);
  }
  break;
  case 4:
  {
    for (int i = 0; i < 4; ++i) {
      rn[i] = surf.m_pos[surf.m_lnode[4 * nface + i]];
    }

    Quad quad = { rn[0], rn[1], rn[2], rn[3] };
    bfound = FastIntersectQuad(ray, quad, q);
  }
  break;
  }

  if (bfound) {
    // make sure the projection is in the direction of the ray
    bfound = (ray.direction * (q.point - ray.origin) > 0.f);
  }

  return bfound;
}
