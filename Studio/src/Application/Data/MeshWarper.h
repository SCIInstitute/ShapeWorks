#pragma once

/**
 * @file MeshWarper.h
 * @brief Mesh warping capability
 *
 * The MeshWarper provides an object to warp meshs for surface reconstruction
 */

#include <vector>
#include <string>
#include <Libs/Mesh/Mesh.h>

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

  Eigen::MatrixXd vertices_;
  Eigen::MatrixXi faces_;
  Eigen::MatrixXd points_;
  Eigen::MatrixXd warp_;

  bool warp_available_ = false;

  vtkSmartPointer<vtkPolyData> reference_mesh_;
  vnl_vector<double> reference_particles_;
};

}