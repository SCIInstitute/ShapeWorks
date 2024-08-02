---
title: shapeworks::MixedEffectsShapeMatrix

---

# shapeworks::MixedEffectsShapeMatrix





Inherits from [shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md), vnl_matrix< double >, [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef double | **[DataType](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-datatype)**  |
| typedef [MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md) | **[Self](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-self)**  |
| typedef [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) | **[Superclass](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[Pointer](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[ConstPointer](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-itktypemacro)**([MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md) , [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) ) |
| void | **[UpdateMeanMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-updatemeanmatrix)**() |
| vnl_vector< double > | **[ComputeMean](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-computemean)**(double k) const |
| void | **[ResizeParameters](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-resizeparameters)**(unsigned int n) |
| virtual void | **[ResizeMeanMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-resizemeanmatrix)**(int rs, int cs) |
| void | **[ResizeExplanatory](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-resizeexplanatory)**(unsigned int n) |
| virtual void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-domainaddeventcallback)**(Object * , const itk::EventObject & e) |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionaddeventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionseteventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getdomainspershape)**() const |
| void | **[SetTimeptsPerIndividual](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-settimeptsperindividual)**(int i) |
| int | **[GetTimeptsPerIndividual](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-gettimeptsperindividual)**() const |
| void | **[SetExplanatory](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setexplanatory)**(std::vector< double > v) |
| void | **[SetExplanatory](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setexplanatory)**(unsigned int i, double q) |
| const double & | **[GetExplanatory](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getexplanatory)**(unsigned int i) const |
| double & | **[GetExplanatory](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getexplanatory)**(unsigned int i) |
| const vnl_vector< double > & | **[GetSlope](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getslope)**() const |
| const vnl_vector< double > & | **[GetIntercept](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getintercept)**() const |
| const vnl_matrix< double > & | **[GetSlopeRandom](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getsloperandom)**() const |
| const vnl_matrix< double > & | **[GetInterceptRandom](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getinterceptrandom)**() const |
| void | **[SetSlope](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setslope)**(const std::vector< double > & v) |
| void | **[SetIntercept](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setintercept)**(const std::vector< double > & v) |
| void | **[EstimateParameters](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-estimateparameters)**() |
| void | **[Initialize](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-initialize)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-beforeiteration)**() |
| void | **[SetRegressionInterval](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-setregressioninterval)**(int i) |
| int | **[GetRegressionInterval](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-getregressioninterval)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-mixedeffectsshapematrix)**() |
| virtual | **[~MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-~mixedeffectsshapematrix)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Additional inherited members

**Public Functions inherited from [shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResizeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-resizematrix)**(int rs, int cs) |
| void | **[PrintMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-checkfornans)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-afteriteration)**() |
| virtual void | **[SetMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-setmatrix)**(const vnl_matrix< double > & m) |

**Protected Functions inherited from [shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md)**

|                | Name           |
| -------------- | -------------- |
| | **[LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-legacyshapematrix)**() |
| virtual | **[~LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-~legacyshapematrix)**() |

**Protected Attributes inherited from [shapeworks::LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md)**

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#variable-m-domainspershape)**  |

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

**Protected Functions inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Observer](../Classes/classshapeworks_1_1Observer.md#function-observer)**() |
| virtual | **[~Observer](../Classes/classshapeworks_1_1Observer.md#function-~observer)**() |

**Public Attributes inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| [DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md) | **[m_DefinedCallbacks](../Classes/classshapeworks_1_1Observer.md#variable-m-definedcallbacks)**  |


## Public Types Documentation

### typedef DataType

```cpp
typedef double shapeworks::MixedEffectsShapeMatrix::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef MixedEffectsShapeMatrix shapeworks::MixedEffectsShapeMatrix::Self;
```


### typedef Superclass

```cpp
typedef LegacyShapeMatrix shapeworks::MixedEffectsShapeMatrix::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::MixedEffectsShapeMatrix::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::MixedEffectsShapeMatrix::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::MixedEffectsShapeMatrix::ConstWeakPointer;
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
    MixedEffectsShapeMatrix ,
    LegacyShapeMatrix 
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
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::LegacyShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function PositionAddEventCallback

```cpp
inline virtual void PositionAddEventCallback(
    Object * o,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::LegacyShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionaddeventcallback)


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::LegacyShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplements**: [shapeworks::LegacyShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionremoveeventcallback)


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


**Reimplements**: [shapeworks::LegacyShapeMatrix::BeforeIteration](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-beforeiteration)


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

### function MixedEffectsShapeMatrix

```cpp
inline MixedEffectsShapeMatrix()
```


### function ~MixedEffectsShapeMatrix

```cpp
inline virtual ~MixedEffectsShapeMatrix()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


-------------------------------

Updated on 2024-08-02 at 04:46:45 +0000