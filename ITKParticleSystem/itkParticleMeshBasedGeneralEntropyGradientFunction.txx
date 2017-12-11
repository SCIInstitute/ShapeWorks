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
    if (m_PointsUpdate.rows() != rows
            || m_PointsUpdate.cols() != num_samples)
        m_PointsUpdate.set_size(rows, num_samples);

    m_PointsUpdate.fill(0.0);

    vnl_matrix_type points_minus_mean(num_dims, num_samples, 0.0);

    m_mean.clear();
    m_mean.set_size(num_dims,1);
    m_mean.fill(0.0);

    for (unsigned int i = 0; i < num_dims; i++)
    {
        m_mean(i,0) = (m_ShapeData->get_n_rows(i, 1)).mean();
        points_minus_mean.set_row(i, m_ShapeData->get_row(i) - m_mean(i,0) );
    }

    vnl_diag_matrix<double> W;

    m_InverseCovMatrix.set_size(num_dims, num_dims);
    m_InverseCovMatrix.fill(0.0);
    vnl_matrix_type pinvMat(num_samples, num_samples, 0.0);

    if (this->m_UseMeanEnergy)
    {
        pinvMat.set_identity();
        m_InverseCovMatrix.set_identity();
    }
    else
    {
        vnl_svd <double> svd(points_minus_mean);

        vnl_matrix_type U = svd.U();
        vnl_matrix_type V = svd.V();

        W = svd.W();

        vnl_diag_matrix<double> invLambda = svd.W()*svd.W();

        invLambda.set_diagonal(invLambda.get_diagonal()/(double)(num_samples-1) + m_MinimumVariance);
        invLambda.invert_in_place();

        pinvMat = (V * invLambda) * V.transpose();
        m_InverseCovMatrix = (U * invLambda) * U.transpose();
    }

    vnl_matrix_type Q = points_minus_mean * pinvMat;

    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).
    vnl_matrix_type J;
    vnl_matrix_type v;

    for (unsigned int j = 0; j < num_samples; j++)
    {
        int num = 0;
        int num2 = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;
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

            J.clear();
            J.set_size(num_attr, VDimension);
            J.fill(0.0);
            v.clear();
            v.set_size(num_attr, 1);
            v.fill(0.0);

            for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++)
            {
                v = Q.extract( num_attr, 1, num + p*num_attr, j );
                J = m_ShapeGradient->extract( num_attr, 3, num + p*num_attr, 3*j );

                vnl_matrix_type dx = J.transpose() * v;
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    m_PointsUpdate(num2 + p*VDimension + vd, j) = dx(vd, 0);
            }
        }
    }


    m_CurrentEnergy = 0.0;

    if (m_UseMeanEnergy)
        m_CurrentEnergy = points_minus_mean.frobenius_norm();
    else
    {
        m_MinimumEigenValue = W(0)*W(0) + m_MinimumVariance; //symEigen.D(0, 0);
        for (unsigned int i = 0; i < num_samples; i++)
        {
            double val_i = W(i)*W(i) + m_MinimumVariance;
            if ( val_i < m_MinimumEigenValue) //(symEigen.D(i, i) < m_MinimumEigenValue)
                m_MinimumEigenValue = val_i; //symEigen.D(i, i);
            m_CurrentEnergy += log(val_i); //log(symEigen.D(i,i));
        }
    }

    m_CurrentEnergy /= 2.0;

    if (!m_UseMeanEnergy)
    {
        for (unsigned int i =0; i < num_samples; i++)
            std::cout << i << ": "<< W(i)*W(i) << std::endl; //symEigen.D(i, i) - m_MinimumVariance << std::endl;

        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = "
                  << m_MinimumVariance << std::endl;
    }
    else
        std::cout << "FeaMesh_ENERGY = " << m_CurrentEnergy << std::endl;



//    for (unsigned int j = 0; j < num_samples; j++)
//    {
//        int i = 0;
//        int num = 0;
//        int num2 = 0;
//        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
//        {
//            int dom = d + j*m_DomainsPerShape;

//            const ParticleImplicitSurfaceDomain<float, 3>* domain
//                    = static_cast<const ParticleImplicitSurfaceDomain<float ,3>*>(c->GetDomain(dom));

//            const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
//                 = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(c->GetDomain(dom));

