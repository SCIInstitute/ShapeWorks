
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

  Eigen::MatrixXd points_minus_mean(num_dims, num_samples);

  m_points_mean->clear();
  m_points_mean->set_size(num_dims, 1);
  m_points_mean->fill(0.0);

  for (unsigned int i = 0; i < num_dims; i++) {
    m_points_mean->put(i, 0, (m_ShapeData->get_n_rows(i, 1)).mean());
    points_minus_mean.row(i) = Eigen::Map<const Eigen::VectorXd>(
                                   m_ShapeData->get_row(i).data_block(), num_samples).array() - m_points_mean->get(i, 0);
  }

  Eigen::VectorXd W;

  Eigen::MatrixXd gramMat(num_samples, num_samples);
  Eigen::MatrixXd pinvMat(num_samples, num_samples);

  if (this->m_UseMeanEnergy) {
    pinvMat.setIdentity();
    m_InverseCovMatrix->setZero();
  } else {
    TIME_START("correspondence::gramMat");
    gramMat = points_minus_mean.transpose() * points_minus_mean;
    TIME_STOP("correspondence::gramMat");

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(gramMat, Eigen::ComputeFullU | Eigen::ComputeFullV);

    Eigen::MatrixXd UG = svd.matrixU();
    W = svd.singularValues();

    Eigen::VectorXd invLambda = W;
    invLambda = (invLambda.array() / (double)(num_samples - 1) + m_MinimumVariance).inverse();

    pinvMat = UG * invLambda.asDiagonal() * UG.transpose();

    Eigen::MatrixXd projMat = points_minus_mean * UG;
    const Eigen::MatrixXd lhs = projMat * invLambda.asDiagonal();
    const Eigen::MatrixXd rhs = invLambda.asDiagonal() * projMat.transpose();

    // resize the inverse covariance matrix if necessary
    if (m_InverseCovMatrix->rows() != num_dims || m_InverseCovMatrix->cols() != num_dims) {
      m_InverseCovMatrix->resize(num_dims, num_dims);
    }
    TIME_START("correspondence::covariance_multiply");
    m_InverseCovMatrix->noalias() = lhs * rhs;
    TIME_STOP("correspondence::covariance_multiply");
  }

  Eigen::MatrixXd Q = points_minus_mean * pinvMat;

  // Compute the update matrix in coordinate space by multiplication with the
  // Jacobian.  Each shape gradient must be transformed by a different Jacobian
  // so we have to do this individually for each shape (sample).

  {
    Eigen::MatrixXd Jmatrix;
    Eigen::MatrixXd v;
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

          Jmatrix.resize(num_attr, VDimension);
          Jmatrix.setZero();
          v.resize(num_attr, 1);
          v.setZero();

          for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++) {
            // Extract from Eigen Q matrix
            v = Q.block(num + p * num_attr, j, num_attr, 1);

            // Map VNL matrix data to Eigen for extraction
            Eigen::Map<const Eigen::MatrixXd> shapeGradMap(
                m_ShapeGradient->data_block(), m_ShapeGradient->rows(), m_ShapeGradient->cols());
            Jmatrix = shapeGradMap.block(num + p * num_attr, 3 * j, num_attr, 3);

            Eigen::MatrixXd dx = Jmatrix.transpose() * v;
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
    m_CurrentEnergy = points_minus_mean.norm();
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


CorrespondenceFunction::VectorType CorrespondenceFunction::Evaluate(unsigned int idx, unsigned int d,
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

  vnl_matrix_type tmp1(sz_Yidx, sz_Yidx, 0.0);

  if (this->m_UseMeanEnergy) {
    tmp1.set_identity();
  } else {
    // extract 3x3 submatrix at k,k
    Eigen::MatrixXd region = m_InverseCovMatrix->block(sz_Yidx, sz_Yidx, 3, 3);
    // convert to vnl
    for (unsigned int i = 0; i < 3; i++) {
      for (unsigned int j = 0; j < 3; j++) {
        tmp1(i, j) = region(i, j);
      }
    }
  }

  vnl_matrix_type Y_dom_idx(sz_Yidx, 1, 0.0);

  Y_dom_idx = m_ShapeData->extract(sz_Yidx, 1, num, sampNum) - m_points_mean->extract(sz_Yidx, 1, num);

  vnl_matrix_type tmp = Y_dom_idx.transpose() * tmp1;
  tmp *= Y_dom_idx;

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

}  // namespace shapeworks
