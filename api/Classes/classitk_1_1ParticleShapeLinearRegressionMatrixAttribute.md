---
title: itk::ParticleShapeLinearRegressionMatrixAttribute

---

# itk::ParticleShapeLinearRegressionMatrixAttribute



 [More...](#detailed-description)

Inherits from [itk::ParticleShapeMatrixAttribute< T, VDimension >](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md), vnl_matrix< T >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-datatype)**  |
| typedef [ParticleShapeLinearRegressionMatrixAttribute](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md) | **[Self](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-self)**  |
| typedef [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)< T, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-itktypemacro)**([ParticleShapeLinearRegressionMatrixAttribute](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md) , [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) ) |
| void | **[UpdateMeanMatrix](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-updatemeanmatrix)**() |
| vnl_vector< double > | **[ComputeMean](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-computemean)**(double k) const |
| void | **[ResizeParameters](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-resizeparameters)**(unsigned int n) |
| virtual void | **[ResizeMeanMatrix](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-resizemeanmatrix)**(int rs, int cs) |
| void | **[ResizeExplanatory](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-resizeexplanatory)**(unsigned int n) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-domainaddeventcallback)**(Object * , const EventObject & e) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getdomainspershape)**() const |
| void | **[SetExplanatory](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setexplanatory)**(std::vector< double > v) |
| void | **[SetExplanatory](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setexplanatory)**(unsigned int i, double q) |
| const double & | **[GetExplanatory](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getexplanatory)**(unsigned int i) const |
| double & | **[GetExplanatory](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getexplanatory)**(unsigned int i) |
| const vnl_vector< double > & | **[GetSlope](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getslope)**() const |
| const vnl_vector< double > & | **[GetIntercept](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getintercept)**() const |
| void | **[SetSlope](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setslope)**(const std::vector< double > & v) |
| void | **[SetIntercept](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setintercept)**(const std::vector< double > & v) |
| void | **[EstimateParameters](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-estimateparameters)**() |
| void | **[Initialize](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-initialize)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-beforeiteration)**() |
| void | **[SetRegressionInterval](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-setregressioninterval)**(int i) |
| int | **[GetRegressionInterval](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-getregressioninterval)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleShapeLinearRegressionMatrixAttribute](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-particleshapelinearregressionmatrixattribute)**() |
| virtual | **[~ParticleShapeLinearRegressionMatrixAttribute](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-~particleshapelinearregressionmatrixattribute)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleShapeLinearRegressionMatrixAttribute.md#function-printself)**(std::ostream & os, Indent indent) const |

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
class itk::ParticleShapeLinearRegressionMatrixAttribute;
```

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleShapeLinearRegressionMatrixAttribute itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleShapeMatrixAttribute<T,VDimension> itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleShapeLinearRegressionMatrixAttribute< T, VDimension >::ConstWeakPointer;
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
    ParticleShapeLinearRegressionMatrixAttribute ,
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

### function ParticleShapeLinearRegressionMatrixAttribute

```cpp
inline ParticleShapeLinearRegressionMatrixAttribute()
```


### function ~ParticleShapeLinearRegressionMatrixAttribute

```cpp
inline virtual ~ParticleShapeLinearRegressionMatrixAttribute()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2021-11-12 at 04:35:44 +0000