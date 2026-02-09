
#include "LegacyCorrespondenceFunction.h"

#include <string>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Utils/ParticleGaussianModeWriter.h"
#include "Logging.h"
#include "Profiling.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include <Eigen/Dense>

namespace shapeworks {
void LegacyCorrespondenceFunction ::WriteModes(const std::string& prefix, int n) const {
  typename ParticleGaussianModeWriter<VDimension>::Pointer writer = ParticleGaussianModeWriter<VDimension>::New();
  writer->SetShapeMatrix(m_ShapeMatrix);
  writer->SetFileName(prefix.c_str());
  writer->SetNumberOfModes(n);
  writer->Update();
}

void LegacyCorrespondenceFunction ::ComputeCovarianceMatrix() {
  // NOTE: This code requires that indices be contiguous, i.e. it wont work if
  // you start deleting particles.
  const unsigned int num_samples = m_ShapeMatrix->cols();
  const unsigned int num_dims = m_ShapeMatrix->rows();

  // Do we need to resize the covariance matrix?
  if (m_PointsUpdate->rows() != num_dims || m_PointsUpdate->cols() != num_samples) {
    m_PointsUpdate->set_size(num_dims, num_samples);
    m_PointsUpdate->fill(0.0);
  }

  // Lazy precomputation: do it on first call when shape matrix is populated
  if (m_NeedsPrecomputation && !m_HasPrecomputedFixedDomains && num_samples > 0) {
    m_NeedsPrecomputation = false;
    PrecomputeForFixedDomains(particle_system_);
  }

  // =========================================================================
  // FIXED SHAPE SPACE PATH: Use precomputed data if available
  // =========================================================================
  if (m_HasPrecomputedFixedDomains && !m_UseMeanEnergy) {
    TIME_SCOPE("ComputeCovarianceMatrix::FixedShapeSpace");

    // Use precomputed mean from fixed shapes
    m_points_mean = m_FixedMean;

    // Compute centered data for all shapes using fixed mean
    vnl_matrix_type points_minus_mean(num_dims, num_samples);
    for (unsigned int j = 0; j < num_samples; j++) {
      for (unsigned int i = 0; i < num_dims; i++) {
        points_minus_mean(i, j) = m_ShapeMatrix->operator()(i, j) - (*m_FixedMean)(i, 0);
      }
    }

    // Create Eigen maps for precomputed matrices (VNL uses row-major storage)
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> Y_fixed_map(
        m_FixedY->data_block(), num_dims, m_PrecomputedNumFixedSamples);
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> GradientBasis_map(
        m_FixedGradientBasis->data_block(), num_dims, m_PrecomputedNumFixedSamples);
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> pinvMat_map(
        m_FixedPinvMat->data_block(), m_PrecomputedNumFixedSamples, m_PrecomputedNumFixedSamples);

    const double N = m_PrecomputedNumFixedSamples - 1;
    const double N_sq = N * N;

    // For each non-fixed shape, compute gradient using Schur complement
    for (unsigned int j = 0; j < num_samples; j++) {
      // Check if this shape is fixed
      bool is_fixed = false;
      for (int idx : m_FixedDomainIndices) {
        if (idx == (int)j) {
          is_fixed = true;
          break;
        }
      }

      if (!is_fixed) {
        // Extract y_j
        Eigen::VectorXd y_j(num_dims);
        for (unsigned int i = 0; i < num_dims; i++) {
          y_j[i] = points_minus_mean(i, j);
        }

        // proj = Y_fixed^T * y_j
        Eigen::VectorXd proj = Y_fixed_map.transpose() * y_j;

        // grad = GradientBasis * proj
        Eigen::VectorXd grad = GradientBasis_map * proj;

        // pinv_proj = pinvMat_fixed * proj
        Eigen::VectorXd pinv_proj = pinvMat_map * proj;

        // Schur complement scalars
        double y_j_norm_sq = y_j.squaredNorm();
        double proj_pinv_proj = proj.dot(pinv_proj);

        // s = ||y_j||^2/N + α - proj^T * pinvMat * proj / N^2
        double s = y_j_norm_sq / N + m_MinimumVariance - proj_pinv_proj / N_sq;

        // Update = (y_j - grad / N) / s
        for (unsigned int i = 0; i < num_dims; i++) {
          m_PointsUpdate->put(i, j, (y_j[i] - grad[i] / N) / s);
        }
      } else {
        // Fixed shapes get zero gradient
        for (unsigned int i = 0; i < num_dims; i++) {
          m_PointsUpdate->put(i, j, 0.0);
        }
      }
    }

    // Compute energy using precomputed eigenvalues
    m_CurrentEnergy = 0.0;
    m_MinimumEigenValue = (*m_FixedW)(0) * (*m_FixedW)(0) + m_MinimumVariance;
    for (int i = 0; i < m_PrecomputedNumFixedSamples; i++) {
      double w_i = (*m_FixedW)(i);
      double val = w_i * w_i / (m_PrecomputedNumFixedSamples - 1) + m_MinimumVariance;
      if (val < m_MinimumEigenValue) m_MinimumEigenValue = val;
      m_CurrentEnergy += log(val);
    }
    m_CurrentEnergy /= 2.0;

    return;  // Done with fixed shape space path
  }

  // =========================================================================
  // ORIGINAL PATH: Full computation
  // =========================================================================
  TIME_SCOPE(m_UseMeanEnergy ? "ComputeCovarianceMatrix::MeanEnergy" : "ComputeCovarianceMatrix::Entropy");

  vnl_matrix_type points_minus_mean;
  points_minus_mean.clear();
  points_minus_mean.set_size(num_dims, num_samples);
  points_minus_mean.fill(0.0);

  m_points_mean->clear();
  m_points_mean->set_size(num_dims, 1);

  // Compute the covariance matrix.
  // (A is D' in Davies paper)
  // Compute the mean shape vector.
  double _total = 0.0;
  for (unsigned int j = 0; j < num_dims; j++) {
    double total = 0.0;
    for (unsigned int i = 0; i < num_samples; i++) {
      total += m_ShapeMatrix->operator()(j, i);
    }
    m_points_mean->put(j, 0, total / (double)num_samples);
    _total += total;
  }

  for (unsigned int j = 0; j < num_dims; j++) {
    for (unsigned int i = 0; i < num_samples; i++) {
      points_minus_mean(j, i) = m_ShapeMatrix->operator()(j, i) - m_points_mean->get(j, 0);
    }
  }

  vnl_diag_matrix<double> W;

  vnl_matrix_type gramMat(num_samples, num_samples, 0.0);
  vnl_matrix_type pinvMat(num_samples, num_samples, 0.0);  // gramMat inverse

  if (this->m_UseMeanEnergy) {
    pinvMat.set_identity();
  } else {
    gramMat = points_minus_mean.transpose() * points_minus_mean;

    vnl_svd<double> svd(gramMat);

    vnl_matrix_type UG = svd.U();
    W = svd.W();

    vnl_diag_matrix<double> invLambda = svd.W();

    invLambda.set_diagonal(invLambda.get_diagonal() / (double)(num_samples - 1) + m_MinimumVariance);
    invLambda.invert_in_place();

    pinvMat = (UG * invLambda) * UG.transpose();

    // Note: The full dM × dM inverse covariance matrix is NOT needed.
    // Per thesis equation 2.35, the gradient is simply: Y × (Y^T Y + αI)^{-1}
    // which is Y × pinvMat. No covariance matrix multiplication required.
  }
  m_PointsUpdate->update(points_minus_mean * pinvMat);

  //     std::cout << m_PointsUpdate.extract(num_dims, num_samples,0,0) << std::endl;

  m_CurrentEnergy = 0.0;

  if (m_UseMeanEnergy)
    m_CurrentEnergy = points_minus_mean.frobenius_norm();
  else {
    m_MinimumEigenValue = W(0) * W(0) + m_MinimumVariance;
    for (unsigned int i = 0; i < num_samples; i++) {
      double val_i = W(i) * W(i) + m_MinimumVariance;
      if (val_i < m_MinimumEigenValue) m_MinimumEigenValue = val_i;
      m_CurrentEnergy += log(val_i);
    }
  }

  m_CurrentEnergy /= 2.0;
  if (m_UseMeanEnergy) m_MinimumEigenValue = m_CurrentEnergy / 2.0;
}

LegacyCorrespondenceFunction::VectorType LegacyCorrespondenceFunction ::evaluate(unsigned int idx, unsigned int d,
                                                                                 const ParticleSystem* system,
                                                                                 double& maxdt, double& energy) const {
  // NOTE: This code requires that indices be contiguous, i.e. it won't work if
  // you start deleting particles.
  const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

  maxdt = m_MinimumEigenValue;

  VectorType gradE;
  unsigned int k = 0;
  int dom = d % DomainsPerShape;
  for (int i = 0; i < dom; i++) k += system->GetNumberOfParticles(i) * VDimension;
  k += idx * VDimension;

  // Per-particle energy: squared Euclidean distance from mean
  // This is used for line search step-size adaptation.
  // Note: We always use Euclidean distance (identity matrix) regardless of m_UseMeanEnergy,
  // because the full inverse covariance matrix is not computed.
  vnl_matrix_type Xi(3, 1, 0.0);
  Xi(0, 0) = m_ShapeMatrix->operator()(k, d / DomainsPerShape) - m_points_mean->get(k, 0);
  Xi(1, 0) = m_ShapeMatrix->operator()(k + 1, d / DomainsPerShape) - m_points_mean->get(k + 1, 0);
  Xi(2, 0) = m_ShapeMatrix->operator()(k + 2, d / DomainsPerShape) - m_points_mean->get(k + 2, 0);

  // energy = ||Xi||^2 = Xi^T * Xi
  vnl_matrix_type tmp = Xi.transpose() * Xi;
  energy = tmp(0, 0);

  for (unsigned int i = 0; i < VDimension; i++) {
    gradE[i] = m_PointsUpdate->get(k + i, d / DomainsPerShape);
  }

  return system->TransformVector(gradE, system->GetInversePrefixTransform(d) * system->GetInverseTransform(d));
}

//---------------------------------------------------------------------------
void LegacyCorrespondenceFunction::ClearPrecomputedFixedDomains() {
  m_HasPrecomputedFixedDomains = false;
  m_PrecomputedNumFixedSamples = 0;
  m_PrecomputedNumDims = 0;
  m_FixedY.reset();
  m_FixedMean.reset();
  m_FixedPinvMat.reset();
  m_FixedGradientBasis.reset();
  m_FixedW.reset();
  m_FixedDomainIndices.clear();
}

//---------------------------------------------------------------------------
void LegacyCorrespondenceFunction::PrecomputeForFixedDomains(const ParticleSystem* ps) {
  TIME_SCOPE("LegacyCorrespondenceFunction::PrecomputeForFixedDomains");

  // Clear any existing precomputed data
  ClearPrecomputedFixedDomains();

  // Count fixed and non-fixed shapes
  const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();
  int total_shapes = m_ShapeMatrix->cols();
  int num_fixed = 0;
  int num_unfixed = 0;

  SW_LOG("LegacyPrecompute: ShapeMatrix rows={}, cols={}, DomainsPerShape={}, PS domains={}",
         m_ShapeMatrix->rows(), total_shapes, DomainsPerShape, ps->GetNumberOfDomains());

  // Identify which shapes are fixed
  for (int shape = 0; shape < total_shapes; shape++) {
    bool shape_is_fixed = true;
    for (unsigned int d = 0; d < DomainsPerShape; d++) {
      int dom = shape * DomainsPerShape + d;
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
    SW_LOG("LegacyPrecompute: No benefit from precomputation (fixed={}, unfixed={})",
           num_fixed, num_unfixed);
    return;
  }

  SW_LOG("LegacyPrecompute: Precomputing for {} fixed shapes, {} unfixed shapes",
         num_fixed, num_unfixed);

  int num_dims = m_ShapeMatrix->rows();
  int num_fixed_samples = num_fixed;

  m_PrecomputedNumFixedSamples = num_fixed_samples;
  m_PrecomputedNumDims = num_dims;

  // Step 1: Extract fixed shape data and compute mean
  m_FixedY = std::make_shared<vnl_matrix_type>(num_dims, num_fixed_samples);
  m_FixedMean = std::make_shared<vnl_matrix_type>(num_dims, 1);
  m_FixedMean->fill(0.0);

  for (int i = 0; i < num_fixed_samples; i++) {
    int fixed_shape_idx = m_FixedDomainIndices[i];
    for (int r = 0; r < num_dims; r++) {
      double val = m_ShapeMatrix->operator()(r, fixed_shape_idx);
      m_FixedY->put(r, i, val);
      (*m_FixedMean)(r, 0) += val;
    }
  }

  // Compute mean
  for (int r = 0; r < num_dims; r++) {
    (*m_FixedMean)(r, 0) /= num_fixed_samples;
  }

  // Step 2: Center the fixed data
  for (int i = 0; i < num_fixed_samples; i++) {
    for (int r = 0; r < num_dims; r++) {
      (*m_FixedY)(r, i) -= (*m_FixedMean)(r, 0);
    }
  }

  // Step 3: Compute Gram matrix and SVD for fixed shapes
  vnl_matrix_type gramMat = m_FixedY->transpose() * (*m_FixedY);
  vnl_svd<double> svd(gramMat);

  vnl_matrix_type UG = svd.U();
  m_FixedW = std::make_shared<vnl_diag_matrix<double>>(svd.W());

  // Step 4: Compute regularized pseudoinverse
  vnl_diag_matrix<double> invLambda = svd.W();
  invLambda.set_diagonal(invLambda.get_diagonal() / (double)(num_fixed_samples - 1) + m_MinimumVariance);
  invLambda.invert_in_place();

  m_FixedPinvMat = std::make_shared<vnl_matrix_type>((UG * invLambda) * UG.transpose());

  // Step 5: Precompute gradient basis
  m_FixedGradientBasis = std::make_shared<vnl_matrix_type>((*m_FixedY) * (*m_FixedPinvMat));

  m_HasPrecomputedFixedDomains = true;

  SW_LOG("LegacyPrecompute: Precomputation complete. Fixed shapes: {}, Dimensions: {}",
         num_fixed_samples, num_dims);
}

}  // namespace shapeworks
