#pragma once

#include <Eigen/Dense>
// #include "cnpy.h"

namespace shapeworks {
class MorphologicalDeviationScore {
 public:
  MorphologicalDeviationScore();
  // Fit PPCA model on control shapes
  bool SetControlShapes(const Eigen::MatrixXd& X);  // (n_samples x n_features)
  /// Get Mahalanobis-based deviation score for test samples (non-fixed
  /// shapes/domains)
  Eigen::VectorXd GetMorphoDevScore(const Eigen::MatrixXd& X);  // (n,)

 private:
  /// Flag to ensure control shapes are set and PCA model is in place
  bool is_fitted_ = false;

  // Fitted model parameters
  Eigen::RowVectorXd mean_;                        // (1 x d)
  Eigen::MatrixXd components_;                     // (d x q)
  Eigen::VectorXd principal_components_variance_;  // (q,)
  int n_components_ = 0;
  double noise_variance_ = 0.0;
  double retained_variance_ratio_ = 0.95;
  // Derived matrices
  Eigen::MatrixXd precision_matrix_;  // (d x d)
  // Helper functions
  bool FitPPCA(const Eigen::MatrixXd& X);
  Eigen::MatrixXd ComputeCovarianceMatrix();
  Eigen::MatrixXd ComputePrecisionMatrix();
//   inline static void save_vector(const Eigen::VectorXd& v,
//                                  const std::string& fname) {
//     cnpy::npy_save(fname, v.data(), {(size_t)v.size()}, "w");
//   };
//   inline static void save_matrix(const Eigen::MatrixXd m,
//                                  const std::string& fname) {
//     Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
//         X_rm = m;
//     cnpy::npy_save(fname, X_rm.data(), {(size_t)m.rows(), (size_t)m.cols()},
//                    "w");
//   };
};
}  // namespace shapeworks