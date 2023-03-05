#pragma once

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkParticleEvents.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk {

/** \class ParticleAttribute
 *  \brief
 */
class ParticleAttribute : public DataObject {
 public:
  /** Standard class typedefs */
  typedef ParticleAttribute Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleAttribute, DataObject);

  /** Data structure indicating which callback functions are defined by a
      subclass.  The ParticleSystem class will reference this structure to
      determine which callbacks to register.  This is just a list of every
      event defined in itkParticleEvent.h */
  struct DefinedCallbacksStruct {
    DefinedCallbacksStruct()
        : DomainAddEvent(false),
          TransformSetEvent(false),
          PrefixTransformSetEvent(false),
          NeighborhoodSetEvent(false),
          PositionSetEvent(false),
          PositionAddEvent(false),
          PositionRemoveEvent(false) {}
    bool DomainAddEvent;
    bool TransformSetEvent;
    bool PrefixTransformSetEvent;
    bool NeighborhoodSetEvent;
    bool PositionSetEvent;
    bool PositionAddEvent;
    bool PositionRemoveEvent;
  };

  DefinedCallbacksStruct m_DefinedCallbacks;

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */

  virtual void EventWithIndexCallback(Object*, const EventObject&) {}
  virtual void DomainAddEventCallback(Object*, const EventObject&) {}
  virtual void TransformSetEventCallback(Object*, const EventObject&) {}
  virtual void PrefixTransformSetEventCallback(Object*, const EventObject&) {}
  virtual void NeighborhoodSetEventCallback(Object*, const EventObject&) {}
  virtual void PositionSetEventCallback(Object*, const EventObject&) {}
  virtual void PositionAddEventCallback(Object*, const EventObject&) {}
  virtual void PositionRemoveEventCallback(Object*, const EventObject&) {}

 protected:
  ParticleAttribute() {}
  virtual ~ParticleAttribute(){};

  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  ParticleAttribute(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented
};

}  // end namespace itk
