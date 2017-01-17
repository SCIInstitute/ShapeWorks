#ifndef __itkParticleMeshBasedGeneralEntropyGradientFunction_txx
#define __itkParticleMeshBasedGeneralEntropyGradientFunction_txx

#include "TriMesh.h"
#include <math.h>
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticleImplicitSurfaceDomain.h"
namespace itk
{
template <unsigned int VDimension>
void
ParticleMeshBasedGeneralEntropyGradientFunction<VDimension>
::ComputeUpdates()
{

//    const itk::ParticleImplicitSurfaceDomain<float, 3>* dom
//            = static_cast<const itk::ParticleImplicitSurfaceDomain<float
//            ,3>*>(m_Sampler->GetParticleSystem()->GetDomain(i));

    const unsigned int num_samples   = this->m_ParticleSystem->GetNumberOfDomains() / m_DomainsPerShape;
    const unsigned int num_functions = m_AttributeScales.size();
    const unsigned int num_particles = this->m_ParticleSystem->GetNumberOfParticles();
    const unsigned int num_dims      = num_particles * num_functions;

    // Do we need to resize the update matrix?
    if (m_PointsUpdate.rows() != VDimension * num_particles * m_DomainsPerShape
            || m_PointsUpdate.cols() != num_samples)
    {
        m_PointsUpdate.set_size(VDimension * num_particles * m_DomainsPerShape, num_samples);
    }

    vnl_matrix_type points_minus_mean(num_dims, num_samples);
    vnl_vector_type means(num_dims, 0.0);

    std::vector<float> fVals;
    // Compute the shape vector. Y
    for (unsigned int j = 0; j < num_samples; j++)
    {
        int i = 0;
        int num = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;

            const itk::ParticleImplicitSurfaceDomain<float, 3>* domain
                    = static_cast<const itk::ParticleImplicitSurfaceDomain<float ,3>*>(this->m_ParticleSystem->GetDomain(dom));

//            TriMesh *ptr = this->m_DomainList[dom]->GetMesh();
            TriMesh *ptr = domain->GetMesh();

            if (d > 0)
                num += m_AttributesPerDomain[d-1];

            for (unsigned int p = 0; p < num_particles; p++)
            {
                PointType pt_ps = this->m_ParticleSystem->GetPosition(p, dom);
                point pt;
                pt.clear();
                pt[0] = pt_ps[0];
                pt[1] = pt_ps[1];
                pt[2] = pt_ps[2];
                fVals.clear();
                ptr->GetFeatureValues(pt, fVals);

                if (fVals.size() != m_AttributesPerDomain[d])
                    std::cerr << "Unexpected Inconsistency in number of attributes... check" << std::endl;

                for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
                {
                    int i1 = i++;
                    points_minus_mean(i1, j) = std::log(fVals[c]+1e-10)*m_AttributeScales[num+c];
                    means(i1) += points_minus_mean(i1, j)/(double)num_samples;
                }
            }
        }
        if (i != num_dims)
            std::cerr << "Bug in code while creating feature matrix... check" << std::endl;
    }

//    std::cout << points_minus_mean.extract(num_dims, num_samples, 0, 0) << std::endl;
//    std::cout << means.extract(num_dims, 0) << std::endl;

    for (unsigned int i = 0; i < points_minus_mean.rows(); i++)
    {
        for (unsigned int j = 0; j < points_minus_mean.cols(); j++)
            points_minus_mean(i,j) = points_minus_mean(i,j) - means(i);
    }

//    std::cout << points_minus_mean.extract(num_dims, num_samples, 0, 0) << std::endl;

    // Compute the covariance in the dual space (transposed shape matrix)
    vnl_matrix_type A =  points_minus_mean.transpose()
        * points_minus_mean * (1.0/((double)(num_samples-1)));

    // Regularize A
    for (unsigned int i = 0; i < num_samples; i++)
      {
      A(i, i) = A(i, i) + m_MinimumVariance;
      }

