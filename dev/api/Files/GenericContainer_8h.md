---
title: Libs/Optimize/Container/GenericContainer.h

---

# Libs/Optimize/Container/GenericContainer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md)** <br>This class seems to be only a wrapper around std::vector implementing ITK DataObject (for smart pointer?)  |




## Source code

```cpp
#pragma once

#include <map>

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace shapeworks {
template <class T>
class GenericContainer : public itk::DataObject {
 public:
  typedef T DataType;
  typedef GenericContainer Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleContainer, itk::DataObject);

  inline T& operator[](size_t k) {
    if (k >= data.size()) {
      data.resize(k + 1);
    }
    return data[k];
  }

  inline T& Get(size_t k) { return (*this)[k]; }

  unsigned long int GetSize() const { return data.size(); }

 protected:
  GenericContainer() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticleContainer: " << std::endl;
  }
  virtual ~GenericContainer(){};

 private:
  GenericContainer(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented

  std::vector<T> data;
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2023-05-11 at 06:01:12 +0000
