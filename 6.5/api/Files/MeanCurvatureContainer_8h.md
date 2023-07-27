---
title: Libs/Optimize/Container/MeanCurvatureContainer.h

---

# Libs/Optimize/Container/MeanCurvatureContainer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md)**  |




## Source code

```cpp
#pragma once

#include "GenericContainer.h"
#include "GenericContainerArray.h"
#include "Libs/Optimize/Domain/ImageDomainWithCurvature.h"
#include "ParticleSystem.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace shapeworks {
template <class TNumericType, unsigned int VDimension>
class MeanCurvatureContainer : public GenericContainerArray<TNumericType> {
 public:
  typedef TNumericType NumericType;
  typedef MeanCurvatureContainer Self;
  typedef GenericContainerArray<TNumericType> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  typedef ParticleSystem ParticleSystemType;
  typedef typename ParticleSystemType::PointType PointType;
  typedef vnl_vector_fixed<TNumericType, VDimension> VnlVectorType;

  itkNewMacro(Self);

  itkTypeMacro(ParticleMeanCurvatureAttribute, GenericContainerArray);

  void SetVerbosity(unsigned int val) { m_verbosity = val; }

  unsigned int GetVerbosity() { return m_verbosity; }

  virtual void PositionAddEventCallback(itk::Object* o, const itk::EventObject& e) {
    Superclass::PositionAddEventCallback(o, e);
    const ParticlePositionAddEvent& event = dynamic_cast<const ParticlePositionAddEvent&>(e);
    const ParticleSystemType* ps = dynamic_cast<const ParticleSystemType*>(o);
    this->ComputeMeanCurvature(ps, event.GetPositionIndex(), event.GetDomainIndex());
  }

  virtual void PositionSetEventCallback(itk::Object* o, const itk::EventObject& e) {
    const ParticlePositionSetEvent& event = dynamic_cast<const ParticlePositionSetEvent&>(e);
    const ParticleSystemType* ps = dynamic_cast<const ParticleSystemType*>(o);
    this->ComputeMeanCurvature(ps, event.GetPositionIndex(), event.GetDomainIndex());
  }

  virtual void DomainAddEventCallback(itk::Object* o, const itk::EventObject& e) {
    Superclass::DomainAddEventCallback(o, e);
    m_MeanCurvatureList.push_back(0.0);
    m_CurvatureStandardDeviationList.push_back(0.0);
    const ParticleDomainAddEvent& event = dynamic_cast<const ParticleDomainAddEvent&>(e);
    const ParticleSystemType* ps = dynamic_cast<const ParticleSystemType*>(o);

    if (!ps->GetDomainFlag(event.GetDomainIndex())) {
      this->ComputeCurvatureStatistics(ps, event.GetDomainIndex());
    }
  }

  inline void ComputeMeanCurvature(const ParticleSystemType* system, unsigned int idx, unsigned int dom) {
    //  Get the position and index.
    PointType pos = system->GetPosition(idx, dom);
    this->operator[](dom)->operator[](idx) = system->GetDomain(dom)->GetCurvature(pos, idx);
  }

  virtual void ComputeCurvatureStatistics(const ParticleSystemType*, unsigned int d);

  double GetMeanCurvature(int d) { return m_MeanCurvatureList[d]; }
  double GetCurvatureStandardDeviation(int d) { return m_CurvatureStandardDeviationList[d]; }

 protected:
  MeanCurvatureContainer() {
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.DomainAddEvent = true;
  }
  virtual ~MeanCurvatureContainer(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  MeanCurvatureContainer(const Self&);  // purposely not implemented
  void operator=(const Self&);                  // purposely not implemented

  std::vector<double> m_MeanCurvatureList;
  std::vector<double> m_CurvatureStandardDeviationList;
  unsigned int m_verbosity;
};

}  // namespace shapeworks

#include "MeanCurvatureContainer.txx"
```


-------------------------------

Updated on 2023-07-27 at 15:31:34 +0000
