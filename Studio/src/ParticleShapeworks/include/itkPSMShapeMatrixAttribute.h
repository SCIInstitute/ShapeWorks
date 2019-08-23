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
#ifndef __itkPSMShapeMatrixAttribute_h
#define __itkPSMShapeMatrixAttribute_h

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkPSMAttribute.h"
#include "itkPSMContainer.h"
#include "itkPSMParticleSystem.h"
#include "vnl/vnl_matrix.h"

namespace itk
{
/** \class PSMShapeMatrixAttribute
 *
 * \brief Each column describes a shape.  A shape may be composed of
 * m_DomainsPerShape domains (default 1). In this implementation, all
 * domains must have the same number of particles.
 *
 * This class defines a shape matrix. Each of the columns in the
 * matrix is a point-based representation of a different shape.
 *
 * \ingroup PSM
 * \ingroup PSMAttributes
 * \ingroup PSMContainers
 * \author Josh Cates
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT PSMShapeMatrixAttribute
  : public vnl_matrix<T>, public PSMAttribute<VDimension>
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef PSMShapeMatrixAttribute Self;
  typedef PSMAttribute<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMShapeMatrixAttribute, PSMAttribute);
  
  /** Perform any initialization steps.  This can be overridden by subclasses. */
  virtual void Initialize() {};

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the PSMParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object *, const EventObject &e);

  /** Resize the existing matrix to a given size. */  
  virtual void ResizeMatrix(int , int );

  /** Callback defining actions when a position is added to a
      ParticleSystem.  This method will likely be called from
      itkParticleSystem with the event ParticleAddEvent. */  
  virtual void PositionAddEventCallback(Object *o, const EventObject &e);
  
  /** Callback for defining actions when a position value is set --
      usually from ParticleSystem. */
  virtual void PositionSetEventCallback(Object *o, const EventObject &e);
  
  /** Reserved for future implementation. */  
  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    itkExceptionMacro("This Attribute class does not support particle position removal.");
  }
  
  /** Set/Get the number of domains per shape.  This can only be safely done
      before shapes are initialized with points! */
  void SetDomainsPerShape(int i)
  { m_DomainsPerShape = i; }
  int GetDomainsPerShape() const
  { return m_DomainsPerShape; }

  /** Method defining actions before the iteration of a solver that is
      using this class (via a PSM function object). */
  virtual void BeforeIteration() {}

  /** Method defining actions after the iteration of a solver that is
      using this class (via a PSM function object). */
  virtual void AfterIteration() {}

  /** Directly supply the matrix data. */
  virtual void SetMatrix(const vnl_matrix<T> &m)
  {
    vnl_matrix<T>::operator=(m);
  }
  
protected:
  PSMShapeMatrixAttribute() : m_DomainsPerShape(1)
  {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionSetEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~PSMShapeMatrixAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }

  int m_DomainsPerShape;

private:
  PSMShapeMatrixAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMShapeMatrixAttribute.hxx"
#endif

#endif
