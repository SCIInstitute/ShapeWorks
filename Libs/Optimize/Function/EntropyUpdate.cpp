#include "EntropyUpdate.h"

#include "Profiling.h"

namespace shapeworks {

//---------------------------------------------------------------------------
void ComputeEntropyUpdate(const Eigen::Ref<const RowMatrixType>& Y, double alpha, RowMatrixType& update,
                          Eigen::VectorXd& eigenvalues, int force_dual) {
  const Eigen::Index d = Y.rows();
  const Eigen::Index n = Y.cols();
  const double scale = 1.0 / static_cast<double>(n - 1);
  const bool dual = force_dual >= 0 ? force_dual == 1 : n > d;

  // gram = YᵀY (N x N), or the d x d covariance YYᵀ when N > d. Both share their nonzero eigenvalues and give the
  // same update: Y (YᵀY/(N-1) + αI)⁻¹ = (YYᵀ/(N-1) + αI)⁻¹ Y (#2705)
  TIME_START("correspondence::gramMat");
  Eigen::MatrixXd gram = dual ? Eigen::MatrixXd(Y * Y.transpose()) : Eigen::MatrixXd(Y.transpose() * Y);
  TIME_STOP("correspondence::gramMat");

  // The energy only needs the eigenvalues, so skip the eigenvectors, which are most of the cost (#2711)
  TIME_START("correspondence::svd");
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(gram, Eigen::EigenvaluesOnly);
  TIME_STOP("correspondence::svd");

  // gram/(N-1) + αI is positive definite for α > 0, so the update is a Cholesky solve
  TIME_START("correspondence::pinvMat");
  Eigen::MatrixXd system = gram * scale;
  system.diagonal().array() += alpha;
  Eigen::LLT<Eigen::MatrixXd> llt(system);
  TIME_STOP("correspondence::pinvMat");

  if (llt.info() == Eigen::Success) {
    if (dual) {
      update.noalias() = llt.solve(Y);
    } else {
      // update = Y · system⁻¹ = (system⁻¹ Yᵀ)ᵀ, since system is symmetric
      update.noalias() = llt.solve(Y.transpose()).transpose();
    }
  } else {
    // not numerically positive definite (α too small): fall back to the eigendecomposition
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> full(gram);
    Eigen::ArrayXd inv = (full.eigenvalues().array() * scale + alpha).inverse();
    Eigen::MatrixXd inverse = full.eigenvectors() * inv.matrix().asDiagonal() * full.eigenvectors().transpose();
    if (dual) {
      update.noalias() = inverse * Y;
    } else {
      update.noalias() = Y * inverse;
    }
  }

  if (!dual) {
    eigenvalues = eig.eigenvalues();
  } else {
    // YᵀY has the d eigenvalues of YYᵀ and N-d zeros
    eigenvalues = Eigen::VectorXd::Zero(n);
    if (n >= d) {
      eigenvalues.tail(d) = eig.eigenvalues();
    } else {
      eigenvalues = eig.eigenvalues().tail(n);
    }
  }
}

}  // namespace shapeworks
