---
title: itk::ParticleRegionNeighborhood

---

# itk::ParticleRegionNeighborhood



 [More...](#detailed-description)


`#include <itkParticleRegionNeighborhood.h>`

Inherits from [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md), DataObject

## Protected Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md) | **[Self](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-self)**  |
| typedef [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-constweakpointer)**  |
| typedef [Superclass::PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype)**  |
| typedef [Superclass::PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) | **[PointContainerType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointcontainertype)**  |
| typedef [Superclass::DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype) | **[DomainType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-domaintype)**  |
| typedef [Superclass::PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[PointVectorType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointvectortype)**  |
| typedef [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md)< VDimension > | **[PointTreeType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtreetype)**  |

## Protected Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleContainer](../Classes/classitk_1_1ParticleContainer.md)< [IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) > | **[IteratorMapType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-iteratormaptype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itktypemacro)**([ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md) , [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual PointVectorType | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) & , int idx, double ) const |
| virtual void | **[SetDomain](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setdomain)**([DomainType](../Classes/classitk_1_1ParticleDomain.md) * p) |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itksetmacro)**(TreeLevels , unsigned int ) |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itkgetmacro)**(TreeLevels , unsigned int ) |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual void | **[AddPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-addposition)**(const [PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setposition)**(const [PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-particleregionneighborhood)**() |
| virtual | **[~ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-~particleregionneighborhood)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| PointTreeType::Pointer | **[m_Tree](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-tree)**  |
| IteratorMapType::Pointer | **[m_IteratorMap](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-iteratormap)**  |
| unsigned int | **[m_TreeLevels](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-treelevels)**  |

## Additional inherited members

**Public Functions inherited from [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(Domain , [DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype) ) |
| | **[itkGetConstObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(Domain , [DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype) ) |

**Protected Functions inherited from [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| virtual | **[~ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |


## Detailed Description

```cpp
template <unsigned int VDimension =3>
class itk::ParticleRegionNeighborhood;
```


[ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md) is a general purpose neighborhood object that computes neighborhoods based on distance from a point. It requires a domain that provides bounds information and a distance metric. This class uses a [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md) to cache point and index values so that FindNeighborhoodPoints is somewhat optimized. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleRegionNeighborhood itk::ParticleRegionNeighborhood< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef ParticleNeighborhood<VDimension> itk::ParticleRegionNeighborhood< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleRegionNeighborhood< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleRegionNeighborhood< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleRegionNeighborhood< VDimension >::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleRegionNeighborhood< VDimension >::PointType;
```


Inherited typedefs from parent class. 


### typedef PointContainerType

```cpp
typedef Superclass::PointContainerType itk::ParticleRegionNeighborhood< VDimension >::PointContainerType;
```


### typedef DomainType

```cpp
typedef Superclass::DomainType itk::ParticleRegionNeighborhood< VDimension >::DomainType;
```


### typedef PointVectorType

```cpp
typedef Superclass::PointVectorType itk::ParticleRegionNeighborhood< VDimension >::PointVectorType;
```


### typedef PointTreeType

```cpp
typedef PowerOfTwoPointTree<VDimension> itk::ParticleRegionNeighborhood< VDimension >::PointTreeType;
```


PowerOfTwoTree type used to sort points according to location. 


## Protected Types Documentation

### typedef IteratorMapType

```cpp
typedef ParticleContainer<IteratorNodePair> itk::ParticleRegionNeighborhood< VDimension >::IteratorMapType;
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


**Reimplements**: [itk::ParticleNeighborhood::FindNeighborhoodPoints](../Classes/classitk_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)


Compile a list of points that are within a specified radius of a given point. This implementation uses a PowerOfTwoTree to sort points according to location. 


### function SetDomain

```cpp
virtual void SetDomain(
    DomainType * p
)
```


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
    Indent indent
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


**Reimplements**: [itk::ParticleNeighborhood::AddPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-addposition)


For efficiency, itkNeighborhoods are not necessarily observers of itkParticleSystem, but have specific methods invoked for various events. AddPosition is called by itkParticleSystem when a particle location is added. SetPosition is called when a particle location is set. RemovePosition is called when a particle location is removed. 


### function SetPosition

```cpp
virtual void SetPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplements**: [itk::ParticleNeighborhood::SetPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-setposition)


### function RemovePosition

```cpp
virtual void RemovePosition(
    unsigned int idx,
    int threadId =0
)
```


**Reimplements**: [itk::ParticleNeighborhood::RemovePosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-removeposition)


## Protected Functions Documentation

### function ParticleRegionNeighborhood

```cpp
inline ParticleRegionNeighborhood()
```


### function ~ParticleRegionNeighborhood

```cpp
inline virtual ~ParticleRegionNeighborhood()
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

Updated on 2022-01-22 at 00:21:04 +0000