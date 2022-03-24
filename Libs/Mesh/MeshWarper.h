#pragma once

/**
 * @file MeshWarper.h
 * @brief Mesh warping capability
 *
 * The MeshWarper provides an object to warp meshes for surface reconstruction
 */

#include <vector>
#include <vtkPolyData.h>
#include <Eigen/Eigen>

namespace shapeworks {

/**
 * \class MeshWarper
 * \ingroup Group-Mesh
 *
 * This class implements mesh warping based on correspondence particles
 *
 */
class MeshWarper {

public:

  //! Set the reference mesh and particles
  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                          const Eigen::MatrixXd& reference_particles);

  //! Generate warp, return true on success
  bool generate_warp();

  //! Return if the warp is available
  bool get_warp_available();

  //! Build a mesh for a given set of particles
  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::MatrixXd& particles);

  //! Return if set as a contour
  bool is_contour() { return this->is_contour_; }

  //! Return true if warping has removed any bad particle(s)
  bool has_bad_particles() const { return this->bad_particle_count() > 0; }

  vtkSmartPointer<vtkPolyData> get_reference_mesh() { return this->reference_mesh_; }
  const Eigen::MatrixXd& get_reference_particles() const { return this->reference_particles_; }
  const Eigen::MatrixXd& get_warp_matrix() const { return this->warp_; }

protected:

  //! For overriding to handle progress updates
  virtual void update_progress(float p) {}

private:

  //! Check if the warp is ready, if not do it (thread safely), return true if warp is valid
  bool check_warp_ready();

  //! Add particles as vertices to reference mesh
  void add_particle_vertices();

  //! Remove the bad particles from a set of particles
  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  //! Split a cell on the edge
  void split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                          std::vector<vtkSmartPointer<vtkIdList>>& new_triangles);

  //! Identify the good particles
  void find_good_particles();

  //! Prep incoming mesh
  static vtkSmartPointer<vtkPolyData> prep_mesh(vtkSmartPointer<vtkPolyData> mesh);

  //! Clean mesh (remove deleted)
  static vtkSmartPointer<vtkPolyData> clean_mesh(vtkSmartPointer<vtkPolyData> mesh);

  //! Recreate mesh, dropping deleted cells
  vtkSmartPointer<vtkPolyData> recreate_mesh(vtkSmartPointer<vtkPolyData> mesh);

  //! Generate the warp matrix
  bool generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF,
                            const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W);

  //! Generate a polydata from a set of points (e.g. warp the reference mesh)
  vtkSmartPointer<vtkPolyData> warp_mesh(const Eigen::MatrixXd& points);

  //! Return the number of bad particles
  size_t bad_particle_count() const { return size_t(reference_particles_.rows()) - good_particles_.size(); }

  // Members
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
  //! Reference particles (matrix [Nx3])
  Eigen::MatrixXd reference_particles_;
  //! Whether the reference is a contour
  bool is_contour_ = false;
};
}
