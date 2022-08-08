---
title: shapeworks::ParticleImageDomainWithGradN

---

# shapeworks::ParticleImageDomainWithGradN



 [More...](#detailed-description)


`#include <ParticleImageDomainWithGradN.h>`

Inherits from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md), [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md), [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md), [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

Inherited by [shapeworks::ParticleImageDomainWithCurvature< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)< T > | **[Superclass](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-superclass)**  |
| using std::shared_ptr< [ParticleImageDomainWithGradN](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#using-pointer)**  |
| typedef [Superclass::PointType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-pointtype)**  |
| typedef Superclass::ImageType | **[ImageType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-imagetype)**  |
| typedef Superclass::GradNType | **[GradNType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-gradntype)**  |
| typedef Superclass::GradNType | **[VnlMatrixType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-deletepartialderivativeimages)**() override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-deleteimages)**() override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradN](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-particleimagedomainwithgradn)**() |
| virtual | **[~ParticleImageDomainWithGradN](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-~particleimagedomainwithgradn)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-printself)**(std::ostream & os, itk::Indent indent) const |

## Additional inherited members

**Public Types inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| typedef itk::FixedArray< T, DIMENSION > | **[VectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vnlvectortype)**  |

**Public Functions inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const override |

**Protected Functions inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-particleimagedomainwithgradients)**() |
| virtual | **[~ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-~particleimagedomainwithgradients)**() |
| openvdb::VectorGrid::Ptr | **[GetVDBGradient](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-getvdbgradient)**() |

**Public Functions inherited from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsurfacearea)**() const override |
| [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) | **[GetOrigin](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getorigin)**() const |
| ImageType::SizeType | **[GetSize](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsize)**() const |
| ImageType::SpacingType | **[GetSpacing](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getspacing)**() const |
| ImageType::RegionType::IndexType | **[GetIndex](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getindex)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) p) const override |
| T | **[Sample](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-sample)**(const [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) & p) const |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-updatezerocrossingpoint)**() override |

**Protected Functions inherited from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getvdbimage)**() const |
| | **[ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-particleimagedomain)**() |
| virtual | **[~ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-~particleimagedomain)**() |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) & p) const |

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
| using vnl_vector_fixed< double, 3 > | **[VectorDoubleType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectordoubletype)**  |
| using vnl_vector_fixed< float, 3 > | **[VectorFloatType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectorfloattype)**  |

**Public Functions inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1ParticleDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx, bool dbg =false) const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[UpdateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx, VectorDoubleType & update) const =0 |
| virtual void | **[InvalidateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-invalidateparticleposition)**(int idx) const |
| virtual VectorDoubleType | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleDomain.md#function-projectvectortosurfacetangent)**(VectorDoubleType & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const =0 |
| virtual VectorFloatType | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const =0 |
| virtual VectorFloatType | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const =0 |
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
class shapeworks::ParticleImageDomainWithGradN;
```


**See**: 

  * [ParticleImageDomain](../Classes/classshapeworks_1_1ParticleImageDomain.md)
  * ParticleClipRegionDomain 
  * [ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)


An image domain that extends [ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md) with gradient of normals information.

## Public Types Documentation

### typedef Superclass

```cpp
typedef ParticleImageDomainWithGradients<T> shapeworks::ParticleImageDomainWithGradN< T >::Superclass;
```


Standard class typedefs 


### using Pointer

```cpp
using shapeworks::ParticleImageDomainWithGradN< T >::Pointer =  std::shared_ptr<ParticleImageDomainWithGradN>;
```


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::ParticleImageDomainWithGradN< T >::PointType;
```


Point type of the domain (not necessarily of the image). 


### typedef ImageType

```cpp
typedef Superclass::ImageType shapeworks::ParticleImageDomainWithGradN< T >::ImageType;
```


### typedef GradNType

```cpp
typedef Superclass::GradNType shapeworks::ParticleImageDomainWithGradN< T >::GradNType;
```


### typedef VnlMatrixType

```cpp
typedef Superclass::GradNType shapeworks::ParticleImageDomainWithGradN< T >::VnlMatrixType;
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


### function SampleGradNAtPoint

```cpp
inline virtual GradNType SampleGradNAtPoint(
    const PointType & p,
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradnatpoint)


Sample the GradN at a point. This method performs no bounds checking. To check bounds, use IsInsideBuffer. SampleGradN returns a vnl matrix of size VDimension x VDimension. 


### function DeletePartialDerivativeImages

```cpp
inline virtual void DeletePartialDerivativeImages() override
```


**Reimplements**: [shapeworks::ParticleDomain::DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deletepartialderivativeimages)


### function DeleteImages

```cpp
inline virtual void DeleteImages() override
```


**Reimplements**: [shapeworks::ParticleImageDomainWithGradients::DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-deleteimages)


Used when a domain is fixed. 


## Protected Functions Documentation

### function ParticleImageDomainWithGradN

```cpp
inline ParticleImageDomainWithGradN()
```


### function ~ParticleImageDomainWithGradN

```cpp
inline virtual ~ParticleImageDomainWithGradN()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


-------------------------------

Updated on 2022-08-08 at 20:30:43 +0000