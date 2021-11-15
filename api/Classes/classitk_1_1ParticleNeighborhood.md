---
title: itk::ParticleNeighborhood

---

# itk::ParticleNeighborhood



 [More...](#detailed-description)


`#include <itkParticleNeighborhood.h>`

Inherits from DataObject

Inherited by [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md) | **[Self](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-constweakpointer)**  |
| typedef Point< double, VDimension > | **[PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype)**  |
| typedef [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md) | **[DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype)**  |
| typedef [ParticleContainer](../Classes/classitk_1_1ParticleContainer.md)< [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) > | **[PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype)**  |
| typedef std::vector< [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md)< VDimension > > | **[PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itktypemacro)**([ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md) , DataObject ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| virtual [PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, double ) const |
| virtual [PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , double ) const |
| virtual [PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , std::vector< double > & , double ) const |
| virtual unsigned int | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, double , [PointVectorType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointvectortype) & ) const |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(Domain , [DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype) ) |
| | **[itkGetConstObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(Domain , [DomainType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-domaintype) ) |
| virtual void | **[AddPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-addposition)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-setposition)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleNeighborhood.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |

## Detailed Description

```cpp
template <unsigned int VDimension =3>
class itk::ParticleNeighborhood;
```


A [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md) is responsible for computing neighborhoods of particles. Given a point position in a domain, and a neighborhood radius, the [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md) returns a list of points that are neighbors of that point. The base class, [ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md), must be subclassed to provide functionality; the base class will throw an exception when FindNeighborhoodPoints is called. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleNeighborhood itk::ParticleNeighborhood< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject itk::ParticleNeighborhood< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleNeighborhood< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleNeighborhood< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleNeighborhood< VDimension >::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef Point<double, VDimension> itk::ParticleNeighborhood< VDimension >::PointType;
```


Point type used to store particle locations. 


### typedef DomainType

```cpp
typedef ParticleDomain itk::ParticleNeighborhood< VDimension >::DomainType;
```


Domain type. The Domain object provides bounds and distance information. 


### typedef PointContainerType

```cpp
typedef ParticleContainer<PointType> itk::ParticleNeighborhood< VDimension >::PointContainerType;
```


Container type for points. This matches the itkParticleSystem container type. 


### typedef PointVectorType

```cpp
typedef std::vector<ParticlePointIndexPair<VDimension> > itk::ParticleNeighborhood< VDimension >::PointVectorType;
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


**Reimplemented by**: [itk::ParticleRegionNeighborhood::FindNeighborhoodPoints](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-findneighborhoodpoints), [itk::ParticleRegionNeighborhood::FindNeighborhoodPoints](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-findneighborhoodpoints)


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


**Reimplemented by**: [itk::ParticleSurfaceNeighborhood::FindNeighborhoodPoints](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)


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


**Reimplemented by**: [itk::ParticleSurfaceNeighborhood::FindNeighborhoodPoints](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)


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


### function itkSetObjectMacro

```cpp
itkSetObjectMacro(
    Domain ,
    DomainType 
)
```


Set the Domain that this neighborhood will use. The Domain object is important because it defines bounds and distance measures. 


### function itkGetConstObjectMacro

```cpp
itkGetConstObjectMacro(
    Domain ,
    DomainType 
)
```


### function AddPosition

```cpp
inline virtual void AddPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [itk::ParticleRegionNeighborhood::AddPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-addposition), [itk::ParticleRegionNeighborhood::AddPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-addposition)


For efficiency, itkNeighborhoods are not necessarily observers of itkParticleSystem, but have specific methods invoked for various events. AddPosition is called by itkParticleSystem when a particle location is added. SetPosition is called when a particle location is set. RemovePosition is called when a particle location is removed. 


### function SetPosition

```cpp
inline virtual void SetPosition(
    const PointType & p,
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [itk::ParticleRegionNeighborhood::SetPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setposition), [itk::ParticleRegionNeighborhood::SetPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setposition)


### function RemovePosition

```cpp
inline virtual void RemovePosition(
    unsigned int idx,
    int threadId =0
)
```


**Reimplemented by**: [itk::ParticleRegionNeighborhood::RemovePosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-removeposition), [itk::ParticleRegionNeighborhood::RemovePosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-removeposition)


## Protected Functions Documentation

### function ParticleNeighborhood

```cpp
inline ParticleNeighborhood()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticleNeighborhood

```cpp
inline virtual ~ParticleNeighborhood()
```


-------------------------------

Updated on 2021-11-15 at 18:13:24 +0000