---
title: Libs/Optimize/ParticleEvents.h

---

# Libs/Optimize/ParticleEvents.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md)**  |




## Source code

```cpp
#pragma once

#include "itkEventObject.h"

namespace shapeworks {
class ParticleEvent : public itk::EventObject {
 public:
  typedef ParticleEvent Self;

  ParticleEvent() {}
  virtual ~ParticleEvent() {}


  ParticleEvent(const ParticleEvent& v) : itk::EventObject(v) {
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;
  }
  const ParticleEvent& operator=(const ParticleEvent& v) {
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;

    return *this;
  }

  virtual const char* GetEventName() const { return "ParticleEvent"; }

  virtual bool CheckEvent(const ::itk::EventObject* e) const { return dynamic_cast<const Self*>(e); }

  virtual ::itk::EventObject* MakeObject() const { return new Self; }

  inline void SetPositionIndex(int i) { m_PositionIndex = i; }
  int GetPositionIndex() const { return m_PositionIndex; }

  inline void SetDomainIndex(int i) { m_DomainIndex = i; }
  int GetDomainIndex() const { return m_DomainIndex; }

 private:
  int m_PositionIndex;
  int m_DomainIndex;
};

itkEventMacro(ParticleDomainAddEvent, ParticleEvent);
itkEventMacro(ParticleTransformSetEvent, ParticleEvent);
itkEventMacro(ParticlePrefixTransformSetEvent, ParticleEvent);
itkEventMacro(ParticleNeighborhoodSetEvent, ParticleEvent);
itkEventMacro(ParticlePositionSetEvent, ParticleEvent);
itkEventMacro(ParticlePositionAddEvent, ParticleEvent);
itkEventMacro(ParticlePositionRemoveEvent, ParticleEvent);

}  // end namespace shapeworks
```


-------------------------------

Updated on 2024-05-16 at 20:36:02 +0000
