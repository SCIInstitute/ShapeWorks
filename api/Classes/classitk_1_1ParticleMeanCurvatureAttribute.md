---
title: itk::ParticleMeanCurvatureAttribute

---

# itk::ParticleMeanCurvatureAttribute



 [More...](#detailed-description)

Inherits from [itk::ParticleContainerArrayAttribute< TNumericType, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md), std::vector< ParticleContainer< T >::Pointer >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef TNumericType | **[NumericType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-numerictype)**  |
| typedef [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) | **[Self](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-self)**  |
| typedef [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)< TNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-particlesystemtype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-pointtype)**  |
| typedef vnl_vector_fixed< TNumericType, VDimension > | **[VnlVectorType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-vnlvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-itktypemacro)**([ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) , [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md) ) |
| void | **[SetVerbosity](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-setverbosity)**(unsigned int val) |
| unsigned int | **[GetVerbosity](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-getverbosity)**() |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & ) |
| void | **[ComputeMeanCurvature](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-computemeancurvature)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-particlesystemtype) * system, unsigned int idx, unsigned int dom) |
| virtual void | **[ComputeCurvatureStatistics](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-computecurvaturestatistics)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#typedef-particlesystemtype) * , unsigned int d) |
| double | **[GetMeanCurvature](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-getmeancurvature)**(int d) |
| double | **[GetCurvatureStandardDeviation](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-getcurvaturestandarddeviation)**(int d) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-particlemeancurvatureattribute)**() |
| virtual | **[~ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-~particlemeancurvatureattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

## Additional inherited members

**Public Types inherited from [itk::ParticleContainerArrayAttribute< TNumericType, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#typedef-datatype)**  |

**Public Functions inherited from [itk::ParticleContainerArrayAttribute< TNumericType, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[ZeroAllValues](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-zeroallvalues)**() |

**Protected Functions inherited from [itk::ParticleContainerArrayAttribute< TNumericType, VDimension >](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-particlecontainerarrayattribute)**() |
| virtual | **[~ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-~particlecontainerarrayattribute)**() |

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
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |

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
template <class TNumericType ,
unsigned int VDimension>
class itk::ParticleMeanCurvatureAttribute;
```

## Public Types Documentation

### typedef NumericType

```cpp
typedef TNumericType itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::NumericType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleMeanCurvatureAttribute itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleContainerArrayAttribute<TNumericType,VDimension> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::ParticleSystemType;
```


Numeric types. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::PointType;
```


### typedef VnlVectorType

```cpp
typedef vnl_vector_fixed<TNumericType, VDimension> itk::ParticleMeanCurvatureAttribute< TNumericType, VDimension >::VnlVectorType;
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
    ParticleMeanCurvatureAttribute ,
    ParticleContainerArrayAttribute 
)
```


Run-time type information (and related methods). 


### function SetVerbosity

```cpp
inline void SetVerbosity(
    unsigned int val
)
```


### function GetVerbosity

```cpp
inline unsigned int GetVerbosity()
```


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleContainerArrayAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-positionaddeventcallback)


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionseteventcallback)


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleContainerArrayAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleContainerArrayAttribute.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function ComputeMeanCurvature

```cpp
inline void ComputeMeanCurvature(
    const ParticleSystemType * system,
    unsigned int idx,
    unsigned int dom
)
```


### function ComputeCurvatureStatistics

```cpp
virtual void ComputeCurvatureStatistics(
    const ParticleSystemType * ,
    unsigned int d
)
```


Compute the mean and std deviation of the curvature on the image surface. 


### function GetMeanCurvature

```cpp
inline double GetMeanCurvature(
    int d
)
```


### function GetCurvatureStandardDeviation

```cpp
inline double GetCurvatureStandardDeviation(
    int d
)
```


## Protected Functions Documentation

### function ParticleMeanCurvatureAttribute

```cpp
inline ParticleMeanCurvatureAttribute()
```


### function ~ParticleMeanCurvatureAttribute

```cpp
inline virtual ~ParticleMeanCurvatureAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2021-11-14 at 17:52:09 +0000