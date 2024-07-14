---
title: Libs/Optimize/Function/DualVectorFunction.h

---

# Libs/Optimize/Function/DualVectorFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md)**  |




## Source code

```cpp
#pragma once

#include "ParticleSystemEvaluation.h"
#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {

class DualVectorFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef DualVectorFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(DualVectorFunction, VectorFunction);

  typedef typename Superclass::VectorType VectorType;

  itkNewMacro(Self);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system,
                              double& maxmove) const {
    double maxA, maxB, maxC;
    maxA = 0;
    maxB = 0;
    maxC = 0;
    VectorType ansA;
    ansA.fill(0.0);
    VectorType ansB;
    ansB.fill(0.0);
    VectorType ansC;
    ansC.fill(0.0);

    const_cast<DualVectorFunction*>(this)->m_Counter = m_Counter + 1.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (m_AOn == true) {
      ansA = m_FunctionA->Evaluate(idx, d, system, maxA);
      const_cast<DualVectorFunction*>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
    }

    if (m_BOn == true) {
      ansB = m_FunctionB->Evaluate(idx, d, system, maxB);
      const_cast<DualVectorFunction*>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
    }

    if (m_RelativeGradientScaling == 0.0) {
      ansB.fill(0.0);
      maxB = 0.0;
    }

    // get maxmove and predicted move for current configuration
    VectorType predictedMove;
    predictedMove.fill(0.0);
    if (m_BOn == true) {
      if (m_AOn == true)  // both A and B are active
      {
        if (maxB > maxA) {
          maxmove = maxB;
        } else {
          maxmove = maxA;
        }

        maxmove = maxA;  // always driven by the sampling to decrease the senstivity to covariance regularization

        predictedMove = ansA + m_RelativeGradientScaling * ansB;

        return (predictedMove);
      } else  // B is active, A is not active
      {
        maxmove = maxB;

        predictedMove = ansB;

        return (predictedMove);
      }
    } else  // only A is active
    {
      maxmove = maxA;
      return ansA;
    }
    maxmove = 0.0;
    return ansA;
  }

  virtual double EnergyA(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    m_FunctionA->BeforeEvaluate(idx, d, system);
    double ansA = 0.0;
    if (m_AOn == true) {
      ansA = m_FunctionA->Energy(idx, d, system);
    }
    return ansA;
  }

  virtual double EnergyB(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    m_FunctionB->BeforeEvaluate(idx, d, system);
    double ansB = 0.0;
    if (m_BOn == true) {
      ansB = m_FunctionB->Energy(idx, d, system);
    }
    ansB *= m_RelativeEnergyScaling;
    return ansB;
  }

  virtual double Energy(unsigned int idx, unsigned int d, const ParticleSystem* system) const {
    double ansA = 0.0;
    double ansB = 0.0;
    double ansC = 0.0;
    double finalEnergy = 0.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (m_AOn == true) {
      ansA = m_FunctionA->Energy(idx, d, system);
    }

    if (m_BOn == true) {
      ansB = m_FunctionB->Energy(idx, d, system);
    }

    if (m_RelativeEnergyScaling == 0) {
      ansB = 0.0;
    }

    // compute final energy for current configuration
    if (m_BOn == true) {
      if (m_AOn == true)  // both A and B are active
      {
        finalEnergy = ansA + m_RelativeEnergyScaling * ansB;
        return (finalEnergy);
      } else  // B is active, A is not active
      {
        finalEnergy = ansB;
        return finalEnergy;
      }
    } else  // only A is active
    {
      return ansA;
    }

    return 0.0;
  }

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxmove,
                              double& energy) const {
    double maxA = 0.0;
    double maxB = 0.0;
    double energyA = 0.0;
    double energyB = 0.0;
    VectorType ansA;
    ansA.fill(0.0);
    VectorType ansB;
    ansB.fill(0.0);

    const_cast<DualVectorFunction*>(this)->m_Counter = m_Counter + 1.0;

    // evaluate individual functions: A = surface energy, B = correspondence
    if (m_AOn == true) {
      ansA = m_FunctionA->Evaluate(idx, d, system, maxA, energyA);

      const_cast<DualVectorFunction*>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
      const_cast<DualVectorFunction*>(this)->m_AverageEnergyA = m_AverageEnergyA + energyA;
    }

    if (m_BOn == true) {
      ansB = m_FunctionB->Evaluate(idx, d, system, maxB, energyB);

      const_cast<DualVectorFunction*>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
      const_cast<DualVectorFunction*>(this)->m_AverageEnergyB = m_AverageEnergyB + energyB;
    }

    if (m_RelativeEnergyScaling == 0.0) {
      energyB = 0.0;
      ansB.fill(0.0);
    }

    if (m_RelativeGradientScaling == 0.0) {
      maxB = 0.0;
      ansB.fill(0.0);
    }

    // compute final energy, maxmove and predicted move based on current configuration
    VectorType predictedMove;
    predictedMove.fill(0.0);
    if (m_BOn == true) {
      if (m_AOn == true)  // both A and B are active
      {
        if (maxB > maxA) {
          maxmove = maxB;
        } else {
          maxmove = maxA;
        }

        energy = energyA + m_RelativeEnergyScaling * energyB;

        maxmove = maxA;  // always driven by the sampling to decrease the senstivity to covariance regularization

        predictedMove = ansA + m_RelativeGradientScaling * ansB;

        return (predictedMove);
      } else  // only B is active, A is not active
      {
        maxmove = maxB;
        energy = energyB;
        predictedMove = ansB;

        return (predictedMove);
      }
    } else  // only A is active
    {
      maxmove = maxA;
      energy = energyA;
      return ansA;
    }
    maxmove = 0.0;
    return ansA;
  }

  virtual void BeforeEvaluate(unsigned int idx, unsigned int d, const ParticleSystem* system) {
    if (m_AOn == true) {
      m_FunctionA->BeforeEvaluate(idx, d, system);
    }

    if (m_BOn == true) {
      m_FunctionB->BeforeEvaluate(idx, d, system);
    }
  }

  virtual void AfterIteration() {
    if (m_AOn) m_FunctionA->AfterIteration();
    if (m_BOn) {
      m_FunctionB->AfterIteration();
    }
  }

  virtual void BeforeIteration() {
    if (m_AOn) m_FunctionA->BeforeIteration();
    if (m_BOn) {
      m_FunctionB->BeforeIteration();
    }
    m_AverageGradMagA = 0.0;
    m_AverageGradMagB = 0.0;
    m_AverageEnergyA = 0.0;
    m_Counter = 0.0;
  }

  virtual void SetParticleSystem(ParticleSystem* p) {
    Superclass::SetParticleSystem(p);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetParticleSystem(p);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetParticleSystem(p);
  }

  void SetDomainNumber(unsigned int i) {
    Superclass::SetDomainNumber(i);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetDomainNumber(i);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetDomainNumber(i);
  }

  void SetFunctionA(VectorFunction* o) {
    m_FunctionA = o;
    m_FunctionA->SetDomainNumber(this->GetDomainNumber());
    m_FunctionA->SetParticleSystem(this->GetParticleSystem());
  }

  VectorFunction* GetFunctionA() { return m_FunctionA.GetPointer(); }

  VectorFunction* GetFunctionB() { return m_FunctionB.GetPointer(); }

  void SetFunctionB(VectorFunction* o) {
    m_FunctionB = o;
    m_FunctionB->SetDomainNumber(this->GetDomainNumber());
    m_FunctionB->SetParticleSystem(this->GetParticleSystem());
  }

  void SetAOn() { m_AOn = true; }
  void SetAOff() { m_AOn = false; }
  void SetAOn(bool s) { m_AOn = s; }
  bool GetAOn() const { return m_AOn; }
  void SetBOn() { m_BOn = true; }
  void SetBOff() { m_BOn = false; }
  void SetBOn(bool s) { m_BOn = s; }
  bool GetBOn() const { return m_BOn; }

  void SetRelativeEnergyScaling(double r) override { m_RelativeEnergyScaling = r; }
  double GetRelativeEnergyScaling() const override { return m_RelativeEnergyScaling; }

  void SetRelativeGradientScaling(double r) { m_RelativeGradientScaling = r; }
  double GetRelativeGradientScaling() const { return m_RelativeGradientScaling; }

  double GetAverageGradMagA() const {
    if (m_Counter != 0.0)
      return m_AverageGradMagA / m_Counter;
    else
      return 0.0;
  }
  double GetAverageGradMagB() const {
    if (m_Counter != 0.0)
      return m_AverageGradMagB / m_Counter;
    else
      return 0.0;
  }

  double GetAverageEnergyA() const {
    if (m_Counter != 0.0)
      return m_AverageEnergyA / m_Counter;
    else
      return 0.0;
  }
  double GetAverageEnergyB() const {
    if (m_Counter != 0.0)
      return m_AverageEnergyB / m_Counter;
    else
      return 0.0;
  }

  virtual typename VectorFunction::Pointer Clone() {
    typename DualVectorFunction::Pointer copy = DualVectorFunction::New();
    copy->m_AOn = this->m_AOn;
    copy->m_BOn = this->m_BOn;

    copy->m_RelativeGradientScaling = this->m_RelativeGradientScaling;
    copy->m_RelativeEnergyScaling = this->m_RelativeEnergyScaling;
    copy->m_AverageGradMagA = this->m_AverageGradMagA;
    copy->m_AverageGradMagB = this->m_AverageGradMagB;
    copy->m_AverageEnergyA = this->m_AverageEnergyA;
    copy->m_AverageEnergyB = this->m_AverageEnergyB;
    copy->m_Counter = this->m_Counter;

    if (this->m_FunctionA) copy->m_FunctionA = this->m_FunctionA->Clone();
    if (this->m_FunctionB) copy->m_FunctionB = this->m_FunctionB->Clone();

    if (!copy->m_FunctionA) copy->m_AOn = false;
    if (!copy->m_FunctionB) copy->m_BOn = false;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    return (VectorFunction::Pointer)copy;
  }

 protected:
  DualVectorFunction()
      : m_AOn(true), m_BOn(false), m_RelativeGradientScaling(1.0), m_RelativeEnergyScaling(1.0) {}

  virtual ~DualVectorFunction() {}
  void operator=(const DualVectorFunction&);
  DualVectorFunction(const DualVectorFunction&);

  bool m_AOn;
  bool m_BOn;
  double m_RelativeGradientScaling;
  double m_RelativeEnergyScaling;
  double m_AverageGradMagA;
  double m_AverageGradMagB;
  double m_AverageEnergyA;
  double m_AverageEnergyB;
  double m_Counter;

  VectorFunction::Pointer m_FunctionA;
  VectorFunction::Pointer m_FunctionB;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-14 at 17:07:25 +0000
