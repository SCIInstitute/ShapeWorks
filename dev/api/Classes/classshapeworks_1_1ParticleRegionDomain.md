---
title: shapeworks::ParticleRegionDomain

---

# shapeworks::ParticleRegionDomain





Inherits from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)

Inherited by [shapeworks::ImageDomain< T >](../Classes/classshapeworks_1_1ImageDomain.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleRegionDomain.md#using-pointer)**  |
| typedef [ParticleDomain::PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleRegionDomain.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[ApplyConstraints](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-applyconstraints)**([PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetUpperBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getupperbound)**() const |
| virtual const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & | **[GetLowerBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-getlowerbound)**() const |
| void | **[SetUpperBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setupperbound)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) _UpperBound) |
| void | **[SetLowerBound](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setlowerbound)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) _LowerBound) |
| void | **[SetRegion](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-setregion)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & lowerBound, const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & upperBound) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-particleregiondomain)**() |
| virtual | **[~ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-~particleregiondomain)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| bool | **[IsInsideBuffer](../Classes/classshapeworks_1_1ParticleRegionDomain.md#function-isinsidebuffer)**(const [PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) & p) const |

## Additional inherited members

**Public Types inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| using vnl_matrix_fixed< float, 3, 3 > | **[GradNType](../Classes/classshapeworks_1_1ParticleDomain.md#using-gradntype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorDoubleType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectordoubletype)**  |
| using vnl_vector_fixed< float, 3 > | **[VectorFloatType](../Classes/classshapeworks_1_1ParticleDomain.md#using-vectorfloattype)**  |

**Public Functions inherited from [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
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


## Public Types Documentation

### using Pointer

```cpp
using shapeworks::ParticleRegionDomain::Pointer =  std::shared_ptr<ParticleRegionDomain>;
```


Standard class typedefs 


### typedef PointType

```cpp
typedef ParticleDomain::PointType shapeworks::ParticleRegionDomain::PointType;
```


Point type used to store particle locations. 


## Public Functions Documentation

### function ApplyConstraints

```cpp
inline virtual bool ApplyConstraints(
    PointType & p
) const
```


Apply any constraints to the given point location. This method may, for example, implement boundary conditions or restrict points to lie on a surface. This function will clip the point to the boundaries. 


### function GetUpperBound

```cpp
inline virtual const PointType & GetUpperBound() const
```


**Reimplements**: [shapeworks::ParticleDomain::GetUpperBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getupperbound)


Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function GetLowerBound

```cpp
inline virtual const PointType & GetLowerBound() const
```


**Reimplements**: [shapeworks::ParticleDomain::GetLowerBound](../Classes/classshapeworks_1_1ParticleDomain.md#function-getlowerbound)


Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md). 


### function SetUpperBound

```cpp
inline void SetUpperBound(
    const PointType _UpperBound
)
```


### function SetLowerBound

```cpp
inline void SetLowerBound(
    const PointType _LowerBound
)
```


### function SetRegion

```cpp
inline void SetRegion(
    const PointType & lowerBound,
    const PointType & upperBound
)
```


## Protected Functions Documentation

### function ParticleRegionDomain

```cpp
inline ParticleRegionDomain()
```


### function ~ParticleRegionDomain

```cpp
inline virtual ~ParticleRegionDomain()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function IsInsideBuffer

```cpp
inline bool IsInsideBuffer(
    const PointType & p
) const
```


Check whether the point p may be sampled in this domain. 


-------------------------------

Updated on 2024-06-20 at 06:09:45 +0000