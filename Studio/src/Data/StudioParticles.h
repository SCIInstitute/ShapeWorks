#pragma once

#include <vector>
#include <vnl_vector.h>
#include <vtkSmartPointer.h>
#include <itkPoint.h>
#include <cassert>

#include "Shapeworks.h"

class vtkTransform;

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

  void set_local_particles(int domain, Eigen::VectorXd particles);
  void set_world_particles(int domain, Eigen::VectorXd particles);

  std::vector<Eigen::VectorXd> get_local_particles();
  std::vector<Eigen::VectorXd> get_world_particles();

  Eigen::VectorXd get_local_particles(int domain);
  Eigen::VectorXd get_world_particles(int domain);
  //! Get untransformed original world particles from optimizer
  Eigen::VectorXd get_raw_world_particles(int domain);

  Eigen::VectorXd get_combined_local_particles();
  Eigen::VectorXd get_combined_global_particles();

  std::vector<itk::Point<double>> get_local_points(int domain);
  std::vector<itk::Point<double>> get_world_points(int domain);

  //! Return which domain a particle belongs to when they are concatenated together
  int get_domain_for_combined_id(int id);

  void set_transform(vtkSmartPointer<vtkTransform> transform);
  void set_procrustes_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms);

private:

  void transform_global_particles();

  std::vector<itk::Point<double>> vnl_to_point_vector(const Eigen::VectorXd& vnl);

  Eigen::VectorXd combine(const std::vector<Eigen::VectorXd>& vnl);

  void set_particles(int domain, std::vector<itk::Point<double>> particles, bool local);
  std::vector<Eigen::VectorXd> local_particles_; // one for each domain
  std::vector<Eigen::VectorXd> global_particles_; // one for each domain
  std::vector<Eigen::VectorXd> transformed_global_particles_; // one for each domain

  vtkSmartPointer<vtkTransform> transform_;
  std::vector<vtkSmartPointer<vtkTransform>> procrustes_transforms_;
};
}
