---
title: shapeworks::ParticleDomain

---

# shapeworks::ParticleDomain





Inherited by [shapeworks::ContourDomain](../Classes/classshapeworks_1_1ContourDomain.md), [shapeworks::MeshDomain](../Classes/classshapeworks_1_1MeshDomain.md), [shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef std::shared_ptr< [ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleDomain.md#typedef-pointer)**  |
| using itk::Point< double, 3 > | **[PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype)**  |
| using vnl_matrix_fixed< float, 3, 3 > | **[GradNType](../Classes/classshapeworks_1_1ParticleDomain.md#using-gradntype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorDoubleType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectordoubletype)**  |
| using vnl_vector_fixed< float, 3 > | **[VectorFloatType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectorfloattype)**  |

## Public Functions

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
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const =0 |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacemeancurvature)**() const =0 |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacestddevcurvature)**() const =0 |
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

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md#function-particledomain)**() |
| virtual | **[~ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md#function-~particledomain)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_FixedDomain](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-fixeddomain)**  |
| int | **[m_DomainID](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-domainid)**  |
| std::string | **[m_DomainName](../Classes/classshapeworks_1_1ParticleDomain.md#variable-m-domainname)**  |
| std::shared_ptr< [shapeworks::Constraints](../Classes/classshapeworks_1_1Constraints.md) > | **[constraints](../Classes/classshapeworks_1_1ParticleDomain.md#variable-constraints)**  |

## Public Types Documentation

### typedef Pointer

```cpp
typedef std::shared_ptr<ParticleDomain> shapeworks::ParticleDomain::Pointer;
```


### using PointType

```cpp
using shapeworks::ParticleDomain::PointType =  itk::Point<double, 3>;
```


Point type used to store particle locations. 


### using GradNType

```cpp
using shapeworks::ParticleDomain::GradNType =  vnl_matrix_fixed<float, 3, 3>;
```


### using VectorDoubleType

```cpp
using shapeworks::ParticleDomain::VectorDoubleType =  vnl_vector_fixed<double, 3>;
```


### using VectorFloatType

```cpp
using shapeworks::ParticleDomain::VectorFloatType =  vnl_vector_fixed<float, 3>;
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


**Reimplemented by**: [shapeworks::ContourDomain::ApplyConstraints](../Classes/classshapeworks_1_1ContourDomain.md#function-applyconstraints), [shapeworks::ImplicitSurfaceDomain::ApplyConstraints](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md#function-applyconstraints), [shapeworks::MeshDomain::ApplyConstraints](../Classes/classshapeworks_1_1MeshDomain.md#function-applyconstraints)


Apply any constraints to the given point location. This should force the point to a position on the surface that satisfies all constraints. 


### function UpdateParticlePosition

```cpp
virtual PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    VectorDoubleType & update
) const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::UpdateParticlePosition](../Classes/classshapeworks_1_1ContourDomain.md#function-updateparticleposition), [shapeworks::MeshDomain::UpdateParticlePosition](../Classes/classshapeworks_1_1MeshDomain.md#function-updateparticleposition)


Applies the update to the point and returns the new point position. 


### function InvalidateParticlePosition

```cpp
inline virtual void InvalidateParticlePosition(
    int idx
) const
```


**Reimplemented by**: [shapeworks::ContourDomain::InvalidateParticlePosition](../Classes/classshapeworks_1_1ContourDomain.md#function-invalidateparticleposition), [shapeworks::MeshDomain::InvalidateParticlePosition](../Classes/classshapeworks_1_1MeshDomain.md#function-invalidateparticleposition)


### function ProjectVectorToSurfaceTangent

```cpp
virtual VectorDoubleType ProjectVectorToSurfaceTangent(
    VectorDoubleType & gradE,
    const PointType & pos,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ContourDomain.md#function-projectvectortosurfacetangent), [shapeworks::MeshDomain::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshDomain.md#function-projectvectortosurfacetangent)


Projects the vector to the surface tangent at the point. 


### function SampleGradientAtPoint

```cpp
virtual VectorFloatType SampleGradientAtPoint(
    const PointType & point,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::ImageDomainWithGradients::SampleGradientAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-samplegradientatpoint), [shapeworks::ContourDomain::SampleGradientAtPoint](../Classes/classshapeworks_1_1ContourDomain.md#function-samplegradientatpoint), [shapeworks::MeshDomain::SampleGradientAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplegradientatpoint)


### function SampleNormalAtPoint

```cpp
virtual VectorFloatType SampleNormalAtPoint(
    const PointType & point,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::ImageDomainWithGradients::SampleNormalAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-samplenormalatpoint), [shapeworks::ContourDomain::SampleNormalAtPoint](../Classes/classshapeworks_1_1ContourDomain.md#function-samplenormalatpoint), [shapeworks::MeshDomain::SampleNormalAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplenormalatpoint)


### function SampleGradNAtPoint

```cpp
virtual GradNType SampleGradNAtPoint(
    const PointType & p,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::SampleGradNAtPoint](../Classes/classshapeworks_1_1ContourDomain.md#function-samplegradnatpoint), [shapeworks::ImageDomainWithGradN::SampleGradNAtPoint](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-samplegradnatpoint), [shapeworks::MeshDomain::SampleGradNAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplegradnatpoint)


### function Distance

```cpp
inline virtual double Distance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    VectorDoubleType * out_grad =nullptr
) const
```


**Reimplemented by**: [shapeworks::ContourDomain::Distance](../Classes/classshapeworks_1_1ContourDomain.md#function-distance)


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


**Reimplemented by**: [shapeworks::ContourDomain::SquaredDistance](../Classes/classshapeworks_1_1ContourDomain.md#function-squareddistance), [shapeworks::MeshDomain::SquaredDistance](../Classes/classshapeworks_1_1MeshDomain.md#function-squareddistance)


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


**Reimplemented by**: [shapeworks::MeshDomain::IsWithinDistance](../Classes/classshapeworks_1_1MeshDomain.md#function-iswithindistance)


Returns whether or not the two points are separated by the given distance 


### function GetCurvature

```cpp
virtual double GetCurvature(
    const PointType & p,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetCurvature](../Classes/classshapeworks_1_1ContourDomain.md#function-getcurvature), [shapeworks::ImageDomainWithCurvature::GetCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getcurvature), [shapeworks::MeshDomain::GetCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


### function GetSurfaceMeanCurvature

```cpp
virtual double GetSurfaceMeanCurvature() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ContourDomain.md#function-getsurfacemeancurvature), [shapeworks::ImageDomainWithCurvature::GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getsurfacemeancurvature), [shapeworks::MeshDomain::GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacemeancurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


### function GetSurfaceStdDevCurvature

```cpp
virtual double GetSurfaceStdDevCurvature() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ContourDomain.md#function-getsurfacestddevcurvature), [shapeworks::ImageDomainWithCurvature::GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md#function-getsurfacestddevcurvature), [shapeworks::MeshDomain::GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacestddevcurvature)


Used in [ParticleMeanCurvatureAttribute](../Classes/classParticleMeanCurvatureAttribute.md)


### function GetLowerBound

```cpp
virtual const PointType & GetLowerBound() const =0
```


**Reimplemented by**: [shapeworks::ParticleRegionDomain::GetLowerBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getlowerbound), [shapeworks::ContourDomain::GetLowerBound](../Classes/classshapeworks_1_1ContourDomain.md#function-getlowerbound), [shapeworks::MeshDomain::GetLowerBound](../Classes/classshapeworks_1_1MeshDomain.md#function-getlowerbound)


Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function GetUpperBound

```cpp
virtual const PointType & GetUpperBound() const =0
```


**Reimplemented by**: [shapeworks::ParticleRegionDomain::GetUpperBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getupperbound), [shapeworks::ContourDomain::GetUpperBound](../Classes/classshapeworks_1_1ContourDomain.md#function-getupperbound), [shapeworks::MeshDomain::GetUpperBound](../Classes/classshapeworks_1_1MeshDomain.md#function-getupperbound)


Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function GetZeroCrossingPoint

```cpp
virtual PointType GetZeroCrossingPoint() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetZeroCrossingPoint](../Classes/classshapeworks_1_1ContourDomain.md#function-getzerocrossingpoint), [shapeworks::ImplicitSurfaceDomain::GetZeroCrossingPoint](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md#function-getzerocrossingpoint), [shapeworks::MeshDomain::GetZeroCrossingPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


### function GetSurfaceArea

```cpp
virtual double GetSurfaceArea() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetSurfaceArea](../Classes/classshapeworks_1_1ContourDomain.md#function-getsurfacearea), [shapeworks::ImageDomain::GetSurfaceArea](../Classes/classshapeworks_1_1ImageDomain.md#function-getsurfacearea), [shapeworks::MeshDomain::GetSurfaceArea](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacearea)


Use for neighborhood radius. 


### function GetValidLocationNear

```cpp
virtual PointType GetValidLocationNear(
    PointType p
) const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetValidLocationNear](../Classes/classshapeworks_1_1ContourDomain.md#function-getvalidlocationnear), [shapeworks::ImageDomain::GetValidLocationNear](../Classes/classshapeworks_1_1ImageDomain.md#function-getvalidlocationnear), [shapeworks::MeshDomain::GetValidLocationNear](../Classes/classshapeworks_1_1MeshDomain.md#function-getvalidlocationnear)


GetValidLocation returns a PointType location on the surface. Used for placing the first particle. 


### function GetMaxDiameter

```cpp
virtual double GetMaxDiameter() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetMaxDiameter](../Classes/classshapeworks_1_1ContourDomain.md#function-getmaxdiameter), [shapeworks::ImageDomain::GetMaxDiameter](../Classes/classshapeworks_1_1ImageDomain.md#function-getmaxdiameter), [shapeworks::MeshDomain::GetMaxDiameter](../Classes/classshapeworks_1_1MeshDomain.md#function-getmaxdiameter)


GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma 


### function DeleteImages

```cpp
virtual void DeleteImages() =0
```


**Reimplemented by**: [shapeworks::ContourDomain::DeleteImages](../Classes/classshapeworks_1_1ContourDomain.md#function-deleteimages), [shapeworks::ImageDomain::DeleteImages](../Classes/classshapeworks_1_1ImageDomain.md#function-deleteimages), [shapeworks::ImageDomainWithGradients::DeleteImages](../Classes/classshapeworks_1_1ImageDomainWithGradients.md#function-deleteimages), [shapeworks::ImageDomainWithGradN::DeleteImages](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-deleteimages), [shapeworks::MeshDomain::DeleteImages](../Classes/classshapeworks_1_1MeshDomain.md#function-deleteimages)


### function DeletePartialDerivativeImages

```cpp
virtual void DeletePartialDerivativeImages() =0
```


**Reimplemented by**: [shapeworks::ContourDomain::DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ContourDomain.md#function-deletepartialderivativeimages), [shapeworks::ImageDomainWithGradN::DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ImageDomainWithGradN.md#function-deletepartialderivativeimages), [shapeworks::MeshDomain::DeletePartialDerivativeImages](../Classes/classshapeworks_1_1MeshDomain.md#function-deletepartialderivativeimages)


### function UpdateZeroCrossingPoint

```cpp
virtual void UpdateZeroCrossingPoint() =0
```


**Reimplemented by**: [shapeworks::ContourDomain::UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ContourDomain.md#function-updatezerocrossingpoint), [shapeworks::ImageDomain::UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ImageDomain.md#function-updatezerocrossingpoint), [shapeworks::MeshDomain::UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-updatezerocrossingpoint)


### function IsDomainFixed

```cpp
inline bool IsDomainFixed() const
```


### function GetDomainType

```cpp
virtual shapeworks::DomainType GetDomainType() const =0
```


**Reimplemented by**: [shapeworks::ContourDomain::GetDomainType](../Classes/classshapeworks_1_1ContourDomain.md#function-getdomaintype), [shapeworks::ImplicitSurfaceDomain::GetDomainType](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md#function-getdomaintype), [shapeworks::MeshDomain::GetDomainType](../Classes/classshapeworks_1_1MeshDomain.md#function-getdomaintype)


### function GetConstraints

```cpp
inline std::shared_ptr< shapeworks::Constraints > GetConstraints() const
```


### function GetPositionAfterSplit

```cpp
inline virtual PointType GetPositionAfterSplit(
    const PointType & pt,
    const VectorDoubleType & local_direction,
    const VectorDoubleType & global_direction,
    double epsilon
) const
```


**Reimplemented by**: [shapeworks::ContourDomain::GetPositionAfterSplit](../Classes/classshapeworks_1_1ContourDomain.md#function-getpositionaftersplit)


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
std::shared_ptr< shapeworks::Constraints > constraints;
```


-------------------------------

Updated on 2023-05-08 at 20:57:57 +0000