#pragma once

#include <Eigen/Eigen>
#include <string>
#include <vector>

#include "ParticleSystemEvaluation.h"
#include "Shapeworks.h"

namespace shapeworks {

class Project;

/**
 * \class ParticleShapeStatistics
 * This class computes various statistics for a set of correspondence positions
 * and group ids.
 */
class ParticleShapeStatistics {
 public:
  ParticleShapeStatistics(){};
  ParticleShapeStatistics(std::shared_ptr<Project> project);
  ~ParticleShapeStatistics(){};

  int do_pca(std::vector<std::vector<Point>> global_pts, int domainsPerShape = 1);

  int do_pca(ParticleSystemEvaluation particleSystem, int domainsPerShape = 1);

  //! Loads a set of point files and pre-computes some statistics.
  int import_points(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids);

  //! Loads a set of point files and pre-computes statistics for multi-level analysis
  void compute_multi_level_analysis_statistics(std::vector<Eigen::VectorXd> points, unsigned int dps);

  //! Compute shape variations for MLCA
  int compute_shape_dev_modes_for_mca();

  //! Compute pose variations for MLCA
  int compute_relative_pose_modes_for_mca();

  //! Set number of particles per domain/object (required for multi-level analysis)
  void set_num_particles_per_domain(const std::vector<int>& num_particles_array);

  //! Loads a set of point files and pre-computes some statistics.
  int read_point_files(const std::string& s);

  //! Writes a text file in comma-separated format.  Suitable for reading into excel or R or Matlab for analysis.
  int write_csv_file(const std::string& s);

  //! Computes PCA modes from the set of correspondence mode positions. Requires that ReadPointFiles be called first.
  int compute_modes();

  //! Return the number of modes
  int get_num_modes() const;

  //! Computes the principal component loadings, or projections onto the
  //!  principal componenent axes for each of the samples.  ComputeModes must be called first.
  int principal_component_projections();

  //! Returns the sample size
  int get_num_samples() const { return num_samples_; }
  int get_group1_num_samples() const { return num_samples_group1_; }
  int get_group2_num_samples() const { return num_samples_group2_; }

  //! Returns the number of dimensions (this is number of points times Dimension)
  int get_num_dimensions() const { return num_dimensions_; }
  //! Return Number of objects present in Multi-Object Shape Structure
  int get_domains_per_shape() { return domains_per_shape_; }
  //! Returns Number of Particles Array
  std::vector<int> NumberOfPointsArray() { return num_particles_array_; }

  //! Returns the group ids
  int GroupID(unsigned int i) const { return group_ids_[i]; }
  const std::vector<int>& GroupID() const { return group_ids_; }

  //! Returns the eigenvectors/values.
  const Eigen::MatrixXd& get_eigen_vectors() const { return eigenvectors_; }
  const std::vector<double>& get_eigen_values() const { return eigenvalues_; }

  //! Returns the eigenvectors/eigenvalues for morphological and relative pose pariations of MLCA
  const Eigen::MatrixXd& get_eigenvectors_rel_pos() { return eigenvectors_rel_pose_; }
  const std::vector<double>& get_eigenvalues_shape_dev() { return eigenvalues_shape_dev_; }
  const Eigen::MatrixXd& get_eigenvectors_shape_dev() { return eigenvectors_shape_dev_; }
  const std::vector<double>& get_eigenvalues_rel_pose() { return eigenvalues_rel_pose_; }
  //! Returns the mean vector of within and between subspace
  const Eigen::VectorXd& get_mean_shape_dev() { return mean_shape_dev_; }
  const Eigen::VectorXd& get_mean_rel_pos() { return mean_rel_pose_; }

  //! Returns the mean shape.
  const Eigen::VectorXd& get_mean() const { return mean_; }
  const Eigen::VectorXd& get_group1_mean() const { return mean1_; }
  const Eigen::VectorXd& get_group2_mean() const { return mean2_; }

  //! Returns group2 - group1 mean
  const Eigen::VectorXd& get_group_difference() const { return groupdiff_; }

  //! Returns the median shape for the set of shapes with Group ID equal to the
  //! integer argument.  For example, compute_median_shape(0) returns the median
  //! shape for the set of shapes with m_groupIDs == 0. The median shape is
  //! defined as the shape with the minimum sum of Euclidean L1 norms to all
  //! other shapes in that group.  Arguments passed to this function are set to
  //! the index number of the median shape for Group A and Group B,
  //! respectively.*/
  int compute_median_shape(const int ID);

