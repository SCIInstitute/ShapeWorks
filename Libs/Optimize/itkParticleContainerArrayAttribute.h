#pragma once

#include <vector>

#include "itkDataObject.h"
#include "itkParticleAttribute.h"
#include "itkParticleContainer.h"
#include "itkWeakPointer.h"

namespace itk {

/*!
 * @class ParticleContainerArrayAttribute
 * @brief This class appears to be an array of ParticleContainers (e.g. std::vector) that implements the ParticleAttribute
 * listener interface.  The array size tracks the number of domains in the system.
 */
template <class T, unsigned int VDimension>
class ITK_EXPORT ParticleContainerArrayAttribute : public std::vector<typename ParticleContainer<T>::Pointer>,
                                                   public ParticleAttribute {
 public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef ParticleContainerArrayAttribute Self;
  typedef ParticleAttribute Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleContainer, ParticleAttribute);

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object*, const EventObject&) {
    this->resize(this->size() + 1);
    this->operator[](this->size() - 1) = ParticleContainer<T>::New();
  }

  virtual void PositionAddEventCallback(Object* o, const EventObject& e) {
    const itk::ParticlePositionAddEvent& event = dynamic_cast<const itk::ParticlePositionAddEvent&>(e);
    this->operator[](event.GetDomainIndex())->operator[](event.GetPositionIndex()) = 0.0;
  }

  virtual void PositionRemoveEventCallback(Object*, const EventObject&) {
    // NEED TO IMPLEMENT THIS
  }

  void ZeroAllValues() {
    for (unsigned d = 0; d < this->size(); d++) {
      for (unsigned int i = 0; i < this->operator[](d)->GetSize(); i++) {
        this->operator[](d)->operator[](i) = 0.0;
      }
    }
  }

 protected:
  ParticleContainerArrayAttribute() {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~ParticleContainerArrayAttribute(){};

  void PrintSelf(std::ostream& os, Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  ParticleContainerArrayAttribute(const Self&);  // purposely not implemented
  void operator=(const Self&);                   // purposely not implemented
};

}  // namespace itk
