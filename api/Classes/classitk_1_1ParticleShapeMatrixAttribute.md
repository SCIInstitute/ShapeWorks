---
title: itk::ParticleShapeMatrixAttribute
summary: Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE NOT ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

---

# itk::ParticleShapeMatrixAttribute



Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES!  [More...](#detailed-description)


`#include <itkParticleShapeMatrixAttribute.h>`

Inherits from vnl_matrix< T >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

Inherited by [itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md), [itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-datatype)**  |
| typedef [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) | **[Self](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-self)**  |
| typedef [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| virtual [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & e) |
| virtual void | **[ResizeMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-resizematrix)**(int rs, int cs) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[PrintMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-checkfornans)**() |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-getdomainspershape)**() const |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-afteriteration)**() |
| virtual void | **[SetMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-setmatrix)**(const vnl_matrix< T > & m) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-particleshapematrixattribute)**() |
| virtual | **[~ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-~particleshapematrixattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#variable-m-domainspershape)**  |

## Additional inherited members

**Public Classes inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md)**  |

**Public Functions inherited from [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleAttribute.md#function-itktypemacro)**([ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) , DataObject ) |
| virtual void | **[EventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventcallback)**(Object * , const EventObject & ) |
| virtual void | **[EventWithIndexCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventwithindexcallback)**(Object * , const EventObject & ) |
| virtual void | **[TransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-transformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PrefixTransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-prefixtransformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[NeighborhoodSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-neighborhoodseteventcallback)**(Object * , const EventObject & ) |

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
class itk::ParticleShapeMatrixAttribute;
```

Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

Each column represents a single shape. 

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleShapeMatrixAttribute< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleShapeMatrixAttribute itk::ParticleShapeMatrixAttribute< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleAttribute<VDimension> itk::ParticleShapeMatrixAttribute< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleShapeMatrixAttribute< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleShapeMatrixAttribute< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleShapeMatrixAttribute< T, VDimension >::ConstWeakPointer;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. Run-time type information (and related methods). 


### function DomainAddEventCallback

```cpp
inline virtual ParticleAttribute void DomainAddEventCallback(
    Object * ,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-domainaddeventcallback)


**Reimplemented by**: [itk::ParticleShapeLinearRegressionMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function ResizeMatrix

```cpp
inline virtual void ResizeMatrix(
    int rs,
    int cs
)
```


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionaddeventcallback)


**Reimplemented by**: [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionaddeventcallback)


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionseteventcallback)


**Reimplemented by**: [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionremoveeventcallback)


**Reimplemented by**: [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionremoveeventcallback)


### function PrintMatrix

```cpp
inline void PrintMatrix()
```


### function CheckForNans

```cpp
inline bool CheckForNans()
```


### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    int i
)
```


Set/Get the number of domains per shape. This can only be safely done before shapes are initialized with points! 


### function GetDomainsPerShape

```cpp
inline int GetDomainsPerShape() const
```


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplemented by**: [itk::ParticleShapeLinearRegressionMatrixAttribute::BeforeIteration](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-beforeiteration), [itk::ParticleShapeLinearRegressionMatrixAttribute::BeforeIteration](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-beforeiteration), [itk::ParticleShapeMixedEffectsMatrixAttribute::BeforeIteration](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-beforeiteration), [itk::ParticleShapeMixedEffectsMatrixAttribute::BeforeIteration](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-beforeiteration)


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


### function SetMatrix

```cpp
inline virtual void SetMatrix(
    const vnl_matrix< T > & m
)
```


## Protected Functions Documentation

### function ParticleShapeMatrixAttribute

```cpp
inline ParticleShapeMatrixAttribute()
```


### function ~ParticleShapeMatrixAttribute

```cpp
inline virtual ~ParticleShapeMatrixAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


## Protected Attributes Documentation

### variable m_DomainsPerShape

```cpp
int m_DomainsPerShape;
```


-------------------------------

Updated on 2022-01-12 at 19:02:30 +0000