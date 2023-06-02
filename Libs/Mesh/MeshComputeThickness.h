#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

void compute_thickness(Mesh& mesh, Image &image, Image *dt, double threshold, double min_dist, double max_dist);

}  // namespace shapeworks::mesh
