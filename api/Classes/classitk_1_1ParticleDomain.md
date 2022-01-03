---
title: itk::ParticleDomain

---

# itk::ParticleDomain





Inherits from DataObject

Inherited by [itk::ContourDomain](../Classes/classitk_1_1ContourDomain.md), [itk::MeshDomain](../Classes/classitk_1_1MeshDomain.md), [itk::ParticleRegionDomain](../Classes/classitk_1_1ParticleRegionDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md) > | **[Pointer](../Classes/classitk_1_1ParticleDomain.md#typedef-pointer)**  |
| typedef Point< double, DIMENSION > | **[PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype)**  |
| typedef vnl_matrix_fixed< float, DIMENSION, DIMENSION > | **[GradNType](../Classes/classitk_1_1ParticleDomain.md#typedef-gradntype)**  |

## Public Functions

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

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleDomain](../Classes/classitk_1_1ParticleDomain.md#function-particledomain)**() |
| virtual | **[~ParticleDomain](../Classes/classitk_1_1ParticleDomain.md#function-~particledomain)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleDomain.md#function-printself)**(std::ostream & os, Indent indent) const |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_FixedDomain](../Classes/classitk_1_1ParticleDomain.md#variable-m-fixeddomain)**  |
| int | **[m_DomainID](../Classes/classitk_1_1ParticleDomain.md#variable-m-domainid)**  |
| std::string | **[m_DomainName](../Classes/classitk_1_1ParticleDomain.md#variable-m-domainname)**  |
| std::shared_ptr< [Constraints](../Classes/classitk_1_1Constraints.md) > | **[constraints](../Classes/classitk_1_1ParticleDomain.md#variable-constraints)**  |

## Public Types Documentation

### typedef Pointer

```cpp
typedef SmartPointer<ParticleDomain> itk::ParticleDomain::Pointer;
```


Standard class typedefs 


### typedef PointType

```cpp
typedef Point<double, DIMENSION> itk::ParticleDomain::PointType;
```


Point type used to store particle locations. 


### typedef GradNType

```cpp
typedef vnl_matrix_fixed<float, DIMENSION, DIMENSION> itk::ParticleDomain::GradNType;
```


## Public Functions Documentation

### function ApplyConstraints

```cpp
virtual bool ApplyConstraints(
    PointType & p,
    int idx,
    bool dbg =false
) const =0
```


**Reimplemented by**: [itk::ContourDomain::ApplyConstraints](../Classes/classitk_1_1ContourDomain.md#function-applyconstraints), [itk::ParticleImplicitSurfaceDomain::ApplyConstraints](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-applyconstraints), [itk::MeshDomain::ApplyConstraints](../Classes/classitk_1_1MeshDomain.md#function-applyconstraints)


Apply any constraints to the given point location. This should force the point to a position on the surface that satisfies all constraints. 


### function UpdateParticlePosition

```cpp
virtual PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & update
) const =0
```


**Reimplemented by**: [itk::ContourDomain::UpdateParticlePosition](../Classes/classitk_1_1ContourDomain.md#function-updateparticleposition), [itk::ParticleImplicitSurfaceDomain::UpdateParticlePosition](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-updateparticleposition), [itk::MeshDomain::UpdateParticlePosition](../Classes/classitk_1_1MeshDomain.md#function-updateparticleposition)


Applies the update to the point and returns the new point position. 


### function InvalidateParticlePosition

```cpp
inline virtual void InvalidateParticlePosition(
    int idx
) const
```


**Reimplemented by**: [itk::ContourDomain::InvalidateParticlePosition](../Classes/classitk_1_1ContourDomain.md#function-invalidateparticleposition), [itk::MeshDomain::InvalidateParticlePosition](../Classes/classitk_1_1MeshDomain.md#function-invalidateparticleposition)


### function ProjectVectorToSurfaceTangent

```cpp
virtual vnl_vector_fixed< double, DIMENSION > ProjectVectorToSurfaceTangent(
    vnl_vector_fixed< double, DIMENSION > & gradE,
    const PointType & pos,
    int idx
) const =0
```


**Reimplemented by**: [itk::ContourDomain::ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ContourDomain.md#function-projectvectortosurfacetangent), [itk::ParticleImageDomainWithGradients::ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-projectvectortosurfacetangent), [itk::MeshDomain::ProjectVectorToSurfaceTangent](../Classes/classitk_1_1MeshDomain.md#function-projectvectortosurfacetangent)


Projects the vector to the surface tangent at the point. 


### function SampleGradientAtPoint

```cpp
virtual vnl_vector_fixed< float, DIMENSION > SampleGradientAtPoint(
    const PointType & point,
    int idx
) const =0
```


**Reimplemented by**: [itk::ParticleImageDomainWithGradients::SampleGradientAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-samplegradientatpoint), [itk::ContourDomain::SampleGradientAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplegradientatpoint), [itk::MeshDomain::SampleGradientAtPoint](../Classes/classitk_1_1MeshDomain.md#function-samplegradientatpoint)


### function SampleNormalAtPoint

```cpp
virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    const PointType & point,
    int idx
) const =0
```


**Reimplemented by**: [itk::ParticleImageDomainWithGradients::SampleNormalAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-samplenormalatpoint), [itk::ContourDomain::SampleNormalAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplenormalatpoint), [itk::MeshDomain::SampleNormalAtPoint](../Classes/classitk_1_1MeshDomain.md#function-samplenormalatpoint)


### function SampleGradNAtPoint

```cpp
virtual GradNType SampleGradNAtPoint(
    const PointType & p,
    int idx
) const =0
```


**Reimplemented by**: [itk::ContourDomain::SampleGradNAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplegradnatpoint), [itk::ParticleImageDomainWithGradN::SampleGradNAtPoint](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-samplegradnatpoint), [itk::MeshDomain::SampleGradNAtPoint](../Classes/classitk_1_1MeshDomain.md#function-samplegradnatpoint)


### function Distance

```cpp
inline virtual double Distance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    vnl_vector_fixed< double, DIMENSION > * out_grad =nullptr
) const
```


**Reimplemented by**: [itk::MeshDomain::Distance](../Classes/classitk_1_1MeshDomain.md#function-distance)


Distance between locations is used for computing energy and neighborhoods. Optionally return the gradient of the distance 


### function SquaredDistance

```cpp
inline virtual double SquaredDistance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b
) const
```


**Reimplemented by**: [itk::ContourDomain::SquaredDistance](../Classes/classitk_1_1ContourDomain.md#function-squareddistance), [itk::MeshDomain::SquaredDistance](../Classes/classitk_1_1MeshDomain.md#function-squareddistance)


Squared Distance between locations is used for computing sigma. 


### function IsWithinDistance

```cpp
inline virtual bool IsWithinDistance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    double test_dist,
    double & distance
) const
```


**Reimplemented by**: [itk::MeshDomain::IsWithinDistance](../Classes/classitk_1_1MeshDomain.md#function-iswithindistance)


Returns whether or not the two points are separated by the given distance 


### function GetCurvature

```cpp
virtual double GetCurvature(
    const PointType & p,
    int idx
) const =0
```


**Reimplemented by**: [itk::ContourDomain::GetCurvature](../Classes/classitk_1_1ContourDomain.md#function-getcurvature), [itk::ParticleImageDomainWithCurvature::GetCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getcurvature), [itk::MeshDomain::GetCurvature](../Classes/classitk_1_1MeshDomain.md#function-getcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


### function GetSurfaceMeanCurvature

```cpp
virtual double GetSurfaceMeanCurvature() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetSurfaceMeanCurvature](../Classes/classitk_1_1ContourDomain.md#function-getsurfacemeancurvature), [itk::ParticleImageDomainWithCurvature::GetSurfaceMeanCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacemeancurvature), [itk::MeshDomain::GetSurfaceMeanCurvature](../Classes/classitk_1_1MeshDomain.md#function-getsurfacemeancurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


### function GetSurfaceStdDevCurvature

```cpp
virtual double GetSurfaceStdDevCurvature() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetSurfaceStdDevCurvature](../Classes/classitk_1_1ContourDomain.md#function-getsurfacestddevcurvature), [itk::ParticleImageDomainWithCurvature::GetSurfaceStdDevCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md#function-getsurfacestddevcurvature), [itk::MeshDomain::GetSurfaceStdDevCurvature](../Classes/classitk_1_1MeshDomain.md#function-getsurfacestddevcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)


### function GetLowerBound

```cpp
virtual const PointType & GetLowerBound() const =0
```


**Reimplemented by**: [itk::ParticleRegionDomain::GetLowerBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-getlowerbound), [itk::ContourDomain::GetLowerBound](../Classes/classitk_1_1ContourDomain.md#function-getlowerbound), [itk::MeshDomain::GetLowerBound](../Classes/classitk_1_1MeshDomain.md#function-getlowerbound)


Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md). 


### function GetUpperBound

```cpp
virtual const PointType & GetUpperBound() const =0
```


**Reimplemented by**: [itk::ParticleRegionDomain::GetUpperBound](../Classes/classitk_1_1ParticleRegionDomain.md#function-getupperbound), [itk::ContourDomain::GetUpperBound](../Classes/classitk_1_1ContourDomain.md#function-getupperbound), [itk::MeshDomain::GetUpperBound](../Classes/classitk_1_1MeshDomain.md#function-getupperbound)


Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md). 


### function GetZeroCrossingPoint

```cpp
virtual PointType GetZeroCrossingPoint() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetZeroCrossingPoint](../Classes/classitk_1_1ContourDomain.md#function-getzerocrossingpoint), [itk::ParticleImplicitSurfaceDomain::GetZeroCrossingPoint](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getzerocrossingpoint), [itk::MeshDomain::GetZeroCrossingPoint](../Classes/classitk_1_1MeshDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


### function GetSurfaceArea

```cpp
virtual double GetSurfaceArea() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetSurfaceArea](../Classes/classitk_1_1ContourDomain.md#function-getsurfacearea), [itk::ParticleImageDomain::GetSurfaceArea](../Classes/classitk_1_1ParticleImageDomain.md#function-getsurfacearea), [itk::MeshDomain::GetSurfaceArea](../Classes/classitk_1_1MeshDomain.md#function-getsurfacearea)


Use for neighborhood radius. 


### function GetValidLocationNear

```cpp
virtual PointType GetValidLocationNear(
    PointType p
) const =0
```


**Reimplemented by**: [itk::ContourDomain::GetValidLocationNear](../Classes/classitk_1_1ContourDomain.md#function-getvalidlocationnear), [itk::ParticleImageDomain::GetValidLocationNear](../Classes/classitk_1_1ParticleImageDomain.md#function-getvalidlocationnear), [itk::MeshDomain::GetValidLocationNear](../Classes/classitk_1_1MeshDomain.md#function-getvalidlocationnear)


GetValidLocation returns a PointType location on the surface. Used for placing the first particle. 


### function GetMaxDiameter

```cpp
virtual double GetMaxDiameter() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetMaxDiameter](../Classes/classitk_1_1ContourDomain.md#function-getmaxdiameter), [itk::ParticleImageDomain::GetMaxDiameter](../Classes/classitk_1_1ParticleImageDomain.md#function-getmaxdiameter), [itk::MeshDomain::GetMaxDiameter](../Classes/classitk_1_1MeshDomain.md#function-getmaxdiameter)


GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma 


### function DeleteImages

```cpp
virtual void DeleteImages() =0
```


**Reimplemented by**: [itk::ContourDomain::DeleteImages](../Classes/classitk_1_1ContourDomain.md#function-deleteimages), [itk::ParticleImageDomain::DeleteImages](../Classes/classitk_1_1ParticleImageDomain.md#function-deleteimages), [itk::ParticleImageDomainWithGradients::DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradients.md#function-deleteimages), [itk::ParticleImageDomainWithGradN::DeleteImages](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-deleteimages), [itk::MeshDomain::DeleteImages](../Classes/classitk_1_1MeshDomain.md#function-deleteimages)


### function DeletePartialDerivativeImages

```cpp
virtual void DeletePartialDerivativeImages() =0
```


**Reimplemented by**: [itk::ContourDomain::DeletePartialDerivativeImages](../Classes/classitk_1_1ContourDomain.md#function-deletepartialderivativeimages), [itk::ParticleImageDomainWithGradN::DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleImageDomainWithGradN.md#function-deletepartialderivativeimages), [itk::MeshDomain::DeletePartialDerivativeImages](../Classes/classitk_1_1MeshDomain.md#function-deletepartialderivativeimages)


### function UpdateZeroCrossingPoint

```cpp
virtual void UpdateZeroCrossingPoint() =0
```


**Reimplemented by**: [itk::ContourDomain::UpdateZeroCrossingPoint](../Classes/classitk_1_1ContourDomain.md#function-updatezerocrossingpoint), [itk::ParticleImageDomain::UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleImageDomain.md#function-updatezerocrossingpoint), [itk::MeshDomain::UpdateZeroCrossingPoint](../Classes/classitk_1_1MeshDomain.md#function-updatezerocrossingpoint)


### function IsDomainFixed

```cpp
inline bool IsDomainFixed() const
```


### function GetDomainType

```cpp
virtual shapeworks::DomainType GetDomainType() const =0
```


**Reimplemented by**: [itk::ContourDomain::GetDomainType](../Classes/classitk_1_1ContourDomain.md#function-getdomaintype), [itk::ParticleImplicitSurfaceDomain::GetDomainType](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md#function-getdomaintype), [itk::MeshDomain::GetDomainType](../Classes/classitk_1_1MeshDomain.md#function-getdomaintype)


### function GetConstraints

```cpp
inline std::shared_ptr< Constraints > GetConstraints() const
```


### function GetPositionAfterSplit

```cpp
inline virtual PointType GetPositionAfterSplit(
    const PointType & pt,
    const vnl_vector_fixed< double, 3 > & random,
    double epsilon
) const
```


**Reimplemented by**: [itk::ContourDomain::GetPositionAfterSplit](../Classes/classitk_1_1ContourDomain.md#function-getpositionaftersplit)


### function SetDomainID

```cpp
inline void SetDomainID(
    int id
)
```


### function SetDomainName

```cpp
inline void SetDomainName(
    std::string name
)
```


## Protected Functions Documentation

### function ParticleDomain

```cpp
inline ParticleDomain()
```


### function ~ParticleDomain

```cpp
inline virtual ~ParticleDomain()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


## Protected Attributes Documentation

### variable m_FixedDomain

```cpp
bool m_FixedDomain {true};
```


### variable m_DomainID

```cpp
int m_DomainID {-1};
```


### variable m_DomainName

```cpp
std::string m_DomainName;
```


### variable constraints

```cpp
std::shared_ptr< Constraints > constraints;
```


-------------------------------

Updated on 2022-01-03 at 16:16:08 +0000