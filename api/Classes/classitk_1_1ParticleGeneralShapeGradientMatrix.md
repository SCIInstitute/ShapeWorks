---
title: itk::ParticleGeneralShapeGradientMatrix
summary: Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE NOT ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

---

# itk::ParticleGeneralShapeGradientMatrix



Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES!  [More...](#detailed-description)


`#include <itkParticleGeneralShapeGradientMatrix.h>`

Inherits from vnl_matrix< T >, [itk::ParticleAttribute< VDimension >](../Classes/classitk_1_1ParticleAttribute.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-datatype)**  |
| typedef [ParticleGeneralShapeGradientMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md) | **[Self](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-self)**  |
| typedef [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#typedef-particlesystemtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleAttribute.md#typedef-self) ) |
| virtual [ParticleAttribute](../Classes/classitk_1_1ParticleAttribute.md) void | **[BeforeIteration](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-afteriteration)**() |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-getdomainspershape)**() const |
| void | **[SetAttributesPerDomain](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setattributesperdomain)**(const std::vector< int > & i) |
| void | **[SetAttributeScales](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetXYZ](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setxyz)**(int i, bool val) |
| void | **[SetNormals](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setnormals)**(int i, bool val) |
| virtual void | **[SetMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setmatrix)**(const vnl_matrix< T > & m) |
| virtual void | **[ResizeMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-resizematrix)**(int rs, int cs) |
| void | **[SetValues](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-setvalues)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * ps, int idx, int d) |
| virtual void | **[DomainAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-domainaddeventcallback)**(Object * , const EventObject & e) |
| virtual void | **[PositionAddEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionaddeventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionSetEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionseteventcallback)**(Object * o, const EventObject & e) |
| virtual void | **[PositionRemoveEventCallback](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-positionremoveeventcallback)**(Object * , const EventObject & ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGeneralShapeGradientMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-particlegeneralshapegradientmatrix)**() |
| virtual | **[~ParticleGeneralShapeGradientMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-~particlegeneralshapegradientmatrix)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#function-printself)**(std::ostream & os, Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md#variable-m-domainspershape)**  |

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
class itk::ParticleGeneralShapeGradientMatrix;
```

Each column describes a shape. A shape may be composed of m_DomainsPerShape domains (default 1). ALL DOMAINS ARE _NOT_ ASSUMED TO HAVE THE SAME NUMBER OF PARTICLES! 

Each column represents a single shape. 

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleGeneralShapeGradientMatrix itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::Self;
```


### typedef Superclass

```cpp
typedef ParticleAttribute<VDimension> itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleGeneralShapeGradientMatrix< T, VDimension >::ParticleSystemType;
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


### function SetValues

```cpp
inline void SetValues(
    const ParticleSystemType * ps,
    int idx,
    int d
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


## Protected Functions Documentation

### function ParticleGeneralShapeGradientMatrix

```cpp
inline ParticleGeneralShapeGradientMatrix()
```


### function ~ParticleGeneralShapeGradientMatrix

```cpp
inline virtual ~ParticleGeneralShapeGradientMatrix()
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

Updated on 2022-01-03 at 16:16:08 +0000