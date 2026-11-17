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

  if (!dual) {
    // gramMat = Yᵀ Y (N x N) is symmetric positive-semidefinite, so a self-adjoint eigensolver yields
    // the same eigenpairs as a general SVD but much faster.
    TIME_START("correspondence::gramMat");
    Eigen::MatrixXd gram = Y.transpose() * Y;
    TIME_STOP("correspondence::gramMat");

    TIME_START("correspondence::svd");
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(gram);
    TIME_STOP("correspondence::svd");
    eigenvalues = eig.eigenvalues();

    // pinvMat = UG · diag(1 / (λ/(N-1) + α)) · UGᵀ (symmetric)
    TIME_START("correspondence::pinvMat");
    Eigen::ArrayXd inv = (eigenvalues.array() * scale + alpha).inverse();
    Eigen::MatrixXd pinv = eig.eigenvectors() * inv.matrix().asDiagonal() * eig.eigenvectors().transpose();
    TIME_STOP("correspondence::pinvMat");

    // update = Y · pinvMat (thesis eq. 2.35: gradient is Y × (YᵀY + αI)⁻¹)
    update.noalias() = Y * pinv;
  } else {
    // Same update from the d x d covariance: Y (YᵀY/(N-1) + αI)⁻¹ = (YYᵀ/(N-1) + αI)⁻¹ Y
    TIME_START("correspondence::gramMat");
    Eigen::MatrixXd cov = Y * Y.transpose();
    TIME_STOP("correspondence::gramMat");

    TIME_START("correspondence::svd");
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
    TIME_STOP("correspondence::svd");

    TIME_START("correspondence::pinvMat");
    Eigen::ArrayXd inv = (eig.eigenvalues().array() * scale + alpha).inverse();
    Eigen::MatrixXd cov_inv = eig.eigenvectors() * inv.matrix().asDiagonal() * eig.eigenvectors().transpose();
    TIME_STOP("correspondence::pinvMat");
    update.noalias() = cov_inv * Y;

    // YᵀY and YYᵀ share their nonzero eigenvalues; the rest are zero
    eigenvalues = Eigen::VectorXd::Zero(n);
    if (n >= d) {
      eigenvalues.tail(d) = eig.eigenvalues();
    } else {
      eigenvalues = eig.eigenvalues().tail(n);
    }
  }
}

}  // namespace shapeworks
