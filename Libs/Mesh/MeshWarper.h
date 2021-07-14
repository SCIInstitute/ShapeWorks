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
#include <boost/filesystem.hpp>

namespace shapeworks {

class MeshWarper {

public:

  //! Set the reference mesh and particles
  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                          const Eigen::MatrixXd& reference_particles);

  //! Return if the warp is available
  bool get_warp_available();

  //! Build a mesh for a given set of particles
  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::MatrixXd& particles);

protected:

  //! Generate warp, return true on success
  virtual bool generate_warp();

  virtual void update_progress(float p) {};

private:

  //! Add particles as vertices to reference mesh
  void add_particle_vertices();

  //! Remove the bad particles from a set of particles
  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  //! Split a cell on the edge
  void split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1);

  //! Identify the good particles
  void find_good_particles();

  //! Check if the warp is ready, return true if warp is valid
  bool check_warp_ready();

  //! Clean incoming mesh
  static vtkSmartPointer<vtkPolyData> clean_mesh(vtkSmartPointer<vtkPolyData> mesh);

  bool generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF,
                            const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W);

  vtkSmartPointer<vtkPolyData> warp_mesh(const Eigen::MatrixXd& points);

  Eigen::MatrixXi faces_;
  Eigen::MatrixXd vertices_;
  Eigen::MatrixXd warp_;

  std::vector<int> good_particles_;

  bool needs_warp_ = true;

  bool warp_available_ = false;

  //! Reference mesh as it was given to us
  vtkSmartPointer<vtkPolyData> incoming_reference_mesh_;
  //! Processed reference mesh
  vtkSmartPointer<vtkPolyData> reference_mesh_;
  //! Reference particles
  Eigen::MatrixXd reference_particles_;


};

}