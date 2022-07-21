---
title: Libs/Optimize/ParticleSystem/itkParticleContainerArrayAttribute.h

---

# Libs/Optimize/ParticleSystem/itkParticleContainerArrayAttribute.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)**  |




## Source code

```cpp
#pragma once

#include "itkDataObject.h"
#include "itkWeakPointer.h"
#include "itkParticleAttribute.h"
#include "itkParticleContainer.h"
#include <vector>

namespace itk
{
template <class T, unsigned int VDimension>
class ITK_EXPORT ParticleContainerArrayAttribute
  : public std::vector<typename ParticleContainer<T>::Pointer >, public ParticleAttribute<VDimension>
{
public:
  typedef T DataType;
  typedef ParticleContainerArrayAttribute Self;
  typedef ParticleAttribute<VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleContainer, ParticleAttribute);

  virtual void DomainAddEventCallback(Object *, const EventObject &)
  {
    this->resize( this->size() +1);
    this->operator[](this->size() -1) = ParticleContainer<T>::New();
  }

  virtual void PositionAddEventCallback(Object *o, const EventObject &e) 
  {
    const itk::ParticlePositionAddEvent &event
      = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
    this->operator[](event.GetDomainIndex())->operator[](event.GetPositionIndex()) = 0.0;    
  }

  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
  {
    // NEED TO IMPLEMENT THIS
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
  ParticleContainerArrayAttribute()
  {
    this->m_DefinedCallbacks.DomainAddEvent = true;
    this->m_DefinedCallbacks.PositionAddEvent = true;
    this->m_DefinedCallbacks.PositionRemoveEvent = true;
  }
  virtual ~ParticleContainerArrayAttribute() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {  Superclass::PrintSelf(os,indent);  }
  
 private:
  ParticleContainerArrayAttribute(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};

} // end namespace
```


-------------------------------

Updated on 2022-07-21 at 19:15:50 +0000
