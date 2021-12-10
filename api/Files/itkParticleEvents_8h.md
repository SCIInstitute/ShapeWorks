---
title: Libs/Optimize/ParticleSystem/itkParticleEvents.h

---

# Libs/Optimize/ParticleSystem/itkParticleEvents.h



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
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleEvents.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleEvents_h
#define __itkParticleEvents_h

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


#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleEvents+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleEvents.txx"
#endif

#endif
```


-------------------------------

Updated on 2021-12-10 at 00:32:12 +0000
