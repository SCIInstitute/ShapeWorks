
#include "CorrespondenceFunction.h"

#include <math.h>

#include "Libs/Utils/Utils.h"
#include "Profiling.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_diag_matrix.h"

namespace shapeworks {

void CorrespondenceFunction::ComputeUpdates(const ParticleSystem* c) {
  TIME_SCOPE("CorrespondenceFunction::ComputeUpdates");
  num_dims = m_ShapeData->rows();
  num_samples = m_ShapeData->cols();

  int rows = 0;
  for (int i = 0; i < m_DomainsPerShape; i++) rows += VDimension * c->GetNumberOfParticles(i);

  // Do we need to resize the update matrix?
  if (m_PointsUpdate->rows() != rows || m_PointsUpdate->cols() != num_samples) {
    m_PointsUpdate->set_size(rows, num_samples);
  }

  m_PointsUpdate->fill(0.0);

  // =========================================================================
  // FIXED SHAPE SPACE PATH: Use precomputed data if available
  // =========================================================================
  if (m_HasPrecomputedFixedDomains && !m_UseMeanEnergy) {
    TIME_SCOPE("ComputeUpdates::FixedShapeSpace");

    // Use precomputed mean from fixed shapes
    m_points_mean = m_FixedMean;

    // For each non-fixed shape, compute its gradient using precomputed data
    vnl_matrix_type Q(num_dims, num_samples);
    Q.fill(0.0);

    // Compute centered data for all shapes using fixed mean
    vnl_matrix_type points_minus_mean(num_dims, num_samples);
    for (int j = 0; j < num_samples; j++) {
      for (int i = 0; i < num_dims; i++) {
        points_minus_mean(i, j) = m_ShapeData->get(i, j) - (*m_FixedMean)(i, 0);
      }
    }

    // For each non-fixed shape, compute gradient using precomputed basis
    // gradient_j = Y_fixed * pinvMat_fixed * (Y_fixed^T * y_j) + self term
    //
    // Approximation: We use the fixed shape space structure to compute
    // gradients for new shapes. This is the "fixed shape space" approach.
    for (int j = 0; j < num_samples; j++) {
      // Check if this shape is fixed (skip gradient computation for fixed shapes)
      bool is_fixed = false;
      for (int idx : m_FixedDomainIndices) {
        if (idx == j) {
          is_fixed = true;
          break;
        }
      }

      if (!is_fixed) {
        // Extract y_j (the centered data for shape j)
        vnl_vector<double> y_j(num_dims);
        for (int i = 0; i < num_dims; i++) {
          y_j[i] = points_minus_mean(i, j);
        }

        // Compute projection: proj = Y_fixed^T * y_j  (N_fixed × 1)
        vnl_vector<double> proj(m_PrecomputedNumFixedSamples, 0.0);
        for (int k = 0; k < m_PrecomputedNumFixedSamples; k++) {
          for (int i = 0; i < num_dims; i++) {
            proj[k] += (*m_FixedY)(i, k) * y_j[i];
          }
        }

        // Compute gradient: grad = GradientBasis * (pinvMat * proj)
        // But GradientBasis = Y_fixed * pinvMat, so:
        // grad = Y_fixed * pinvMat * proj
        // We can compute this as: GradientBasis * (pinvMat^{-1} * pinvMat * proj)
        // Actually simpler: grad = sum_k (GradientBasis[:,k] * coeffs[k])
        // where coeffs = pinvMat * proj (but we precomputed GradientBasis = Y * pinvMat)
        // So: grad = GradientBasis * proj_normalized where proj is already weighted

        // Actually, the gradient for shape j is:
        // ∂G/∂y_j = contribution from the cross-covariance with fixed shapes
        // In the fixed shape space approximation:
        // grad_j ≈ Y_fixed * pinvMat_fixed * (Y_fixed^T * y_j) / ||y_j||
        //        = GradientBasis * (Y_fixed^T * y_j)

        // But we need to normalize properly. Let's use:
        // Q[:,j] = M_fixed * coeffs where coeffs = Y_fixed^T * y_j normalized

        vnl_vector<double> grad(num_dims, 0.0);
        for (int i = 0; i < num_dims; i++) {
          for (int k = 0; k < m_PrecomputedNumFixedSamples; k++) {
            grad[i] += (*m_FixedGradientBasis)(i, k) * proj[k];
          }
        }

        // Normalize by number of fixed samples (similar to original algorithm)
        double scale = 1.0 / (m_PrecomputedNumFixedSamples - 1);
        for (int i = 0; i < num_dims; i++) {
          Q(i, j) = grad[i] * scale;
        }
      }
    }

    // Now compute the updates from Q (same as original code)
    {
      vnl_matrix_type Jmatrix;
      vnl_matrix_type v;
      for (int j = 0; j < num_samples; j++) {
        int num = 0;
        int num2 = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++) {
          int dom = d + j * m_DomainsPerShape;
          if (c->GetDomainFlag(dom) == false) {
            if (d > 0) {
              num += m_AttributesPerDomain[d - 1] * c->GetNumberOfParticles(d - 1);
              if (m_UseXYZ[d - 1]) num += 3 * c->GetNumberOfParticles(d - 1);
              if (m_UseNormals[d - 1]) num += 3 * c->GetNumberOfParticles(d - 1);
              num2 += VDimension * c->GetNumberOfParticles(d - 1);
            }

            int num_attr = m_AttributesPerDomain[d];
            if (m_UseXYZ[d]) num_attr += 3;
            if (m_UseNormals[d]) num_attr += 3;

            Jmatrix.clear();
            Jmatrix.set_size(num_attr, VDimension);
            Jmatrix.fill(0.0);
            v.clear();
            v.set_size(num_attr, 1);
            v.fill(0.0);

            for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++) {
              v = Q.extract(num_attr, 1, num + p * num_attr, j);
              Jmatrix = m_ShapeGradient->extract(num_attr, 3, num + p * num_attr, 3 * j);
              vnl_matrix_type dx = Jmatrix.transpose() * v;
              for (unsigned int vd = 0; vd < VDimension; vd++) {
                m_PointsUpdate->put(num2 + p * VDimension + vd, j, dx(vd, 0));
              }
            }
          }
        }
      }
    }

