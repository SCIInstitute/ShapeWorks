#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

//! Compute the cortical thickness of a mesh and image (e.g. CT)
void compute_thickness(Mesh &mesh, Image &image, Image *dt, double max_dist, std::string distance_mesh);

}  // namespace shapeworks::mesh
