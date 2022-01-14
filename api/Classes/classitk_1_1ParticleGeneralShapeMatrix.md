---
title: itk::ParticleGeneralShapeMatrix
summary: Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE NOT ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

---

# itk::ParticleGeneralShapeMatrix



Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES!  [More...](#detailed-description)


`#include <itkParticleGeneralShapeMatrix.h>`

Inherits from vnl_matrix< T >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-datatype)**  |
| typedef [ParticleGeneralShapeMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md) | **[Self](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-self)**  |
| typedef [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#typedef-particlesystemtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| virtual [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) void | **[BeforeIteration](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-afteriteration)**() |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-getdomainspershape)**() const |
| void | **[SetAttributesPerDomain](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setattributesperdomain)**(const std::vector< int > & i) |
| void | **[SetAttributeScales](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetXYZ](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setxyz)**(int i, bool val) |
| void | **[SetNormals](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setnormals)**(int i, bool val) |
| virtual void | **[SetMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setmatrix)**(const vnl_matrix< T > & m) |
| virtual void | **[ResizeMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-resizematrix)**(int rs, int cs) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-domainaddeventcallback)**(Object * , const EventObject & e) |
| void | **[SetValues](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-setvalues)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * ps, int idx, int d) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |
| void | **[PrintMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-printmatrix)**() |
| bool | **[CheckForNans](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-checkfornans)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGeneralShapeMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-particlegeneralshapematrix)**() |
| virtual | **[~ParticleGeneralShapeMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-~particlegeneralshapematrix)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#function-printself)**(std::ostream & os, Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md#variable-m-domainspershape)**  |

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
class itk::ParticleGeneralShapeMatrix;
```

Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

Each column represents a single shape. 

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleGeneralShapeMatrix< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleGeneralShapeMatrix itk::ParticleGeneralShapeMatrix< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleAttribute<VDimension> itk::ParticleGeneralShapeMatrix< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleGeneralShapeMatrix< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleGeneralShapeMatrix< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleGeneralShapeMatrix< T, VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleGeneralShapeMatrix< T, VDimension >::ParticleSystemType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. Run-time type information (and related methods). 


### function BeforeIteration

```cpp
inline virtual ParticleAttribute void BeforeIteration()
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
    const vnl_matrix< T > & m
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
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::DomainAddEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-domainaddeventcallback)


### function SetValues

```cpp
inline void SetValues(
    const ParticleSystemType * ps,
    int idx,
    int d
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


### function PositionSetEventCallback

```cpp
inline virtual void PositionSetEventCallback(
    Object * o,
    const EventObject & e
)
```


**Reimplements**: [itk::ParticleAttribute::PositionSetEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionseteventcallback)


### function PositionRemoveEventCallback

```cpp
inline virtual void PositionRemoveEventCallback(
    Object * ,
    const EventObject & 
)
```


**Reimplements**: [itk::ParticleAttribute::PositionRemoveEventCallback](../Classes/classitk_1_1ParticleAttribute.md#function-positionremoveeventcallback)


### function PrintMatrix

```cpp
inline void PrintMatrix()
```


### function CheckForNans

```cpp
inline bool CheckForNans()
```


## Protected Functions Documentation

### function ParticleGeneralShapeMatrix

```cpp
inline ParticleGeneralShapeMatrix()
```


### function ~ParticleGeneralShapeMatrix

```cpp
inline virtual ~ParticleGeneralShapeMatrix()
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

Updated on 2022-01-14 at 02:25:59 +0000