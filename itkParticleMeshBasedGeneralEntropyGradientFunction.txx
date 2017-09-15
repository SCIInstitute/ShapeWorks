/*---
 * Author - Praful Agrawal
 ---*/

#ifndef __itkParticleMeshBasedGeneralEntropyGradientFunction_txx
#define __itkParticleMeshBasedGeneralEntropyGradientFunction_txx

#include "TriMesh.h"
#include <math.h>
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleImplicitSurfaceDomain.h"

#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithHessians.h"

namespace itk
{
template <unsigned int VDimension>
void
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::ComputeUpdates()
{
    int rows = 0;
    for (int i = 0; i < m_DomainsPerShape; i++)
        rows += VDimension*this->m_ParticleSystem->GetNumberOfParticles(i);

    // Do we need to resize the update matrix?
    if (m_PointsUpdate.rows() != rows
            || m_PointsUpdate.cols() != num_samples)
        m_PointsUpdate.set_size(rows, num_samples);

    m_PointsUpdate.fill(0.0);

    vnl_matrix_type points_minus_mean = GetYMatrix(this->m_ParticleSystem);

    m_mean.set_size(num_dims);
    m_mean.fill(0.0);

    for (unsigned int i = 0; i < num_dims; i++)
    {
        for (unsigned int j = 0; j < num_samples; j++)
            m_mean(i) += points_minus_mean(i,j)/(double)num_samples;
    }

    for (unsigned int i = 0; i < points_minus_mean.rows(); i++)
    {
        for (unsigned int j = 0; j < points_minus_mean.cols(); j++)
            points_minus_mean(i,j) = points_minus_mean(i,j) - m_mean(i);
    }

    vnl_matrix_type pinvMat(num_samples, num_samples, 0.0);

    // Compute the covariance in the dual space (transposed shape matrix)
    vnl_matrix_type A =  points_minus_mean.transpose() * points_minus_mean * (1.0/((double)(num_samples-1)));

    // Regularize A
    for (unsigned int i = 0; i < num_samples; i++)
        A(i, i) = A(i, i) + m_MinimumVariance;

    // Find inverse of covariance matrix
    vnl_symmetric_eigensystem<float> symEigen(A);

    if (this->m_UseMeanEnergy)
        pinvMat.set_identity();
    else
        pinvMat = symEigen.pinverse().transpose();

//    std::cout << points_minus_mean.extract(points_minus_mean.rows(), points_minus_mean.cols()) << std::endl;
    vnl_matrix_type Q = points_minus_mean * pinvMat;

    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).
    vnl_matrix_type J;
    vnl_vector_type v;

