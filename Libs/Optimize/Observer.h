#pragma once

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace shapeworks {

/*!
 * @class ParticleAttribute
 * @brief This class is an observer interface (observer pattern)
 */
class ParticleAttribute : public itk::DataObject {
 public:
  /** Standard class typedefs */
  typedef ParticleAttribute Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleAttribute, DataObject);

  /** Data structure indicating which callback functions are defined by a
      subclass.  The ParticleSystem class will reference this structure to
      determine which callbacks to register. */
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

  virtual void DomainAddEventCallback(Object*, const itk::EventObject&) {}
  virtual void TransformSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PrefixTransformSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void NeighborhoodSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionAddEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {}

 protected:
  ParticleAttribute() {}
  virtual ~ParticleAttribute(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  ParticleAttribute(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented
};

}  // namespace shapeworks
