#pragma once

#include <Mesh.h>

namespace shapeworks::mesh {

//! Compute the geodesic distance from each vertex to the landmarks and store as a field
void compute_landmark_geodesics(Mesh &mesh, const std::vector<Point3>& landmarks);

}  // namespace shapeworks::mesh
