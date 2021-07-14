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

  void set_local_particles(int domain, vnl_vector<double> particles);
  void set_world_particles(int domain, vnl_vector<double> particles);

  std::vector<vnl_vector<double>> get_local_particles();
  std::vector<vnl_vector<double>> get_world_particles();

  vnl_vector<double> get_local_particles(int domain);
  vnl_vector<double> get_world_particles(int domain);

  vnl_vector<double> get_combined_local_particles();
  vnl_vector<double> get_combined_global_particles();

  std::vector<itk::Point<double>> get_local_points(int domain);
  std::vector<itk::Point<double>> get_world_points(int domain);

  //! Return which domain a particle belongs to when they are concatenated together
  int get_domain_for_combined_id(int id);

private:

  std::vector<itk::Point<double>> vnl_to_point_vector(const vnl_vector<double>& vnl);

  vnl_vector<double> combine(const std::vector<vnl_vector<double>>& vnl);

  void set_particles(int domain, std::vector<itk::Point<double>> particles, bool local);
  std::vector<vnl_vector<double>> local_particles_; // one for each domain
  std::vector<vnl_vector<double>> global_particles_; // one for each domain

};
}
