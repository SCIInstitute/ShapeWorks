---
title: Libs/Particles/ParticleShapeStatistics.h

---

# Libs/Particles/ParticleShapeStatistics.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Eigen>
#include <string>
#include <vector>

#include "ParticleSystemEvaluation.h"
#include "Shapeworks.h"

namespace shapeworks {

class Project;

class ParticleShapeStatistics {
 public:
  ParticleShapeStatistics(){};
  ParticleShapeStatistics(std::shared_ptr<Project> project);
  ~ParticleShapeStatistics(){};

  int do_pca(std::vector<std::vector<Point>> global_pts, int domainsPerShape = 1);

  int do_pca(ParticleSystemEvaluation particleSystem, int domainsPerShape = 1);

  int import_points(std::vector<Eigen::VectorXd> points, std::vector<int> group_ids);

  void compute_multi_level_analysis_statistics(std::vector<Eigen::VectorXd> points, unsigned int dps);

  int compute_shape_dev_modes_for_mca();

  int compute_relative_pose_modes_for_mca();

  void set_num_particles_per_domain(const std::vector<int>& num_particles_array);

  int read_point_files(const std::string& s);

  int write_csv_file(const std::string& s);

  int compute_modes();

  int get_num_modes() const;

  int principal_component_projections();

  int get_num_samples() const { return num_samples_; }
  int get_group1_num_samples() const { return num_samples_group1_; }
  int get_group2_num_samples() const { return num_samples_group2_; }

  int get_num_dimensions() const { return num_dimensions_; }
  int get_domains_per_shape() { return domains_per_shape_; }
  std::vector<int> NumberOfPointsArray() { return num_particles_array_; }

  int GroupID(unsigned int i) const { return group_ids_[i]; }
  const std::vector<int>& GroupID() const { return group_ids_; }

  const Eigen::MatrixXd& get_eigen_vectors() const { return eigenvectors_; }
  const std::vector<double>& get_eigen_values() const { return eigenvalues_; }

  const Eigen::MatrixXd& get_eigenvectors_rel_pos() { return eigenvectors_rel_pose_; }
  const std::vector<double>& get_eigenvalues_shape_dev() { return eigenvalues_shape_dev_; }
  const Eigen::MatrixXd& get_eigenvectors_shape_dev() { return eigenvectors_shape_dev_; }
  const std::vector<double>& get_eigenvalues_rel_pose() { return eigenvalues_rel_pose_; }
  const Eigen::VectorXd& get_mean_shape_dev() { return mean_shape_dev_; }
  const Eigen::VectorXd& get_mean_rel_pos() { return mean_rel_pose_; }

  const Eigen::VectorXd& get_mean() const { return mean_; }
  const Eigen::VectorXd& get_group1_mean() const { return mean1_; }
  const Eigen::VectorXd& get_group2_mean() const { return mean2_; }

  const Eigen::VectorXd& get_group_difference() const { return groupdiff_; }

  int compute_median_shape(const int ID);

  double l1_norm(unsigned int a, unsigned int b);

  Eigen::MatrixXd& get_pca_loadings() { return principals_; }

  const std::vector<double>& get_percent_variance_by_mode() const { return percent_variance_by_mode_; }

  static int simple_linear_regression(const std::vector<double>& y, const std::vector<double>& x, double& a, double& b);

  Eigen::VectorXd get_compactness(const std::function<void(float)>& progress_callback = nullptr) const;
  Eigen::VectorXd get_specificity(const std::function<void(float)>& progress_callback = nullptr) const;
  Eigen::VectorXd get_generalization(const std::function<void(float)>& progress_callback = nullptr) const;

  Eigen::MatrixXd get_group1_matrix() const;
  Eigen::MatrixXd get_group2_matrix() const;

  Eigen::MatrixXd& matrix() { return matrix_; };

  void set_num_values_per_particle(int value_per_particle) { values_per_particle_ = value_per_particle; }

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
  Eigen::VectorXd mean2_;
  Eigen::MatrixXd points_minus_mean_;
  Eigen::MatrixXd shapes_;

  std::vector<double> percent_variance_by_mode_;
  Eigen::MatrixXd principals_;

  Eigen::VectorXd groupdiff_;

  // Variables for MLCA
  std::vector<int> num_particles_array_;     // Number of Particles for each object in the multi-object shape structure
  Eigen::MatrixXd eigenvectors_rel_pose_;   // Eigenvectors defined for relative pose subspace
  Eigen::MatrixXd eigenvectors_shape_dev_;  // Eigenvectors defined for morphological subspace
  std::vector<double> eigenvalues_rel_pose_;   // Eigenvalues defined in relative pose subspace
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
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-04-03 at 19:47:52 +0000
