
#include "CGALGeodesicDistance.h"


namespace shapeworks
{
  double CGALGeodesicDistance::ComputeDistance(int facea, int faceb, trimesh::vec barya, trimesh::vec baryb) {
    face_iterator face_iterator_a = faces(mesh).first;
    std::advance(face_iterator_a, facea);
    Traits::Barycentric_coordinates face_location_a = {{barya[0], barya[1], barya[2]}};

    face_iterator face_iterator_b = faces(mesh).first;
    std::advance(face_iterator_b, faceb);
    Traits::Barycentric_coordinates face_location_b = {{baryb[0], baryb[1], baryb[2]}};

    shortest_paths->remove_all_source_points();
    shortest_paths->add_source_point(*face_iterator_a, face_location_a);

    double distance = shortest_paths->shortest_distance_to_source_points(*face_iterator_b, face_location_b).first;
    return distance;
  }

  CGALGeodesicDistance::CGALGeodesicDistance(trimesh::TriMesh *trimeshmesh) {
    ConvertTriMeshToCGALMesh(trimeshmesh);
    shortest_paths = new Surface_mesh_shortest_path(mesh);
  }

  void CGALGeodesicDistance::ConvertTriMeshToCGALMesh(trimesh::TriMesh *trimeshmesh) {
    std::vector< vertex_descriptor> vdescriptors;
    for (int i = 0; i < trimeshmesh->vertices.size(); i++) {
      trimesh::point pt = trimeshmesh->vertices[i];
      vertex_descriptor vt = mesh.add_vertex(K::Point_3(pt[0], pt[1], pt[2]));
      vdescriptors.push_back(vt);
    }
    CGALMesh::Property_map<vertex_descriptor, K::Point_3> locations = mesh.points();
    for (int i = 0; i < trimeshmesh->faces.size(); i++) {
      trimesh::TriMesh::Face face = trimeshmesh->faces[i];
      face_descriptor f = mesh.add_face(vdescriptors[face[0]], vdescriptors[face[1]], vdescriptors[face[2]]);
    }
  }
}
