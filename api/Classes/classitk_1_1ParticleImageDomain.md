---
title: itk::ParticleImageDomain

---

# itk::ParticleImageDomain



 [More...](#detailed-description)


`#include <itkParticleImageDomain.h>`

Inherits from [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md), [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md), DataObject

Inherited by [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointer)**  |
| typedef Image< T, DIMENSION > | **[ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype)**  |
| typedef [ParticleRegionDomain::PointType](../Classes/classitk_1_1ParticleRegionDomain.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classitk_1_1ParticleImageDomain.md#function-setimage)**([ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype) * I, double narrow_band) |
| virtual double | **[GetSurfaceArea](../Classes/classitk_1_1ParticleImageDomain.md#function-getsurfacearea)**() const override |
| [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) | **[GetOrigin](../Classes/classitk_1_1ParticleImageDomain.md#function-getorigin)**() const |
| ImageType::SizeType | **[GetSize](../Classes/classitk_1_1ParticleImageDomain.md#function-getsize)**() const |
| ImageType::SpacingType | **[GetSpacing](../Classes/classitk_1_1ParticleImageDomain.md#function-getspacing)**() const |
| ImageType::RegionType::IndexType | **[GetIndex](../Classes/classitk_1_1ParticleImageDomain.md#function-getindex)**() const |
| virtual [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) | **[GetValidLocationNear](../Classes/classitk_1_1ParticleImageDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) p) const override |
| T | **[Sample](../Classes/classitk_1_1ParticleImageDomain.md#function-sample)**(const [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) & p) const |
| virtual double | **[GetMaxDiameter](../Classes/classitk_1_1ParticleImageDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ParticleImageDomain.md#function-deleteimages)**() override |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleImageDomain.md#function-updatezerocrossingpoint)**() override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classitk_1_1ParticleImageDomain.md#function-getvdbimage)**() const |
| | **[ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-particleimagedomain)**() |
| virtual | **[~ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-~particleimagedomain)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleImageDomain.md#function-printself)**(std::ostream & os, Indent indent) const |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classitk_1_1ParticleImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classitk_1_1ParticleImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) & p) const |

## Additional inherited members

**Public Functions inherited from [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classitk_1_1ParticleRegionDomain.md#function-applyconstraints)**([PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p) const |
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-getupperbound)**() const |
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-getlowerbound)**() const |
| void | **[SetUpperBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-setupperbound)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) _UpperBound) |
| void | **[SetLowerBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-setlowerbound)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) _LowerBound) |
| void | **[SetRegion](../Classes/classitk_1_1ParticleRegionDomain.md#function-setregion)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & lowerBound, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & upperBound) |

**Protected Functions inherited from [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md#function-particleregiondomain)**() |
| virtual | **[~ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md#function-~particleregiondomain)**() |
| bool | **[IsInsideBuffer](../Classes/classitk_1_1ParticleRegionDomain.md#function-isinsidebuffer)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p) const |

**Public Types inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| typedef vnl_matrix_fixed< float, DIMENSION, DIMENSION > | **[GradNType](../Classes/classitk_1_1ParticleDomain.md#typedef-gradntype)**  |

**Public Functions inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classitk_1_1ParticleDomain.md#function-applyconstraints)**([PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx, bool dbg =false) const =0 |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[UpdateParticlePosition](../Classes/classitk_1_1ParticleDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & point, int idx, vnl_vector_fixed< double, DIMENSION > & update) const =0 |
| virtual void | **[InvalidateParticlePosition](../Classes/classitk_1_1ParticleDomain.md#function-invalidateparticleposition)**(int idx) const |
| virtual vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ParticleDomain.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & pos, int idx) const =0 |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplegradientatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & point, int idx) const =0 |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplenormalatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & point, int idx) const =0 |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const =0 |
| virtual double | **[Distance](../Classes/classitk_1_1ParticleDomain.md#function-distance)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & b, int idx_b, vnl_vector_fixed< double, DIMENSION > * out_grad =nullptr) const |
| virtual double | **[SquaredDistance](../Classes/classitk_1_1ParticleDomain.md#function-squareddistance)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & b, int idx_b) const |
| virtual bool | **[IsWithinDistance](../Classes/classitk_1_1ParticleDomain.md#function-iswithindistance)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & b, int idx_b, double test_dist, double & distance) const |
| virtual double | **[GetCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getcurvature)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const =0 |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacemeancurvature)**() const =0 |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacestddevcurvature)**() const =0 |
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classitk_1_1ParticleDomain.md#function-getlowerbound)**() const =0 |
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classitk_1_1ParticleDomain.md#function-getupperbound)**() const =0 |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-getzerocrossingpoint)**() const =0 |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleDomain.md#function-deletepartialderivativeimages)**() =0 |
| bool | **[IsDomainFixed](../Classes/classitk_1_1ParticleDomain.md#function-isdomainfixed)**() const |
| virtual shapeworks::DomainType | **[GetDomainType](../Classes/classitk_1_1ParticleDomain.md#function-getdomaintype)**() const =0 |
| std::shared_ptr< [Constraints](../Classes/classitk_1_1Constraints.md) > | **[GetConstraints](../Classes/classitk_1_1ParticleDomain.md#function-getconstraints)**() const |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetPositionAfterSplit](../Classes/classitk_1_1ParticleDomain.md#function-getpositionaftersplit)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & pt, const vnl_vector_fixed< double, 3 > & random, double epsilon) const |
| void | **[SetDomainID](../Classes/classitk_1_1ParticleDomain.md#function-setdomainid)**(int id) |
| void | **[SetDomainName](../Classes/classitk_1_1ParticleDomain.md#function-setdomainname)**(std::string name) |

**Protected Functions inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleDomain](../Classes/classitk_1_1ParticleDomain.md#function-particledomain)**() |
| virtual | **[~ParticleDomain](../Classes/classitk_1_1ParticleDomain.md#function-~particledomain)**() |

**Protected Attributes inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| bool | **[m_FixedDomain](../Classes/classitk_1_1ParticleDomain.md#variable-m-fixeddomain)**  |
| int | **[m_DomainID](../Classes/classitk_1_1ParticleDomain.md#variable-m-domainid)**  |
| std::string | **[m_DomainName](../Classes/classitk_1_1ParticleDomain.md#variable-m-domainname)**  |
| std::shared_ptr< [Constraints](../Classes/classitk_1_1Constraints.md) > | **[constraints](../Classes/classitk_1_1ParticleDomain.md#variable-constraints)**  |


## Detailed Description

```cpp
template <class T >
class itk::ParticleImageDomain;
```


A bounding-box region domain that sets its bounding box according to the origin, spacing, and RequestedRegion of a specified itk::Image. This Domain object may be sampled for interpolated image values using the Sample(Point) method. 

## Public Types Documentation

### typedef Pointer

```cpp
typedef SmartPointer<ParticleImageDomain> itk::ParticleImageDomain< T >::Pointer;
```


Standard class typedefs 


### typedef ImageType

```cpp
typedef Image<T, DIMENSION> itk::ParticleImageDomain< T >::ImageType;
```


Type of the ITK image used by this class. 


### typedef PointType

```cpp
typedef ParticleRegionDomain::PointType itk::ParticleImageDomain< T >::PointType;
```


Point type of the domain (not the image). 


## Public Functions Documentation

### function SetImage

```cpp
inline void SetImage(
    ImageType * I,
    double narrow_band
)
```


Set/Get the itk::Image specifying the particle domain. The set method modifies the parent class LowerBound and UpperBound. 


### function GetSurfaceArea

```cpp
inline virtual double GetSurfaceArea() const override
```


**Reimplements**: [itk::ParticleDomain::GetSurfaceArea](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacearea)


Use for neighborhood radius. 


### function GetOrigin

```cpp
inline PointType GetOrigin() const
```


### function GetSize

```cpp
inline ImageType::SizeType GetSize() const
```


### function GetSpacing

```cpp
inline ImageType::SpacingType GetSpacing() const
```


### function GetIndex

```cpp
inline ImageType::RegionType::IndexType GetIndex() const
```


### function GetValidLocationNear

```cpp
inline virtual PointType GetValidLocationNear(
    PointType p
) const override
```


**Reimplements**: [itk::ParticleDomain::GetValidLocationNear](../Classes/classitk_1_1ParticleDomain.md#function-getvalidlocationnear)


GetValidLocation returns a PointType location on the surface. Used for placing the first particle. 


### function Sample

```cpp
inline T Sample(
    const PointType & p
) const
```


Sample the image at a point. This method performs bounds checking. 


### function GetMaxDiameter

```cpp
inline virtual double GetMaxDiameter() const override
```


**Reimplements**: [itk::ParticleDomain::GetMaxDiameter](../Classes/classitk_1_1ParticleDomain.md#function-getmaxdiameter)


GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma 


### function DeleteImages

```cpp
inline virtual void DeleteImages() override
```


**Reimplements**: [itk::ParticleDomain::DeleteImages](../Classes/classitk_1_1ParticleDomain.md#function-deleteimages)


**Reimplemented by**: [itk::ParticleImageDomainWithGradients::DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-deleteimages), [itk::ParticleImageDomainWithGradN::DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-deleteimages)


Used when a domain is fixed. 


### function UpdateZeroCrossingPoint

```cpp
inline virtual void UpdateZeroCrossingPoint() override
```


**Reimplements**: [itk::ParticleDomain::UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-updatezerocrossingpoint)


## Protected Functions Documentation

### function GetVDBImage

```cpp
inline openvdb::FloatGrid::Ptr GetVDBImage() const
```


### function ParticleImageDomain

```cpp
inline ParticleImageDomain()
```


### function ~ParticleImageDomain

```cpp
inline virtual ~ParticleImageDomain()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function transform

```cpp
inline openvdb::math::Transform::Ptr transform() const
```


### function ToVDBCoord

```cpp
inline openvdb::Vec3R ToVDBCoord(
    const PointType & p
) const
```


-------------------------------

Updated on 2022-01-27 at 02:24:31 +0000