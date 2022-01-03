---
title: itk::ParticleAttribute

---

# itk::ParticleAttribute



 [More...](#detailed-description)

Inherits from DataObject

Inherited by [itk::ParticleContainerArrayAttribute< T, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md), [itk::ParticleGeneralShapeGradientMatrix< T, VDimension >](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md), [itk::ParticleGeneralShapeMatrix< T, VDimension >](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md), [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md), [itk::ParticleContainerArrayAttribute< double, Dimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md), [itk::ParticleContainerArrayAttribute< TNumericType, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md), [itk::ParticleGeneralShapeGradientMatrix< double, Dimension >](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md), [itk::ParticleGeneralShapeMatrix< double, Dimension >](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md), [itk::ParticleShapeMatrixAttribute< double, Dimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md), [itk::ParticleShapeMatrixAttribute< double, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)

## Public Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) | **[Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticleAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleAttribute.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleAttribute.md#function-itktypemacro)**([ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) , DataObject ) |
| virtual void | **[EventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventcallback)**(Object * , const EventObject & ) |
| virtual void | **[EventWithIndexCallback](../Classes/classitk_1_1ParticleAttribute.md#function-eventwithindexcallback)**(Object * , const EventObject & ) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & ) |
| virtual void | **[TransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-transformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PrefixTransformSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-prefixtransformseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[NeighborhoodSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-neighborhoodseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionseteventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionaddeventcallback)**(Object * , const EventObject & ) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md#function-particleattribute)**() |
| virtual | **[~ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md#function-~particleattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| [DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md) | **[m_DefinedCallbacks](../Classes/classitk_1_1ParticleAttribute.md#variable-m-definedcallbacks)**  |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticleAttribute;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleAttribute itk::ParticleAttribute< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject itk::ParticleAttribute< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleAttribute< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleAttribute< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleAttribute< VDimension >::ConstWeakPointer;
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
    ParticleAttribute ,
    DataObject 
)
```


Run-time type information (and related methods). 


### function EventCallback

```cpp
inline virtual void EventCallback(
    Object * ,
    const EventObject & 
)
```


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function EventWithIndexCallback

```cpp
inline virtual void EventWithIndexCallback(
    Object * ,
    const EventObject & 
)
```


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplemented by**: [itk::ParticleContainerArrayAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-domainaddeventcallback), [itk::ParticleContainerArrayAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-domainaddeventcallback), [itk::ParticleContainerArrayAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-domainaddeventcallback), [itk::ParticleGeneralShapeGradientMatrix::DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-domainaddeventcallback), [itk::ParticleGeneralShapeGradientMatrix::DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-domainaddeventcallback), [itk::ParticleGeneralShapeMatrix::DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-domainaddeventcallback), [itk::ParticleGeneralShapeMatrix::DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-domainaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-domainaddeventcallback), [itk::ParticleMeanCurvatureAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-domainaddeventcallback)


### function TransformSetEventCallback

```cpp
inline virtual void TransformSetEventCallback(
    Object * ,
    const EventObject & 
)
```


### function PrefixTransformSetEventCallback

```cpp
inline virtual void PrefixTransformSetEventCallback(
    Object * ,
    const EventObject & 
)
```


### function NeighborhoodSetEventCallback

```cpp
inline virtual void NeighborhoodSetEventCallback(
    Object * ,
    const EventObject & 
)
```


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplemented by**: [itk::ParticleGeneralShapeGradientMatrix::PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionseteventcallback), [itk::ParticleGeneralShapeGradientMatrix::PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionseteventcallback), [itk::ParticleGeneralShapeMatrix::PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionseteventcallback), [itk::ParticleGeneralShapeMatrix::PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionseteventcallback), [itk::ParticleMeanCurvatureAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-positionseteventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionseteventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionseteventcallback)


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplemented by**: [itk::ParticleContainerArrayAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionaddeventcallback), [itk::ParticleContainerArrayAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionaddeventcallback), [itk::ParticleContainerArrayAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionaddeventcallback), [itk::ParticleGeneralShapeGradientMatrix::PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionaddeventcallback), [itk::ParticleGeneralShapeGradientMatrix::PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionaddeventcallback), [itk::ParticleGeneralShapeMatrix::PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionaddeventcallback), [itk::ParticleGeneralShapeMatrix::PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionaddeventcallback), [itk::ParticleMeanCurvatureAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionaddeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionaddeventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplemented by**: [itk::ParticleContainerArrayAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionremoveeventcallback), [itk::ParticleContainerArrayAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionremoveeventcallback), [itk::ParticleContainerArrayAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionremoveeventcallback), [itk::ParticleGeneralShapeGradientMatrix::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionremoveeventcallback), [itk::ParticleGeneralShapeGradientMatrix::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionremoveeventcallback), [itk::ParticleGeneralShapeMatrix::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionremoveeventcallback), [itk::ParticleGeneralShapeMatrix::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionremoveeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeLinearRegressionMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionremoveeventcallback), [itk::ParticleShapeMixedEffectsMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionremoveeventcallback)


## Protected Functions Documentation

### function ParticleAttribute

```cpp
inline ParticleAttribute()
```


### function ~ParticleAttribute

```cpp
inline virtual ~ParticleAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


## Public Attributes Documentation

### variable m_DefinedCallbacks

```cpp
DefinedCallbacksStruct m_DefinedCallbacks;
```


-------------------------------

Updated on 2022-01-03 at 16:16:08 +0000