    for (unsigned int j = 0; j < num_samples; j++)
    {
        int i = 0;
        int num = 0;
        int num2 = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;

            const ParticleImplicitSurfaceDomain<float, 3>* domain
                    = static_cast<const ParticleImplicitSurfaceDomain<float ,3>*>(this->m_ParticleSystem->GetDomain(dom));

            const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
                 = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(this->m_ParticleSystem->GetDomain(dom));

            const ParticleImageDomainWithHessians<float, 3> * domainWithHess
                 = static_cast<const ParticleImageDomainWithHessians<float ,3> *>(this->m_ParticleSystem->GetDomain(dom));

            TriMesh *ptr = domain->GetMesh();
            if (d > 0)
            {
                num += m_AttributesPerDomain[d-1];
                if (m_UseXYZ[d-1])
                    num += 3;
                if (m_UseNormals[d-1])
                    num += 3;

                num2 += this->m_ParticleSystem->GetNumberOfParticles(d-1)*VDimension;
            }
            for (unsigned int p = 0; p < this->m_ParticleSystem->GetNumberOfParticles(dom); p++)
            {
                int num_attr = m_AttributesPerDomain[d];
                if (m_UseXYZ[d])
                    num_attr += 3;
                if (m_UseNormals[d])
                    num_attr += 3;

                J.clear();
                J.set_size(num_attr, VDimension);
                J.fill(0.0);
                v.clear();
                v.set_size(num_attr);
                v.fill(0.0);
                PointType pt_ps = this->m_ParticleSystem->GetPosition(p, dom);
                point pt;
                pt.clear();
                pt[0] = pt_ps[0];
                pt[1] = pt_ps[1];
                pt[2] = pt_ps[2];

                int s = 0;
                if (m_UseXYZ[d])
                {
                    for (unsigned int c = s; c < s+VDimension; c++)
                    {
                        for (unsigned int vd = 0; vd < VDimension; vd++)
                        {
                            if (vd == c-s)
                                J(c, vd) = 1.0*m_AttributeScales[num+c];
                            else
                                J(c, vd) = 0.0;
                        }
                        v(c) = Q(i++, j);
                    }
                    s += VDimension;
                }
                if (m_UseNormals[d])
                {
                    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleGradientVnl(pt_ps);
                    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pN = pG.normalize();
                    float grad_mag = pG.magnitude();

                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType pH = domainWithHess->SampleHessianVnl(pt_ps);

                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat1;
                    mat1.set_identity();
                    vnl_matrix_type nrml(VDimension, 1);
                    nrml.fill(0.0);
                    nrml(0,0) = pN[0]; nrml(1,0) = pN[1]; nrml(2,0) = pN[2];
                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat2 = nrml * nrml.transpose();

                    for (unsigned int x1 = 0; x1 < VDimension; x1++)
                    {
                        for (unsigned int x2 = 0; x2 < VDimension; x2++)
                        {
                            mat1(x1, x2) -= mat2(x1, x2);
                            pH(x1, x2)   /= grad_mag;
                        }
                    }

                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat3 = pH * mat1;

                    // mat3 = H/|grad_f| * (I - n*n');
                    for (unsigned int c = s; c < s+VDimension; c++)
                    {
                        for (unsigned int vd = 0; vd < VDimension; vd++)
                            J(c, vd) = mat3(c-s, vd)*m_AttributeScales[num+c];
                        v(c) = Q(i++, j);
                    }
                    s += VDimension;
                }

                for (unsigned int c = s; c < s+m_AttributesPerDomain[d]; c++)
                {
                    point dc;
                    dc.clear();
                    dc = ptr->GetFeatureDerivative(pt, c);
                    for (unsigned int vd = 0; vd < VDimension; vd++)
                        J(c, vd) = dc[vd]*m_AttributeScales[num+c];
                    v(c) = Q(i++,j);
                }
                s += m_AttributesPerDomain[d];
                if (s!=num_attr)
                    std::cerr << "Inconsistency in mesh based fea" << std::endl;

                vnl_vector_type dx = J.transpose() * v;
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    m_PointsUpdate(num2 + p*VDimension + vd, j) = dx[vd];
            }
        }
    }

//    std::cout << m_PointsUpdate.extract(num_dims, num_samples,0,0) << std::endl;

    m_CurrentEnergy = 0.0;

    if (m_UseMeanEnergy)
        m_CurrentEnergy = points_minus_mean.frobenius_norm();
    else
    {
        m_MinimumEigenValue = symEigen.D(0, 0);
        for (unsigned int i = 0; i < num_samples; i++)
        {
            if (symEigen.D(i, i) < m_MinimumEigenValue)
                m_MinimumEigenValue = symEigen.D(i, i);
            m_CurrentEnergy += log(symEigen.D(i,i));
        }
    }
    m_CurrentEnergy /= 2.0;

    if (!m_UseMeanEnergy)
    {
        for (unsigned int i =0; i < num_samples; i++)
            std::cout << i << ": "<< symEigen.D(i, i) - m_MinimumVariance << std::endl;

        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = "
                  << m_MinimumVariance <<  std::endl;
    }
    else
        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << std::endl;

}

