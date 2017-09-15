#ifndef __itkParticleMeshBasedGeneralMeanGradientFunction_txx
#define __itkParticleMeshBasedGeneralMeanGradientFunction_txx

#include "TriMesh.h"
#include <math.h>
#include "itkParticleImplicitSurfaceDomain.h"

#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithHessians.h"
namespace itk
{

template <unsigned int VDimension>
typename ParticleMeshBasedGeneralMeanGradientFunction<VDimension>::VectorType
ParticleMeshBasedGeneralMeanGradientFunction<VDimension>
::Evaluate(unsigned int idx, unsigned int d, const ParticleSystemType * system,
           double &maxmove, double &energy) const
{
    const double N      = (double)(system->GetNumberOfDomains() / m_DomainsPerShape);
    unsigned int domIdx = d % m_DomainsPerShape;
    unsigned int ndims  = m_AttributesPerDomain[domIdx];

    vnl_vector_type points_minus_mean(ndims, 0.0);
    vnl_vector_type means(ndims, 0.0);
    std::vector<float> fVals;
    VectorType gradE;

    int num = 0;
    if (domIdx > 0)
    {
        for (unsigned int i = 0; i < domIdx; i++)
            num += m_AttributesPerDomain[i];
    }

    for (unsigned int j = 0; j < N; j++)
    {
        unsigned int dom = domIdx + j*m_DomainsPerShape;

        const itk::ParticleImplicitSurfaceDomain<float, 3>* domain
                = static_cast<const itk::ParticleImplicitSurfaceDomain<float ,3>*>(this->m_ParticleSystem->GetDomain(dom));

        TriMesh *ptr = domain->GetMesh();

        PointType pt_ps = this->m_ParticleSystem->GetTransformedPosition(idx, dom);
        point pt;
        pt.clear();
        pt[0] = pt_ps[0];
        pt[1] = pt_ps[1];
        pt[2] = pt_ps[2];
        fVals.clear();
        ptr->GetFeatureValues(pt, fVals);

        if (fVals.size() != ndims)
            std::cerr << "Unexpected Inconsistency in number of attributes... check" << std::endl;

        for (unsigned int c = 0; c < ndims; c++)
        {
            if (dom == d)
                points_minus_mean(c) = fVals[c]*m_AttributeScales[num+c];

            means(c) += fVals[c]*m_AttributeScales[num+c]/(double)N;
        }
    }

    for (unsigned int c = 0; c < ndims; c++)
        points_minus_mean(c) -= means(c);

    vnl_matrix_type J(ndims, VDimension, 0.0);

    const itk::ParticleImplicitSurfaceDomain<float, 3>* domain
            = static_cast<const itk::ParticleImplicitSurfaceDomain<float ,3>*>(this->m_ParticleSystem->GetDomain(d));

    const ParticleImageDomainWithGradients<float, 3> * domainWithGrad
         = static_cast<const ParticleImageDomainWithGradients<float ,3> *>(this->m_ParticleSystem->GetDomain(d));
    const ParticleImageDomainWithHessians<float, 3> * domainWithHess
         = static_cast<const ParticleImageDomainWithHessians<float ,3> *>(this->m_ParticleSystem->GetDomain(d));

    TriMesh *ptr = domain->GetMesh();

    PointType pos = system->GetTransformedPosition(idx, d);
    point pt;
    pt.clear();
    pt[0] = pos[0];
    pt[1] = pos[1];
    pt[2] = pos[2];

    typename ParticleImageDomainWithGradients<float,3>::VnlVectorType pG = domainWithGrad->SampleNormalVnl(pos);

    typename ParticleImageDomainWithHessians<float,3>::VnlMatrixType pH = domainWithHess->SampleHessianVnl(pos);

    for (unsigned int c = 0; c < ndims; c++)
    {
        point dc;
        dc.clear();
        dc = ptr->GetFeatureDerivative(pt, c);
        for (unsigned int vd = 0; vd < VDimension; vd++)
            J(c, vd) = dc[vd]*m_AttributeScales[num+c];
    }

    vnl_vector_type dx = J.transpose() * points_minus_mean;

    for (unsigned int vd = 0; vd < VDimension; vd++)
        gradE[vd] = dx[vd];

    energy = points_minus_mean.squared_magnitude() * 0.5;

    maxmove = domain->GetImage()->GetSpacing()[0];

    //  Transform the gradient according to the transform of the given domain and
    //  return.
    return system->TransformVector(gradE, system->GetInversePrefixTransform(d)
                                   * system->GetInverseTransform(d));
}


} //end namespace

#endif
