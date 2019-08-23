/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMAttribute_h
#define __itkPSMAttribute_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkCommand.h"
#include "itkPSMEvents.h"

namespace itk
{

/** \class PSMAttribute
 *  \brief Base class for PSMParticleSystem attribute classes.
 * \ingroup ParticleShapeModeling
 */
template< unsigned int VDimension>
class ITK_EXPORT PSMAttribute : public DataObject
{
public:
  /** Standard class typedefs */
  typedef PSMAttribute Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMAttribute, DataObject);

  /** Data structure indicating which callback functions are defined by a
      subclass.  The PSMParticleSystem class will reference this structure to
      determine which callbacks to register.  This is just a list of every
      event defined in itkParticleEvent.h */
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

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the PSMParticleSystem will know to register
      the appropriate event with this method. */
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
  PSMAttribute() {}
  virtual ~PSMAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

private:
  PSMAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#endif
