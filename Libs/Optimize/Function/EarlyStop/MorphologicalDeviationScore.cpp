#include "MorphologicalDeviationScore.h"
#include <Logging.h>
namespace shapeworks {

MorphologicalDeviationScore::MorphologicalDeviationScore() = default;

//---------------------------------------------------------------------------
bool MorphologicalDeviationScore::SetControlShapes(const Eigen::MatrixXd& X) {
  try {
    bool ppca_status = FitPPCA(X);
    precision_matrix_ = ComputePrecisionMatrix();
    is_fitted_ = ppca_status;
  } catch (std::exception& e) {
    SW_ERROR("Exception in setting control shapes for early stopping {}",
             e.what());
    return false;
  }
  return is_fitted_;
}

//---------------------------------------------------------------------------
bool MorphologicalDeviationScore::FitPPCA(const Eigen::MatrixXd& X) {
  const int n = X.rows();  // n_samples
  const int d = X.cols();  // n_features

  mean_ = X.colwise().mean();                 // (1 x d)
  Eigen::MatrixXd X_c = X.rowwise() - mean_;  // (n x d)

  try {
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(
        X_c, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd s = svd.singularValues();  // (r,)
    Eigen::MatrixXd V = svd.matrixV();         // (d x r)
    Eigen::VectorXd eigvals = (s.array().square()) / (n - 1);

    // save_matrix(X, std::string(DEBUG_FILES_PTH) + "/X.npy");
    // save_matrix(X_c, std::string(DEBUG_FILES_PTH) + "/X_c.npy");
    // save_matrix(V, std::string(DEBUG_FILES_PTH) + "/V.npy");
    // save_vector(eigvals, std::string(DEBUG_FILES_PTH) + "/eigvals.npy");

    Eigen::VectorXd cumsum = eigvals;
    for (int i = 1; i < eigvals.size(); ++i) cumsum[i] += cumsum[i - 1];
    Eigen::VectorXd cumvar = cumsum / eigvals.sum();
    n_components_ = 0;
    while (n_components_ < cumvar.size() && cumvar[n_components_] <= retained_variance_ratio_)
      ++n_components_;
    int q = n_components_;
    principal_components_variance_ = eigvals.head(q);  // (q,)
    components_ = V.leftCols(q);                       // (d x q)
    // save_matrix(components_, std::string(DEBUG_FILES_PTH) + "/components.npy");
    // save_vector(principal_components_variance_,
    //             std::string(DEBUG_FILES_PTH) + "/eigvals.npy");
    noise_variance_ = eigvals.tail(eigvals.size() - q).sum() / double(d - q);
    return true;

  } catch (std::exception& e) {
    SW_ERROR(
        "SVD computation failed for MorphologicalDeviationScore:  {}",
        e.what());
    return false;
  }
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MorphologicalDeviationScore::ComputeCovarianceMatrix() {
  const int d = components_.rows();
  const int q = components_.cols();

  if (q == 0) {
    return noise_variance_ * Eigen::MatrixXd::Identity(d, d);
  }

  Eigen::MatrixXd diag_lambda = principal_components_variance_.asDiagonal();  // (q x q)
  Eigen::MatrixXd cov =
      components_ *
          (diag_lambda - noise_variance_ * Eigen::MatrixXd::Identity(q, q)) *
          components_.transpose() +
      noise_variance_ * Eigen::MatrixXd::Identity(d, d);  // (d x d)
  return cov;
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MorphologicalDeviationScore::ComputePrecisionMatrix() {
  try {
    const int d = components_.rows();
    const int q = components_.cols();

    Eigen::MatrixXd A_inv = (1 / noise_variance_) * Eigen::MatrixXd::Identity(d, d);

    Eigen::MatrixXd U = components_;
    Eigen::MatrixXd C_inv = principal_components_variance_.cwiseInverse().asDiagonal();
    Eigen::MatrixXd V = components_.transpose();
    // Woodbury matrix identity
    // (A + U C V)^(-1)
    //   = A^(-1)
    //   - A^(-1) * U * (C^(-1) + V * A^(-1) * U)^(-1) * V * A^(-1)

    Eigen::MatrixXd precision = A_inv - A_inv * U * (C_inv + V * A_inv * U).inverse() * V * A_inv;
    // save_matrix(precision, std::string(DEBUG_FILES_PTH)+"/precision.npy");

    return precision;
  } catch (std::exception& e) {
    SW_ERROR(
        "Failed to compute precision matrix in the early stopping score function: {}",
        e.what());
    return Eigen::MatrixXd();
  }
}

//---------------------------------------------------------------------------
Eigen::VectorXd MorphologicalDeviationScore::GetMorphoDevScore(
    const Eigen::MatrixXd& X) {
  try {

    if (!is_fitted_) {
      throw std::runtime_error(
          "PPCA model is not fitted on control shapes.");
    }
    const int n = X.rows();
    // Eigen::MatrixXd X_c = X.rowwise() - mean_;
    // Eigen::VectorXd dist(n);
    // for (int i = 0; i < n; ++i) {
    //   Eigen::RowVectorXd xi = X_c.row(i);
    //   dist(i) = std::sqrt(
    //       std::max(0.0, (xi * precision_matrix_ * xi.transpose())(0)));
    // }

    Eigen::MatrixXd X_bar = X.rowwise() - mean_;
    Eigen::MatrixXd Y = X_bar * precision_matrix_; // (n_samples × n_features)
    Eigen::VectorXd sq_mahal = (X_bar.array() * Y.array()).rowwise().sum(); // quadratic forms
    Eigen::VectorXd mahalanobis = sq_mahal.array().sqrt(); // final distances
    return mahalanobis;

  } catch (std::exception& e) {
    SW_ERROR(
        "Exception in computing Mahalanobis distance for early stopping score "
        "function {}",
        e.what());
    return Eigen::VectorXd();
  }
}

}  // namespace shapeworks