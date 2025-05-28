---
title: Libs/Optimize/GradientDescentOptimizer.h

---

# Libs/Optimize/GradientDescentOptimizer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md)**  |




## Source code

```cpp
#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Function/VectorFunction.h"
#include "ParticleSystem.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

class GradientDescentOptimizer : public itk::Object {
 public:
  using TGradientNumericType = float;
  constexpr static unsigned int VDimension = 3;
  typedef GradientDescentOptimizer Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  typedef shapeworks::ImageDomainWithGradients<TGradientNumericType> DomainType;

  itkTypeMacro(ParticleGradientDescentPositionOptimizer, Object);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef ParticleSystem ParticleSystemType;

  typedef VectorFunction GradientFunctionType;

  typedef typename GradientFunctionType::VectorType VectorType;

  typedef typename ParticleSystemType::PointType PointType;

  void StartOptimization() { this->StartAdaptiveGaussSeidelOptimization(); }
  void StartAdaptiveGaussSeidelOptimization();

  void AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                      const double& maximumUpdateAllowed, size_t index);

  inline void StopOptimization() { this->m_StopOptimization = true; }

  inline void AbortProcessing() {
    this->m_StopOptimization = true;
    this->m_AbortProcessing = true;
  }

  itkGetMacro(NumberOfIterations, unsigned int);
  itkSetMacro(NumberOfIterations, unsigned int);

  void SetVerbosity(unsigned int val) { m_verbosity = val; }

  unsigned int GetVerbosity() { return m_verbosity; }

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

  void SetInitializationMode(bool b) { m_initialization_mode = b; }

  void SetCheckIterations(size_t si) { m_check_iterations = si; }

  void SetInitializationStartScalingFactor(double si) { m_initialization_start_scaling_factor = si; }

 protected:
  GradientDescentOptimizer();
  GradientDescentOptimizer(const GradientDescentOptimizer&);
  const GradientDescentOptimizer& operator=(const GradientDescentOptimizer&);
  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }
  virtual ~GradientDescentOptimizer(){};

 private:
  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename GradientFunctionType::Pointer m_GradientFunction;
  bool m_StopOptimization;
  bool m_AbortProcessing = false;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  std::vector<std::vector<double> > m_TimeSteps;
  unsigned int m_verbosity;

  // Adaptive Initialization variables
  bool m_initialization_mode = false;
  size_t m_check_iterations = 50;
  double m_initialization_start_scaling_factor;

  void ResetTimeStepVectors();
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-05-28 at 23:15:55 +0000
