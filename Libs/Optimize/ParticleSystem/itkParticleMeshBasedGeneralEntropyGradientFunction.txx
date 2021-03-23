/*---
 * Author - Praful Agrawal
 ---*/

#ifndef __itkParticleMeshBasedGeneralEntropyGradientFunction_txx
#define __itkParticleMeshBasedGeneralEntropyGradientFunction_txx

#include "TriMesh.h"
#include <math.h>
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_diag_matrix.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include <ctime>
#include <time.h>
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithGradN.h"
#include "Libs/Utils/Utils.h"
#include <Eigen/SVD>

namespace itk
{
template <unsigned int VDimension>
void
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::ComputeUpdates(const ParticleSystemType *c)
{
    num_dims = m_ShapeData->rows();
    num_samples = m_ShapeData->cols();

     int rows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++)
        rows += VDimension*c->GetNumberOfParticles(i);

    // Do we need to resize the update matrix?
    if (m_PointsUpdate->rows() != rows
            || m_PointsUpdate->cols() != num_samples)
        m_PointsUpdate->set_size(rows, num_samples);

    m_PointsUpdate->fill(0.0);

    vnl_matrix_type points_minus_mean(num_dims, num_samples, 0.0);

    m_points_mean->clear();
    m_points_mean->set_size(num_dims,1);
    m_points_mean->fill(0.0);

    for (unsigned int i = 0; i < num_dims; i++)
    {
        m_points_mean->put(i,0, (m_ShapeData->get_n_rows(i, 1)).mean());
        points_minus_mean.set_row(i, m_ShapeData->get_row(i) - m_points_mean->get(i,0) );
    }

    Eigen::JacobiSVD<Eigen::MatrixXd>::SingularValuesType W;
    vnl_matrix_type pinvMat(num_samples, num_samples, 0.0); //gramMat inverse

    if (this->m_UseMeanEnergy)
    {
      pinvMat.set_identity();
      m_InverseCovMatrix->clear();
    }
    else
    {
      using RowMajorMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
      Eigen::Map<RowMajorMatrix> e_points_minus_mean(points_minus_mean.data_block(),
                                                     points_minus_mean.rows(), points_minus_mean.cols());
      Eigen::Map<RowMajorMatrix> e_pinvMat(pinvMat.data_block(),
                                           pinvMat.rows(), pinvMat.cols());
      m_InverseCovMatrix->set_size(num_dims, num_dims);
      Eigen::Map<RowMajorMatrix> e_invCov(m_InverseCovMatrix->data_block(),
                                          m_InverseCovMatrix->rows(), m_InverseCovMatrix->cols());

      const auto gram_mat = e_points_minus_mean.transpose() * e_points_minus_mean;
      const Eigen::JacobiSVD<RowMajorMatrix> svd(gram_mat, Eigen::ComputeFullU);
      const auto& U = svd.matrixU();
      W = svd.singularValues();

      const auto inv_lambda = (W.array() / (double)(num_samples - 1) + m_MinimumVariance).inverse().matrix();
      e_pinvMat.noalias() = U * inv_lambda * U.transpose();

      const auto proj_mat = e_points_minus_mean * U;
      const auto lhs = proj_mat * inv_lambda;
      const auto rhs = inv_lambda * proj_mat.transpose(); // invLambda doesn't need to be transposed since its a diagonal matrix
      e_invCov.noalias() = lhs * rhs;
    }