//            const ParticleImageDomainWithHessians<float, 3> * domainWithHess
//                 = static_cast<const ParticleImageDomainWithHessians<float ,3> *>(c->GetDomain(dom));

//            TriMesh *ptr = domain->GetMesh();
//            if (d > 0)
//            {
//                num += m_AttributesPerDomain[d-1];
//                if (m_UseXYZ[d-1])
//                    num += 3;
//                if (m_UseNormals[d-1])
//                    num += 3;

//                num2 += c->GetNumberOfParticles(d-1)*VDimension;
//            }
//            for (unsigned int p = 0; p < c->GetNumberOfParticles(dom); p++)
//            {
//                int num_attr = m_AttributesPerDomain[d];
//                if (m_UseXYZ[d])
//                    num_attr += 3;
//                if (m_UseNormals[d])
//                    num_attr += 3;

//                J.clear();
//                J.set_size(num_attr, VDimension);
//                J.fill(0.0);
//                v.clear();
//                v.set_size(num_attr);
//                v.fill(0.0);
//                PointType pt_ps = c->GetPosition(p, dom);
//                point pt;
//                pt.clear();
//                pt[0] = pt_ps[0];
//                pt[1] = pt_ps[1];
//                pt[2] = pt_ps[2];

//                int s = 0;
//                if (m_UseXYZ[d])
//                {
//                    for (unsigned int c = s; c < s+VDimension; c++)
//                    {
//                        for (unsigned int vd = 0; vd < VDimension; vd++)
//                        {
//                            if (vd == c-s)
//                                J(c, vd) = 1.0*m_AttributeScales[num+c];
//                            else
//                                J(c, vd) = 0.0;
//                        }
//                        v(c) = Q(i++, j);
//                    }
//                    s += VDimension;
//                }
//                if (m_UseNormals[d])
//                {
//                    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleGradientVnl(pt_ps);
//                    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pN = pG.normalize();
//                    float grad_mag = pG.magnitude();

//                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType pH = domainWithHess->SampleHessianVnl(pt_ps);

//                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat1;
//                    mat1.set_identity();
//                    vnl_matrix_type nrml(VDimension, 1);
//                    nrml.fill(0.0);
//                    nrml(0,0) = pN[0]; nrml(1,0) = pN[1]; nrml(2,0) = pN[2];
//                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat2 = nrml * nrml.transpose();

//                    for (unsigned int x1 = 0; x1 < VDimension; x1++)
//                    {
//                        for (unsigned int x2 = 0; x2 < VDimension; x2++)
//                        {
//                            mat1(x1, x2) -= mat2(x1, x2);
//                            pH(x1, x2)   /= grad_mag;
//                        }
//                    }

//                    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType mat3 = pH * mat1;

//                    // mat3 = H/|grad_f| * (I - n*n');
//                    for (unsigned int c = s; c < s+VDimension; c++)
//                    {
//                        for (unsigned int vd = 0; vd < VDimension; vd++)
//                            J(c, vd) = mat3(c-s, vd)*m_AttributeScales[num+c];
//                        v(c) = Q(i++, j);
//                    }
//                    s += VDimension;
//                }

//                for (unsigned int c = s; c < s+m_AttributesPerDomain[d]; c++)
//                {
//                    point dc;
//                    dc.clear();
//                    dc = ptr->GetFeatureDerivative(pt, c-s);
//                    for (unsigned int vd = 0; vd < VDimension; vd++)
//                        J(c, vd) = dc[vd]*m_AttributeScales[num+c];
//                    v(c) = Q(i++,j);
//                }
//                s += m_AttributesPerDomain[d];
//                if (s!=num_attr)
//                    std::cerr << "Inconsistency in mesh based fea" << std::endl;

//                vnl_vector_type dx = J.transpose() * v;
//                for (unsigned int vd = 0; vd < VDimension; vd++)
//                    m_PointsUpdate(num2 + p*VDimension + vd, j) = dx[vd];
//            }
//        }
//    }

//    std::cout << m_PointsUpdate.extract(num_dims, num_samples,0,0) << std::endl;




}

/*

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
                 = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(c->GetDomain(dom));

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

*/

