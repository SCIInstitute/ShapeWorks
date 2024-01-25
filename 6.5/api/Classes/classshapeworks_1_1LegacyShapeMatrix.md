---
title: shapeworks::LegacyShapeMatrix

---

# shapeworks::LegacyShapeMatrix





Inherits from vnl_matrix< double >, [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)

Inherited by [shapeworks::LinearRegressionShapeMatrix](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md), [shapeworks::MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef double | **[DataType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-datatype)**  |
| typedef [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) | **[Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-self)**  |
| typedef [Observer](../Classes/classshapeworks_1_1Observer.md) | **[Superclass](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[Pointer](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[ConstPointer](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) ) |
| virtual [Observer](../Classes/classshapeworks_1_1Observer.md) void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-domainaddeventcallback)**(Object * , const itk::EventObject & e) |
| virtual void | **[ResizeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-resizematrix)**(int rs, int cs) |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionaddeventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionseteventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[PrintMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-checkfornans)**() |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-getdomainspershape)**() const |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-afteriteration)**() |
| virtual void | **[SetMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-setmatrix)**(const vnl_matrix< double > & m) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-legacyshapematrix)**() |
| virtual | **[~LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-~legacyshapematrix)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#variable-m-domainspershape)**  |

## Additional inherited members

**Public Classes inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md)**  |

**Public Functions inherited from [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1Observer.md#function-itktypemacro)**([Observer](../Classes/classshapeworks_1_1Observer.md) , DataObject ) |
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
typedef double shapeworks::LegacyShapeMatrix::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef LegacyShapeMatrix shapeworks::LegacyShapeMatrix::Self;
```


### typedef Superclass

```cpp
typedef Observer shapeworks::LegacyShapeMatrix::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::LegacyShapeMatrix::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::LegacyShapeMatrix::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::LegacyShapeMatrix::ConstWeakPointer;
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
inline virtual Observer void DomainAddEventCallback(
    Object * ,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::Observer::DomainAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-domainaddeventcallback)


**Reimplemented by**: [shapeworks::LinearRegressionShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-domainaddeventcallback), [shapeworks::MixedEffectsShapeMatrix::DomainAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


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
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::Observer::PositionAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionaddeventcallback)


**Reimplemented by**: [shapeworks::LinearRegressionShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionaddeventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionAddEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionaddeventcallback)


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::Observer::PositionSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionseteventcallback)


**Reimplemented by**: [shapeworks::LinearRegressionShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionseteventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionSetEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplements**: [shapeworks::Observer::PositionRemoveEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionremoveeventcallback)


**Reimplemented by**: [shapeworks::LinearRegressionShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-positionremoveeventcallback), [shapeworks::MixedEffectsShapeMatrix::PositionRemoveEventCallback](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-positionremoveeventcallback)


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


**Reimplemented by**: [shapeworks::LinearRegressionShapeMatrix::BeforeIteration](../Classes/classshapeworks_1_1LinearRegressionShapeMatrix.md#function-beforeiteration), [shapeworks::MixedEffectsShapeMatrix::BeforeIteration](../Classes/classshapeworks_1_1MixedEffectsShapeMatrix.md#function-beforeiteration)


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


### function SetMatrix

```cpp
inline virtual void SetMatrix(
    const vnl_matrix< double > & m
)
```


## Protected Functions Documentation

### function LegacyShapeMatrix

```cpp
inline LegacyShapeMatrix()
```


### function ~LegacyShapeMatrix

```cpp
inline virtual ~LegacyShapeMatrix()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


## Protected Attributes Documentation

### variable m_DomainsPerShape

```cpp
int m_DomainsPerShape;
```


-------------------------------

Updated on 2024-01-25 at 03:19:25 +0000