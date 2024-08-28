#pragma once

#include <Mesh.h>

namespace shapeworks::mesh {

void project_particles(Mesh &inner_mesh, Mesh &outer_mesh, Eigen::MatrixXd &particles);

}  // namespace shapeworks::mesh
