---
title: Libs/Optimize/ParticleSystem/itkParticleCurvatureEntropyPlanarConstraint.h

---

# Libs/Optimize/ParticleSystem/itkParticleCurvatureEntropyPlanarConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleCurvatureEntropyPlanarConstraint.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleCurvatureEntropyGradientFunction_h
#define __itkParticleCurvatureEntropyGradientFunction_h

#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"

namespace itk
{

template <class TGradientNumericType, unsigned int VDimension>
class ParticleCurvatureEntropyGradientFunction
  : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
  typedef ParticleCurvatureEntropyGradientFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
  itkTypeMacro( ParticleCurvatureEntropyGradientFunction, ParticleEntropyGradientFunction);

  typedef typename Superclass::GradientNumericType GradientNumericType;
  typedef typename Superclass::ParticleSystemType ParticleSystemType;
  typedef typename Superclass::VectorType VectorType;
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::GradientVectorType GradientVectorType;
  
  typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension>
  MeanCurvatureCacheType;

  typedef typename ParticleImageDomainWithCurvature<TGradientNumericType, VDimension>
  ::VnlMatrixType VnlMatrixType;

  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  inline virtual VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystemType *c,
                              double& d) const 
  {
    double e;
    return this->Evaluate(a, b, c, d, e);
  }
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
                              double&, double & ) const;

  virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystemType *);

  inline virtual double Energy(unsigned int a, unsigned int b, const ParticleSystemType *c) const
  {
    double d, e;
    this->Evaluate(a, b, c, d, e);
    return e;  
  }
  
  inline double ComputeKappa(double mc, unsigned int d) const
  {
    //  double myKappa =  (1.0 + m_Rho * m_MeanCurvatureCache->operator[](
    //  this->GetDomainNumber())->operator[](idx) * (m_SamplesPerCurvature /
    //  twopi)) /
    //  ( m_SamplesPerCurvature * beta);
    double maxmc = m_MeanCurvatureCache->GetMeanCurvature(d)
      + 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation(d);
    double minmc = m_MeanCurvatureCache->GetMeanCurvature(d)
      - 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation(d);
    return 1.0 +  m_Rho * (mc - minmc) / (maxmc - minmc);
  }
  
  virtual void AfterIteration()  {  }

  virtual void BeforeIteration()
  {
    //  this->ComputeKappaValues();
  }

  virtual double EstimateSigma( unsigned int, unsigned int, const typename ParticleSystemType::PointVectorType &,
                                const std::vector<double> &,
                                const PointType &, double,  double,  int &err, double &) const;

  //  void ComputeKappaValues();
  
  void SetMeanCurvatureCache( MeanCurvatureCacheType *s)
  {    m_MeanCurvatureCache = s;  }
  MeanCurvatureCacheType *GetMeanCurvatureCache()
  {   return  m_MeanCurvatureCache.GetPointer();  }
  const MeanCurvatureCacheType *GetMeanCurvatureCache() const
  {   return  m_MeanCurvatureCache.GetPointer();  }
 
  //  void SetGamma(double g)
  //  { m_Gamma = g; }
  //  void SetBeta(double g)
  //  { m_Beta = g; }
//   void SetCurvatureScale(double g)
//   { m_CurvatureScale = g; }
//  void SetSamplesPerCurvature(double g)
//  { m_SamplesPerCurvature = g; }
  void SetRho(double g)
  { m_Rho= g; }
  //  double GetGamma() const
  //  { return m_Gamma; }
  //  double GetBeta() const
  //  { return m_Beta; }
//   double GetCurvatureScale() const
//   { return m_CurvatureScale; }
//  double GetSamplesPerCurvature() const
//  { return m_SamplesPerCurvature; }
  double GetRho() const
  { return m_Rho; }

protected:
  ParticleCurvatureEntropyGradientFunction() :  m_Counter(0),
                                               m_Rho(1.0) {}
  virtual ~ParticleCurvatureEntropyGradientFunction() {}
  void operator=(const ParticleCurvatureEntropyGradientFunction &);
  ParticleCurvatureEntropyGradientFunction(const ParticleCurvatureEntropyGradientFunction &);
  typename MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;
  //  double m_Gamma;
  //  double m_Beta;
  //  double m_CurvatureScale;
  //  double m_SamplesPerCurvature;
  double m_Rho;
  unsigned int m_Counter;
  double m_avgKappa;
  
  double m_CurrentSigma;
  typename ParticleSystemType::PointVectorType m_CurrentNeighborhood;

  std::vector<double> m_CurrentWeights;
    
  
};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleCurvatureEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleCurvatureEntropyGradientFunction.txx"
#endif

#include "itkParticleCurvatureEntropyGradientFunction.txx"

#endif
```


-------------------------------

Updated on 2022-01-14 at 16:56:17 +0000
