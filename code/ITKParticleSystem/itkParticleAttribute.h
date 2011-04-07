/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleAttribute.h,v $

  Date:      $Date: 2011/03/24 01:17:33 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleAttribute_h

#define __itkParticleAttribute_h



#include "itkDataObject.h"

#include "itkPoint.h"

#include "itkWeakPointer.h"

#include "itkCommand.h"

#include "itkParticleEvents.h"



namespace itk

{



#ifndef __itkParticleSystem_h

template<unsigned int VDimension> class ParticleSystem;

#endif



/** \class ParticleAttribute

 *  \brief 

 */

template< unsigned int VDimension>

class ITK_EXPORT ParticleAttribute : public DataObject

{

public:

  /** Standard class typedefs */

  typedef ParticleAttribute Self;

  typedef DataObject Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self>  ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);



  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleAttribute, DataObject);



  /** Data structure indicating which callback functions are defined by a

      subclass.  The ParticleSystem class will reference this structure to

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

      should be set to true so that the ParticleSystem will know to register

      the appropriate event with this method. */

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





//#if ITK_TEMPLATE_EXPLICIT

//# include "Templates/itkParticleAttribute+-.h"

//#endif



//#if ITK_TEMPLATE_TXX

//# include "itkParticleAttribute.txx"

//#endif



#endif

