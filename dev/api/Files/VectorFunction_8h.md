---
title: Libs/Optimize/Function/VectorFunction.h

---

# Libs/Optimize/Function/VectorFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**  |




## Source code

```cpp
#pragma once

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "Libs/Optimize/ParticleSystem.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

class VectorFunction : public itk::LightObject {
 public:
  constexpr static unsigned int VDimension = 3;
  typedef VectorFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::LightObject Superclass;
  itkTypeMacro(VectorFunction, LightObject);

  typedef vnl_vector_fixed<double, VDimension> VectorType;

  //  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double& maxtimestep,
                              double& energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystem*) const = 0;

  virtual void ResetBuffers() {}

  virtual void AfterIteration() {}

  virtual void BeforeIteration() {}

  virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystem*) {}

  virtual void SetParticleSystem(ParticleSystem* p) { m_ParticleSystem = p; }
  virtual ParticleSystem* GetParticleSystem() const { return m_ParticleSystem; }
  virtual void SetDomainNumber(unsigned int i) { m_DomainNumber = i; }
  virtual int GetDomainNumber() const { return m_DomainNumber; }

  virtual VectorFunction::Pointer Clone() {
    std::cerr << "Error: base class VectorFunction Clone method called!\n";
    std::cerr << "Threaded run of current parameters not supported!\n";
    return nullptr;
  }

  virtual double GetRelativeEnergyScaling() const { return 1.0; }
  virtual void SetRelativeEnergyScaling(double r) { return; }

 protected:
  VectorFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~VectorFunction() {}
  void operator=(const VectorFunction&);
  VectorFunction(const VectorFunction&);

  ParticleSystem* m_ParticleSystem;
  unsigned int m_DomainNumber;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-03 at 18:21:07 +0000
