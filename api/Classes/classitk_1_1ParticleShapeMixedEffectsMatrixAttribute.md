---
title: itk::ParticleShapeMixedEffectsMatrixAttribute

---

# itk::ParticleShapeMixedEffectsMatrixAttribute



 [More...](#detailed-description)

Inherits from [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md), vnl_matrix< T >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-datatype)**  |
| typedef [ParticleShapeMixedEffectsMatrixAttribute](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md) | **[Self](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-self)**  |
| typedef [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)< T, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-itktypemacro)**([ParticleShapeMixedEffectsMatrixAttribute](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md) , [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) ) |
| void | **[UpdateMeanMatrix](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-updatemeanmatrix)**() |
| vnl_vector< double > | **[ComputeMean](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-computemean)**(double k) const |
| void | **[ResizeParameters](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-resizeparameters)**(unsigned int n) |
| virtual void | **[ResizeMeanMatrix](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-resizemeanmatrix)**(int rs, int cs) |
| void | **[ResizeExplanatory](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-resizeexplanatory)**(unsigned int n) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & e) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getdomainspershape)**() const |
| void | **[SetTimeptsPerIndividual](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-settimeptsperindividual)**(int i) |
| int | **[GetTimeptsPerIndividual](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-gettimeptsperindividual)**() const |
| void | **[SetExplanatory](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setexplanatory)**(std::vector< double > v) |
| void | **[SetExplanatory](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setexplanatory)**(unsigned int i, double q) |
| const double & | **[GetExplanatory](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getexplanatory)**(unsigned int i) const |
| double & | **[GetExplanatory](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getexplanatory)**(unsigned int i) |
| const vnl_vector< double > & | **[GetSlope](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getslope)**() const |
| const vnl_vector< double > & | **[GetIntercept](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getintercept)**() const |
| const vnl_matrix< double > & | **[GetSlopeRandom](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getsloperandom)**() const |
| const vnl_matrix< double > & | **[GetInterceptRandom](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getinterceptrandom)**() const |
| void | **[SetSlope](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setslope)**(const std::vector< double > & v) |
| void | **[SetIntercept](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setintercept)**(const std::vector< double > & v) |
| void | **[EstimateParameters](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-estimateparameters)**() |
| void | **[Initialize](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-initialize)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-beforeiteration)**() |
| void | **[SetRegressionInterval](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-setregressioninterval)**(int i) |
| int | **[GetRegressionInterval](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-getregressioninterval)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleShapeMixedEffectsMatrixAttribute](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-particleshapemixedeffectsmatrixattribute)**() |
| virtual | **[~ParticleShapeMixedEffectsMatrixAttribute](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-~particleshapemixedeffectsmatrixattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleShapeMixedEffectsMatrixAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

## Additional inherited members

**Public Functions inherited from [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResizeMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-resizematrix)**(int rs, int cs) |
| void | **[PrintMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-checkfornans)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-afteriteration)**() |
| virtual void | **[SetMatrix](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-setmatrix)**(const vnl_matrix< T > & m) |

**Protected Functions inherited from [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-particleshapematrixattribute)**() |
| virtual | **[~ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-~particleshapematrixattribute)**() |

**Protected Attributes inherited from [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#variable-m-domainspershape)**  |

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
class itk::ParticleShapeMixedEffectsMatrixAttribute;
```

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleShapeMixedEffectsMatrixAttribute itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleShapeMatrixAttribute<T,VDimension> itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleShapeMixedEffectsMatrixAttribute< T, VDimension >::ConstWeakPointer;
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
    ParticleShapeMixedEffectsMatrixAttribute ,
    ParticleShapeMatrixAttribute 
)
```


Run-time type information (and related methods). 


### function UpdateMeanMatrix

```cpp
inline void UpdateMeanMatrix()
```


### function ComputeMean

```cpp
inline vnl_vector< double > ComputeMean(
    double k
) const
```


### function ResizeParameters

```cpp
inline void ResizeParameters(
    unsigned int n
)
```


### function ResizeMeanMatrix

```cpp
inline virtual void ResizeMeanMatrix(
    int rs,
    int cs
)
```


### function ResizeExplanatory

```cpp
inline void ResizeExplanatory(
    unsigned int n
)
```


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleShapeMatrixAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleShapeMatrixAttribute::PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionaddeventcallback)


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleShapeMatrixAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleShapeMatrixAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-positionremoveeventcallback)


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


### function SetTimeptsPerIndividual

```cpp
inline void SetTimeptsPerIndividual(
    int i
)
```


### function GetTimeptsPerIndividual

```cpp
inline int GetTimeptsPerIndividual() const
```


### function SetExplanatory

```cpp
inline void SetExplanatory(
    std::vector< double > v
)
```


### function SetExplanatory

```cpp
inline void SetExplanatory(
    unsigned int i,
    double q
)
```


### function GetExplanatory

```cpp
inline const double & GetExplanatory(
    unsigned int i
) const
```


### function GetExplanatory

```cpp
inline double & GetExplanatory(
    unsigned int i
)
```


### function GetSlope

```cpp
inline const vnl_vector< double > & GetSlope() const
```


### function GetIntercept

```cpp
inline const vnl_vector< double > & GetIntercept() const
```


### function GetSlopeRandom

```cpp
inline const vnl_matrix< double > & GetSlopeRandom() const
```


### function GetInterceptRandom

```cpp
inline const vnl_matrix< double > & GetInterceptRandom() const
```


### function SetSlope

```cpp
inline void SetSlope(
    const std::vector< double > & v
)
```


### function SetIntercept

```cpp
inline void SetIntercept(
    const std::vector< double > & v
)
```


### function EstimateParameters

```cpp
inline void EstimateParameters()
```


### function Initialize

```cpp
inline void Initialize()
```


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [itk::ParticleShapeMatrixAttribute::BeforeIteration](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#function-beforeiteration)


### function SetRegressionInterval

```cpp
inline void SetRegressionInterval(
    int i
)
```


### function GetRegressionInterval

```cpp
inline int GetRegressionInterval() const
```


## Protected Functions Documentation

### function ParticleShapeMixedEffectsMatrixAttribute

```cpp
inline ParticleShapeMixedEffectsMatrixAttribute()
```


### function ~ParticleShapeMixedEffectsMatrixAttribute

```cpp
inline virtual ~ParticleShapeMixedEffectsMatrixAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2022-01-03 at 16:16:09 +0000