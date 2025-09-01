#pragma once

/**
 * @file MeshWarper.h
 * @brief Mesh warping capability
 *
 * The MeshWarper provides an object to warp meshes for surface reconstruction
 */

#include <vtkPolyData.h>

#include <Eigen/Eigen>
#include <vector>

namespace shapeworks {

/**
 * \class MeshWarper
 * \ingroup Group-Mesh
 *
 * This class implements mesh warping based on correspondence particles.
 * Correspondence points are embedded into the mesh as new vertices (traingles split).  Then a biharmonic deformation
 * is used to warp the mesh to new sets of correspondence particles.
 *
 * It can optionally be used to warp landmarks along with the mesh by embedding them as vertices
 *
 */
class MeshWarper {
 public:
  //! Set the reference mesh and particles
  void set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh, const Eigen::MatrixXd& reference_particles,
                          const Eigen::MatrixXd& landmarks = {});

  //! Generate warp, return true on success
  bool generate_warp();

  //! Return if the warp is available
  bool get_warp_available();

  bool is_contour() const { return is_contour_; }

  //! Build a mesh for a given set of particles
  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::MatrixXd& particles);

  //! Return the landmarks (matrix [Nx3]) from the warped builded mesh
  Eigen::MatrixXd extract_landmarks(vtkSmartPointer<vtkPolyData> warped_mesh);

  //! Return if set as a contour
  bool is_contour() { return this->is_contour_; }

  //! Return the map of landmarks id (Key) to vertice index (Value)
  std::map<int, int> get_landmarks_map() const { return landmarks_map_; }

  //! Return the indexes of good particles (those that really control the warping)
  std::vector<int> get_good_particle_indices() const { return good_particles_; }

  //! Return the warp matrix
  const Eigen::MatrixXd& get_warp_matrix() const { return this->warp_; }

  //! Return true if warping has removed any bad particle(s)
  bool has_bad_particles() const { return this->bad_particle_count() > 0; }

  //! Return the reference mesh which has been cleaned and vertices added
  vtkSmartPointer<vtkPolyData> get_reference_mesh() { return this->reference_mesh_; }

  //! Return the reference particles
  const Eigen::MatrixXd& get_reference_particles() const { return this->reference_particles_; }

  //! Prep incoming mesh
  static vtkSmartPointer<vtkPolyData> prep_mesh(vtkSmartPointer<vtkPolyData> mesh);

 protected:
  //! For overriding to handle progress updates
  virtual void update_progress(float p) {}

 private:
  //! Check if the warp is ready, if not do it (thread safely), return true if warp is valid
  bool check_warp_ready();

  //! Add particles as vertices to reference mesh
  void add_particle_vertices(Eigen::MatrixXd& vertices);

  //! Remove the bad particles from a set of particles
  Eigen::MatrixXd remove_bad_particles(const Eigen::MatrixXd& particles);

  //! Split a cell on the edge
  void split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                          std::vector<vtkSmartPointer<vtkIdList>>& new_triangles);

  //! Identify the good particles
  void find_good_particles();

  //! Construct the map from landmarks to vertex ids
  bool find_landmarks_vertices_on_ref_mesh();

  //! Clean mesh (remove deleted)
  static vtkSmartPointer<vtkPolyData> clean_mesh(vtkSmartPointer<vtkPolyData> mesh);

  //! Remove zero area triangles from a mesh
  static vtkSmartPointer<vtkPolyData> remove_zero_area_triangles(vtkSmartPointer<vtkPolyData> mesh);

  //! Recreate mesh, dropping deleted cells
  static vtkSmartPointer<vtkPolyData> recreate_mesh(vtkSmartPointer<vtkPolyData> mesh);

  //! Generate the warp matrix
  bool generate_warp_matrix(Eigen::MatrixXd target_vertices, Eigen::MatrixXi target_faces,
                            const Eigen::MatrixXd& references_vertices, Eigen::MatrixXd& warp);

  //! Generate a polydata from a set of points (e.g. warp the reference mesh)
  vtkSmartPointer<vtkPolyData> warp_mesh(const Eigen::MatrixXd& points);

  //! Return the number of bad particles
  size_t bad_particle_count() const { return size_t(reference_particles_.rows()) - good_particles_.size(); }

  void diagnose_biharmonic_failure(const Eigen::MatrixXd& TV, const Eigen::MatrixXi& TF,
                                   const std::vector<std::vector<int>>& S, int k);


  // Members
  Eigen::MatrixXi faces_;
  Eigen::MatrixXd vertices_;
  Eigen::MatrixXd warp_;
  Eigen::MatrixXd landmarks_points_;

  std::vector<int> good_particles_;

  bool needs_warp_ = true;

  bool warp_available_ = false;

  std::map<int, int> landmarks_map_;  // map the landmarks id (Key) to the vertex(point) id (Value) belonging to the
                                      // clean Reference mesh
  //! Reference mesh as it was given to us
  vtkSmartPointer<vtkPolyData> incoming_reference_mesh_;
  //! Processed reference mesh
  vtkSmartPointer<vtkPolyData> reference_mesh_;
  //! Reference particles (matrix [Nx3])
  Eigen::MatrixXd reference_particles_;
  //! Whether the reference is a contour
  bool is_contour_ = false;
};
}  // namespace shapeworks
