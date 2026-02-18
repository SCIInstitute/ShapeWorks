#pragma once

#include <Eigen/Dense>

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
  bool is_fitted_ = false;

  // Fitted model parameters
  Eigen::RowVectorXd mean_;            // (1 x d)
  double noise_variance_ = 0.0;
  double retained_variance_ratio_ = 0.95;

  // Full-rank basis and per-dimension precision weights for scoring
  Eigen::MatrixXd all_components_;     // (d x rank) — all non-zero eigenvectors
  Eigen::VectorXd precision_weights_;  // (rank,) — 1/lambda_i or 1/noise_variance

  bool FitPPCA(const Eigen::MatrixXd& X);
};
}  // namespace shapeworks
