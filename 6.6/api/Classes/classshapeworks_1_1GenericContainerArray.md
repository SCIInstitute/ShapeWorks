---
title: shapeworks::GenericContainerArray
summary: This class appears to be an array of GenericContainers (e.g. std::vector) that implements the Observer interface. The array size tracks the number of domains in the system. E.g. one per domain. 

---

# shapeworks::GenericContainerArray



This class appears to be an array of GenericContainers (e.g. std::vector) that implements the [Observer]() interface. The array size tracks the number of domains in the system. E.g. one per domain.  [More...](#detailed-description)


`#include <GenericContainerArray.h>`

Inherits from std::vector< GenericContainer< T >::Pointer >, [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-datatype)**  |
| typedef [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md) | **[Self](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-self)**  |
| typedef [Observer](../Classes/classshapeworks_1_1Observer.md) | **[Superclass](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1GenericContainerArray.md) > | **[Pointer](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1GenericContainerArray.md) > | **[ConstPointer](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1GenericContainerArray.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1GenericContainerArray.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1GenericContainerArray.md) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1GenericContainerArray.md#function-itktypemacro)**(ParticleContainer , [Observer](../Classes/classshapeworks_1_1Observer.md) ) |
| virtual void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-domainaddeventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionaddeventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[ZeroAllValues](../Classes/classshapeworks_1_1GenericContainerArray.md#function-zeroallvalues)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md#function-genericcontainerarray)**() |
| virtual | **[~GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md#function-~genericcontainerarray)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1GenericContainerArray.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Additional inherited members

**Public Classes inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md)**  |

**Public Functions inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[TransformSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-transformseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PrefixTransformSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-prefixtransformseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[NeighborhoodSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-neighborhoodseteventcallback)**(Object * , const itk::EventObject & ) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionseteventcallback)**(Object * , const itk::EventObject & ) |

**Protected Functions inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Observer](../Classes/classshapeworks_1_1Observer.md#function-observer)**() |
| virtual | **[~Observer](../Classes/classshapeworks_1_1Observer.md#function-~observer)**() |

**Public Attributes inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| [DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md) | **[m_DefinedCallbacks](../Classes/classshapeworks_1_1Observer.md#variable-m-definedcallbacks)**  |


## Detailed Description

```cpp
template <class T >
class shapeworks::GenericContainerArray;
```

This class appears to be an array of GenericContainers (e.g. std::vector) that implements the [Observer]() interface. The array size tracks the number of domains in the system. E.g. one per domain. 
## Public Types Documentation

### typedef DataType

```cpp
typedef T shapeworks::GenericContainerArray< T >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef GenericContainerArray shapeworks::GenericContainerArray< T >::Self;
```


### typedef Superclass

```cpp
typedef Observer shapeworks::GenericContainerArray< T >::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::GenericContainerArray< T >::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::GenericContainerArray< T >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::GenericContainerArray< T >::ConstWeakPointer;
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
    ParticleContainer ,
    Observer 
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


**Reimplements**: [shapeworks::Observer::DomainAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::Observer::PositionAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionaddeventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplements**: [shapeworks::Observer::PositionRemoveEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionremoveeventcallback)


### function ZeroAllValues

```cpp
inline void ZeroAllValues()
```


## Protected Functions Documentation

### function GenericContainerArray

```cpp
inline GenericContainerArray()
```


### function ~GenericContainerArray

```cpp
inline virtual ~GenericContainerArray()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


-------------------------------

Updated on 2024-05-06 at 07:59:17 +0000