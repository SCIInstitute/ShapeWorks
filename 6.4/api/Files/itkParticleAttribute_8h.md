---
title: Libs/Optimize/itkParticleAttribute.h

---

# Libs/Optimize/itkParticleAttribute.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md)**  |
| struct | **[itk::ParticleAttribute::DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md)**  |




## Source code

```cpp
#pragma once

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkCommand.h"
#include "itkParticleEvents.h"

namespace itk
{

//class ParticleSystem;

template< unsigned int VDimension>
class ITK_EXPORT ParticleAttribute : public DataObject
{
public:
  typedef ParticleAttribute Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  itkNewMacro(Self);

  itkTypeMacro(ParticleAttribute, DataObject);

  struct DefinedCallbacksStruct
  {
    DefinedCallbacksStruct() : Event(false),
                               EventWithIndex(false),
                               DomainAddEvent(false),
                               TransformSetEvent(false),
                               PrefixTransformSetEvent(false),
                               NeighborhoodSetEvent(false),
                               PositionSetEvent(false),
                               PositionAddEvent(false),
                               PositionRemoveEvent(false) {}
    bool Event;
    bool EventWithIndex;
    bool DomainAddEvent;
    bool TransformSetEvent;
    bool PrefixTransformSetEvent;
    bool NeighborhoodSetEvent;
    bool PositionSetEvent;
    bool PositionAddEvent;
    bool PositionRemoveEvent;
  };
  
  DefinedCallbacksStruct  m_DefinedCallbacks;

//   virtual void EventCallback(ParticleSystem<VDimension> *, const ParticleEvent &) {}
//   virtual void EventWithIndexCallback(ParticleSystem<VDimension> *, const ParticleEventWithIndex &) {}
//   virtual void DomainAddEventCallback(ParticleSystem<VDimension> *, const ParticleDomainAddEvent &) {}

//   virtual void TransformSetEventCallback(ParticleSystem<VDimension> *, const ParticleTransformSetEvent &) {}
//   virtual void NeighborhoodSetEventCallback(ParticleSystem<VDimension> *, const ParticleNeighborhoodSetEvent &) {}
//   virtual void PositionSetEventCallback(ParticleSystem<VDimension> *, const ParticlePositionSetEvent &) {}
//   virtual void PositionAddEventCallback(ParticleSystem<VDimension> *, const ParticlePositionAddEvent &) {}
//   virtual void PositionRemoveEventCallback(ParticleSystem<VDimension> *,
//   const ParticlePositionRemoveEvent &) {}

  virtual void EventCallback(Object *, const EventObject &) {}
  virtual void EventWithIndexCallback(Object *, const EventObject &) {}
  virtual void DomainAddEventCallback(Object *, const EventObject &) {}
  virtual void TransformSetEventCallback(Object *, const EventObject &) {}
  virtual void PrefixTransformSetEventCallback(Object *, const EventObject &) {}
  virtual void NeighborhoodSetEventCallback(Object *, const EventObject &) {}
  virtual void PositionSetEventCallback(Object *, const EventObject &) {}
  virtual void PositionAddEventCallback(Object *, const EventObject &) {}
  virtual void PositionRemoveEventCallback(Object *, const EventObject &) {}

protected:
  ParticleAttribute() {}
  virtual ~ParticleAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  ParticleAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk
```


-------------------------------

Updated on 2023-03-11 at 06:26:44 +0000
