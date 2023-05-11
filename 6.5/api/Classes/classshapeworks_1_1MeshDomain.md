---
title: shapeworks::MeshDomain

---

# shapeworks::MeshDomain





Inherits from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [MeshDomain](../Classes/classshapeworks_1_1MeshDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1MeshDomain.md#using-pointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual DomainType | **[GetDomainType](../Classes/classshapeworks_1_1MeshDomain.md#function-getdomaintype)**() const override |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1MeshDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx, bool dbg =false) const override |
| bool | **[ApplyVectorConstraints](../Classes/classshapeworks_1_1MeshDomain.md#function-applyvectorconstraints)**(VectorDoubleType & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos) const |
| virtual VectorDoubleType | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshDomain.md#function-projectvectortosurfacetangent)**(VectorDoubleType & gradE, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & pos, int idx) const override |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[UpdateParticlePosition](../Classes/classshapeworks_1_1MeshDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx, VectorDoubleType & update) const override |
| virtual void | **[InvalidateParticlePosition](../Classes/classshapeworks_1_1MeshDomain.md#function-invalidateparticleposition)**(int idx) const override |
| virtual double | **[GetCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getcurvature)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacestddevcurvature)**() const override |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1MeshDomain.md#function-getlowerbound)**() const override |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1MeshDomain.md#function-getupperbound)**() const override |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetZeroCrossingPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-getzerocrossingpoint)**() const override |
| virtual [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[GetValidLocationNear](../Classes/classshapeworks_1_1MeshDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) p) const override |
| virtual double | **[GetSurfaceArea](../Classes/classshapeworks_1_1MeshDomain.md#function-getsurfacearea)**() const override |
| virtual double | **[GetMaxDiameter](../Classes/classshapeworks_1_1MeshDomain.md#function-getmaxdiameter)**() const override |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplegradientatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const override |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplenormalatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & point, int idx) const override |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p, int idx) const override |
| double | **[Distance](../Classes/classshapeworks_1_1MeshDomain.md#function-distance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, vnl_vector_fixed< double, DIMENSION > * out_grad =nullptr) const override |
| virtual double | **[SquaredDistance](../Classes/classshapeworks_1_1MeshDomain.md#function-squareddistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b) const override |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1MeshDomain.md#function-iswithindistance)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & a, int idx_a, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & b, int idx_b, double test_dist, double & distance) const override |
| virtual void | **[DeleteImages](../Classes/classshapeworks_1_1MeshDomain.md#function-deleteimages)**() override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classshapeworks_1_1MeshDomain.md#function-deletepartialderivativeimages)**() override |
| void | **[SetMesh](../Classes/classshapeworks_1_1MeshDomain.md#function-setmesh)**(std::shared_ptr< [shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md) > mesh_) |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1MeshDomain.md#function-updatezerocrossingpoint)**() override |

## Additional inherited members

**Public Types inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| using itk::Point< double, 3 > | **[PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype)**  |
| using vnl_matrix_fixed< float, 3, 3 > | **[GradNType](../Classes/classshapeworks_1_1ParticleDomain.md#using-gradntype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorDoubleType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectordoubletype)**  |
| using vnl_vector_fixed< float, 3 > | **[VectorFloatType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectorfloattype)**  |

**Public Functions inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
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


## Public Types Documentation

### using Pointer

```cpp
using shapeworks::MeshDomain::Pointer =  std::shared_ptr<MeshDomain>;
```


## Public Functions Documentation

### function GetDomainType

```cpp
inline virtual DomainType GetDomainType() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetDomainType](../Classes/classshapeworks_1_1ParticleDomain.md#function-getdomaintype)


### function ApplyConstraints

```cpp
virtual bool ApplyConstraints(
    PointType & p,
    int idx,
    bool dbg =false
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::ApplyConstraints](../Classes/classshapeworks_1_1ParticleDomain.md#function-applyconstraints)


Apply any constraints to the given point location. This should force the point to a position on the surface that satisfies all constraints. 


### function ApplyVectorConstraints

```cpp
bool ApplyVectorConstraints(
    VectorDoubleType & gradE,
    const PointType & pos
) const
```


### function ProjectVectorToSurfaceTangent

```cpp
virtual VectorDoubleType ProjectVectorToSurfaceTangent(
    VectorDoubleType & gradE,
    const PointType & pos,
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1ParticleDomain.md#function-projectvectortosurfacetangent)


Projects the vector to the surface tangent at the point. 


### function UpdateParticlePosition

```cpp
virtual PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    VectorDoubleType & update
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::UpdateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-updateparticleposition)


Applies the update to the point and returns the new point position. 


### function InvalidateParticlePosition

```cpp
virtual void InvalidateParticlePosition(
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::InvalidateParticlePosition](../Classes/classshapeworks_1_1ParticleDomain.md#function-invalidateparticleposition)


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


### function GetLowerBound

```cpp
inline virtual const PointType & GetLowerBound() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetLowerBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getlowerbound)


Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function GetUpperBound

```cpp
inline virtual const PointType & GetUpperBound() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetUpperBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getupperbound)


Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function GetZeroCrossingPoint

```cpp
inline virtual PointType GetZeroCrossingPoint() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


### function GetValidLocationNear

```cpp
inline virtual PointType GetValidLocationNear(
    PointType p
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetValidLocationNear](../Classes/classshapeworks_1_1ParticleDomain.md#function-getvalidlocationnear)


GetValidLocation returns a PointType location on the surface. Used for placing the first particle. 


### function GetSurfaceArea

```cpp
inline virtual double GetSurfaceArea() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetSurfaceArea](../Classes/classshapeworks_1_1ParticleDomain.md#function-getsurfacearea)


Use for neighborhood radius. 


### function GetMaxDiameter

```cpp
virtual double GetMaxDiameter() const override
```


**Reimplements**: [shapeworks::ParticleDomain::GetMaxDiameter](../Classes/classshapeworks_1_1ParticleDomain.md#function-getmaxdiameter)


GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma 


### function SampleGradientAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleGradientAtPoint(
    const PointType & point,
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::SampleGradientAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradientatpoint)


### function SampleNormalAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    const PointType & point,
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::SampleNormalAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplenormalatpoint)


### function SampleGradNAtPoint

```cpp
inline virtual GradNType SampleGradNAtPoint(
    const PointType & p,
    int idx
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::SampleGradNAtPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-samplegradnatpoint)


### function Distance

```cpp
inline double Distance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    vnl_vector_fixed< double, DIMENSION > * out_grad =nullptr
) const override
```


### function SquaredDistance

```cpp
inline virtual double SquaredDistance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::SquaredDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-squareddistance)


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
) const override
```


**Reimplements**: [shapeworks::ParticleDomain::IsWithinDistance](../Classes/classshapeworks_1_1ParticleDomain.md#function-iswithindistance)


Returns whether or not the two points are separated by the given distance 


### function DeleteImages

```cpp
inline virtual void DeleteImages() override
```


**Reimplements**: [shapeworks::ParticleDomain::DeleteImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deleteimages)


### function DeletePartialDerivativeImages

```cpp
inline virtual void DeletePartialDerivativeImages() override
```


**Reimplements**: [shapeworks::ParticleDomain::DeletePartialDerivativeImages](../Classes/classshapeworks_1_1ParticleDomain.md#function-deletepartialderivativeimages)


### function SetMesh

```cpp
inline void SetMesh(
    std::shared_ptr< shapeworks::MeshWrapper > mesh_
)
```


### function UpdateZeroCrossingPoint

```cpp
inline virtual void UpdateZeroCrossingPoint() override
```


**Reimplements**: [shapeworks::ParticleDomain::UpdateZeroCrossingPoint](../Classes/classshapeworks_1_1ParticleDomain.md#function-updatezerocrossingpoint)


-------------------------------

Updated on 2023-05-11 at 06:01:11 +0000