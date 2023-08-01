#pragma once

#include <Eigen/Core>

namespace shapeworks {

namespace particles {

//---------------------------------------------------------------------------
Eigen::VectorXd read_particles(std::string filename);

//---------------------------------------------------------------------------
void write_particles(std::string filename, const Eigen::VectorXd& points);

}  // namespace particles

}  // namespace shapeworks
