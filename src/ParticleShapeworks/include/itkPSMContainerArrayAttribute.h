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
#ifndef __itkPSMContainerArrayAttribute_h
#define __itkPSMContainerArrayAttribute_h

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkPSMAttribute.h"
#include "itkPSMContainer.h"
#include <vector>

namespace itk
{
/** \class PSMContainerArrayAttribute
 * \brief 
 * \ingroup PSM
 * \ingroup PSMAttributes
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT PSMContainerArrayAttribute
  : public std::vector<typename PSMContainer<T>::Pointer >,
  public PSMAttribute<VDimension>
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef PSMContainerArrayAttribute Self;
  typedef PSMAttribute<VDimension> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMContainerArrayAttribute, PSMAttribute);

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object *, const EventObject &)
  {
    this->resize( this->size() +1);
    this->operator[](this->size() -1) = PSMContainer<T>::New();
  }

  virtual void PositionAddEventCallback(Object *, const EventObject &e) 
  {
    const itk::ParticlePositionAddEvent &event
      = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
    this->operator[](event.GetDomainIndex())->operator[](event.GetPositionIndex()) = 0.0;    
  }

  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    itkExceptionMacro("This class does note support removal of particles positions.");
  }

  void ZeroAllValues()
  {
    for (unsigned d = 0; d < this->size(); d++)
      {
        for (unsigned int i = 0; i < this->operator[](d)->GetSize(); i++)
          {
            this->operator[](d)->operator[](i) = 0.0;
          }
        
      }
  }
  
 protected:
  PSMContainerArrayAttribute()
    {
      this->m_DefinedCallbacks.DomainAddEvent = true;
      this->m_DefinedCallbacks.PositionAddEvent = true;
      this->m_DefinedCallbacks.PositionRemoveEvent = true;
    }
  virtual ~PSMContainerArrayAttribute() {};
  
  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }
  
 private:
  PSMContainerArrayAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};
 
} // end namespace

#endif
