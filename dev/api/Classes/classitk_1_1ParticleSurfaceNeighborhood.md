---
title: itk::ParticleSurfaceNeighborhood

---

# itk::ParticleSurfaceNeighborhood



 [More...](#detailed-description)


`#include <itkParticleSurfaceNeighborhood.h>`

Inherits from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md), [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md), DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef TImage | **[ImageType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-imagetype)**  |
| typedef [ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) | **[Self](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-self)**  |
| typedef [ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md)< TImage::ImageDimension > | **[Superclass](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) > | **[Pointer](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) > | **[ConstPointer](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-constweakpointer)**  |
| typedef ImageType::PixelType | **[NumericType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-numerictype)**  |
| typedef [Superclass::PointTreeType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtreetype) | **[PointTreeType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointtreetype)**  |
| typedef vnl_vector_fixed< NumericType, TImage::ImageDimension > | **[GradientVectorType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-gradientvectortype)**  |
| typedef [Superclass::PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointtype)**  |
| typedef Superclass::PointContainerType | **[PointContainerType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointcontainertype)**  |
| typedef [shapeworks::ParticleImplicitSurfaceDomain](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md)< typename TImage::PixelType > | **[DomainType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-domaintype)**  |
| typedef Superclass::PointVectorType | **[PointVectorType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-itkstaticconstmacro)**(Dimension , unsigned int , TImage::ImageDimension ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-itktypemacro)**([ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) , [ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md) ) |
| virtual PointVectorType | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , std::vector< double > & , double ) const override |
| virtual PointVectorType | **[FindNeighborhoodPoints](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#typedef-pointtype) & , int idx, std::vector< double > & , double ) const override |
| void | **[SetWeightingEnabled](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-setweightingenabled)**(bool is_enabled) |
| bool | **[IsWeightingEnabled](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-isweightingenabled)**() const |
| void | **[SetForceEuclidean](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-setforceeuclidean)**(bool is_enabled) |
| bool | **[IsForceEuclidean](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-isforceeuclidean)**() const |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-printself)**(std::ostream & os, Indent indent) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-particlesurfaceneighborhood)**() |
| virtual | **[~ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md#function-~particlesurfaceneighborhood)**() |

## Additional inherited members

**Protected Classes inherited from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| struct | **[IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md)**  |

**Protected Types inherited from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleContainer](../Classes/classitk_1_1ParticleContainer.md)< [IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) > | **[IteratorMapType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-iteratormaptype)**  |

**Public Functions inherited from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[SetDomain](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setdomain)**(DomainType::Pointer p) override |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itksetmacro)**(TreeLevels , unsigned int ) |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-itkgetmacro)**(TreeLevels , unsigned int ) |
| virtual void | **[AddPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-addposition)**(const [PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-setposition)**(const [PointType](../Classes/classitk_1_1ParticleRegionNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

**Protected Functions inherited from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-particleregionneighborhood)**() |
| virtual | **[~ParticleRegionNeighborhood](../Classes/classitk_1_1ParticleRegionNeighborhood.md#function-~particleregionneighborhood)**() |

**Protected Attributes inherited from [itk::ParticleRegionNeighborhood< TImage::ImageDimension >](../Classes/classitk_1_1ParticleRegionNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| PointTreeType::Pointer | **[m_Tree](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-tree)**  |
| IteratorMapType::Pointer | **[m_IteratorMap](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-iteratormap)**  |
| unsigned int | **[m_TreeLevels](../Classes/classitk_1_1ParticleRegionNeighborhood.md#variable-m-treelevels)**  |

**Public Functions inherited from [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itksetobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| | **[itkGetConstObjectMacro](../Classes/classitk_1_1ParticleNeighborhood.md#function-itkgetconstobjectmacro)**(PointContainer , [PointContainerType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointcontainertype) ) |
| virtual void | **[SetDomain](../Classes/classitk_1_1ParticleNeighborhood.md#function-setdomain)**(DomainType::Pointer domain) |
| DomainType::Pointer | **[GetDomain](../Classes/classitk_1_1ParticleNeighborhood.md#function-getdomain)**() const |
| virtual void | **[AddPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-addposition)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[SetPosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-setposition)**(const [PointType](../Classes/classitk_1_1ParticleNeighborhood.md#typedef-pointtype) & p, unsigned int idx, int threadId =0) |
| virtual void | **[RemovePosition](../Classes/classitk_1_1ParticleNeighborhood.md#function-removeposition)**(unsigned int idx, int threadId =0) |

**Protected Functions inherited from [itk::ParticleNeighborhood< 3 >](../Classes/classitk_1_1ParticleNeighborhood.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-particleneighborhood)**() |
| virtual | **[~ParticleNeighborhood](../Classes/classitk_1_1ParticleNeighborhood.md#function-~particleneighborhood)**() |


## Detailed Description

```cpp
template <class TImage >
class itk::ParticleSurfaceNeighborhood;
```


[ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md) is a general purpose neighborhood object that computes neighborhoods based on distance from a point. It requires a domain that provides bounds information and a distance metric. This class uses a [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md) to cache point and index values so that FindNeighborhoodPoints is somewhat optimized. 

## Public Types Documentation

### typedef ImageType

```cpp
typedef TImage itk::ParticleSurfaceNeighborhood< TImage >::ImageType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleSurfaceNeighborhood itk::ParticleSurfaceNeighborhood< TImage >::Self;
```


### typedef Superclass

```cpp
typedef ParticleRegionNeighborhood<TImage::ImageDimension> itk::ParticleSurfaceNeighborhood< TImage >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleSurfaceNeighborhood< TImage >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleSurfaceNeighborhood< TImage >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleSurfaceNeighborhood< TImage >::ConstWeakPointer;
```


### typedef NumericType

```cpp
typedef ImageType::PixelType itk::ParticleSurfaceNeighborhood< TImage >::NumericType;
```


### typedef PointTreeType

```cpp
typedef Superclass::PointTreeType itk::ParticleSurfaceNeighborhood< TImage >::PointTreeType;
```


### typedef GradientVectorType

```cpp
typedef vnl_vector_fixed<NumericType, TImage::ImageDimension> itk::ParticleSurfaceNeighborhood< TImage >::GradientVectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleSurfaceNeighborhood< TImage >::PointType;
```


Inherited typedefs from parent class. 


### typedef PointContainerType

```cpp
typedef Superclass::PointContainerType itk::ParticleSurfaceNeighborhood< TImage >::PointContainerType;
```


### typedef DomainType

```cpp
typedef shapeworks::ParticleImplicitSurfaceDomain<typename TImage::PixelType> itk::ParticleSurfaceNeighborhood< TImage >::DomainType;
```


### typedef PointVectorType

```cpp
typedef Superclass::PointVectorType itk::ParticleSurfaceNeighborhood< TImage >::PointVectorType;
```


## Public Functions Documentation

### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    TImage::ImageDimension 
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
    Indent indent
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

Updated on 2022-11-21 at 20:30:35 +0000