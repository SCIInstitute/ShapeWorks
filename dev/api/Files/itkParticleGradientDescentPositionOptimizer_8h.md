---
title: Libs/Optimize/ParticleSystem/itkParticleGradientDescentPositionOptimizer.h

---

# Libs/Optimize/ParticleSystem/itkParticleGradientDescentPositionOptimizer.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md)**  |




## Source code

```cpp
#pragma once

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include <vector>
#include "vnl/vnl_vector_fixed.h"
#include "itkParticleVectorFunction.h"
#include "ParticleImageDomainWithGradients.h"
#include <algorithm>
#include <limits>

namespace itk
{

template <class TGradientNumericType, unsigned int VDimension>
class ParticleGradientDescentPositionOptimizer : public Object
{
public:
  typedef ParticleGradientDescentPositionOptimizer Self;
  typedef Object Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  typedef shapeworks::ParticleImageDomainWithGradients<TGradientNumericType> DomainType;
  
  itkTypeMacro(ParticleGradientDescentPositionOptimizer, Object);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef ParticleSystem ParticleSystemType;

  typedef ParticleVectorFunction<VDimension> GradientFunctionType;
  
  typedef typename GradientFunctionType::VectorType VectorType;

  typedef typename ParticleSystemType::PointType PointType;

  void StartOptimization()
  {
    this->StartAdaptiveGaussSeidelOptimization();
  }
  void StartAdaptiveGaussSeidelOptimization();

  void AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                      const double& maximumUpdateAllowed);

  inline void StopOptimization()
  {    this->m_StopOptimization = true;  }

  inline void AbortProcessing()
  {    this->m_StopOptimization = true;
       this->m_AbortProcessing = true;  }

  itkGetMacro(NumberOfIterations, unsigned int);
  itkSetMacro(NumberOfIterations, unsigned int);

  void SetVerbosity(unsigned int val)
  {
      m_verbosity = val;
  }

  unsigned int GetVerbosity()
  { return m_verbosity; }

  itkGetMacro(TimeStep, double);
  itkSetMacro(TimeStep, double);

  itkGetMacro(MaximumNumberOfIterations, unsigned int);
  itkSetMacro(MaximumNumberOfIterations, unsigned int);

  itkGetMacro(Tolerance, double);
  itkSetMacro(Tolerance, double);
  
  itkGetObjectMacro(ParticleSystem, ParticleSystemType);
  itkSetObjectMacro(ParticleSystem, ParticleSystemType);

  itkGetObjectMacro(GradientFunction, GradientFunctionType);
  itkSetObjectMacro(GradientFunction, GradientFunctionType);

protected:
  ParticleGradientDescentPositionOptimizer();
  ParticleGradientDescentPositionOptimizer(const ParticleGradientDescentPositionOptimizer &);
  const ParticleGradientDescentPositionOptimizer &operator=(const ParticleGradientDescentPositionOptimizer &);
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleGradientDescentPositionOptimizer() {};

private:
  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename GradientFunctionType::Pointer m_GradientFunction;
  bool m_StopOptimization;
  bool m_AbortProcessing = false;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  std::vector< std::vector<double> > m_TimeSteps;
  unsigned int m_verbosity;

  void ResetTimeStepVectors();
};


} // end namespace

#include "itkParticleGradientDescentPositionOptimizer.txx"
```


-------------------------------

Updated on 2022-07-30 at 21:20:17 +0000
