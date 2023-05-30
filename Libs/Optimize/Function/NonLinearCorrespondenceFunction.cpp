

#include "NonLinearCorrespondenceFunction.h"

#include <string>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Utils/ParticleGaussianModeWriter.h"
#include "Libs/Utils/Utils.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace shapeworks {
void NonLinearCorrespondenceFunction ::WriteModes(const std::string& prefix, int n) const {
  typename ParticleGaussianModeWriter<VDimension>::Pointer writer = ParticleGaussianModeWriter<VDimension>::New();
  writer->SetShapeMatrix(m_ShapeMatrix);
  writer->SetFileName(prefix.c_str());
  writer->SetNumberOfModes(n);
  writer->Update();
}

void NonLinearCorrespondenceFunction ::ComputeLatentSpaceGradientUpdates() {

  const unsigned int num_samples = m_ShapeMatrix->cols();
  const unsigned int num_dims = m_LatentDimensions;

  if (m_PointsUpdate->rows() != num_dims || m_PointsUpdate->cols() != num_samples) {
    m_PointsUpdate->set_size(num_dims, num_samples);
    m_PointsUpdate->fill(0.0);
  }
  vnl_matrix_type points_minus_mean;
  points_minus_mean.clear();
  points_minus_mean.set_size(num_dims, num_samples);
  points_minus_mean.fill(0.0);

  m_points_mean_latent->clear();
  m_points_mean_latent->set_size(num_dims, 1);

  // mean for non linear space
  int full_num_dims = m_ShapeMatrix->rows();
  for (unsigned int j = 0; j < full_num_dims; j++) {
    double total = 0.0;
    for (unsigned int i = 0; i < num_samples; i++) {
      total += m_ShapeMatrix->operator()(j, i);
    }
    m_points_mean->put(j, 0, total / (double)num_samples);
  }
  // mean for gaussian latent space
  double _total = 0.0;
  for (unsigned int j = 0; j < num_dims; j++) {
    double total = 0.0;
    for (unsigned int i = 0; i < num_samples; i++) {
      total += m_LatentShapeMatrix->operator()(j, i);
    }
    m_points_mean_latent->put(j, 0, total / (double)num_samples);
    _total += total;
  }

  for (unsigned int j = 0; j < num_dims; j++) {
    for (unsigned int i = 0; i < num_samples; i++) {
      points_minus_mean(j, i) = m_LatentShapeMatrix->operator()(j, i) - m_points_mean_latent->get(j, 0);
    }
  }

  vnl_diag_matrix<double> W;

  vnl_matrix_type gramMat(num_samples, num_samples, 0.0);
  vnl_matrix_type pinvMat(num_samples, num_samples, 0.0);  // gramMat inverse

  if (this->m_UseMeanEnergy) {
    pinvMat.set_identity();
    m_InverseCovMatrix->clear();
  } else {
    gramMat = points_minus_mean.transpose() * points_minus_mean;

    vnl_svd<double> svd(gramMat);

    vnl_matrix_type UG = svd.U();
    W = svd.W();

    vnl_diag_matrix<double> invLambda = svd.W();

    invLambda.set_diagonal(invLambda.get_diagonal() / (double)(num_samples - 1) + m_MinimumVariance);
    invLambda.invert_in_place();

    pinvMat = (UG * invLambda) * UG.transpose();

    vnl_matrix_type projMat = points_minus_mean * UG;
    const auto lhs = projMat * invLambda;
    const auto rhs =
        invLambda * projMat.transpose();  // invLambda doesn't need to be transposed since its a diagonal matrix
    m_InverseCovMatrix->set_size(num_dims, num_dims);
    Utils::multiply_into(*m_InverseCovMatrix, lhs, rhs);
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

void NonLinearCorrespondenceFunction ::ComputeCovarianceMatrix() {
  // NOTE: This code requires that indices be contiguous, i.e. it wont work if
  // you start deleting particles.
  const unsigned int num_samples = m_ShapeMatrix->cols();
  const unsigned int num_dims = m_ShapeMatrix->rows();

  // Do we need to resize the covariance matrix?
  if (m_PointsUpdate->rows() != num_dims || m_PointsUpdate->cols() != num_samples) {
    m_PointsUpdate->set_size(num_dims, num_samples);
    m_PointsUpdate->fill(0.0);
  }
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
    m_InverseCovMatrix->clear();
  } else {
    gramMat = points_minus_mean.transpose() * points_minus_mean;

    vnl_svd<double> svd(gramMat);

    vnl_matrix_type UG = svd.U();
    W = svd.W();

    vnl_diag_matrix<double> invLambda = svd.W();

    invLambda.set_diagonal(invLambda.get_diagonal() / (double)(num_samples - 1) + m_MinimumVariance);
    invLambda.invert_in_place();

    pinvMat = (UG * invLambda) * UG.transpose();

    vnl_matrix_type projMat = points_minus_mean * UG;
    const auto lhs = projMat * invLambda;
    const auto rhs =
        invLambda * projMat.transpose();  // invLambda doesn't need to be transposed since its a diagonal matrix
    m_InverseCovMatrix->set_size(num_dims, num_dims);
    Utils::multiply_into(*m_InverseCovMatrix, lhs, rhs);
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

NonLinearCorrespondenceFunction::VectorType NonLinearCorrespondenceFunction ::Evaluate(unsigned int idx, unsigned int d,
                                                                                       const ParticleSystem* system,
                                                                                       double& maxdt,
                                                                                       double& energy) const {
  const unsigned int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();

  maxdt = m_non_linear_max_move;

  VectorType gradE;
  unsigned int k = 0;
  int dom = d % DomainsPerShape;
  for (int i = 0; i < dom; i++) k += system->GetNumberOfParticles(i) * VDimension;
  k += idx * VDimension;

  vnl_matrix_type Xi(3, 1, 0.0);
  Xi(0, 0) = m_ShapeMatrix->operator()(k, d / DomainsPerShape) - m_points_mean->get(k, 0);
  Xi(1, 0) = m_ShapeMatrix->operator()(k + 1, d / DomainsPerShape) - m_points_mean->get(k + 1, 0);
  Xi(2, 0) = m_ShapeMatrix->operator()(k + 2, d / DomainsPerShape) - m_points_mean->get(k + 2, 0);

  vnl_matrix_type tmp1(3, 3, 0.0);

  tmp1.set_identity(); // TODO: Try other strategies

  vnl_matrix_type tmp = Xi.transpose() * tmp1;

  tmp *= Xi;

  energy = tmp(0, 0);

  for (unsigned int i = 0; i < VDimension; i++) {
    gradE[i] = m_PointsUpdateNonLinear->get(k + i, d / DomainsPerShape);
  }

  return system->TransformVector(gradE, system->GetInversePrefixTransform(d) * system->GetInverseTransform(d));
}

}  // namespace shapeworks
