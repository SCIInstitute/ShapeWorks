---
title: shapeworks::ParticleImplicitSurfaceDomain

---

# shapeworks::ParticleImplicitSurfaceDomain



 [More...](#detailed-description)


`#include <ParticleImplicitSurfaceDomain.h>`

Inherits from [shapeworks::ParticleImageDomainWithCurvature< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md), [shapeworks::ParticleImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md), [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md), [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md), [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md), [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleImageDomainWithCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)< T > | **[Superclass](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#typedef-superclass)**  |
| typedef std::shared_ptr< [ParticleImplicitSurfaceDomain](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#typedef-pointer)**  |
| typedef Superclass::ImageType | **[ImageType](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#typedef-imagetype)**  |
| typedef [Superclass::PointType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[SetTolerance](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-settolerance)**(const T _Tolerance) |
| virtual T | **[GetTolerance](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-gettolerance)**() |
| virtual shapeworks::DomainType | **[GetDomainType](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-getdomaintype)**() const override |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx, bool dbg =false) const override |
| [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[UpdateParticlePosition](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx, vnl_vector_fixed< double, DIMENSION > & update) const override |
| void | **[SetMesh](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-setmesh)**(TriMesh * mesh) |
| void | **[SetFeaMesh](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-setfeamesh)**(const char * feaFile) |
| void | **[SetFeaGrad](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-setfeagrad)**(const char * feaGradFile) |
| void | **[SetFids](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-setfids)**(const char * fidsFile) |
| [meshFIM](../Classes/classmeshFIM.md) * | **[GetMesh](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-getmesh)**() |
| [meshFIM](../Classes/classmeshFIM.md) * | **[GetMesh](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-getmesh)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-getzerocrossingpoint)**() const override |
| | **[ParticleImplicitSurfaceDomain](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-particleimplicitsurfacedomain)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ParticleImplicitSurfaceDomain](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md#function-~particleimplicitsurfacedomain)**() |

## Additional inherited members

**Public Types inherited from [shapeworks::ParticleImageDomainWithCurvature< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::VnlMatrixType | **[VnlMatrixType](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#typedef-vnlmatrixtype)**  |

**Public Functions inherited from [shapeworks::ParticleImageDomainWithCurvature< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-getsurfacestddevcurvature)**() const override |

**Protected Functions inherited from [shapeworks::ParticleImageDomainWithCurvature< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-particleimagedomainwithcurvature)**() |
| virtual | **[~ParticleImageDomainWithCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md#function-~particleimagedomainwithcurvature)**() |

**Public Types inherited from [shapeworks::ParticleImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::GradNType | **[GradNType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-gradntype)**  |
| typedef Superclass::GradNType | **[VnlMatrixType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#typedef-vnlmatrixtype)**  |

**Public Functions inherited from [shapeworks::ParticleImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-deletepartialderivativeimages)**() override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-deleteimages)**() override |

**Protected Functions inherited from [shapeworks::ParticleImageDomainWithGradN< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradN](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-particleimagedomainwithgradn)**() |
| virtual | **[~ParticleImageDomainWithGradN](../Classes/classshapeworks_1_1ParticleImageDomainWithGradN.md#function-~particleimagedomainwithgradn)**() |

**Public Types inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| typedef itk::FixedArray< T, DIMENSION > | **[VectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vectortype)**  |
| typedef vnl_vector_fixed< T, DIMENSION > | **[VnlVectorType](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#typedef-vnlvectortype)**  |

**Public Functions inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const |
| vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-deleteimages)**() override |

**Protected Functions inherited from [shapeworks::ParticleImageDomainWithGradients< T >](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-particleimagedomainwithgradients)**() |
| virtual | **[~ParticleImageDomainWithGradients](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-~particleimagedomainwithgradients)**() |
| openvdb::VectorGrid::Ptr | **[GetVDBGradient](../Classes/classshapeworks_1_1ParticleImageDomainWithGradients.md#function-getvdbgradient)**() |

**Public Functions inherited from [shapeworks::ParticleImageDomain< T >](../Classes/classshapeworks_1_1ParticleImageDomain.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[SetImage](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-setimage)**([ImageType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-imagetype) * I, double narrow_band) |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsurfacearea)**() const override |
| [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) | **[GetOrigin](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getorigin)**() const |
| ImageType::SizeType | **[GetSize](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getsize)**() const |
| ImageType::SpacingType | **[GetSpacing](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getspacing)**() const |
| ImageType::RegionType::IndexType | **[GetIndex](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getindex)**() const |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) p) const override |
| T | **[Sample](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-sample)**(const [PointType](../Classes/classshapeworks_1_1ParticleImageDomain.md#using-pointtype) & p) const |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleImageDomain.md#function-deleteimages)**() override |
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
| virtual void | **[InvalidateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-invalidateparticleposition)**(int idx) const |
| virtual VectorDoubleType | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleDomain.md#function-projectvectortosurfacetangent)**(VectorDoubleType & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const =0 |
| virtual VectorFloatType | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const =0 |
| virtual VectorFloatType | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const =0 |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[Distance](../Classes/classshapeworks_1_1ParticleDomain.md#function-distance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, VectorDoubleType * out_grad =nullptr) const |
| virtual double | **[SquaredDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-squareddistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b) const |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-iswithindistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, double test_dist, double & distance) const |
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacemeancurvature)**() const =0 |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacestddevcurvature)**() const =0 |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getlowerbound)**() const =0 |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getupperbound)**() const =0 |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacearea)**() const =0 |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1ParticleDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) p) const =0 |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1ParticleDomain.md#function-getmaxdiameter)**() const =0 |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deleteimages)**() =0 |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deletepartialderivativeimages)**() =0 |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-updatezerocrossingpoint)**() =0 |
| bool | **[IsDomainFixed](../Classes/classshapeworks_1_1ParticleDomain.md#function-isdomainfixed)**() const |
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
class shapeworks::ParticleImplicitSurfaceDomain;
```


A 3D cartesian domain that constrains points so that they always lie an implicit surface. The implicit surface is defined as the zero isosurface of the given image. [Constraints](../Classes/classshapeworks_1_1Constraints.md) are applied using a Newton-Raphson iteration, and this class assumes it has a distance transform as an image. 

## Public Types Documentation

### typedef Superclass

```cpp
typedef ParticleImageDomainWithCurvature<T> shapeworks::ParticleImplicitSurfaceDomain< T >::Superclass;
```


Standard class typedefs 


### typedef Pointer

```cpp
typedef std::shared_ptr<ParticleImplicitSurfaceDomain> shapeworks::ParticleImplicitSurfaceDomain< T >::Pointer;
```


### typedef ImageType

```cpp
typedef Superclass::ImageType shapeworks::ParticleImplicitSurfaceDomain< T >::ImageType;
```


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::ParticleImplicitSurfaceDomain< T >::PointType;
```


## Public Functions Documentation

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


**Reimplements**: [shapeworks::ParticleDomain::GetDomainType](../Classes/classshapeworks_1_1ParticleDomain.md#function-getdomaintype)


### function ApplyConstraints

```cpp
inline virtual bool ApplyConstraints(
    PointType & p,
    int idx,
    bool dbg =false
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::ApplyConstraints](../Classes/classshapeworks_1_1ParticleDomain.md#function-applyconstraints)


Apply any constraints to the given point location. This method constrains points to lie within the given domain and on a given implicit surface. If the point is not already on the surface, it is projected back to the surface using a Newton-Raphson iteration. IMPORTANT: This method returns the true/false value of its superclass, and does not indicate changes only due to projection. This is done for speed: we typically will only want to know if a point tried to move outside of the bounding box domain, since movement off the surface will be very common. Consider subclassing this method to add a check for significant differences in the input and output points. 


### function UpdateParticlePosition

```cpp
inline PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & update
) const override
```


### function SetMesh

```cpp
inline void SetMesh(
    TriMesh * mesh
)
```


### function SetFeaMesh

```cpp
inline void SetFeaMesh(
    const char * feaFile
)
```


### function SetFeaGrad

```cpp
inline void SetFeaGrad(
    const char * feaGradFile
)
```


### function SetFids

```cpp
inline void SetFids(
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


**Reimplements**: [shapeworks::ParticleDomain::GetZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


### function ParticleImplicitSurfaceDomain

```cpp
inline ParticleImplicitSurfaceDomain()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~ParticleImplicitSurfaceDomain

```cpp
inline virtual ~ParticleImplicitSurfaceDomain()
```


-------------------------------

Updated on 2022-04-15 at 07:45:37 +0000