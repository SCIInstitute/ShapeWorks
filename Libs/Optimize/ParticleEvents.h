#pragma once

#include "itkEventObject.h"

namespace shapeworks {
/**
 * \class ParticleEvent.
 *  Parent class for all Particle events.
 *
 *  Event that carries Position index and a Domain index information.  This is
 *  used, for example to indicate which position has changed in the particle
 *  system on InvokeEvent.
 */
class ParticleEvent : public itk::EventObject {
 public:
  typedef ParticleEvent Self;

  ParticleEvent() {}
  virtual ~ParticleEvent() {}


  /** Copy constructor and operator= */
  ParticleEvent(const ParticleEvent& v) : itk::EventObject(v) {
    m_PositionIndex = v.m_PositionIndex;
    m_DomainIndex = v.m_DomainIndex;
  }
  const ParticleEvent& operator=(const ParticleEvent& v) {
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
  int m_PositionIndex;
  int m_DomainIndex;
};

itkEventMacro(ParticleDomainAddEvent, ParticleEvent);
itkEventMacro(ParticleTransformSetEvent, ParticleEvent);
itkEventMacro(ParticlePrefixTransformSetEvent, ParticleEvent);
itkEventMacro(ParticlePositionSetEvent, ParticleEvent);
itkEventMacro(ParticlePositionAddEvent, ParticleEvent);
itkEventMacro(ParticlePositionRemoveEvent, ParticleEvent);

}  // end namespace shapeworks
