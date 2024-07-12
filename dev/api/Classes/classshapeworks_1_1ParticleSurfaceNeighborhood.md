---
title: shapeworks::ParticleSurfaceNeighborhood

---

# shapeworks::ParticleSurfaceNeighborhood



 [More...](#detailed-description)


`#include <ParticleSurfaceNeighborhood.h>`

Inherits from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md), [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md), itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef itk::Image< float, 3 > | **[TImage](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-timage)**  |
| typedef [TImage](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-timage) | **[ImageType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-imagetype)**  |
| typedef [ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md) | **[Self](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-self)**  |
| typedef [ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md) | **[Superclass](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-constweakpointer)**  |
| typedef ImageType::PixelType | **[NumericType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-numerictype)**  |
| typedef Superclass::PointTreeType | **[PointTreeType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-pointtreetype)**  |
| typedef vnl_vector_fixed< NumericType, TImage::ImageDimension > | **[GradientVectorType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-gradientvectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-pointtype)**  |
| typedef Superclass::PointContainerType | **[PointContainerType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-pointcontainertype)**  |
| typedef [shapeworks::ImplicitSurfaceDomain](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md)< typename TImage::PixelType > | **[DomainType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-domaintype)**  |
| typedef Superclass::PointVectorType | **[PointVectorType](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#typedef-pointvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , 3 ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-itktypemacro)**([ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md) , [ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md) ) |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , std::vector< double > & , double ) const override |
| virtual [PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , double ) const override |
| void | **[SetWeightingEnabled](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-setweightingenabled)**(bool is_enabled) |
| bool | **[IsWeightingEnabled](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-isweightingenabled)**() const |
| void | **[SetForceEuclidean](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-setforceeuclidean)**(bool is_enabled) |
| bool | **[IsForceEuclidean](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-isforceeuclidean)**() const |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-particlesurfaceneighborhood)**() |
| virtual | **[~ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md#function-~particlesurfaceneighborhood)**() |

## Additional inherited members

**Protected Classes inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md)**  |

**Protected Types inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md)< [IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) > | **[IteratorMapType](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#typedef-iteratormaptype)**  |

**Public Functions inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[SetDomain](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setdomain)**(DomainType::Pointer p) override |
| | **[itkSetMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itksetmacro)**(TreeLevels , unsigned int ) |
| | **[itkGetMacro](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-itkgetmacro)**(TreeLevels , unsigned int ) |
| virtual void | **[AddPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-addposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-setposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

**Protected Functions inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-particleregionneighborhood)**() |
| virtual | **[~ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#function-~particleregionneighborhood)**() |

**Public Attributes inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-vdimension)**  |

**Protected Attributes inherited from [shapeworks::ParticleRegionNeighborhood](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| PointTreeType::Pointer | **[m_Tree](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-tree)**  |
| IteratorMapType::Pointer | **[m_IteratorMap](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-iteratormap)**  |
| unsigned int | **[m_TreeLevels](../Classes/classshapeworks_1_1ParticleRegionNeighborhood.md#variable-m-treelevels)**  |

**Public Functions inherited from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| virtual void | **[SetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setdomain)**(DomainType::Pointer domain) |
| DomainType::Pointer | **[GetDomain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-getdomain)**() const |
| virtual void | **[AddPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-addposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-setposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

**Protected Functions inherited from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| virtual | **[~ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |

**Public Attributes inherited from [shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1ParticleNeighborhood.md#variable-vdimension)**  |


## Detailed Description

```cpp
class shapeworks::ParticleSurfaceNeighborhood;
```


[ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md) is a general purpose neighborhood object that computes neighborhoods based on distance from a point. It requires a domain that provides bounds information and a distance metric. This class uses a [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md) to cache point and index values so that FindNeighborhoodPoints is somewhat optimized. 

## Public Types Documentation

### typedef TImage

```cpp
typedef itk::Image<float, 3> shapeworks::ParticleSurfaceNeighborhood::TImage;
```


Standard class typedefs 


### typedef ImageType

```cpp
typedef TImage shapeworks::ParticleSurfaceNeighborhood::ImageType;
```


### typedef Self

```cpp
typedef ParticleSurfaceNeighborhood shapeworks::ParticleSurfaceNeighborhood::Self;
```


### typedef Superclass

```cpp
typedef ParticleRegionNeighborhood shapeworks::ParticleSurfaceNeighborhood::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticleSurfaceNeighborhood::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticleSurfaceNeighborhood::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticleSurfaceNeighborhood::ConstWeakPointer;
```


### typedef NumericType

```cpp
typedef ImageType::PixelType shapeworks::ParticleSurfaceNeighborhood::NumericType;
```


### typedef PointTreeType

```cpp
typedef Superclass::PointTreeType shapeworks::ParticleSurfaceNeighborhood::PointTreeType;
```


### typedef GradientVectorType

```cpp
typedef vnl_vector_fixed<NumericType, TImage::ImageDimension> shapeworks::ParticleSurfaceNeighborhood::GradientVectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::ParticleSurfaceNeighborhood::PointType;
```


Inherited typedefs from parent class. 


### typedef PointContainerType

```cpp
typedef Superclass::PointContainerType shapeworks::ParticleSurfaceNeighborhood::PointContainerType;
```


### typedef DomainType

```cpp
typedef shapeworks::ImplicitSurfaceDomain<typename TImage::PixelType> shapeworks::ParticleSurfaceNeighborhood::DomainType;
```


### typedef PointVectorType

```cpp
typedef Superclass::PointVectorType shapeworks::ParticleSurfaceNeighborhood::PointVectorType;
```


## Public Functions Documentation

### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    3 
)
```


Expose the image dimension. 


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
    ParticleSurfaceNeighborhood ,
    ParticleRegionNeighborhood 
)
```


Run-time type information (and related methods). 


### function FindNeighborhoodPoints

```cpp
virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    std::vector< double > & ,
    std::vector< double > & ,
    double 
) const override
```


**Reimplements**: [shapeworks::ParticleNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)


Compile a list of points that are within a specified radius of a given point. This implementation uses a PowerOfTwoTree to sort points according to location. 


### function FindNeighborhoodPoints

```cpp
virtual PointVectorType FindNeighborhoodPoints(
    const PointType & ,
    int idx,
    std::vector< double > & ,
    double 
) const override
```


**Reimplements**: [shapeworks::ParticleNeighborhood::FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-findneighborhoodpoints)


This method finds neighborhood points as in the previous method, but also computes a vector of weights associated with each of those points. 


### function SetWeightingEnabled

```cpp
inline void SetWeightingEnabled(
    bool is_enabled
)
```


### function IsWeightingEnabled

```cpp
inline bool IsWeightingEnabled() const
```


### function SetForceEuclidean

```cpp
inline void SetForceEuclidean(
    bool is_enabled
)
```


### function IsForceEuclidean

```cpp
inline bool IsForceEuclidean() const
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


## Protected Functions Documentation

### function ParticleSurfaceNeighborhood

```cpp
inline ParticleSurfaceNeighborhood()
```


### function ~ParticleSurfaceNeighborhood

```cpp
inline virtual ~ParticleSurfaceNeighborhood()
```


-------------------------------

Updated on 2024-07-12 at 20:10:35 +0000