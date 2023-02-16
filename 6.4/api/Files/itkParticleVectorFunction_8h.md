---
title: Libs/Optimize/itkParticleVectorFunction.h

---

# Libs/Optimize/itkParticleVectorFunction.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)**  |




## Source code

```cpp
#pragma once

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkParticleSystem.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk {

template <unsigned int VDimension>
class ParticleVectorFunction : public LightObject {
 public:
  typedef ParticleVectorFunction Self;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro(ParticleVectorFunction, LightObject);

  typedef ParticleSystem ParticleSystemType;

  typedef vnl_vector_fixed<double, VDimension> VectorType;

  //  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *, double &maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *, double &maxtimestep,
                              double &energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const = 0;

  virtual void ResetBuffers() {}

  virtual void AfterIteration() {}

  virtual void BeforeIteration() {}

  virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystemType *) {}

  virtual void SetParticleSystem(ParticleSystemType *p) { m_ParticleSystem = p; }
  virtual ParticleSystemType *GetParticleSystem() const { return m_ParticleSystem; }
  virtual void SetDomainNumber(unsigned int i) { m_DomainNumber = i; }
  virtual int GetDomainNumber() const { return m_DomainNumber; }

  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone() {
    std::cerr << "Error: base class ParticleVectorFunction Clone method called!\n";
    std::cerr << "Threaded run of current parameters not supported!\n";
    return nullptr;
  }

  virtual double GetRelativeEnergyScaling() const {return 1.0;}
  virtual void SetRelativeEnergyScaling(double r) {return;}

 protected:
  ParticleVectorFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~ParticleVectorFunction() {}
  void operator=(const ParticleVectorFunction &);
  ParticleVectorFunction(const ParticleVectorFunction &);

  ParticleSystemType *m_ParticleSystem;
  unsigned int m_DomainNumber;
};

}  // namespace itk
```


-------------------------------

Updated on 2023-02-16 at 01:32:44 +0000
