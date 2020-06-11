#ifndef __itkParticleGoodBadAssessment_txx
#define __itkParticleGoodBadAssessment_txx

#include "itkParticleMeanCurvatureAttribute.h"

#include <vector>
namespace itk{

template<class TGradientNumericType, unsigned int VDimension>
std::vector<std::vector<int> >
ParticleGoodBadAssessment<TGradientNumericType, VDimension>::RunAssessment(const ParticleSystemType * m_ParticleSystem, MeanCurvatureCacheType * m_MeanCurvatureCache)
{


    const int totalDomains = m_ParticleSystem->GetNumberOfDomains();
    const int numShapes    = totalDomains / m_DomainsPerShape;
    std::vector<std::vector<int> > badIds;
    badIds.resize(m_DomainsPerShape);
    for (int i = 0; i < m_DomainsPerShape; i++)
    {
        std::vector<vnl_matrix<double> > normals;
        normals.resize(numShapes);
        for (int j = 0; j < numShapes; j++)
        {
            int d = j*m_DomainsPerShape + i;
            normals[j] = computeParticlesNormals(d, m_ParticleSystem);
        }

//        std::vector<int> goodIds;
//        std::vector<double> meanDotPdt;

        for (int n = 0; n < m_ParticleSystem->GetNumberOfParticles(i); n++)
        {
            bool flag = true;
            for (int a = 0; a < numShapes; a++)
            {
                if (!flag) break;
                for (int b = a+1; b < numShapes; b++)
                {
                    double dotPdt = normals[a][n][0]*normals[b][n][0] +
                            normals[a][n][1]*normals[b][n][1] +
                            normals[a][n][2]*normals[b][n][2];

                    int dom_a       = a*m_DomainsPerShape + i;
                    int dom_b       = b*m_DomainsPerShape + i;
                    double curv_a_n = m_MeanCurvatureCache->operator[](dom_a)->operator[](n);
                    double curv_b_n = m_MeanCurvatureCache->operator[](dom_b)->operator[](n);
                    double curv_a   = m_MeanCurvatureCache->GetMeanCurvature(dom_a);
                    double curv_b   = m_MeanCurvatureCache->GetMeanCurvature(dom_b);

                    double val     = m_CriterionAngle * 0.5 * (curv_a_n/curv_a + curv_b_n/curv_b);

                    if (dotPdt < std::cos(val))
                    {
                        badIds[i].push_back(n);
                        flag = false;
                        break;
                    }
                }
            }
        } //n

//        if (badIds.size() > goodIds.size())
//            continue;

//        std::vector<int> sortedGoodIds = sortIdcs(meanDotPdt);

//        vnl_vector_fixed<double, 3> random;
//        srand(1);
//        for (int ii = 0; ii < 3; ii++)
//            random[ii] = static_cast<double>(rand());
//        double norm = random.magnitude();
//        random /= norm;

//        PointType newpos;
//        for (int b = 0; b < badIds.size(); b++)
//        {
//            unsigned int idx = goodIds[sortedGoodIds[b]];
//            for (int c = 0; c < numShapes; c++)
//            {
//                int d = c*m_DomainsPerShape + i;

//                for (int z=0; z<VDimension; z++)
//                    newpos[z] = m_ParticleSystem->GetPosition(idx, d)[z] + epsilon*random[z];

//                dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(d))->ApplyConstraints(newpos);
//                m_ParticleSystem->SetPosition(newpos, badIds[b], d);
//            }
//        }

//        std::cout << badIds.size() << " bad points re-generated in domain " << i << std::endl;

    }//m_Domains_per_shape

    return badIds;
}


template<class TGradientNumericType, unsigned int VDimension>
vnl_matrix<double>
ParticleGoodBadAssessment<TGradientNumericType, VDimension>::computeParticlesNormals(int d, const ParticleSystemType * m_ParticleSystem)
{
    const DomainType * domain = static_cast<const DomainType *>(m_ParticleSystem->GetDomain(d));
    int num = m_ParticleSystem->GetNumberOfParticles(d);
    vnl_matrix<double> normals(num, VDimension);

    for (int i = 0; i < num; i++)
    {
        PointType pt = m_ParticleSystem->GetPosition(i, d);
        NormalType n = domain->SampleNormalAtPoint(pt);
        for (int j = 0; j < VDimension; j++)
            normals[i][j] = n[j];
    }
    return normals;
}
}

#endif
