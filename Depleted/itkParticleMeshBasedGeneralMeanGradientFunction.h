#ifndef ITKMESHBASEDENSEMBLEGENERALMEANGRADIENTFUNCTION_H
#define ITKMESHBASEDENSEMBLEGENERALMEANGRADIENTFUNCTION_H

#include "itkParticleVectorFunction.h"
#include <vector>

namespace itk
{
template <unsigned int VDimension>
class ParticleMeshBasedGeneralMeanGradientFunction : public ParticleVectorFunction<VDimension>
{
public:
    typedef ParticleMeshBasedGeneralMeanGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleVectorFunction<VDimension> Superclass;
    itkTypeMacro( ParticleMeshBasedGeneralMeanGradientFunction, ParticleVectorFunction);

    /** Type of particle system. */
    typedef typename Superclass::ParticleSystemType ParticleSystemType;

    /** Vector & Point types. */
    typedef typename Superclass::VectorType VectorType;
    typedef typename ParticleSystemType::PointType PointType;
    typedef vnl_vector<float> vnl_vector_type;
    typedef vnl_matrix<float> vnl_matrix_type;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Dimensionality of the domain of the particle system. */
    itkStaticConstMacro(Dimension, unsigned int, VDimension);

    /** The first argument is a pointer to the particle system.  The second
        argument is the index of the domain within that particle system.  The
        third argument is the index of the particle location within the given
        domain. */

    virtual VectorType Evaluate(unsigned int, unsigned int,
                                const ParticleSystemType *, double &, double&) const;

    virtual VectorType Evaluate(unsigned int a,  unsigned int b,
                                const ParticleSystemType *c, double &d) const
    {
      double e;
      return this->Evaluate(a,b,c,d,e);
    }


    virtual double Energy(unsigned int a, unsigned int b,
                          const ParticleSystemType *c) const
    {
      double e, d;
      this->Evaluate(a,b,c,d,e);
      return e;
    }

    void SetAttributeScales( const std::vector<double> &s)
    {
      m_AttributeScales = s;
    }

    /** Set/Get the number of domains per shape. */
    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i; }
    int GetDomainsPerShape() const
    { return m_DomainsPerShape; }

    void SetAttributesPerDomain(const std::vector<int> &i)
    { m_AttributesPerDomain = i; }

    virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
    {
      typename ParticleMeshBasedGeneralMeanGradientFunction<VDimension>::Pointer copy =
        ParticleMeshBasedGeneralMeanGradientFunction<VDimension>::New();

      // from itkParticleVectorFunction
      copy->m_DomainNumber = this->m_DomainNumber;
      copy->m_ParticleSystem = this->m_ParticleSystem;

      // local
      copy->m_AttributeScales = this->m_AttributeScales;
      copy->m_AttributesPerDomain = this->m_AttributesPerDomain;
      copy->m_DomainsPerShape = this->m_DomainsPerShape;
      copy->m_PointsUpdate    = this->m_PointsUpdate;

      return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
    }


protected:
    ParticleMeshBasedGeneralMeanGradientFunction() {}

    virtual ~ParticleMeshBasedGeneralMeanGradientFunction() {}
    void operator=(const ParticleMeshBasedGeneralMeanGradientFunction &);
    ParticleMeshBasedGeneralMeanGradientFunction(const ParticleMeshBasedGeneralMeanGradientFunction &);

    std::vector<double> m_AttributeScales; //size \sum_i n_i
    int m_DomainsPerShape;
    std::vector<int> m_AttributesPerDomain; // n
    vnl_matrix_type m_PointsUpdate;
};
} //end namespace

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkParticleMeshBasedGeneralMeanGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkParticleMeshBasedGeneralMeanGradientFunction.txx"
#endif


#endif // ITKMESHBASEDENSEMBLEGENERALMEANGRADIENTFUNCTION_H
