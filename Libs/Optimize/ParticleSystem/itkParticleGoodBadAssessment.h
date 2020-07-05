#ifndef ITKPARTICLEGOODBADASSESSMENT_H
#define ITKPARTICLEGOODBADASSESSMENT_H

#include "itkParticleSystem.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include <algorithm>
namespace itk
{
/** \class ParticleProcrustesRegistration
 *  \brief
 *  Class for replacing bad particles during optimization
 *  Author - Praful Agrawal
 *  Date   - June 05, 2017
 */

template <class TGradientNumericType, unsigned int VDimension>
class ParticleGoodBadAssessment : public Object
{
public:

    typedef ParticleGoodBadAssessment Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef WeakPointer<const Self>  ConstWeakPointer;

    /** Particle system typedefs. */
    typedef ParticleSystem<VDimension> ParticleSystemType;
    typedef typename ParticleSystemType::PointType PointType;

    /** Type of the domain. */
    typedef ParticleImageDomainWithGradients<TGradientNumericType> DomainType;
    typedef typename DomainType::VnlVectorType NormalType;

    typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> MeanCurvatureCacheType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Set/Get the number of Domains in each shape. */
    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i; }
    int GetDomainsPerShape() const
    { return m_DomainsPerShape; }

    void SetEpsilon(double i)
    { epsilon = i; }

    void SetCriterionAngle(double a)
    { m_CriterionAngle = a;}

    void SetPerformAssessment(bool b)
    { m_PerformAssessment = b; }

    /** Performs good/bad points assessment and reports the bad positions of the
        particle system. */
    std::vector<std::vector<int> > RunAssessment(const ParticleSystemType * m_ParticleSystem, MeanCurvatureCacheType * m_MeanCurvatureCache);
    vnl_matrix<double> computeParticlesNormals(int d, const ParticleSystemType * m_ParticleSystem);

    struct IdxCompare
    {
        const std::vector<double>& target;

        IdxCompare(const std::vector<double>& target): target(target) {}

        bool operator()(int a, int b) const { return target[a] < target[b]; }
    };

    std::vector<int> sortIdcs(std::vector<double> v)
    {
        std::vector<int> index(v.size(), 0);
        for (int i = 0; i < v.size(); i++)
            index[i] = i;
        std::sort(index.begin(), index.end(), IdxCompare(v));
        return index;
    }

protected:
  ParticleGoodBadAssessment() : m_DomainsPerShape(1), m_CriterionAngle(90.0), m_PerformAssessment(true) {  }
  virtual ~ParticleGoodBadAssessment() {}

private:
  ParticleGoodBadAssessment(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  int m_DomainsPerShape;
  double m_CriterionAngle;
  bool m_PerformAssessment;
  double epsilon; // m_Spacing from ShapeWorksRunApp


};

} //end namespace

#if ITK_TEMPLATE_TXX
# include "itkParticleGoodBadAssessment.txx"
#endif

#include "itkParticleGoodBadAssessment.txx"

#endif // ITKPARTICLEGOODBADASSESSMENT_H
