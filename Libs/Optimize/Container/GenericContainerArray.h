#pragma once

#include <vector>

#include "GenericContainer.h"
#include "Observer.h"
#include "itkDataObject.h"
#include "ParticleEvents.h"
#include "itkWeakPointer.h"

namespace shapeworks {

/*!
 * @class GenericContainerArray
 * @brief This class appears to be an array of GenericContainers (e.g. std::vector) that implements the Observer
 * interface.  The array size tracks the number of domains in the system.  E.g. one per domain
 */
template <class T>
class GenericContainerArray : public std::vector<typename GenericContainer<T>::Pointer>, public Observer {
 public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef GenericContainerArray Self;
  typedef Observer Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleContainer, Observer);

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object*, const itk::EventObject&) {
    this->resize(this->size() + 1);
    this->operator[](this->size() - 1) = GenericContainer<T>::New();
  }

  virtual void PositionAddEventCallback(Object* o, const itk::EventObject& e) {
    const ParticlePositionAddEvent& event = dynamic_cast<const ParticlePositionAddEvent&>(e);
    this->operator[](event.GetDomainIndex())->operator[](event.GetPositionIndex()) = 0.0;
  }

  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {
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
  GenericContainerArray() {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~GenericContainerArray(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  GenericContainerArray(const Self&);  // purposely not implemented
  void operator=(const Self&);         // purposely not implemented
};

}  // namespace shapeworks
