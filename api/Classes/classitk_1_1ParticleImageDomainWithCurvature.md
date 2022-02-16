---
title: itk::ParticleImageDomainWithCurvature

---

# itk::ParticleImageDomainWithCurvature



 [More...](#detailed-description)


`#include <itkParticleImageDomainWithCurvature.h>`

Inherits from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md), [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md), [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md), [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md), [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md), DataObject

Inherited by [itk::ParticleImplicitSurfaceDomain< T >](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)< T > | **[Superclass](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#typedef-superclass)**  |
| typedef [Superclass::PointType](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#typedef-pointtype)**  |
| typedef Superclass::ImageType | **[ImageType](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#typedef-imagetype)**  |
| typedef Superclass::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-setimage)**([ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype) * I, double narrow_band) |
| virtual double | **[GetCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getcurvature)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacestddevcurvature)**() const override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-particleimagedomainwithcurvature)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-~particleimagedomainwithcurvature)**() |

## Additional inherited members

**Public Types inherited from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md) > | **[Pointer](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-pointer)**  |
| typedef Superclass::GradNType | **[GradNType](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-gradntype)**  |

**Public Functions inherited from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-samplegradnatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-deletepartialderivativeimages)**() override |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-deleteimages)**() override |

**Protected Functions inherited from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-particleimagedomainwithgradn)**() |
| virtual | **[~ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-~particleimagedomainwithgradn)**() |

**Public Types inherited from [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)< T > > | **[Pointer](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#typedef-pointer)**  |
| typedef FixedArray< T, DIMENSION > | **[VectorType](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#typedef-vnlvectortype)**  |

**Public Functions inherited from [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-samplegradientatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-samplenormalatpoint)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & pos, int idx) const override |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-deleteimages)**() override |

**Protected Functions inherited from [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-particleimagedomainwithgradients)**() |
| virtual | **[~ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-~particleimagedomainwithgradients)**() |
| openvdb::VectorGrid::Ptr | **[GetVDBGradient](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-getvdbgradient)**() |

**Public Types inherited from [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointer)**  |

**Public Functions inherited from [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
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

**Protected Functions inherited from [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classitk_1_1ParticleImageDomain.md#function-getvdbimage)**() const |
| | **[ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-particleimagedomain)**() |
| virtual | **[~ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-~particleimagedomain)**() |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classitk_1_1ParticleImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classitk_1_1ParticleImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) & p) const |

**Public Types inherited from [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleRegionDomain.md#typedef-pointer)**  |

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
| typedef SmartPointer< [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleDomain.md#typedef-pointer)**  |
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
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classitk_1_1ParticleDomain.md#function-getlowerbound)**() const =0 |
| virtual const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classitk_1_1ParticleDomain.md#function-getupperbound)**() const =0 |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-getzerocrossingpoint)**() const =0 |
| virtual double | **[GetSurfaceArea](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacearea)**() const =0 |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetValidLocationNear](../Classes/classitk_1_1ParticleDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) p) const =0 |
| virtual double | **[GetMaxDiameter](../Classes/classitk_1_1ParticleDomain.md#function-getmaxdiameter)**() const =0 |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ParticleDomain.md#function-deleteimages)**() =0 |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleDomain.md#function-deletepartialderivativeimages)**() =0 |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-updatezerocrossingpoint)**() =0 |
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
class itk::ParticleImageDomainWithCurvature;
```


**See**: 

  * [ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md)
  * ParticleClipRegionDomain 
  * [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)


An image domain that extends [ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md) with curvature information.

## Public Types Documentation

### typedef Superclass

```cpp
typedef ParticleImageDomainWithGradN<T> itk::ParticleImageDomainWithCurvature< T >::Superclass;
```


Standard class typedefs 


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleImageDomainWithCurvature< T >::PointType;
```


### typedef ImageType

```cpp
typedef Superclass::ImageType itk::ParticleImageDomainWithCurvature< T >::ImageType;
```


### typedef VnlMatrixType

```cpp
typedef Superclass::VnlMatrixType itk::ParticleImageDomainWithCurvature< T >::VnlMatrixType;
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


### function GetCurvature

```cpp
inline virtual double GetCurvature(
    const PointType & p,
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::GetCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


### function GetSurfaceMeanCurvature

```cpp
inline virtual double GetSurfaceMeanCurvature() const override
```


**Reimplements**: [itk::ParticleDomain::GetSurfaceMeanCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacemeancurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


### function GetSurfaceStdDevCurvature

```cpp
inline virtual double GetSurfaceStdDevCurvature() const override
```


**Reimplements**: [itk::ParticleDomain::GetSurfaceStdDevCurvature](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacestddevcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


## Protected Functions Documentation

### function ParticleImageDomainWithCurvature

```cpp
inline ParticleImageDomainWithCurvature()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticleImageDomainWithCurvature

```cpp
inline virtual ~ParticleImageDomainWithCurvature()
```


-------------------------------

Updated on 2022-02-16 at 06:47:27 +0000