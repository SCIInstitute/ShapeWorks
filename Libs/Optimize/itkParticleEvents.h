#pragma once

#include "itkEventObject.h"

namespace itk
{
/**
 * \class ParticleEvent.
 *  Parent class for all Particle events.   Carries thread id information.
*/
class ITK_EXPORT ParticleEvent : public EventObject
{
public:
  typedef ParticleEvent Self;
  
  ParticleEvent() : m_ThreadID(0) {}
  virtual ~ParticleEvent() {}
  
  /** Get/Set the thread responsible for handling this particular event. */
  int GetThreadID() const
  { return m_ThreadID; }
  void SetThreadID(int i)
  { m_ThreadID = i; }

  /** Copy constructor and operator= */
  ParticleEvent(const ParticleEvent &v) : EventObject(v)
  {
    m_ThreadID = v.m_ThreadID;
  }
  const ParticleEvent &operator=(const ParticleEvent &v)
  { m_ThreadID = v.m_ThreadID; return *this; }

  /** Standard ITK event members. */
  virtual const char * GetEventName() const { return "ParticleEvent"; } 

  virtual bool CheckEvent(const ::itk::EventObject* e) const   
  { return dynamic_cast<const Self*>(e); }
  
  virtual ::itk::EventObject* MakeObject() const  
  { return new Self; }  
  
private:
  int m_ThreadID;
};

/**
 * \class ParticleEventWithIndex
 *
 *  Event that carries Position index and a Domain index information.  This is
 *  used, for example to indicate which position has changed in the particle
 *  system on InvokeEvent.
 */
class ITK_EXPORT ParticleEventWithIndex : public ParticleEvent
{
public:
  ParticleEventWithIndex() {}
  ~ParticleEventWithIndex() {}

  /** Copy constructor and operator=. */
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
  
  /** Get/Set the index value. */
  inline void SetPositionIndex(int i)
  {    m_PositionIndex = i;  }
  int GetPositionIndex() const
  { return m_PositionIndex; }

  /** Get/Set the Domain index value. */
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