    // Find inverse of covariance matrix
    vnl_symmetric_eigensystem<float> symEigen(A);
    vnl_matrix_type Q = points_minus_mean * (symEigen.pinverse());

    // Compute the update matrix in coordinate space by multiplication with the
    // Jacobian.  Each shape gradient must be transformed by a different Jacobian
    // so we have to do this individually for each shape (sample).
    vnl_matrix_type J;
    vnl_vector_type v;

    for (unsigned int j = 0; j < num_samples; j++)
    {
        int i = 0;
        int num = 0;
        for (unsigned int d = 0; d < m_DomainsPerShape; d++)
        {
            int dom = d + j*m_DomainsPerShape;

            const itk::ParticleImplicitSurfaceDomain<float, 3>* domain
                    = static_cast<const itk::ParticleImplicitSurfaceDomain<float ,3>*>(this->m_ParticleSystem->GetDomain(dom));

            TriMesh *ptr = domain->GetMesh();
            if (d > 0)
                num += m_AttributesPerDomain[d-1];
            for (unsigned int p = 0; p < num_particles; p++)
            {
                J.clear();
                J.set_size(m_AttributesPerDomain[d], VDimension);
                J.fill(0.0);
                v.clear();
                v.set_size(m_AttributesPerDomain[d]);
                v.fill(0.0);
                PointType pt_ps = this->m_ParticleSystem->GetPosition(p, dom);
                point pt;
                pt.clear();
                pt[0] = pt_ps[0];
                pt[1] = pt_ps[1];
                pt[2] = pt_ps[2];
                for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
                {
                    point dc;
                    dc.clear();
                    dc = ptr->GetFeatureDerivative(pt, c);
//                    std::cout << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
//                    std::cout << dc[0] << " " << dc[1] << " " << dc[2] << std::endl;
                    for (unsigned int vd = 0; vd < VDimension; vd++)
                        J(c, vd) = dc[vd]*m_AttributeScales[num+c];
                    v(c) = Q(i++,j);
                }
                vnl_vector_type dx = J.transpose() * v;
                for (unsigned int vd = 0; vd < VDimension; vd++)
                    m_PointsUpdate(d*num_particles*VDimension + p*VDimension + vd, j) = dx[vd];
            }
        }
    }

//    std::cout << m_PointsUpdate.extract(6, num_samples,0,0) << std::endl;

    m_MinimumEigenValue = std::fabs(symEigen.D(0, 0));

    m_CurrentEnergy = 0.0;
    for (unsigned int i = 0; i < num_samples; i++)
      {
      if (std::fabs(symEigen.D(i, i)) < m_MinimumEigenValue)
        {
            m_MinimumEigenValue = std::fabs(symEigen.D(i, i));
        }
      m_CurrentEnergy += log(std::fabs(symEigen.D(i,i)));
      }
    m_CurrentEnergy /= num_samples;


    for (unsigned int i =0; i < num_samples; i++)
      {
        std::cout << i << ": "<< std::fabs(symEigen.D(i, i)) - m_MinimumVariance << std::endl;
      }
    std::cout << "ENERGY = " << m_CurrentEnergy << "\t MinimumVariance = "
              << m_MinimumVariance <<  std::endl;

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
    int numParticles = system->GetNumberOfParticles();

    energy = m_CurrentEnergy;
    maxdt = m_MinimumEigenValue;

    VectorType gradE;
    unsigned int k = idx * VDimension + dom * numParticles * VDimension;
    for (unsigned int i = 0; i< VDimension; i++)
    {
        gradE[i] = m_PointsUpdate(k + i, sampNum);
    }

    if (idx == 0 ) std::cout << "maxdt= " << maxdt << " idx = " << idx << "\t" << "GradE = " << gradE << std::endl;
    return system->TransformVector(gradE, system->GetInverseTransform(d));
}

} // end namespace
#endif
