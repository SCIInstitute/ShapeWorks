---
title: Libs/Optimize/ParticleSystem/itkParticleContainer.h

---

# Libs/Optimize/ParticleSystem/itkParticleContainer.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleContainer](../Classes/classitk_1_1ParticleContainer.md)**  |




## Source code

```cpp
#pragma once

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkCommand.h"
#include "itkParticleAttribute.h"
#include <map>

namespace itk
{
template <class T>
class ITK_EXPORT ParticleContainer : public DataObject
{
public:
  typedef T DataType;
  typedef ParticleContainer Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  itkNewMacro(Self);

  //  itkTypeMacro(ParticleContainer, ParticleAttribute);
  itkTypeMacro(ParticleContainer, DataObject);
  
  inline T &operator[](size_t k)  {
    if(k >= data.size()) {
      data.resize(k + 1);
    }
    return data[k];
  }

  inline T& Get(size_t k) {
    return (*this)[k];
  }

  unsigned long int GetSize() const  { return data.size(); }

protected:
  ParticleContainer() { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticleContainer: " << std::endl;
  }
  virtual ~ParticleContainer() {};

 private:
  ParticleContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<T> data;
  
};

} // end namespace itk
```


-------------------------------

Updated on 2022-07-22 at 21:28:02 +0000
