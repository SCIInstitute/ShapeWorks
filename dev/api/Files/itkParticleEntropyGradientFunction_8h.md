---
title: Libs/Optimize/itkParticleEntropyGradientFunction.h

---

# Libs/Optimize/itkParticleEntropyGradientFunction.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)** <br>This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution.  |




## Source code

```cpp
#pragma once

#include "itkParticleVectorFunction.h"
#include "itkParticleContainerArrayAttribute.h"
#include "ParticleImageDomainWithGradients.h"
#include <vector>

namespace itk
{

template <class TGradientNumericType, unsigned int VDimension>
class ParticleEntropyGradientFunction : public ParticleVectorFunction<VDimension>
{
public:
  typedef ParticleEntropyGradientFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleVectorFunction<VDimension> Superclass;
  itkTypeMacro( ParticleEntropyGradientFunction, ParticleVectorFunction);

  typedef TGradientNumericType GradientNumericType;

  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  typedef ParticleContainerArrayAttribute<double, VDimension> SigmaCacheType;

  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;
  typedef  vnl_vector_fixed<TGradientNumericType, VDimension> GradientVectorType;
  
  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
                              double &) const;

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
                              double &, double &) const
  {
    itkExceptionMacro("This method not implemented");
    return VectorType();
  }
  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const
  {
    itkExceptionMacro("This method not implemented");
    return 0.0;
  }

  virtual void ResetBuffers()
  {
    m_SpatialSigmaCache->ZeroAllValues();
  }
    
  
  virtual double EstimateSigma(unsigned int idx, const typename ParticleSystemType::PointVectorType &neighborhood, const shapeworks::ParticleDomain *domain,
                                const std::vector<double> &weights,
                                    const PointType &pos, double initial_sigma,  double precision,  int &err) const;

  TGradientNumericType AngleCoefficient(const GradientVectorType&,
                                        const GradientVectorType&) const;
  
  void SetMinimumNeighborhoodRadius( double s)
  { m_MinimumNeighborhoodRadius = s; }
  double GetMinimumNeighborhoodRadius() const
  { return m_MinimumNeighborhoodRadius; }

  void SetMaximumNeighborhoodRadius( double s)
  { m_MaximumNeighborhoodRadius = s; }
  double GetMaximumNeighborhoodRadius() const
  { return m_MaximumNeighborhoodRadius; }

  void SetFlatCutoff(double s)
  { m_FlatCutoff = s; }
  double GetFlatCutoff() const
  { return m_FlatCutoff; }
  void SetNeighborhoodToSigmaRatio(double s)
  { m_NeighborhoodToSigmaRatio = s; }
  double GetNeighborhoodToSigmaRatio() const
  { return m_NeighborhoodToSigmaRatio; }

  void SetSpatialSigmaCache( SigmaCacheType *s)
  {    m_SpatialSigmaCache = s;  }
  SigmaCacheType *GetSpatialSigmaCache()
  {   return  m_SpatialSigmaCache.GetPointer();  }
  const SigmaCacheType *GetSpatialSigmaCache() const
  {   return  m_SpatialSigmaCache.GetPointer();  }

  void ComputeAngularWeights(const PointType &,
                             int,
                             const typename ParticleSystemType::PointVectorType &,
                             const shapeworks::ParticleDomain *,
                             std::vector<double> &) const;


  //  void ComputeNeighborho0d();
  

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleEntropyGradientFunction<TGradientNumericType, VDimension>::Pointer copy =
      ParticleEntropyGradientFunction<TGradientNumericType, VDimension>::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    // local
    copy->m_FlatCutoff = this->m_FlatCutoff;
    copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
    copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
    copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;
    copy->m_SpatialSigmaCache =  this->m_SpatialSigmaCache;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;

  }

protected:
  
  ParticleEntropyGradientFunction() : m_FlatCutoff(0.05), m_NeighborhoodToSigmaRatio(3.0) {}
  virtual ~ParticleEntropyGradientFunction() {}
  void operator=(const ParticleEntropyGradientFunction &);
  ParticleEntropyGradientFunction(const ParticleEntropyGradientFunction &);

  double m_MinimumNeighborhoodRadius;
  double m_MaximumNeighborhoodRadius;
  double m_FlatCutoff;
  double m_NeighborhoodToSigmaRatio;
  typename SigmaCacheType::Pointer m_SpatialSigmaCache;
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleEntropyGradientFunction.txx"
#endif

#include "itkParticleEntropyGradientFunction.txx"
```


-------------------------------

Updated on 2022-12-03 at 09:41:17 +0000
