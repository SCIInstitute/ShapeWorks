---
title: Libs/Optimize/itkParticleEvents.h

---

# Libs/Optimize/itkParticleEvents.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleEvent](../Classes/classitk_1_1ParticleEvent.md)**  |
| class | **[itk::ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md)**  |




## Source code

```cpp
#pragma once

#include "itkEventObject.h"

namespace itk
{
class ITK_EXPORT ParticleEvent : public EventObject
{
public:
  typedef ParticleEvent Self;
  
  ParticleEvent() : m_ThreadID(0) {}
  virtual ~ParticleEvent() {}
  
  int GetThreadID() const
  { return m_ThreadID; }
  void SetThreadID(int i)
  { m_ThreadID = i; }

  ParticleEvent(const ParticleEvent &v) : EventObject(v)
  {
    m_ThreadID = v.m_ThreadID;
  }
  const ParticleEvent &operator=(const ParticleEvent &v)
  { m_ThreadID = v.m_ThreadID; return *this; }

  virtual const char * GetEventName() const { return "ParticleEvent"; } 

  virtual bool CheckEvent(const ::itk::EventObject* e) const   
  { return dynamic_cast<const Self*>(e); }
  
  virtual ::itk::EventObject* MakeObject() const  
  { return new Self; }  
  
private:
  int m_ThreadID;
};

class ITK_EXPORT ParticleEventWithIndex : public ParticleEvent
{
public:
  ParticleEventWithIndex() {}
  ~ParticleEventWithIndex() {}

  ParticleEventWithIndex(const ParticleEventWithIndex &v)
  {
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;
  }
  const ParticleEventWithIndex &operator=(const ParticleEventWithIndex &v)
  {
    ParticleEvent::operator=(v);
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;
    return *this;
  }
  
  inline void SetPositionIndex(int i)
  {    m_PositionIndex = i;  }
  int GetPositionIndex() const
  { return m_PositionIndex; }

  inline void SetDomainIndex(int i)
  {    m_DomainIndex = i;  }
  int GetDomainIndex() const
  { return m_DomainIndex; }

private:
  int m_PositionIndex;
  int m_DomainIndex;
  
};

itkEventMacro( ParticleDomainAddEvent, ParticleEventWithIndex );
itkEventMacro( ParticleTransformSetEvent, ParticleEventWithIndex );
itkEventMacro( ParticlePrefixTransformSetEvent, ParticleEventWithIndex );
itkEventMacro( ParticleNeighborhoodSetEvent, ParticleEventWithIndex );
itkEventMacro( ParticlePositionSetEvent, ParticleEventWithIndex );
itkEventMacro( ParticlePositionAddEvent, ParticleEventWithIndex );
itkEventMacro( ParticlePositionRemoveEvent, ParticleEventWithIndex );

} // end namespace itk
```


-------------------------------

Updated on 2022-10-24 at 22:34:05 +0000
