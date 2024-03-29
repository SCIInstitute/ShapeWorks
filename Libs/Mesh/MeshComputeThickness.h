#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

//! Compute the cortical thickness of a mesh and image (e.g. CT)
void compute_thickness(Mesh &mesh, Image &image, Image *dt, double max_dist, double median_radius,
                       std::string distance_mesh);

//! Compute an internal mesh based on the thickness values
Mesh compute_inner_mesh(const Mesh &mesh, std::string array_name);

//! Summarize the internal intensities values of the area inside the inner mesh on the outer mesh
void summarize_internal_intensities(Mesh &outer_mesh, Mesh &inner_mesh, Image &image);

}  // namespace shapeworks::mesh
