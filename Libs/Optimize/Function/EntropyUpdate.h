#pragma once

#include <Eigen/Dense>

namespace shapeworks {

using RowMatrixType = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

/// Correspondence (entropy) update for the mean-centered shape matrix Y (d x N):
/// update = Y (YᵀY/(N-1) + αI)⁻¹, and the N eigenvalues of YᵀY in increasing order.
/// When N > d this is computed from the d x d covariance instead, using
/// Y (YᵀY/(N-1) + αI)⁻¹ = (YYᵀ/(N-1) + αI)⁻¹ Y, so the cost no longer grows as N³ (#2705).
/// force_dual: -1 chooses automatically, 0 always uses the N x N form, 1 the d x d form.
void ComputeEntropyUpdate(const Eigen::Ref<const RowMatrixType>& Y, double alpha, RowMatrixType& update,
                          Eigen::VectorXd& eigenvalues, int force_dual = -1);

}  // namespace shapeworks