  //! Returns the euclidean L1 norm between shape a and b
  double l1_norm(unsigned int a, unsigned int b);

  //! Returns the component loadings
  Eigen::MatrixXd& get_pca_loadings() { return principals_; }

  const std::vector<double>& get_percent_variance_by_mode() const { return percent_variance_by_mode_; }

  //! Computes a simple linear regression of the first list of values with
  //! respect to the second y=a + bx. Returns the estimated parameters a & b.
  //! Returns 0 on success and -1 on fail.*/
  static int simple_linear_regression(const std::vector<double>& y, const std::vector<double>& x, double& a, double& b);

  Eigen::VectorXd get_compactness(const std::function<void(float)>& progress_callback = nullptr) const;
  Eigen::VectorXd get_specificity(const std::function<void(float)>& progress_callback = nullptr,
                                  const std::function<bool()>& check_abort = nullptr) const;
  Eigen::VectorXd get_generalization(const std::function<void(float)>& progress_callback = nullptr,
                                     const std::function<bool(void)>& check_abort = nullptr) const;

  Eigen::MatrixXd get_group1_matrix() const;
  Eigen::MatrixXd get_group2_matrix() const;

  Eigen::MatrixXd& matrix() { return matrix_; };

  //! Set the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar)
  void set_num_values_per_particle(int value_per_particle) { values_per_particle_ = value_per_particle; }

  void set_particle_to_surface_mode(bool value) { particle_to_surface_mode_ = value; }
  bool get_particle_to_surface_mode() const { return particle_to_surface_mode_; }
  //! Set the meshes for each sample (used for some evaluation metrics)
  void set_meshes(const std::vector<Mesh>& meshes) { meshes_ = meshes; }

  // import estimated parameters for regression
  inline bool import_regression_parameters(Eigen::VectorXd slope, Eigen::VectorXd intercept) { slope_ = slope; intercept_ = intercept; };
  
 private:
  unsigned int num_samples_group1_;
  unsigned int num_samples_group2_;
  unsigned int num_samples_;
  unsigned int domains_per_shape_;
  unsigned int num_dimensions_;
  std::vector<int> group_ids_;

  Eigen::MatrixXd eigenvectors_;
  std::vector<double> eigenvalues_;
  Eigen::VectorXd mean_;
  Eigen::VectorXd mean1_;
  Eigen::VectorXd mean2_;x
  Eigen::MatrixXd points_minus_mean_;
  Eigen::MatrixXd shapes_;
  
  // for regression tasks
  Eigen::VectorXd slope_;
  Eigen::VectorXd intercept_;
  bool regression_enabled_;

  std::vector<double> percent_variance_by_mode_;
  Eigen::MatrixXd principals_;

  Eigen::VectorXd groupdiff_;

  // Variables for MLCA
  std::vector<int> num_particles_array_;      // Number of Particles for each object in the multi-object shape structure
  Eigen::MatrixXd eigenvectors_rel_pose_;     // Eigenvectors defined for relative pose subspace
  Eigen::MatrixXd eigenvectors_shape_dev_;    // Eigenvectors defined for morphological subspace
  std::vector<double> eigenvalues_rel_pose_;  // Eigenvalues defined in relative pose subspace
  std::vector<double> eigenvalues_shape_dev_;  // Eigenvectors defined in morphological subspace
  Eigen::MatrixXd points_minus_mean_rel_pose_;
  Eigen::MatrixXd points_minus_mean_shape_dev_;
  Eigen::VectorXd mean_shape_dev_;
  Eigen::VectorXd mean_rel_pose_;
  Eigen::MatrixXd super_matrix_;  // Shape Matrix reshaped, used to compute MLCA statistics

  Eigen::MatrixXd matrix_;

  Eigen::MatrixXd group1_matrix_;
  Eigen::MatrixXd group2_matrix_;

  std::vector<Eigen::VectorXd> points_;

  int values_per_particle_ = 3;  // e.g. 3 for x/y/z, 4 for x/y/z/scalar
  bool particle_to_surface_mode_ = false;
  std::vector<Mesh> meshes_;
};

}  // namespace shapeworks
