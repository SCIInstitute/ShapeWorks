---
title: shapeworks::MeanCurvatureContainer

---

# shapeworks::MeanCurvatureContainer



 [More...](#detailed-description)

Inherits from [shapeworks::GenericContainerArray< TNumericType >](../Classes/classshapeworks_1_1GenericContainerArray.md), std::vector< GenericContainer< T >::Pointer >, [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef TNumericType | **[NumericType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-numerictype)**  |
| typedef [MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) | **[Self](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-self)**  |
| typedef [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< TNumericType > | **[Superclass](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) > | **[Pointer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) > | **[ConstPointer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[ParticleSystemType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-particlesystemtype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-pointtype)**  |
| typedef vnl_vector_fixed< TNumericType, VDimension > | **[VnlVectorType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-vnlvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-itktypemacro)**([ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md) , [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md) ) |
| void | **[SetVerbosity](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-setverbosity)**(unsigned int val) |
| unsigned int | **[GetVerbosity](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-getverbosity)**() |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-positionaddeventcallback)**(itk::Object * o, const itk::EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-positionseteventcallback)**(itk::Object * o, const itk::EventObject & e) |
| virtual void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-domainaddeventcallback)**(itk::Object * o, const itk::EventObject & e) |
| void | **[ComputeMeanCurvature](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-computemeancurvature)**(const [ParticleSystemType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-particlesystemtype) * system, unsigned int idx, unsigned int dom) |
| virtual void | **[ComputeCurvatureStatistics](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-computecurvaturestatistics)**(const [ParticleSystemType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#typedef-particlesystemtype) * , unsigned int d) |
| double | **[GetMeanCurvature](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-getmeancurvature)**(int d) |
| double | **[GetCurvatureStandardDeviation](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-getcurvaturestandarddeviation)**(int d) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-meancurvaturecontainer)**() |
| virtual | **[~MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-~meancurvaturecontainer)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1MeanCurvatureContainer.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Additional inherited members

**Public Types inherited from [shapeworks::GenericContainerArray< TNumericType >](../Classes/classshapeworks_1_1GenericContainerArray.md)**

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classshapeworks_1_1GenericContainerArray.md#typedef-datatype)**  |

**Public Functions inherited from [shapeworks::GenericContainerArray< TNumericType >](../Classes/classshapeworks_1_1GenericContainerArray.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1GenericContainerArray.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[ZeroAllValues](../Classes/classshapeworks_1_1GenericContainerArray.md#function-zeroallvalues)**() |

**Protected Functions inherited from [shapeworks::GenericContainerArray< TNumericType >](../Classes/classshapeworks_1_1GenericContainerArray.md)**

|                | Name           |
| -------------- | -------------- |
| | **[GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md#function-genericcontainerarray)**() |
| virtual | **[~GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md#function-~genericcontainerarray)**() |

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
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |

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
template <class TNumericType ,
unsigned int VDimension>
class shapeworks::MeanCurvatureContainer;
```

## Public Types Documentation

### typedef NumericType

```cpp
typedef TNumericType shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::NumericType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef MeanCurvatureContainer shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef GenericContainerArray<TNumericType> shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::ParticleSystemType;
```


Numeric types. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::PointType;
```


### typedef VnlVectorType

```cpp
typedef vnl_vector_fixed<TNumericType, VDimension> shapeworks::MeanCurvatureContainer< TNumericType, VDimension >::VnlVectorType;
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
    GenericContainerArray 
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
    itk::Object * o,
    const itk::EventObject & e
)
```


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    itk::Object * o,
    const itk::EventObject & e
)
```


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    itk::Object * o,
    const itk::EventObject & e
)
```


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

### function MeanCurvatureContainer

```cpp
inline MeanCurvatureContainer()
```


### function ~MeanCurvatureContainer

```cpp
inline virtual ~MeanCurvatureContainer()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


-------------------------------

Updated on 2024-07-10 at 20:24:01 +0000