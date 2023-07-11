---
title: shapeworks::Observer
summary: This class is an observer interface for classes to monitor for changes to the optimizer (observer pattern) 

---

# shapeworks::Observer



This class is an observer interface for classes to monitor for changes to the optimizer (observer pattern) 


`#include <Observer.h>`

Inherits from itk::DataObject

Inherited by [shapeworks::GenericContainerArray< TNumericType >](../Classes/classshapeworks_1_1GenericContainerArray.md), [shapeworks::GenericContainerArray< T >](../Classes/classshapeworks_1_1GenericContainerArray.md), [shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md), [shapeworks::ShapeGradientMatrix](../Classes/classshapeworks_1_1ShapeGradientMatrix.md), [shapeworks::ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md)

## Public Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [Observer](../Classes/classshapeworks_1_1Observer.md) | **[Self](../Classes/classshapeworks_1_1Observer.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1Observer.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1Observer.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1Observer.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1Observer.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1Observer.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1Observer.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1Observer.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1Observer.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1Observer.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1Observer.md#function-itktypemacro)**([Observer](../Classes/classshapeworks_1_1Observer.md) , DataObject ) |
| virtual void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-domainaddeventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[TransformSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-transformseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PrefixTransformSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-prefixtransformseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[NeighborhoodSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-neighborhoodseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionaddeventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[Observer](../Classes/classshapeworks_1_1Observer.md#function-observer)**() |
| virtual | **[~Observer](../Classes/classshapeworks_1_1Observer.md#function-~observer)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1Observer.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| [DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md) | **[m_DefinedCallbacks](../Classes/classshapeworks_1_1Observer.md#variable-m-definedcallbacks)**  |

## Public Types Documentation

### typedef Self

```cpp
typedef Observer shapeworks::Observer::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::Observer::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::Observer::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::Observer::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::Observer::ConstWeakPointer;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkTypeMacro

```cpp
itkTypeMacro(
    Observer ,
    DataObject 
)
```


Run-time type information (and related methods). 


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplemented by**: [shapeworks::GenericContainerArray::DomainAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-domainaddeventcallback), [shapeworks::GenericContainerArray::DomainAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-domainaddeventcallback), [shapeworks::LegacyShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-domainaddeventcallback), [shapeworks::LinearRegressionShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-domainaddeventcallback), [shapeworks::MixedEffectsShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-domainaddeventcallback), [shapeworks::ShapeGradientMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1ShapeGradientMatrix.md#function-domainaddeventcallback), [shapeworks::ShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function TransformSetEventCallback

```cpp
inline virtual void TransformSetEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


### function PrefixTransformSetEventCallback

```cpp
inline virtual void PrefixTransformSetEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


### function NeighborhoodSetEventCallback

```cpp
inline virtual void NeighborhoodSetEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplemented by**: [shapeworks::LegacyShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionseteventcallback), [shapeworks::LinearRegressionShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionseteventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionseteventcallback), [shapeworks::ShapeGradientMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1ShapeGradientMatrix.md#function-positionseteventcallback), [shapeworks::ShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionseteventcallback)


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplemented by**: [shapeworks::GenericContainerArray::PositionAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionaddeventcallback), [shapeworks::GenericContainerArray::PositionAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionaddeventcallback), [shapeworks::LegacyShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionaddeventcallback), [shapeworks::LinearRegressionShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionaddeventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionaddeventcallback), [shapeworks::ShapeGradientMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1ShapeGradientMatrix.md#function-positionaddeventcallback), [shapeworks::ShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionaddeventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplemented by**: [shapeworks::GenericContainerArray::PositionRemoveEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionremoveeventcallback), [shapeworks::GenericContainerArray::PositionRemoveEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionremoveeventcallback), [shapeworks::LegacyShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionremoveeventcallback), [shapeworks::LinearRegressionShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionremoveeventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionremoveeventcallback), [shapeworks::ShapeGradientMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1ShapeGradientMatrix.md#function-positionremoveeventcallback), [shapeworks::ShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionremoveeventcallback)


## Protected Functions Documentation

### function Observer

```cpp
inline Observer()
```


### function ~Observer

```cpp
inline virtual ~Observer()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


## Public Attributes Documentation

### variable m_DefinedCallbacks

```cpp
DefinedCallbacksStruct m_DefinedCallbacks;
```


-------------------------------

Updated on 2023-07-11 at 16:40:30 +0000