template <unsigned int VDimension>
double
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::GetEnergyForPositionInGivenDomain(unsigned int idx, unsigned int dom, const ParticleSystemType *c) const
{
    const ParticleImplicitSurfaceDomain<float, 3>* domain
            = static_cast<const ParticleImplicitSurfaceDomain<float ,3>*>(c->GetDomain(dom));

    const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
         = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(c->GetDomain(dom));

    TriMesh *ptr = domain->GetMesh();

    int d = dom % m_DomainsPerShape;

    PointType pt_ps = c->GetPosition(idx, dom);
    point pt;
    pt.clear();
    pt[0] = pt_ps[0];
    pt[1] = pt_ps[1];
    pt[2] = pt_ps[2];
    std::vector<float> fVals;
    fVals.clear();

    if (m_AttributesPerDomain[d] > 0)
        ptr->GetFeatureValues(pt, fVals);

    if (fVals.size() != m_AttributesPerDomain[d])
        std::cerr << "Unexpected Inconsistency in number of attributes... check" << std::endl;

    int mean_startIdx = 0;
    int sz_Yidx = 0;
    if (m_UseXYZ[d])
        sz_Yidx += 3;
    if (m_UseNormals[d])
        sz_Yidx += 3;
    sz_Yidx += m_AttributesPerDomain[d];
    vnl_matrix_type Y_dom_idx(sz_Yidx, 1, 0.0);

    int num = 0;
    for (int i = 0; i < d; i++)
    {
        mean_startIdx += c->GetNumberOfParticles(i)*m_AttributesPerDomain[i];
        num += m_AttributesPerDomain[i];
        if (m_UseXYZ[i])
        {
            mean_startIdx += c->GetNumberOfParticles(i)*3;
            num += 3;
        }
        if (m_UseNormals[i])
        {
            mean_startIdx += c->GetNumberOfParticles(i)*3;
            num += 3;
        }
    }

    int i = 0;
    int s = 0;
    if (m_UseXYZ[d])
    {
        PointType wpt = c->GetPosition(idx, dom);
//                    PointType wpt = c->GetTransformedPosition(p, dom); -- fix gradient first
        Y_dom_idx(i++, 0) = wpt[0]*m_AttributeScales[num+0] - m_mean(mean_startIdx++,0);
        Y_dom_idx(i++, 0) = wpt[1]*m_AttributeScales[num+1] - m_mean(mean_startIdx++,0);
        Y_dom_idx(i++, 0) = wpt[2]*m_AttributeScales[num+2] - m_mean(mean_startIdx++,0);
        s = 3;
    }

    if (m_UseNormals[d])
    {
        typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleNormalVnl(pt_ps);
        VectorType pN;
        pN[0] = pG[0]; pN[1] = pG[1]; pN[2] = pG[2];
//                    pN = c->TransformVector(pN, c->GetTransform(d) * c->GetPrefixTransform(d));
        Y_dom_idx(i++, 0) = pG[0]*m_AttributeScales[num+0+s] - m_mean(mean_startIdx++,0);
        Y_dom_idx(i++, 0) = pG[1]*m_AttributeScales[num+1+s] - m_mean(mean_startIdx++,0);
        Y_dom_idx(i++, 0) = pG[2]*m_AttributeScales[num+2+s] - m_mean(mean_startIdx++,0);
        s = 6;
    }

    for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
       Y_dom_idx(i++, 0) = fVals[c]*m_AttributeScales[num+c+s] - m_mean(mean_startIdx++,0);

    vnl_matrix_type tmp1 = m_InverseCovMatrix.extract(sz_Yidx, sz_Yidx, num, num);
    vnl_matrix_type tmp = Y_dom_idx.transpose()*tmp1;

    tmp *= Y_dom_idx;

    return tmp(0,0);
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

    vnl_matrix_type tmp1 = m_InverseCovMatrix.extract(sz_Yidx, sz_Yidx, num, num);

    vnl_matrix_type Y_dom_idx(sz_Yidx, 1, 0.0);

    Y_dom_idx = m_ShapeData->extract(sz_Yidx, 1, num, sampNum) - m_mean.extract(sz_Yidx, 1, num);

    vnl_matrix_type tmp = Y_dom_idx.transpose()*tmp1;
    tmp *= Y_dom_idx;

    energy = tmp(0,0);

    //energy = GetEnergyForPositionInGivenDomain(idx, d, system);//m_CurrentEnergy;

    maxdt = m_MinimumEigenValue;

    num = 0;
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
