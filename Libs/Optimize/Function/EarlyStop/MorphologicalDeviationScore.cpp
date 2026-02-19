#include "MorphologicalDeviationScore.h"

#include <Logging.h>
namespace shapeworks {

MorphologicalDeviationScore::MorphologicalDeviationScore() = default;

//---------------------------------------------------------------------------
bool MorphologicalDeviationScore::SetControlShapes(const Eigen::MatrixXd& X) {
  try {
    bool ppca_status = FitPPCA(X);
    is_fitted_ = ppca_status;
  } catch (std::exception& e) {
    SW_ERROR("Exception in setting control shapes for early stopping {}", e.what());
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
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(X_c, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd s = svd.singularValues();  // (r,)
    Eigen::MatrixXd V = svd.matrixV();         // (d x r)
    Eigen::VectorXd eigvals = (s.array().square()) / (n - 1);

    Eigen::VectorXd cumsum = eigvals;
    for (int i = 1; i < eigvals.size(); ++i) cumsum[i] += cumsum[i - 1];
    Eigen::VectorXd cumvar = cumsum / eigvals.sum();
    int q = 0;
    while (q < cumvar.size() && cumvar[q] <= retained_variance_ratio_) ++q;

    if (q == 0) {
      SW_ERROR("PPCA: no components retained at {}% variance", retained_variance_ratio_ * 100);
      return false;
    }

    // Determine effective rank: trim structurally zero eigenvalues
    const double eps = 1e-10;
    int rank = eigvals.size();
    while (rank > q && eigvals[rank - 1] < eps) {
      --rank;
    }

    if (rank <= q) {
      SW_ERROR("PPCA: effective rank ({}) <= retained components ({}), cannot estimate noise variance", rank, q);
      return false;
    }

    // Noise variance: average of excluded-but-real eigenvalues
    // Uses (rank - q) denominator instead of (d - q) to avoid dilution
    // by structurally zero dimensions beyond the data rank
    noise_variance_ = eigvals.segment(q, rank - q).sum() / double(rank - q);

    // Store full rank-dimensional basis for projection-based scoring
    all_components_ = V.leftCols(rank);  // (d x rank)

    // Build per-dimension precision weights
    precision_weights_.resize(rank);
    for (int i = 0; i < q; ++i) {
      precision_weights_[i] = 1.0 / eigvals[i];
    }
    for (int i = q; i < rank; ++i) {
      precision_weights_[i] = 1.0 / noise_variance_;
    }

    SW_LOG("PPCA fit: n={}, d={}, rank={}, q={}, noise_var={:.6f}", n, d, rank, q, noise_variance_);

    return true;

  } catch (std::exception& e) {
    SW_ERROR("SVD computation failed for MorphologicalDeviationScore:  {}", e.what());
    return false;
  }
}

//---------------------------------------------------------------------------
Eigen::VectorXd MorphologicalDeviationScore::GetMorphoDevScore(const Eigen::MatrixXd& X) {
  try {
    if (!is_fitted_) {
      throw std::runtime_error("PPCA model is not fitted on control shapes.");
    }

    // Project into the rank-dimensional subspace, eliminating
    // structurally zero dimensions that cannot be estimated from data
    Eigen::MatrixXd X_bar = X.rowwise() - mean_;  // (n x d)
    Eigen::MatrixXd Z = X_bar * all_components_;  // (n x rank)

    // Apply per-dimension precision weights: 1/lambda_i for retained PCs,
    // 1/noise_variance for excluded-but-real PCs
    Eigen::MatrixXd Z_weighted = Z.array().rowwise() * precision_weights_.transpose().array();  // (n x rank)

    Eigen::VectorXd sq_mahal = (Z.array() * Z_weighted.array()).rowwise().sum();  // (n,)
    Eigen::VectorXd mahalanobis = sq_mahal.array().max(0.0).sqrt();
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