    // Compute energy using precomputed eigenvalues
    m_CurrentEnergy = 0.0;
    m_MinimumEigenValue = (*m_FixedW)(0, 0);
    for (int i = 0; i < m_PrecomputedNumFixedSamples; i++) {
      double w_i = (*m_FixedW)(i, i);
      double val = w_i * w_i / (m_PrecomputedNumFixedSamples - 1) + m_MinimumVariance;
      if (val < m_MinimumEigenValue) m_MinimumEigenValue = val;
      m_CurrentEnergy += log(val);
    }
    m_CurrentEnergy /= 2.0;

    return;  // Done with fixed shape space path
  }

  // =========================================================================
  // ORIGINAL PATH: Full computation (no precomputed data or using mean energy)
  // =========================================================================
  vnl_matrix_type points_minus_mean(num_dims, num_samples, 0.0);

  m_points_mean->clear();
  m_points_mean->set_size(num_dims, 1);
  m_points_mean->fill(0.0);

  for (unsigned int i = 0; i < num_dims; i++) {
    m_points_mean->put(i, 0, (m_ShapeData->get_n_rows(i, 1)).mean());
    points_minus_mean.set_row(i, m_ShapeData->get_row(i) - m_points_mean->get(i, 0));
  }

  vnl_diag_matrix<double> W;

  vnl_matrix_type gramMat(num_samples, num_samples, 0.0);
  vnl_matrix_type pinvMat(num_samples, num_samples, 0.0);  // gramMat inverse

  if (this->m_UseMeanEnergy) {
    pinvMat.set_identity();
  } else {
    TIME_START("correspondence::gramMat");

    // Create Eigen maps that point to the VNL data
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> points_minus_mean_map(
        points_minus_mean.data_block(), points_minus_mean.rows(), points_minus_mean.cols());

    // Perform the computation directly on the mapped data
    Eigen::MatrixXd gramMat_eigen = points_minus_mean_map.transpose() * points_minus_mean_map;

    // Copy result back to VNL matrix
    gramMat.set_size(gramMat_eigen.rows(), gramMat_eigen.cols());
    std::memcpy(gramMat.data_block(), gramMat_eigen.data(), gramMat_eigen.size() * sizeof(double));

    TIME_STOP("correspondence::gramMat");

    TIME_START("correspondence::svd");
    vnl_svd<double> svd(gramMat);
    vnl_matrix_type UG = svd.U();
    W = svd.W();
    vnl_diag_matrix<double> invLambda = svd.W();
    TIME_STOP("correspondence::svd");

    invLambda.set_diagonal(invLambda.get_diagonal() / (double)(num_samples - 1) + m_MinimumVariance);
    invLambda.invert_in_place();

    TIME_START("correspondence::pinvMat");
    pinvMat = (UG * invLambda) * UG.transpose();
    TIME_STOP("correspondence::pinvMat");

    // Note: The full dM × dM inverse covariance matrix is NOT needed.
    // Per thesis equation 2.35, the gradient is simply: Y × (Y^T Y + αI)^{-1}
    // which is Y × pinvMat. No covariance_multiply required.
  }

  vnl_matrix_type Q = points_minus_mean * pinvMat;

  // Compute the update matrix in coordinate space by multiplication with the
  // Jacobian.  Each shape gradient must be transformed by a different Jacobian
  // so we have to do this individually for each shape (sample).

  {
    vnl_matrix_type Jmatrix;
    vnl_matrix_type v;
    for (int j = 0; j < num_samples; j++) {
      int num = 0;
      int num2 = 0;
      for (unsigned int d = 0; d < m_DomainsPerShape; d++) {
        int dom = d + j * m_DomainsPerShape;
        if (c->GetDomainFlag(dom) == false) {
          if (d > 0) {
            num += m_AttributesPerDomain[d - 1] * c->GetNumberOfParticles(d - 1);
            if (m_UseXYZ[d - 1]) num += 3 * c->GetNumberOfParticles(d - 1);
            if (m_UseNormals[d - 1]) num += 3 * c->GetNumberOfParticles(d - 1);

            num2 += c->GetNumberOfParticles(d - 1) * VDimension;
          }

          int num_attr = m_AttributesPerDomain[d];
          if (m_UseXYZ[d]) {
            num_attr += 3;
          }
          if (m_UseNormals[d]) {
            num_attr += 3;
          }

          Jmatrix.clear();
          Jmatrix.set_size(num_attr, VDimension);
          Jmatrix.fill(0.0);
          v.clear();
          v.set_size(num_attr, 1);
          v.fill(0.0);

          for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++) {
            v = Q.extract(num_attr, 1, num + p * num_attr, j);
            Jmatrix = m_ShapeGradient->extract(num_attr, 3, num + p * num_attr, 3 * j);

            vnl_matrix_type dx = Jmatrix.transpose() * v;
            for (unsigned int vd = 0; vd < VDimension; vd++) {
              m_PointsUpdate->put(num2 + p * VDimension + vd, j, dx(vd, 0));
            }
          }
        }
      }
    }
  }
  m_CurrentEnergy = 0.0;

  if (m_UseMeanEnergy) {
    m_CurrentEnergy = points_minus_mean.frobenius_norm();
  } else {
    m_MinimumEigenValue = W(0) * W(0) + m_MinimumVariance;
    for (unsigned int i = 0; i < num_samples; i++) {
      double val_i = W(i) * W(i) + m_MinimumVariance;
      if (val_i < m_MinimumEigenValue) m_MinimumEigenValue = val_i;
      m_CurrentEnergy += log(val_i);
    }
  }

  m_CurrentEnergy /= 2.0;

  if (m_UseMeanEnergy) {
    m_MinimumEigenValue = m_CurrentEnergy / 2.0;
  }
}

