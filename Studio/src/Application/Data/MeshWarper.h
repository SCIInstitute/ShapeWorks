#pragma once

/**
 * @file MeshWarper.h
 * @brief Mesh warping capability
 *
 * The MeshWarper provides an object to warp meshes for surface reconstruction
 */

#include <vector>
#include <string>
#include <Libs/Mesh/Mesh.h>
#include <QMutex>

namespace shapeworks {

class MeshWarper {

public:

  MeshWarper();

  //! Set the reference mesh and particles
  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                          const vnl_vector<double>& reference_particles);

  //! Return if the warp is available
  bool get_warp_available();

  //! Build a mesh for a given set of particles
  vtkSmartPointer<vtkPolyData> build_mesh(const vnl_vector<double>& particles);

private:

  //! Add particles as vertices to reference mesh
  void add_particle_vertices();

  void find_good_particles();

  bool check_warp_ready();

  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  Eigen::MatrixXd vertices_;
  Eigen::MatrixXi faces_;
  Eigen::MatrixXd points_;
  Eigen::MatrixXd warp_;

  std::vector<int> good_particles_;

  bool needs_warp_ = true;

  bool warp_available_ = false;

  vtkSmartPointer<vtkPolyData> incoming_reference_mesh_;
  vtkSmartPointer<vtkPolyData> reference_mesh_;
  vnl_vector<double> reference_particles_;

  // for concurrent access
  QMutex mutex_;
};

}