---
title: shapeworks::ParticleImageDomainWithGradients

---

# shapeworks::ParticleImageDomainWithGradients



 [More...](#detailed-description)


`#include <ParticleImageDomainWithGradients.h>`

Inherits from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md), [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md), [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

Inherited by [shapeworks::ParticleImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)< T > > | **[Pointer](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#using-pointer)**  |
| typedef [ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md)< T >::[PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-pointtype)**  |
| typedef [ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md)< T >::[ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) | **[ImageType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-imagetype)**  |
| typedef itk::FixedArray< T, DIMENSION > | **[VectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vnlvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-deleteimages)**() override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-particleimagedomainwithgradients)**() |
| virtual | **[~ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-~particleimagedomainwithgradients)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| openvdb::VectorGrid::Ptr | **[GetVDBGradient](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-getvdbgradient)**() |

## Additional inherited members

**Public Functions inherited from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsurfacearea)**() const override |
| [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetOrigin](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getorigin)**() const |
| ImageType::SizeType | **[GetSize](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsize)**() const |
| ImageType::SpacingType | **[GetSpacing](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getspacing)**() const |
| ImageType::RegionType::IndexType | **[GetIndex](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getindex)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) p) const override |
| T | **[Sample](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-sample)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-updatezerocrossingpoint)**() override |

**Protected Functions inherited from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getvdbimage)**() const |
| | **[ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-particleimagedomain)**() |
| virtual | **[~ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-~particleimagedomain)**() |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |

**Public Functions inherited from [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getupperbound)**() const |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getlowerbound)**() const |
| void | **[SetUpperBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setupperbound)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) _UpperBound) |
| void | **[SetLowerBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setlowerbound)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) _LowerBound) |
| void | **[SetRegion](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setregion)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & lowerBound, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & upperBound) |

**Protected Functions inherited from [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-particleregiondomain)**() |
| virtual | **[~ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-~particleregiondomain)**() |
| bool | **[IsInsideBuffer](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-isinsidebuffer)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |

