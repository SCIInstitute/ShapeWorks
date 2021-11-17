---
title: itk::ParticleContainerArrayAttribute

---

# itk::ParticleContainerArrayAttribute



 [More...](#detailed-description)

Inherits from std::vector< ParticleContainer< T >::Pointer >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-datatype)**  |
| typedef [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md) | **[Self](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-self)**  |
| typedef [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-itktypemacro)**([ParticleContainer](../Classes/classitk_1_1ParticleContainer.md) , [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) ) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[ZeroAllValues](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-zeroallvalues)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-particlecontainerarrayattribute)**() |
| virtual | **[~ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-~particlecontainerarrayattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

## Additional inherited members

**Public Classes inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md)**  |

**Public Functions inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[EventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventcallback)**(Object * , const EventObject & ) |
| virtual void | **[EventWithIndexCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventwithindexcallback)**(Object * , const EventObject & ) |
| virtual void | **[TransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-transformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PrefixTransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-prefixtransformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[NeighborhoodSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-neighborhoodseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionseteventcallback)**(Object * , const EventObject & ) |

**Protected Functions inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md#function-particleattribute)**() |
| virtual | **[~ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md#function-~particleattribute)**() |

**Public Attributes inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| [DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md) | **[m_DefinedCallbacks](../Classes/classitk_1_1ParticleAttribute.md#variable-m-definedcallbacks)**  |


## Detailed Description

```cpp
template <class T ,
unsigned int VDimension>
class itk::ParticleContainerArrayAttribute;
```

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleContainerArrayAttribute< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleContainerArrayAttribute itk::ParticleContainerArrayAttribute< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleAttribute<VDimension> itk::ParticleContainerArrayAttribute< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleContainerArrayAttribute< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleContainerArrayAttribute< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleContainerArrayAttribute< T, VDimension >::ConstWeakPointer;
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
    ParticleAttribute 
)
```


Run-time type information (and related methods). 


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-domainaddeventcallback)


**Reimplemented by**: [itk::ParticleMeanCurvatureAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionaddeventcallback)


**Reimplemented by**: [itk::ParticleMeanCurvatureAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-positionaddeventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionremoveeventcallback)


### function ZeroAllValues

```cpp
inline void ZeroAllValues()
```


## Protected Functions Documentation

### function ParticleContainerArrayAttribute

```cpp
inline ParticleContainerArrayAttribute()
```


### function ~ParticleContainerArrayAttribute

```cpp
inline virtual ~ParticleContainerArrayAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2021-11-17 at 00:54:20 +0000