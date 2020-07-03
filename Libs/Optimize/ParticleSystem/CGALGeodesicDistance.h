#pragma once

#include "TriMesh.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_shortest_path.h>

namespace shapeworks
{
  typedef CGAL::Simple_cartesian<double> K;
  typedef CGAL::Surface_mesh<K::Point_3> CGALMesh;
  typedef CGAL::Surface_mesh_shortest_path_traits<K, CGALMesh> Traits;
  typedef CGAL::Surface_mesh_shortest_path<Traits> Surface_mesh_shortest_path;


  typedef CGALMesh::Vertex_index vertex_descriptor;
  typedef CGALMesh::Face_index face_descriptor;
  typedef boost::graph_traits<CGALMesh> Graph_traits;
  typedef Graph_traits::vertex_iterator vertex_iterator;
  typedef Graph_traits::face_iterator face_iterator;

  class CGALGeodesicDistance
  {
  public:

    double ComputeDistance(int facea, int faceb, trimesh::vec barya, trimesh::vec baryb);

    CGALGeodesicDistance(trimesh::TriMesh *trimeshmesh);
    ~CGALGeodesicDistance() {}

  private:

    void ConvertTriMeshToCGALMesh(trimesh::TriMesh *trimeshmesh);

    CGALMesh mesh;
    Surface_mesh_shortest_path *shortest_paths;

  };

}