template <unsigned int VDimension>
typename ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>::vnl_matrix_type
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::GetYMatrix(const ParticleSystemType *c) const
{
    vnl_matrix_type points_minus_mean(num_dims, num_samples);

    std::vector<float> fVals;
    // Compute the shape vector. Y
    for (unsigned int j = 0; j < num_samples; j++)
    {
        int i = 0;
        int num = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;

            const ParticleImplicitSurfaceDomain<float, 3>* domain
                    = static_cast<const ParticleImplicitSurfaceDomain<float ,3>*>(c->GetDomain(dom));

            const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
                 = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(this->m_ParticleSystem->GetDomain(dom));

            TriMesh *ptr = domain->GetMesh();

            if (d > 0)
            {
                num += m_AttributesPerDomain[d-1];
                if (m_UseXYZ[d-1])
                    num += 3;
                if (m_UseNormals[d-1])
                    num += 3;
            }

            for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++)
            {
                PointType pt_ps = c->GetPosition(p, dom);
                point pt;
                pt.clear();
                pt[0] = pt_ps[0];
                pt[1] = pt_ps[1];
                pt[2] = pt_ps[2];
                fVals.clear();
                if (m_AttributesPerDomain[d] > 0)
                    ptr->GetFeatureValues(pt, fVals);

                if (fVals.size() != m_AttributesPerDomain[d])
                    std::cerr << "Unexpected Inconsistency in number of attributes... check" << std::endl;

                int s = 0;
                if (m_UseXYZ[d])
                {
                    PointType wpt = c->GetPosition(p, dom);
//                    PointType wpt = c->GetTransformedPosition(p, dom); -- fix gradient first
                    points_minus_mean(i++, j) = wpt[0]*m_AttributeScales[num+0];
                    points_minus_mean(i++, j) = wpt[1]*m_AttributeScales[num+1];
                    points_minus_mean(i++, j) = wpt[2]*m_AttributeScales[num+2];
                    s = 3;
                }

                if (m_UseNormals[d])
                {
                    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleNormalVnl(pt_ps);
                    VectorType pN;
                    pN[0] = pG[0]; pN[1] = pG[1]; pN[2] = pG[2];
//                    pN = c->TransformVector(pN, c->GetTransform(d) * c->GetPrefixTransform(d));
                    points_minus_mean(i++, j) = pG[0]*m_AttributeScales[num+0+s];
                    points_minus_mean(i++, j) = pG[1]*m_AttributeScales[num+1+s];
                    points_minus_mean(i++, j) = pG[2]*m_AttributeScales[num+2+s];
                    s = 6;
                }

                for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
                   points_minus_mean(i++, j) = fVals[c]*m_AttributeScales[num+c+s];
            }
        }
        if (i != num_dims)
            std::cerr << "Bug in input data while creating feature matrix... check" << std::endl;
    }
    return points_minus_mean;
}


template <unsigned int VDimension>
typename ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>::VectorType
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxdt, double &energy) const
{
    // NOTE: This code requires that indices be contiguous, i.e. it won't work if
    // you start deleting particles.
    int dom = d % m_DomainsPerShape;
    int sampNum = d/m_DomainsPerShape;

    energy = m_CurrentEnergy;
    maxdt = m_MinimumEigenValue;

    int num = 0;
    for (int i = 0; i < dom; i++)
        num += system->GetNumberOfParticles(i)*VDimension;

    VectorType gradE;
    unsigned int k = idx * VDimension + num;
    for (unsigned int i = 0; i< VDimension; i++)
        gradE[i] = m_PointsUpdate(k + i, sampNum);

//    if (idx == 0 ) std::cout << "maxdt= " << maxdt << " idx = " << idx << "\t" << "GradE = " << gradE << std::endl;
//    return system->TransformVector(gradE, system->GetInverseTransform(d));
    return gradE;
}

} // end namespace
#endif
