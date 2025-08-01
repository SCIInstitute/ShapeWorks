---
title: shapeworks::ShapeMatrix
summary: Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE NOT ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

---

# shapeworks::ShapeMatrix



Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES!  [More...](#detailed-description)


`#include <ShapeMatrix.h>`

Inherits from vnl_matrix< double >, [shapeworks::Observer](../Classes/classshapeworks_1_1Observer.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef double | **[DataType](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-datatype)**  |
| typedef [ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md) | **[Self](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-self)**  |
| typedef [Observer](../Classes/classshapeworks_1_1Observer.md) | **[Superclass](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ShapeMatrix.md) > | **[Pointer](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ShapeMatrix.md) > | **[ConstPointer](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ShapeMatrix.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ShapeMatrix.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ShapeMatrix.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ShapeMatrix.md) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ShapeMatrix.md#function-itktypemacro)**([ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md) , [Observer](../Classes/classshapeworks_1_1Observer.md) ) |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1ShapeMatrix.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1ShapeMatrix.md#function-afteriteration)**() |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1ShapeMatrix.md#function-getdomainspershape)**() const |
| void | **[SetAttributesPerDomain](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setattributesperdomain)**(const std::vector< int > & i) |
| void | **[SetAttributeScales](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetXYZ](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setxyz)**(int i, bool val) |
| void | **[SetNormals](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setnormals)**(int i, bool val) |
| virtual void | **[SetMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setmatrix)**(const vnl_matrix< double > & m) |
| virtual void | **[ResizeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md#function-resizematrix)**(int rs, int cs) |
| virtual void | **[DomainAddEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-domainaddeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[SetValues](../Classes/classshapeworks_1_1ShapeMatrix.md#function-setvalues)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ps, int idx, int d) |
| virtual void | **[PositionAddEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionaddeventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionseteventcallback)**(Object * o, const itk::EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classshapeworks_1_1ShapeMatrix.md#function-positionremoveeventcallback)**(Object * , const itk::EventObject & ) |
| void | **[PrintMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classshapeworks_1_1ShapeMatrix.md#function-checkfornans)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md#function-shapematrix)**() |
| virtual | **[~ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md#function-~shapematrix)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ShapeMatrix.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classshapeworks_1_1ShapeMatrix.md#variable-m-domainspershape)**  |

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
class shapeworks::ShapeMatrix;
```

Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

Each column represents a single shape. 

## Public Types Documentation

### typedef DataType

```cpp
typedef double shapeworks::ShapeMatrix::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ShapeMatrix shapeworks::ShapeMatrix::Self;
```


### typedef Superclass

```cpp
typedef Observer shapeworks::ShapeMatrix::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ShapeMatrix::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ShapeMatrix::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ShapeMatrix::ConstWeakPointer;
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
    ShapeMatrix ,
    Observer 
)
```


Run-time type information (and related methods). 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


### function AfterIteration

```cpp
inline virtual void AfterIteration()
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


### function SetAttributesPerDomain

```cpp
inline void SetAttributesPerDomain(
    const std::vector< int > & i
)
```


### function SetAttributeScales

```cpp
inline void SetAttributeScales(
    const std::vector< double > & s
)
```


### function SetXYZ

```cpp
inline void SetXYZ(
    int i,
    bool val
)
```


### function SetNormals

```cpp
inline void SetNormals(
    int i,
    bool val
)
```


### function SetMatrix

```cpp
inline virtual void SetMatrix(
    const vnl_matrix< double > & m
)
```


### function ResizeMatrix

```cpp
inline virtual void ResizeMatrix(
    int rs,
    int cs
)
```


### function DomainAddEventCallback

```cpp
inline virtual void DomainAddEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplements**: [shapeworks::Observer::DomainAddEventCallback](../Classes/classshapeworks_1_1Observer.md#function-domainaddeventcallback)


Callbacks that may be defined by a subclass. If a subclass defines one of these callback methods, the corresponding flag in m_DefinedCallbacks should be set to true so that the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) will know to register the appropriate event with this method. 


### function SetValues

```cpp
inline void SetValues(
    const ParticleSystem * ps,
    int idx,
    int d
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


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const itk::EventObject & e
)
```


**Reimplements**: [shapeworks::Observer::PositionSetEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const itk::EventObject & 
)
```


**Reimplements**: [shapeworks::Observer::PositionRemoveEventCallback](../Classes/classshapeworks_1_1Observer.md#function-positionremoveeventcallback)


### function PrintMatrix

```cpp
inline void PrintMatrix()
```


### function CheckForNans

```cpp
inline bool CheckForNans()
```


## Protected Functions Documentation

### function ShapeMatrix

```cpp
inline ShapeMatrix()
```


### function ~ShapeMatrix

```cpp
inline virtual ~ShapeMatrix()
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

Updated on 2025-08-01 at 02:40:46 +0000