CorrespondenceFunction::VectorType CorrespondenceFunction::evaluate(unsigned int idx, unsigned int d,
                                                                    const ParticleSystem* system, double& maxdt,
                                                                    double& energy) const {
  int dom = d % m_DomainsPerShape;      // domain number within shape
  int sampNum = d / m_DomainsPerShape;  // shape number

  int sz_Yidx = 0;
  if (m_UseXYZ[dom]) {
    sz_Yidx += 3;
  }
  if (m_UseNormals[dom]) {
    sz_Yidx += 3;
  }
  sz_Yidx += m_AttributesPerDomain[dom];

  int num = sz_Yidx * idx;

  for (unsigned int i = 0; i < dom; i++) {
    int num1 = 0;
    if (m_UseXYZ[i]) num1 += 3;
    if (m_UseNormals[i]) num1 += 3;
    num1 += m_AttributesPerDomain[i];

    num += num1 * system->GetNumberOfParticles(i);
  }

  // Per-particle energy: squared Euclidean distance from mean
  // This is used for line search step-size adaptation.
  // Note: We always use Euclidean distance (identity matrix) regardless of m_UseMeanEnergy,
  // because the full inverse covariance matrix is not computed.
  vnl_matrix_type Y_dom_idx(sz_Yidx, 1, 0.0);
  Y_dom_idx = m_ShapeData->extract(sz_Yidx, 1, num, sampNum) - m_points_mean->extract(sz_Yidx, 1, num);

  // energy = ||y - mean||^2
  vnl_matrix_type tmp = Y_dom_idx.transpose() * Y_dom_idx;
  energy = tmp(0, 0);

  maxdt = m_MinimumEigenValue;

  num = 0;
  for (int i = 0; i < dom; i++) num += system->GetNumberOfParticles(i) * VDimension;

  VectorType gradE;
  unsigned int k = idx * VDimension + num;
  for (unsigned int i = 0; i < VDimension; i++) {
    gradE[i] = m_PointsUpdate->get(k + i, sampNum);
  }

  return system->TransformVector(gradE, system->GetInversePrefixTransform(d) * system->GetInverseTransform(d));
}

