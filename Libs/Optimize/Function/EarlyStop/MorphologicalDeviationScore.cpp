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
    Eigen::VectorXd eigvals =
        (s.array().square()) / std::max(1, n - 1);  // (r,)

    double total_var = eigvals.sum();
    if (total_var <= 0.0) {
      throw std::runtime_error(
          "Total variance is non-positive. Cannot fit PCA on control shapes");
    }

    Eigen::VectorXd cum_var = eigvals;
    for (int i = 1; i < cum_var.size(); ++i) {
      cum_var(i) += cum_var(i - 1);
    }
    cum_var /= total_var;

    int q = 0;
    for (int i = 0; i < cum_var.size(); ++i) {
      if (cum_var(i) > 0.95) {
        q = i + 1;
        break;
      }
    }
    if (q == 0) q = cum_var.size();

    eigvals_ = eigvals.head(q);   // (q,)
    components_ = V.leftCols(q);  // (d x q)

    if (q < d) {
      double rem_sum = eigvals.tail(eigvals.size() - q).sum();
      noise_variance_ = rem_sum / (d - q);
    } else {
      noise_variance_ = 0.0;
    }
    return true;

  } catch (std::exception& e) {
    SW_ERROR(
        "Exception in SVD computation for early stopping score function {}",
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

  Eigen::MatrixXd diag_lambda = eigvals_.asDiagonal();  // (q x q)
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

    if (q == 0) {
      if (noise_variance_ <= 0.0)
        throw std::runtime_error(
            "Noise variance is zero; precision undefined.");
      return (1.0 / noise_variance_) * Eigen::MatrixXd::Identity(d, d);
    }

    Eigen::MatrixXd P = components_ * components_.transpose();  // (d x d)
    Eigen::MatrixXd term_principal = components_ *
                                     eigvals_.cwiseInverse().asDiagonal() *
                                     components_.transpose();  // (d x d)

    if (noise_variance_ <= 0.0) {
      if (q != d)
        throw std::runtime_error(
            "Noise variance is zero and q < d; covariance is singular.");
      return term_principal;
    }

    Eigen::MatrixXd precision =
        term_principal + (1.0 / noise_variance_) *
                             (Eigen::MatrixXd::Identity(d, d) - P);  // (d x d)
    return precision;

  } catch (std::exception& e) {
    SW_ERROR(
        "Exception in computation of precision matrix in early stopping score "
        "function {}",
        e.what());
    return Eigen::MatrixXd();
  }
}

//---------------------------------------------------------------------------
Eigen::VectorXd MorphologicalDeviationScore::GetMahalanobisDistance(
    const Eigen::MatrixXd& X) {
  try {
    if (!is_fitted_) {
      throw std::runtime_error(
          "PCA model has not been fitted yet on control shapes.");
    }
    const int n = X.rows();
    Eigen::MatrixXd X_c = X.rowwise() - mean_;
    Eigen::VectorXd dist(n);
    for (int i = 0; i < n; ++i) {
      Eigen::RowVectorXd xi = X_c.row(i);
      dist(i) = std::sqrt(
          std::max(0.0, (xi * precision_matrix_ * xi.transpose())(0)));
    }
    return dist;
  } catch (std::exception& e) {
    SW_ERROR(
        "Exception in computing Mahalanobis distance for early stopping score "
        "function {}",
        e.what());
    return Eigen::VectorXd();
  }
}

//---------------------------------------------------------------------------
// double MorphologicalDeviationScore::GetDeviationScore(const Eigen::MatrixXd&
// X_test) {
//     if (!is_fitted_) {
//         throw std::runtime_error("PPCA model has not been fitted. Call
//         SetControlShapes() first.");
//     }
//     Eigen::VectorXd dists = GetMahalanobisDistance(X_test);
//     return dists(0);
// }

}  // namespace shapeworks