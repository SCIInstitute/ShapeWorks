#pragma once

#include "itkEventObject.h"

namespace itk {
/**
 * \class ParticleEvent.
 *  Parent class for all Particle events.   Carries thread id information.
 *
 *  Event that carries Position index and a Domain index information.  This is
 *  used, for example to indicate which position has changed in the particle
 *  system on InvokeEvent.
 */
class ParticleEvent : public EventObject {
 public:
  typedef ParticleEvent Self;

  ParticleEvent() : m_ThreadID(0) {}
  virtual ~ParticleEvent() {}

  /** Get/Set the thread responsible for handling this particular event. */
  int GetThreadID() const { return m_ThreadID; }
  void SetThreadID(int i) { m_ThreadID = i; }

  /** Copy constructor and operator= */
  ParticleEvent(const ParticleEvent& v) : EventObject(v) {
    m_ThreadID = v.m_ThreadID;
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;
  }
  const ParticleEvent& operator=(const ParticleEvent& v) {
    m_ThreadID = v.m_ThreadID;
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;

    return *this;
  }

  /** Standard ITK event members. */
  virtual const char* GetEventName() const { return "ParticleEvent"; }

  virtual bool CheckEvent(const ::itk::EventObject* e) const { return dynamic_cast<const Self*>(e); }

  virtual ::itk::EventObject* MakeObject() const { return new Self; }

  /** Get/Set the index value. */
  inline void SetPositionIndex(int i) { m_PositionIndex = i; }
  int GetPositionIndex() const { return m_PositionIndex; }

  /** Get/Set the Domain index value. */
  inline void SetDomainIndex(int i) { m_DomainIndex = i; }
  int GetDomainIndex() const { return m_DomainIndex; }

 private:
  int m_ThreadID;
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

}  // end namespace itk