    vnl_matrix_type Q = points_minus_mean * pinvMat;

    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).

    {
    vnl_matrix_type Jmatrix;
    vnl_matrix_type v;
    for (int j = 0; j < num_samples; j++)
    {
        int num = 0;
        int num2 = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;
            if (c->GetDomainFlag(dom) == false)
            {
                if (d > 0)
                {
                    num += m_AttributesPerDomain[d-1]*c->GetNumberOfParticles(d-1);
                    if (m_UseXYZ[d-1])
                        num += 3*c->GetNumberOfParticles(d-1);
                    if (m_UseNormals[d-1])
                        num += 3*c->GetNumberOfParticles(d-1);

                    num2 += c->GetNumberOfParticles(d-1)*VDimension;
                }

                int num_attr = m_AttributesPerDomain[d];
                if (m_UseXYZ[d])
                    num_attr += 3;
                if (m_UseNormals[d])
                    num_attr += 3;

                Jmatrix.clear();
                Jmatrix.set_size(num_attr, VDimension);
                Jmatrix.fill(0.0);
                v.clear();
                v.set_size(num_attr, 1);
                v.fill(0.0);

                for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++)
                {
                    v = Q.extract( num_attr, 1, num + p*num_attr, j );
                    Jmatrix = m_ShapeGradient->extract( num_attr, 3, num + p*num_attr, 3*j );

                    vnl_matrix_type dx = Jmatrix.transpose() * v;
                    for (unsigned int vd = 0; vd < VDimension; vd++)
                        m_PointsUpdate->put(num2 + p*VDimension + vd, j, dx(vd, 0));
                }
            }
        }
    }
    }

    m_CurrentEnergy = 0.0;

    if (m_UseMeanEnergy)
        m_CurrentEnergy = points_minus_mean.frobenius_norm();
    else
    {
        m_MinimumEigenValue = W(0)*W(0) + m_MinimumVariance;
        for (unsigned int i = 0; i < num_samples; i++)
        {
            double val_i = W(i)*W(i) + m_MinimumVariance;
            if ( val_i < m_MinimumEigenValue)
                m_MinimumEigenValue = val_i;
            m_CurrentEnergy += log(val_i);
        }
    }

    m_CurrentEnergy /= 2.0;

    if (m_UseMeanEnergy)
        m_MinimumEigenValue = m_CurrentEnergy / 2.0;
}

template <unsigned int VDimension>
typename ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>::VectorType
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    int dom = d % m_DomainsPerShape; //domain number within shape
    int sampNum = d/m_DomainsPerShape; //shape number

    int sz_Yidx = 0;
    if (m_UseXYZ[dom])
        sz_Yidx += 3;
    if (m_UseNormals[dom])
        sz_Yidx += 3;
    sz_Yidx += m_AttributesPerDomain[dom];

    int num = sz_Yidx * idx;

    for (unsigned int i = 0; i < dom; i++)
    {
        int num1 = 0;
        if (m_UseXYZ[i])
            num1 += 3;
        if (m_UseNormals[i])
            num1 += 3;
        num1 += m_AttributesPerDomain[i];

        num += num1 * system->GetNumberOfParticles(i);
    }

    vnl_matrix_type tmp1(sz_Yidx, sz_Yidx, 0.0);

    if (this->m_UseMeanEnergy)
        tmp1.set_identity();
    else
        tmp1 = m_InverseCovMatrix->extract(sz_Yidx, sz_Yidx, num, num);

    vnl_matrix_type Y_dom_idx(sz_Yidx, 1, 0.0);

    Y_dom_idx = m_ShapeData->extract(sz_Yidx, 1, num, sampNum) - m_points_mean->extract(sz_Yidx, 1, num);

    vnl_matrix_type tmp = Y_dom_idx.transpose()*tmp1;
    tmp *= Y_dom_idx;

    energy = tmp(0,0);


    maxdt = m_MinimumEigenValue;

    num = 0;
    for (int i = 0; i < dom; i++)
        num += system->GetNumberOfParticles(i)*VDimension;

    VectorType gradE;
    unsigned int k = idx * VDimension + num;
    for (unsigned int i = 0; i< VDimension; i++)
        gradE[i] = m_PointsUpdate->get(k + i, sampNum);

    return system->TransformVector(gradE, system->GetInversePrefixTransform(d) * system->GetInverseTransform(d));
}

} // end namespace
#endif
