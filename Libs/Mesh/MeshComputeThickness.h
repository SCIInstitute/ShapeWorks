#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

void compute_thickness(Mesh& mesh, Image& image, Image& dt, double threshold);

}  // namespace shapeworks::mesh
