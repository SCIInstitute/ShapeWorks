---
title: shapeworks::ParticleRegionNeighborhood

---

# shapeworks::ParticleRegionNeighborhood



 [More...](#detailed-description)


`#include <ParticleRegionNeighborhood.h>`

Inherits from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md), itk::DataObject

Inherited by [shapeworks::ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md)

## Protected Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md) | **[Self](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-self)**  |
| typedef [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) | **[Superclass](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-constweakpointer)**  |
| using [ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) | **[DomainType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#using-domaintype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-pointtype)**  |
| typedef Superclass::PointContainerType | **[PointContainerType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-pointcontainertype)**  |
| typedef Superclass::PointVectorType | **[PointVectorType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-pointvectortype)**  |
| typedef [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md) | **[PointTreeType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-pointtreetype)**  |

## Protected Types

|                | Name           |
| -------------- | -------------- |
| typedef [GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md)< [IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) > | **[IteratorMapType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-iteratormaptype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itktypemacro)**([ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md) , [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, double ) const |
| virtual void | **[SetDomain](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setdomain)**(DomainType::Pointer p) override |
| | **[itkSetMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itksetmacro)**(TreeLevels , unsigned int ) |
| | **[itkGetMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itkgetmacro)**(TreeLevels , unsigned int ) |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual void | **[AddPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-addposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-particleregionneighborhood)**() |
| virtual | **[~ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-~particleregionneighborhood)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| PointTreeType::Pointer | **[m_Tree](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-tree)**  |
| IteratorMapType::Pointer | **[m_IteratorMap](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-iteratormap)**  |
| unsigned int | **[m_TreeLevels](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-treelevels)**  |

## Additional inherited members

**Public Functions inherited from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| DomainType::Pointer | **[GetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-getdomain)**() const |

**Protected Functions inherited from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| virtual | **[~ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |


## Detailed Description

```cpp
class shapeworks::ParticleRegionNeighborhood;
```


[ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md) is a general purpose neighborhood object that computes neighborhoods based on distance from a point. It requires a domain that provides bounds information and a distance metric. This class uses a [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md) to cache point and index values so that FindNeighborhoodPoints is somewhat optimized. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleRegionNeighborhood shapeworks::ParticleRegionNeighborhood::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef ParticleNeighborhood shapeworks::ParticleRegionNeighborhood::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticleRegionNeighborhood::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticleRegionNeighborhood::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticleRegionNeighborhood::ConstWeakPointer;
```


### using DomainType

```cpp
using shapeworks::ParticleRegionNeighborhood::DomainType =  ParticleDomain;
```


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::ParticleRegionNeighborhood::PointType;
```


Inherited typedefs from parent class. 


### typedef PointContainerType

```cpp
typedef Superclass::PointContainerType shapeworks::ParticleRegionNeighborhood::PointContainerType;
```


### typedef PointVectorType

```cpp
typedef Superclass::PointVectorType shapeworks::ParticleRegionNeighborhood::PointVectorType;
```


### typedef PointTreeType

```cpp
typedef PowerOfTwoPointTree shapeworks::ParticleRegionNeighborhood::PointTreeType;
```


PowerOfTwoTree type used to sort points according to location. 


## Protected Types Documentation

### typedef IteratorMapType

```cpp
typedef GenericContainer<IteratorNodePair> shapeworks::ParticleRegionNeighborhood::IteratorMapType;
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
    ParticleRegionNeighborhood ,
    ParticleNeighborhood 
)
```


Run-time type information (and related methods). 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Dimensionality of the domain of the particle system. 


### function FindNeighborhoodPoints

```cpp
virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    double 
) const
```


**Reimplements**: [shapeworks::ParticleNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)


Compile a list of points that are within a specified radius of a given point. This implementation uses a PowerOfTwoTree to sort points according to location. 


### function SetDomain

```cpp
virtual void SetDomain(
    DomainType::Pointer p
) override
```


**Reimplements**: [shapeworks::ParticleNeighborhood::SetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setdomain)


Override SetDomain so that we can grab the region extent info and construct our tree. 


### function itkSetMacro

```cpp
itkSetMacro(
    TreeLevels ,
    unsigned int 
)
```


Set/Get the number of levels in the sorting tree. 


### function itkGetMacro

```cpp
itkGetMacro(
    TreeLevels ,
    unsigned int 
)
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function AddPosition

```cpp
virtual void AddPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplements**: [shapeworks::ParticleNeighborhood::AddPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-addposition)


For efficiency, itkNeighborhoods are not necessarily observers of itkParticleSystem, but have specific methods invoked for various events. AddPosition is called by itkParticleSystem when a particle location is added. SetPosition is called when a particle location is set. RemovePosition is called when a particle location is removed. 


### function SetPosition

```cpp
virtual void SetPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplements**: [shapeworks::ParticleNeighborhood::SetPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setposition)


### function RemovePosition

```cpp
virtual void RemovePosition(
    unsigned int idx,
    int threadId =0
)
```


**Reimplements**: [shapeworks::ParticleNeighborhood::RemovePosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-removeposition)


## Protected Functions Documentation

### function ParticleRegionNeighborhood

```cpp
inline ParticleRegionNeighborhood()
```


### function ~ParticleRegionNeighborhood

```cpp
inline virtual ~ParticleRegionNeighborhood()
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


## Protected Attributes Documentation

### variable m_Tree

```cpp
PointTreeType::Pointer m_Tree;
```


### variable m_IteratorMap

```cpp
IteratorMapType::Pointer m_IteratorMap;
```


### variable m_TreeLevels

```cpp
unsigned int m_TreeLevels;
```


-------------------------------

Updated on 2024-06-19 at 02:30:59 +0000