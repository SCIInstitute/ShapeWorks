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
#include "itkParticleImageDomainWithHessians.h"
#include "Libs/Utils/Utils.h"

namespace itk
{
template <unsigned int VDimension>
void
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::ComputeUpdates(const ParticleSystemType *c)
{
  std::cerr << "@@@ ComputeUpdates\n";
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

//    if (this->CheckForNans(points_minus_mean))
//        std::cout << "MGEG: 1. Nans exist!!!" << std::endl;
//    else
//        std::cout << "MGEG: 1. No Nans!!!" << std::endl;

    vnl_diag_matrix<double> W;

    vnl_matrix_type gramMat(num_samples, num_samples, 0.0);
    vnl_matrix_type pinvMat(num_samples, num_samples, 0.0); //gramMat inverse

    if (this->m_UseMeanEnergy)
    {
      std::cerr << "@@@ Using mean energy\n";
        pinvMat.set_identity();
        m_InverseCovMatrix->clear();
    }
    else
    {
      std::cerr << "@@@ Not using mean energy\n";
//        vnl_svd <double> svd(points_minus_mean);

//        vnl_matrix_type U = svd.U();
//        vnl_matrix_type V = svd.V();

//        W = svd.W();

//        vnl_diag_matrix<double> invLambda = svd.W()*svd.W();

//        invLambda.set_diagonal(invLambda.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance);
//        invLambda.invert_in_place();

//        pinvMat = (V * invLambda) * V.transpose();
//        m_InverseCovMatrix = (U * invLambda) * U.transpose();

        gramMat = points_minus_mean.transpose()* points_minus_mean;

        vnl_svd <double> svd(gramMat);

        vnl_matrix_type UG = svd.U();
        W = svd.W();

        vnl_diag_matrix<double> invLambda = svd.W();
        invLambda.set_diagonal(invLambda.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance);
        invLambda.invert_in_place();

        pinvMat = (UG * invLambda) * UG.transpose();

        vnl_matrix_type projMat = points_minus_mean * UG;
        const auto lhs = projMat * invLambda;
        const auto rhs = invLambda * projMat.transpose(); // invLambda doesn't need to be transposed since its a diagonal matrix
        m_InverseCovMatrix->set_size(num_dims, num_dims);
        Utils::multiply_into(*m_InverseCovMatrix, lhs, rhs);
    }

    vnl_matrix_type Q = points_minus_mean * pinvMat;

//    if (this->CheckForNans(Q))
//        std::cout << "MGEG: 2. Nans exist!!!" << std::endl;
//    else
//        std::cout << "MGEG: 2. No Nans!!!" << std::endl;


    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).

#pragma omp parallel
    {
    vnl_matrix_type Jmatrix;
    vnl_matrix_type v;
#pragma omp for
    for (int j = 0; j < num_samples; j++)
    {
        int tid = 1;
        int num_threads = 1;

#ifdef SW_USE_OPENMP
        tid = omp_get_thread_num() + 1;
        num_threads = omp_get_num_threads();
#endif /* SW_USE_OPENMP */

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

//    if (this->CheckForNans(m_PointsUpdate))
//        std::cout << "MGEG: 3. Nans exist!!!" << std::endl;
//    else
//        std::cout << "MGEG: 3. No Nans!!!" << std::endl;

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

//    if (!m_UseMeanEnergy)
//    {
//        for (unsigned int i =0; i < num_samples; i++)
//            std::cout << i << ": "<< W(i)*W(i) << std::endl;

//        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = " << m_MinimumVariance << std::endl;
//    }
//    else
//    {
//        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << std::endl;
//    }

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
