#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

//! Compute the cortical thickness of a mesh and image (e.g. CT)
void compute_thickness(Mesh &mesh, Image &image, Image *dt, double max_dist, double median_radius,
                       std::string distance_mesh);

Mesh compute_inner_mesh(const Mesh &mesh, std::string array_name);

}  // namespace shapeworks::mesh
