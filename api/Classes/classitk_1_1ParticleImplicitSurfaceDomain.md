---
title: itk::ParticleImplicitSurfaceDomain

---

# itk::ParticleImplicitSurfaceDomain



 [More...](#detailed-description)


`#include <itkParticleImplicitSurfaceDomain.h>`

Inherits from [itk::ParticleImageDomainWithCurvature< T >](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md), [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md), [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md), [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md), [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md), [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md), DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)< T > | **[Superclass](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#typedef-superclass)**  |
| typedef SmartPointer< [ParticleImplicitSurfaceDomain](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#typedef-pointer)**  |
| typedef Superclass::ImageType | **[ImageType](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#typedef-imagetype)**  |
| typedef [Superclass::PointType](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-itknewmacro)**([ParticleImplicitSurfaceDomain](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md) ) |
| virtual void | **[SetTolerance](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-settolerance)**(const T _Tolerance) |
| virtual T | **[GetTolerance](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-gettolerance)**() |
| virtual shapeworks::DomainType | **[GetDomainType](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getdomaintype)**() const override |
| virtual bool | **[ApplyConstraints](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-applyconstraints)**([PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx, bool dbg =false) const override |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[UpdateParticlePosition](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & point, int idx, vnl_vector_fixed< double, DIMENSION > & update) const override |
| void | **[SetMesh](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-setmesh)**(TriMesh * mesh) |
| void | **[SetFeaMesh](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-setfeamesh)**(const char * feaFile) |
| void | **[SetFeaGrad](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-setfeagrad)**(const char * feaGradFile) |
| void | **[SetFids](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-setfids)**(const char * fidsFile) |
| [meshFIM](../Classes/classmeshFIM.md) * | **[GetMesh](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getmesh)**() |
| [meshFIM](../Classes/classmeshFIM.md) * | **[GetMesh](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getmesh)**() const |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetZeroCrossingPoint](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getzerocrossingpoint)**() const override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImplicitSurfaceDomain](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-particleimplicitsurfacedomain)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticleImplicitSurfaceDomain](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-~particleimplicitsurfacedomain)**() |

## Additional inherited members

**Public Types inherited from [itk::ParticleImageDomainWithCurvature< T >](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#typedef-vnlmatrixtype)**  |

**Public Functions inherited from [itk::ParticleImageDomainWithCurvature< T >](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-setimage)**([ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype) * I, double narrow_band) |
| virtual double | **[GetCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getcurvature)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacestddevcurvature)**() const override |

**Protected Functions inherited from [itk::ParticleImageDomainWithCurvature< T >](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-particleimagedomainwithcurvature)**() |
| virtual | **[~ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-~particleimagedomainwithcurvature)**() |

**Public Types inherited from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::GradNType | **[GradNType](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-gradntype)**  |
| typedef Superclass::GradNType | **[VnlMatrixType](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#typedef-vnlmatrixtype)**  |

**Public Functions inherited from [itk::ParticleImageDomainWithGradN< T >](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-setimage)**([ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype) * I, double narrow_band) |
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
| typedef FixedArray< T, DIMENSION > | **[VectorType](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#typedef-vnlvectortype)**  |

**Public Functions inherited from [itk::ParticleImageDomainWithGradients< T >](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-setimage)**([ImageType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-imagetype) * I, double narrow_band) |
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

**Public Functions inherited from [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md)**

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

**Protected Functions inherited from [itk::ParticleImageDomain< T >](../Classes/classitk_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| openvdb::FloatGrid::Ptr | **[GetVDBImage](../Classes/classitk_1_1ParticleImageDomain.md#function-getvdbimage)**() const |
| | **[ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-particleimagedomain)**() |
| virtual | **[~ParticleImageDomain](../Classes/classitk_1_1ParticleImageDomain.md#function-~particleimagedomain)**() |
| openvdb::math::Transform::Ptr | **[transform](../Classes/classitk_1_1ParticleImageDomain.md#function-transform)**() const |
| openvdb::Vec3R | **[ToVDBCoord](../Classes/classitk_1_1ParticleImageDomain.md#function-tovdbcoord)**(const [PointType](../Classes/classitk_1_1ParticleImageDomain.md#typedef-pointtype) & p) const |

**Public Functions inherited from [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md)**

|                | Name           |
| -------------- | -------------- |
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
| virtual double | **[GetSurfaceArea](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacearea)**() const =0 |
| virtual [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[GetValidLocationNear](../Classes/classitk_1_1ParticleDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) p) const =0 |
| virtual double | **[GetMaxDiameter](../Classes/classitk_1_1ParticleDomain.md#function-getmaxdiameter)**() const =0 |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ParticleDomain.md#function-deleteimages)**() =0 |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleDomain.md#function-deletepartialderivativeimages)**() =0 |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-updatezerocrossingpoint)**() =0 |
| bool | **[IsDomainFixed](../Classes/classitk_1_1ParticleDomain.md#function-isdomainfixed)**() const |
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
class itk::ParticleImplicitSurfaceDomain;
```


A 3D cartesian domain that constrains points so that they always lie an implicit surface. The implicit surface is defined as the zero isosurface of the given image. [Constraints](../Classes/classitk_1_1Constraints.md) are applied using a Newton-Raphson iteration, and this class assumes it has a distance transform as an image. 

## Public Types Documentation

### typedef Superclass

```cpp
typedef ParticleImageDomainWithCurvature<T> itk::ParticleImplicitSurfaceDomain< T >::Superclass;
```


Standard class typedefs 


### typedef Pointer

```cpp
typedef SmartPointer<ParticleImplicitSurfaceDomain> itk::ParticleImplicitSurfaceDomain< T >::Pointer;
```


### typedef ImageType

```cpp
typedef Superclass::ImageType itk::ParticleImplicitSurfaceDomain< T >::ImageType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleImplicitSurfaceDomain< T >::PointType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    ParticleImplicitSurfaceDomain 
)
```


Method for creation through the object factory. 


### function SetTolerance

```cpp
inline virtual void SetTolerance(
    const T _Tolerance
)
```


Set/Get the precision of the projection operation. The resulting projection will be within the specified tolerance. 


### function GetTolerance

```cpp
inline virtual T GetTolerance()
```


### function GetDomainType

```cpp
inline virtual shapeworks::DomainType GetDomainType() const override
```


**Reimplements**: [itk::ParticleDomain::GetDomainType](../Classes/classitk_1_1ParticleDomain.md#function-getdomaintype)


### function ApplyConstraints

```cpp
virtual bool ApplyConstraints(
    PointType & p,
    int idx,
    bool dbg =false
) const override
```


**Reimplements**: [itk::ParticleDomain::ApplyConstraints](../Classes/classitk_1_1ParticleDomain.md#function-applyconstraints)


Apply any constraints to the given point location. This method constrains points to lie within the given domain and on a given implicit surface. If the point is not already on the surface, it is projected back to the surface using a Newton-Raphson iteration. IMPORTANT: This method returns the true/false value of its superclass, and does not indicate changes only due to projection. This is done for speed: we typically will only want to know if a point tried to move outside of the bounding box domain, since movement off the surface will be very common. Consider subclassing this method to add a check for significant differences in the input and output points. 


### function UpdateParticlePosition

```cpp
inline virtual PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & update
) const override
```


**Reimplements**: [itk::ParticleDomain::UpdateParticlePosition](../Classes/classitk_1_1ParticleDomain.md#function-updateparticleposition)


Applies the update to the point and returns the new point position. 


### function SetMesh

```cpp
void SetMesh(
    TriMesh * mesh
)
```


### function SetFeaMesh

```cpp
void SetFeaMesh(
    const char * feaFile
)
```


### function SetFeaGrad

```cpp
void SetFeaGrad(
    const char * feaGradFile
)
```


### function SetFids

```cpp
void SetFids(
    const char * fidsFile
)
```


### function GetMesh

```cpp
inline meshFIM * GetMesh()
```


### function GetMesh

```cpp
inline meshFIM * GetMesh() const
```


### function GetZeroCrossingPoint

```cpp
inline virtual PointType GetZeroCrossingPoint() const override
```


**Reimplements**: [itk::ParticleDomain::GetZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


## Protected Functions Documentation

### function ParticleImplicitSurfaceDomain

```cpp
inline ParticleImplicitSurfaceDomain()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticleImplicitSurfaceDomain

```cpp
inline virtual ~ParticleImplicitSurfaceDomain()
```


-------------------------------

Updated on 2022-02-03 at 23:25:51 +0000