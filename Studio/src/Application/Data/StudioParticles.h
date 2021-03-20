#pragma once

#include <vector>
#include <vnl_vector.h>
#include <itkPoint.h>

namespace shapeworks {

//! Representation of correspondence points for a shape including multiple domains
/*!
 * The StudioParticles class encapsulates the correspondence points
 * for a shape, including multiple domains, local and global points
 *
 */
class StudioParticles {

public:
  StudioParticles();

  void set_local_particles(int domain, std::vector<itk::Point<double>> particles);
  void set_world_particles(int domain, std::vector<itk::Point<double>> particles);

private:

  void set_particles(int domain, std::vector<itk::Point<double>> particles, bool local);
  std::vector<vnl_vector<double>> local_points_; // one for each domain
  std::vector<vnl_vector<double>> global_points_; // one for each domain

};
}
