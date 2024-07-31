---
title: shapeworks::ImageDomainWithCurvature

---

# shapeworks::ImageDomainWithCurvature



 [More...](#detailed-description)

Inherits from [shapeworks::ImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ImageDomainWithGradN.md), [shapeworks::ImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ImageDomainWithGradients.md), [shapeworks::ImageDomain< T >](../Classes/classshapeworks_1_1ImageDomain.md), [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md), [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

Inherited by [shapeworks::ImplicitSurfaceDomain< T >](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ImageDomainWithGradN](../Classes/classshapeworks_1_1ImageDomainWithGradN.md)< T > | **[Superclass](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#typedef-superclass)**  |
| typedef [Superclass::PointType](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#typedef-pointtype)**  |
| typedef Superclass::ImageType | **[ImageType](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#typedef-imagetype)**  |
| typedef Superclass::VnlMatrixType | **[VnlMatrixType](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getsurfacestddevcurvature)**() const override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ImageDomainWithCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-imagedomainwithcurvature)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ImageDomainWithCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-~imagedomainwithcurvature)**() |

## Additional inherited members

**Public Types inherited from [shapeworks::ImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ImageDomainWithGradN](../Classes/classshapeworks_1_1ImageDomainWithGradN.md) > | **[Pointer](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#using-pointer)**  |
| typedef Superclass::GradNType | **[GradNType](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#typedef-gradntype)**  |

**Public Functions inherited from [shapeworks::ImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p, int idx) const override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-deletepartialderivativeimages)**() override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-deleteimages)**() override |

**Protected Functions inherited from [shapeworks::ImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ImageDomainWithGradN](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-imagedomainwithgradn)**() |
| virtual | **[~ImageDomainWithGradN](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-~imagedomainwithgradn)**() |

**Public Types inherited from [shapeworks::ImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ImageDomainWithGradients](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)< T > > | **[Pointer](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#using-pointer)**  |
| typedef itk::FixedArray< T, DIMENSION > | **[VectorType](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#typedef-vnlvectortype)**  |

**Public Functions inherited from [shapeworks::ImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p, int idx) const |
| vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & pos, int idx) const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-deleteimages)**() override |

**Protected Functions inherited from [shapeworks::ImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ImageDomainWithGradients](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-imagedomainwithgradients)**() |
| virtual | **[~ImageDomainWithGradients](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-~imagedomainwithgradients)**() |
| openvdb::VectorGrid::Ptr | **[GetVDBGradient](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-getvdbgradient)**() |

**Public Types inherited from [shapeworks::ImageDomain< T >](../Classes/classshapeworks_1_1ImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ImageDomain](../Classes/classshapeworks_1_1ImageDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ImageDomain.md#using-pointer)**  |

**Public Functions inherited from [shapeworks::ImageDomain< T >](../Classes/classshapeworks_1_1ImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ImageDomain.md#function-getsurfacearea)**() const override |
| [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) | **[GetOrigin](../Classes/classshapeworks_1_1ImageDomain.md#function-getorigin)**() const |
| ImageType::SizeType | **[GetSize](../Classes/classshapeworks_1_1ImageDomain.md#function-getsize)**() const |
| ImageType::SpacingType | **[GetSpacing](../Classes/classshapeworks_1_1ImageDomain.md#function-getspacing)**() const |
| ImageType::RegionType::IndexType | **[GetIndex](../Classes/classshapeworks_1_1ImageDomain.md#function-getindex)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ImageDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) p) const override |
| T | **[Sample](../Classes/classshapeworks_1_1ImageDomain.md#function-sample)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p) const |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ImageDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ImageDomain.md#function-deleteimages)**() override |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ImageDomain.md#function-updatezerocrossingpoint)**() override |

**Protected Functions inherited from [shapeworks::ImageDomain< T >](../Classes/classshapeworks_1_1ImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classshapeworks_1_1ImageDomain.md#function-getvdbimage)**() const |
| | **[ImageDomain](../Classes/classshapeworks_1_1ImageDomain.md#function-imagedomain)**() |
| virtual | **[~ImageDomain](../Classes/classshapeworks_1_1ImageDomain.md#function-~imagedomain)**() |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classshapeworks_1_1ImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classshapeworks_1_1ImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classshapeworks_1_1ImageDomain.md#using-pointtype) & p) const |

**Public Types inherited from [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleRegionDomain.md#using-pointer)**  |

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
| typedef std::shared_ptr< [ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleDomain.md#typedef-pointer)**  |
| using vnl_matrix_fixed< float, 3, 3 > | **[GradNType](../Classes/classshapeworks_1_1ParticleDomain.md#using-gradntype)**  |
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
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[Distance](../Classes/classshapeworks_1_1ParticleDomain.md#function-distance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, VectorDoubleType * out_grad =nullptr) const |
| virtual double | **[SquaredDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-squareddistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b) const |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-iswithindistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, double test_dist, double & distance) const |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getlowerbound)**() const =0 |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getupperbound)**() const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-getzerocrossingpoint)**() const =0 |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacearea)**() const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) p) const =0 |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleDomain.md#function-getmaxdiameter)**() const =0 |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deleteimages)**() =0 |
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
class shapeworks::ImageDomainWithCurvature;
```

## Public Types Documentation

### typedef Superclass

```cpp
typedef ImageDomainWithGradN<T> shapeworks::ImageDomainWithCurvature< T >::Superclass;
```


Standard class typedefs 


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::ImageDomainWithCurvature< T >::PointType;
```


### typedef ImageType

```cpp
typedef Superclass::ImageType shapeworks::ImageDomainWithCurvature< T >::ImageType;
```


### typedef VnlMatrixType

```cpp
typedef Superclass::VnlMatrixType shapeworks::ImageDomainWithCurvature< T >::VnlMatrixType;
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


**Reimplements**: [shapeworks::ParticleDomain::GetCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


### function GetSurfaceMeanCurvature

```cpp
inline virtual double GetSurfaceMeanCurvature() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacemeancurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


### function GetSurfaceStdDevCurvature

```cpp
inline virtual double GetSurfaceStdDevCurvature() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacestddevcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


## Protected Functions Documentation

### function ImageDomainWithCurvature

```cpp
inline ImageDomainWithCurvature()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~ImageDomainWithCurvature

```cpp
inline virtual ~ImageDomainWithCurvature()
```


-------------------------------

Updated on 2024-07-31 at 08:14:44 +0000