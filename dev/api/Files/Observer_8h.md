---
title: Libs/Optimize/Observer.h

---

# Libs/Optimize/Observer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)** <br>This class is an observer interface for classes to monitor for changes to the optimizer (observer pattern)  |
| struct | **[shapeworks::Observer::DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md)**  |




## Source code

```cpp
#pragma once

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace shapeworks {

class Observer : public itk::DataObject {
 public:
  typedef Observer Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(Observer, DataObject);

  struct DefinedCallbacksStruct {
    DefinedCallbacksStruct()
        : DomainAddEvent(false),
          TransformSetEvent(false),
          PrefixTransformSetEvent(false),
          NeighborhoodSetEvent(false),
          PositionSetEvent(false),
          PositionAddEvent(false),
          PositionRemoveEvent(false) {}
    bool DomainAddEvent;
    bool TransformSetEvent;
    bool PrefixTransformSetEvent;
    bool NeighborhoodSetEvent;
    bool PositionSetEvent;
    bool PositionAddEvent;
    bool PositionRemoveEvent;
  };

  DefinedCallbacksStruct m_DefinedCallbacks;

  virtual void DomainAddEventCallback(Object*, const itk::EventObject&) {}
  virtual void TransformSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PrefixTransformSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void NeighborhoodSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionSetEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionAddEventCallback(Object*, const itk::EventObject&) {}
  virtual void PositionRemoveEventCallback(Object*, const itk::EventObject&) {}

 protected:
  Observer() {}
  virtual ~Observer(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  Observer(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-10 at 02:21:13 +0000