//---------------------------------------------------------------------------
void CorrespondenceFunction::ClearPrecomputedFixedDomains() {
  m_HasPrecomputedFixedDomains = false;
  m_PrecomputedNumFixedSamples = 0;
  m_PrecomputedNumDims = 0;
  m_FixedMean.reset();
  m_FixedY.reset();
  m_FixedPinvMat.reset();
  m_FixedGradientBasis.reset();
  m_FixedU.reset();
  m_FixedW.reset();
  m_FixedDomainIndices.clear();
}

//---------------------------------------------------------------------------
void CorrespondenceFunction::PrecomputeForFixedDomains(const ParticleSystem* ps) {
  TIME_SCOPE("CorrespondenceFunction::PrecomputeForFixedDomains");

  // Clear any existing precomputed data
  ClearPrecomputedFixedDomains();

  // Count fixed and non-fixed shapes
  int total_shapes = m_ShapeData->cols();
  int num_fixed = 0;
  int num_unfixed = 0;

  // Identify which shapes are fixed
  // A shape is considered fixed if ALL its domains are fixed
  for (int shape = 0; shape < total_shapes; shape++) {
    bool shape_is_fixed = true;
    for (int d = 0; d < m_DomainsPerShape; d++) {
      int dom = shape * m_DomainsPerShape + d;
      if (!ps->GetDomainFlag(dom)) {
        shape_is_fixed = false;
        break;
      }
    }
    if (shape_is_fixed) {
      m_FixedDomainIndices.push_back(shape);
      num_fixed++;
    } else {
      num_unfixed++;
    }
  }

  // If no fixed shapes or all shapes are fixed, precomputation won't help
  if (num_fixed == 0 || num_unfixed == 0) {
    SW_LOG("PrecomputeForFixedDomains: No benefit from precomputation "
           "(fixed={}, unfixed={})", num_fixed, num_unfixed);
    return;
  }

  SW_LOG("PrecomputeForFixedDomains: Precomputing for {} fixed shapes, {} unfixed shapes",
         num_fixed, num_unfixed);

  int num_dims = m_ShapeData->rows();
  int num_fixed_samples = num_fixed;

  // Store dimensions for validation later
  m_PrecomputedNumFixedSamples = num_fixed_samples;
  m_PrecomputedNumDims = num_dims;

  // Step 1: Extract fixed shape data and compute mean
  // Build Y_fixed matrix: dM × N_fixed
  m_FixedY = std::make_shared<vnl_matrix_type>(num_dims, num_fixed_samples);
  m_FixedMean = std::make_shared<vnl_matrix_type>(num_dims, 1);
  m_FixedMean->fill(0.0);

  // Copy fixed shape data
  for (int i = 0; i < num_fixed_samples; i++) {
    int fixed_shape_idx = m_FixedDomainIndices[i];
    for (int r = 0; r < num_dims; r++) {
      double val = m_ShapeData->get(r, fixed_shape_idx);
      m_FixedY->put(r, i, val);
      (*m_FixedMean)(r, 0) += val;
    }
  }

  // Compute mean
  for (int r = 0; r < num_dims; r++) {
    (*m_FixedMean)(r, 0) /= num_fixed_samples;
  }

  // Center the data: Y_fixed = Y_fixed - mean
  for (int i = 0; i < num_fixed_samples; i++) {
    for (int r = 0; r < num_dims; r++) {
      (*m_FixedY)(r, i) -= (*m_FixedMean)(r, 0);
    }
  }

  // Step 2: Compute Gram matrix G = Y_fixed^T * Y_fixed
  TIME_START("precompute::gramMat");
  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      Y_fixed_map(m_FixedY->data_block(), num_dims, num_fixed_samples);

  Eigen::MatrixXd gramMat_eigen = Y_fixed_map.transpose() * Y_fixed_map;
  TIME_STOP("precompute::gramMat");

  // Step 3: SVD of Gram matrix
  TIME_START("precompute::svd");
  vnl_matrix_type gramMat(num_fixed_samples, num_fixed_samples);
  for (int i = 0; i < num_fixed_samples; i++) {
    for (int j = 0; j < num_fixed_samples; j++) {
      gramMat(i, j) = gramMat_eigen(i, j);
    }
  }

  vnl_svd<double> svd(gramMat);
  m_FixedU = std::make_shared<vnl_matrix_type>(svd.U());
  m_FixedW = std::make_shared<vnl_diag_matrix<double>>(svd.W());
  TIME_STOP("precompute::svd");

  // Step 4: Compute regularized inverse: pinvMat = U * (W/(N-1) + αI)^{-1} * U^T
  TIME_START("precompute::pinvMat");
  vnl_diag_matrix<double> invLambda = *m_FixedW;
  for (int i = 0; i < num_fixed_samples; i++) {
    double val = invLambda(i, i) / (num_fixed_samples - 1) + m_MinimumVariance;
    invLambda(i, i) = 1.0 / val;  // Invert
  }

  m_FixedPinvMat = std::make_shared<vnl_matrix_type>(num_fixed_samples, num_fixed_samples);
  *m_FixedPinvMat = (*m_FixedU) * invLambda * m_FixedU->transpose();
  TIME_STOP("precompute::pinvMat");

  // Step 5: Precompute gradient basis: M = Y_fixed * pinvMat
  // This is dM × N_fixed
  TIME_START("precompute::gradientBasis");
  m_FixedGradientBasis = std::make_shared<vnl_matrix_type>(num_dims, num_fixed_samples);
  *m_FixedGradientBasis = (*m_FixedY) * (*m_FixedPinvMat);
  TIME_STOP("precompute::gradientBasis");

  m_HasPrecomputedFixedDomains = true;

  SW_LOG("PrecomputeForFixedDomains: Precomputation complete. "
         "Fixed shapes: {}, Dimensions: {}", num_fixed_samples, num_dims);
}

}  // namespace shapeworks
