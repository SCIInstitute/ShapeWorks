---
title: shapeworks::ParticleNeighborhood

---

# shapeworks::ParticleNeighborhood



 [More...](#detailed-description)


`#include <ParticleNeighborhood.h>`

Inherits from itk::DataObject

Inherited by [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) | **[Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-constweakpointer)**  |
| typedef itk::Point< double, VDimension > | **[PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype)**  |
| using [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) | **[DomainType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#using-domaintype)**  |
| typedef [GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md)< [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) > | **[PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype)**  |
| typedef std::vector< [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) > | **[PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itktypemacro)**([ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) , DataObject ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, double ) const |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , double ) const |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , std::vector< double > & , double ) const |
| virtual unsigned int | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, double , [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) & ) const |
| virtual void | **[SetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setdomain)**(DomainType::Pointer domain) |
| DomainType::Pointer | **[GetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-getdomain)**() const |
| virtual void | **[AddPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-addposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1ParticleNeighborhood.md#variable-vdimension)**  |

## Detailed Description

```cpp
class shapeworks::ParticleNeighborhood;
```


A [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) is responsible for computing neighborhoods of particles. Given a point position in a domain, and a neighborhood radius, the [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) returns a list of points that are neighbors of that point. The base class, [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md), must be subclassed to provide functionality; the base class will throw an exception when FindNeighborhoodPoints is called. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleNeighborhood shapeworks::ParticleNeighborhood::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::ParticleNeighborhood::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticleNeighborhood::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticleNeighborhood::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticleNeighborhood::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef itk::Point<double, VDimension> shapeworks::ParticleNeighborhood::PointType;
```


Point type used to store particle locations. 


### using DomainType

```cpp
using shapeworks::ParticleNeighborhood::DomainType =  shapeworks::ParticleDomain;
```


Domain type. The Domain object provides bounds and distance information. 


### typedef PointContainerType

```cpp
typedef GenericContainer<PointType> shapeworks::ParticleNeighborhood::PointContainerType;
```


Container type for points. This matches the itkParticleSystem container type. 


### typedef PointVectorType

```cpp
typedef std::vector<ParticlePointIndexPair> shapeworks::ParticleNeighborhood::PointVectorType;
```


Point list (vector) type. This is the type of list returned by FindNeighborhoodPoints. 


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
    ParticleNeighborhood ,
    DataObject 
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


### function itkSetObjectMacro

```cpp
itkSetObjectMacro(
    PointContainer ,
    PointContainerType 
)
```


Set/Get the point container. These are the points parsed by the Neighborhood class when FindNeighborhoodPoints is called. 


### function itkGetConstObjectMacro

```cpp
itkGetConstObjectMacro(
    PointContainer ,
    PointContainerType 
)
```


### function FindNeighborhoodPoints

```cpp
inline virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    double 
) const
```


**Reimplemented by**: [shapeworks::ParticleRegionNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-findneighborhoodpoints)


Compile a list of points that are within a specified radius of a given point. The default implementation will throw an exception. 


### function FindNeighborhoodPoints

```cpp
inline virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    std::vector< double > & ,
    double 
) const
```


**Reimplemented by**: [shapeworks::ParticleSurfaceNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)


This method finds neighborhood points as in the previous method, but also computes a vector of weights associated with each of those points. 


### function FindNeighborhoodPoints

```cpp
inline virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    std::vector< double > & ,
    std::vector< double > & ,
    double 
) const
```


**Reimplemented by**: [shapeworks::ParticleSurfaceNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)


This method finds neighborhood points as in the previous method, but also computes a vector of distances associated with each of those points. 


### function FindNeighborhoodPoints

```cpp
inline virtual unsigned int FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    double ,
    PointVectorType & 
) const
```


### function SetDomain

```cpp
inline virtual void SetDomain(
    DomainType::Pointer domain
)
```


**Reimplemented by**: [shapeworks::ParticleRegionNeighborhood::SetDomain](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setdomain)


Set the Domain that this neighborhood will use. The Domain object is important because it defines bounds and distance measures. 


### function GetDomain

```cpp
inline DomainType::Pointer GetDomain() const
```


### function AddPosition

```cpp
inline virtual void AddPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [shapeworks::ParticleRegionNeighborhood::AddPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-addposition)


For efficiency, itkNeighborhoods are not necessarily observers of itkParticleSystem, but have specific methods invoked for various events. AddPosition is called by itkParticleSystem when a particle location is added. SetPosition is called when a particle location is set. RemovePosition is called when a particle location is removed. 


### function SetPosition

```cpp
inline virtual void SetPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [shapeworks::ParticleRegionNeighborhood::SetPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setposition)


### function RemovePosition

```cpp
inline virtual void RemovePosition(
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [shapeworks::ParticleRegionNeighborhood::RemovePosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-removeposition)


## Protected Functions Documentation

### function ParticleNeighborhood

```cpp
inline ParticleNeighborhood()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~ParticleNeighborhood

```cpp
inline virtual ~ParticleNeighborhood()
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


-------------------------------

Updated on 2024-08-17 at 17:02:53 +0000