**Public Types inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| using vnl_matrix_fixed< float, 3, 3 > | **[GradNType](../Classes/classshapeworks_1_1ParticleDomain.md#using-gradntype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorDoubleType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectordoubletype)**  |
| using vnl_vector_fixed< float, 3 > | **[VectorFloatType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectorfloattype)**  |

**Public Functions inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1ParticleDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx, bool dbg =false) const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[UpdateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx, VectorDoubleType & update) const =0 |
| virtual void | **[InvalidateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-invalidateparticleposition)**(int idx) const |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[Distance](../Classes/classshapeworks_1_1ParticleDomain.md#function-distance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, VectorDoubleType * out_grad =nullptr) const |
| virtual double | **[SquaredDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-squareddistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b) const |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-iswithindistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, double test_dist, double & distance) const |
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacemeancurvature)**() const =0 |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacestddevcurvature)**() const =0 |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getlowerbound)**() const =0 |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getupperbound)**() const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-getzerocrossingpoint)**() const =0 |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacearea)**() const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) p) const =0 |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleDomain.md#function-getmaxdiameter)**() const =0 |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deletepartialderivativeimages)**() =0 |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-updatezerocrossingpoint)**() =0 |
| bool | **[IsDomainFixed](../Classes/classshapeworks_1_1ParticleDomain.md#function-isdomainfixed)**() const |
| virtual shapeworks::DomainType | **[GetDomainType](../Classes/classshapeworks_1_1ParticleDomain.md#function-getdomaintype)**() const =0 |
| std::shared_ptr< [shapeworks::Constraints](../Classes/classshapeworks_1_1Constraints.md) > | **[GetConstraints](../Classes/classshapeworks_1_1ParticleDomain.md#function-getconstraints)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetPositionAfterSplit](../Classes/classshapeworks_1_1ParticleDomain.md#function-getpositionaftersplit)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pt, const VectorDoubleType & local_direction, const VectorDoubleType & global_direction, double epsilon) const |
| void | **[SetDomainID](../Classes/classshapeworks_1_1ParticleDomain.md#function-setdomainid)**(int id) |
| void | **[SetDomainName](../Classes/classshapeworks_1_1ParticleDomain.md#function-setdomainname)**(std::string name) |

**Protected Functions inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md#function-particledomain)**() |
| virtual | **[~ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md#function-~particledomain)**() |

**Protected Attributes inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| bool | **[m_FixedDomain](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-fixeddomain)**  |
| int | **[m_DomainID](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-domainid)**  |
| std::string | **[m_DomainName](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-domainname)**  |
| std::shared_ptr< [shapeworks::Constraints](../Classes/classshapeworks_1_1Constraints.md) > | **[constraints](../Classes/classshapeworks_1_1ParticleDomain.md#variable-constraints)**  |


## Detailed Description

```cpp
template <class T >
class shapeworks::ParticleImageDomainWithGradients;
```


**See**: 

  * [ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md)
  * ParticleClipRegionDomain 
  * [ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)


An image domain that extends [ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md) with image gradient information. Gradient values are interpolated with the SampleGradients(point) method.

## Public Types Documentation

### using Pointer

```cpp
using shapeworks::ParticleImageDomainWithGradients< T >::Pointer =  std::shared_ptr<ParticleImageDomainWithGradients<T> >;
```


### typedef PointType

```cpp
typedef ParticleImageDomain<T>::PointType shapeworks::ParticleImageDomainWithGradients< T >::PointType;
```


Point type of the domain (not necessarily of the image). 


### typedef ImageType

```cpp
typedef ParticleImageDomain<T>::ImageType shapeworks::ParticleImageDomainWithGradients< T >::ImageType;
```


### typedef VectorType

```cpp
typedef itk::FixedArray<T, DIMENSION> shapeworks::ParticleImageDomainWithGradients< T >::VectorType;
```


### typedef VnlVectorType

```cpp
typedef vnl_vector_fixed<T, DIMENSION> shapeworks::ParticleImageDomainWithGradients< T >::VnlVectorType;
```


## Public Functions Documentation

### function SetImage

```cpp
inline void SetImage(
    ImageType * I,
    double narrow_band
)
```


Set/Get the itk::Image specifying the particle domain. The set method modifies the parent class LowerBound and UpperBound. 


### function SampleGradientAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleGradientAtPoint(
    const PointType & p,
    int idx
) const
```


**Reimplements**: [shapeworks::ParticleDomain::SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradientatpoint)


### function SampleNormalAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    const PointType & p,
    int idx
) const
```


**Reimplements**: [shapeworks::ParticleDomain::SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplenormalatpoint)


### function ProjectVectorToSurfaceTangent

```cpp
inline vnl_vector_fixed< double, DIMENSION > ProjectVectorToSurfaceTangent(
    vnl_vector_fixed< double, DIMENSION > & gradE,
    const PointType & pos,
    int idx
) const override
```


This method is called by an optimizer after a call to Evaluate and may be used to apply any constraints the resulting vector, such as a projection to the surface tangent plane. Returns true if the gradient was modified. 


### function DeleteImages

```cpp
inline virtual void DeleteImages() override
```


**Reimplements**: [shapeworks::ParticleImageDomain::DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-deleteimages)


**Reimplemented by**: [shapeworks::ParticleImageDomainWithGradN::DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-deleteimages)


Used when a domain is fixed. 


## Protected Functions Documentation

### function ParticleImageDomainWithGradients

```cpp
inline ParticleImageDomainWithGradients()
```


### function ~ParticleImageDomainWithGradients

```cpp
inline virtual ~ParticleImageDomainWithGradients()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function GetVDBGradient

```cpp
inline openvdb::VectorGrid::Ptr GetVDBGradient()
```


-------------------------------

Updated on 2022-10-21 at 17:00:28 +0000