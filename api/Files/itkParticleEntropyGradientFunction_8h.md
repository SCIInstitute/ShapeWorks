---
title: Libs/Optimize/ParticleSystem/itkParticleEntropyGradientFunction.h

---

# Libs/Optimize/ParticleSystem/itkParticleEntropyGradientFunction.h



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
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEntropyGradientFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEntropyGradientFunction_h
#define __itkParticleEntropyGradientFunction_h

#include "itkParticleVectorFunction.h"
#include "itkParticleContainerArrayAttribute.h"
#include "itkParticleImageDomainWithGradients.h"
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
    
  
  virtual double EstimateSigma(unsigned int idx, const typename ParticleSystemType::PointVectorType &neighborhood, const ParticleDomain *domain,
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
                             const ParticleDomain *,
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

#endif
```


-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000
