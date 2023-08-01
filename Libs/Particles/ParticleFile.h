#pragma once

#include <itkPoint.h>

#include <Eigen/Core>

namespace shapeworks {

namespace particles {

//---------------------------------------------------------------------------
Eigen::VectorXd read_particles(std::string filename);

//---------------------------------------------------------------------------
std::vector<itk::Point<double, 3>> read_particles_as_vector(std::string filename);

//---------------------------------------------------------------------------
void write_particles(std::string filename, const Eigen::VectorXd& points);

//---------------------------------------------------------------------------
void write_particles_from_vector(std::string filename, std::vector<itk::Point<double, 3>> points);

}  // namespace particles

}  // namespace shapeworks
