---
title: itk::ContourDomain

---

# itk::ContourDomain





Inherits from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md), DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef SmartPointer< [ContourDomain](../Classes/classitk_1_1ContourDomain.md) > | **[Pointer](../Classes/classitk_1_1ContourDomain.md#typedef-pointer)**  |
| typedef [ParticleDomain::PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkSimpleNewMacro](../Classes/classitk_1_1ContourDomain.md#function-itksimplenewmacro)**([ContourDomain](../Classes/classitk_1_1ContourDomain.md) ) |
| | **[ContourDomain](../Classes/classitk_1_1ContourDomain.md#function-contourdomain)**() |
| virtual | **[~ContourDomain](../Classes/classitk_1_1ContourDomain.md#function-~contourdomain)**() |
| void | **[SetPolyLine](../Classes/classitk_1_1ContourDomain.md#function-setpolyline)**(vtkSmartPointer< vtkPolyData > poly_data) |
| virtual shapeworks::DomainType | **[GetDomainType](../Classes/classitk_1_1ContourDomain.md#function-getdomaintype)**() const override |
| virtual bool | **[ApplyConstraints](../Classes/classitk_1_1ContourDomain.md#function-applyconstraints)**([PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & p, int idx, bool dbg =false) const override |
| virtual [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) | **[UpdateParticlePosition](../Classes/classitk_1_1ContourDomain.md#function-updateparticleposition)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & point, int idx, vnl_vector_fixed< double, DIMENSION > & update) const override |
| virtual vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ContourDomain.md#function-projectvectortosurfacetangent)**(vnl_vector_fixed< double, DIMENSION > & gradE, const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & pos, int idx) const override |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplenormalatpoint)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & point, int idx) const override |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleGradientAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplegradientatpoint)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & point, int idx) const override |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classitk_1_1ContourDomain.md#function-samplegradnatpoint)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & p, int idx) const override |
| virtual [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) | **[GetValidLocationNear](../Classes/classitk_1_1ContourDomain.md#function-getvalidlocationnear)**([PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) p) const override |
| virtual double | **[GetMaxDiameter](../Classes/classitk_1_1ContourDomain.md#function-getmaxdiameter)**() const override |
| virtual void | **[UpdateZeroCrossingPoint](../Classes/classitk_1_1ContourDomain.md#function-updatezerocrossingpoint)**() override |
| virtual double | **[GetCurvature](../Classes/classitk_1_1ContourDomain.md#function-getcurvature)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & p, int idx) const override |
| virtual double | **[GetSurfaceMeanCurvature](../Classes/classitk_1_1ContourDomain.md#function-getsurfacemeancurvature)**() const override |
| virtual double | **[GetSurfaceStdDevCurvature](../Classes/classitk_1_1ContourDomain.md#function-getsurfacestddevcurvature)**() const override |
| double | **[Distance](../Classes/classitk_1_1ContourDomain.md#function-distance)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & b, int idx_b, vnl_vector_fixed< double, 3 > * out_grad =nullptr) const override |
| virtual double | **[SquaredDistance](../Classes/classitk_1_1ContourDomain.md#function-squareddistance)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & b, int idx_b) const override |
| virtual const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & | **[GetLowerBound](../Classes/classitk_1_1ContourDomain.md#function-getlowerbound)**() const override |
| virtual const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & | **[GetUpperBound](../Classes/classitk_1_1ContourDomain.md#function-getupperbound)**() const override |
| virtual [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) | **[GetZeroCrossingPoint](../Classes/classitk_1_1ContourDomain.md#function-getzerocrossingpoint)**() const override |
| virtual double | **[GetSurfaceArea](../Classes/classitk_1_1ContourDomain.md#function-getsurfacearea)**() const override |
| virtual void | **[DeleteImages](../Classes/classitk_1_1ContourDomain.md#function-deleteimages)**() override |
| virtual void | **[DeletePartialDerivativeImages](../Classes/classitk_1_1ContourDomain.md#function-deletepartialderivativeimages)**() override |
| virtual void | **[InvalidateParticlePosition](../Classes/classitk_1_1ContourDomain.md#function-invalidateparticleposition)**(int idx) const override |
| virtual [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) | **[GetPositionAfterSplit](../Classes/classitk_1_1ContourDomain.md#function-getpositionaftersplit)**(const [PointType](../Classes/classitk_1_1ContourDomain.md#typedef-pointtype) & pt, const vnl_vector_fixed< double, 3 > & random, double epsilon) const override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[PrintSelf](../Classes/classitk_1_1ContourDomain.md#function-printself)**(std::ostream & os, Indent indent) const override |

## Additional inherited members

**Public Types inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| typedef vnl_matrix_fixed< float, DIMENSION, DIMENSION > | **[GradNType](../Classes/classitk_1_1ParticleDomain.md#typedef-gradntype)**  |

**Public Functions inherited from [itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[IsWithinDistance](../Classes/classitk_1_1ParticleDomain.md#function-iswithindistance)**(const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & a, int idx_a, const [PointType](../Classes/classitk_1_1ParticleDomain.md#typedef-pointtype) & b, int idx_b, double test_dist, double & distance) const |
| bool | **[IsDomainFixed](../Classes/classitk_1_1ParticleDomain.md#function-isdomainfixed)**() const |
| std::shared_ptr< [Constraints](../Classes/classitk_1_1Constraints.md) > | **[GetConstraints](../Classes/classitk_1_1ParticleDomain.md#function-getconstraints)**() const |
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


## Public Types Documentation

### typedef Pointer

```cpp
typedef SmartPointer<ContourDomain> itk::ContourDomain::Pointer;
```


Standard class typedefs 


### typedef PointType

```cpp
typedef ParticleDomain::PointType itk::ContourDomain::PointType;
```


Point type used to store particle locations. 


## Public Functions Documentation

### function itkSimpleNewMacro

```cpp
itkSimpleNewMacro(
    ContourDomain 
)
```


### function ContourDomain

```cpp
inline explicit ContourDomain()
```


### function ~ContourDomain

```cpp
inline virtual ~ContourDomain()
```


### function SetPolyLine

```cpp
void SetPolyLine(
    vtkSmartPointer< vtkPolyData > poly_data
)
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


Apply any constraints to the given point location. This should force the point to a position on the surface that satisfies all constraints. 


### function UpdateParticlePosition

```cpp
virtual PointType UpdateParticlePosition(
    const PointType & point,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & update
) const override
```


**Reimplements**: [itk::ParticleDomain::UpdateParticlePosition](../Classes/classitk_1_1ParticleDomain.md#function-updateparticleposition)


Applies the update to the point and returns the new point position. 


### function ProjectVectorToSurfaceTangent

```cpp
virtual vnl_vector_fixed< double, DIMENSION > ProjectVectorToSurfaceTangent(
    vnl_vector_fixed< double, DIMENSION > & gradE,
    const PointType & pos,
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::ProjectVectorToSurfaceTangent](../Classes/classitk_1_1ParticleDomain.md#function-projectvectortosurfacetangent)


Projects the vector to the surface tangent at the point. 


### function SampleNormalAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    const PointType & point,
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::SampleNormalAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplenormalatpoint)


### function SampleGradientAtPoint

```cpp
inline virtual vnl_vector_fixed< float, DIMENSION > SampleGradientAtPoint(
    const PointType & point,
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::SampleGradientAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplegradientatpoint)


### function SampleGradNAtPoint

```cpp
inline virtual GradNType SampleGradNAtPoint(
    const PointType & p,
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::SampleGradNAtPoint](../Classes/classitk_1_1ParticleDomain.md#function-samplegradnatpoint)


### function GetValidLocationNear

```cpp
inline virtual PointType GetValidLocationNear(
    PointType p
) const override
```


**Reimplements**: [itk::ParticleDomain::GetValidLocationNear](../Classes/classitk_1_1ParticleDomain.md#function-getvalidlocationnear)


GetValidLocation returns a PointType location on the surface. Used for placing the first particle. 


### function GetMaxDiameter

```cpp
inline virtual double GetMaxDiameter() const override
```


**Reimplements**: [itk::ParticleDomain::GetMaxDiameter](../Classes/classitk_1_1ParticleDomain.md#function-getmaxdiameter)


GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma 


### function UpdateZeroCrossingPoint

```cpp
inline virtual void UpdateZeroCrossingPoint() override
```


**Reimplements**: [itk::ParticleDomain::UpdateZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-updatezerocrossingpoint)


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


### function Distance

```cpp
double Distance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    vnl_vector_fixed< double, 3 > * out_grad =nullptr
) const override
```


### function SquaredDistance

```cpp
virtual double SquaredDistance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b
) const override
```


**Reimplements**: [itk::ParticleDomain::SquaredDistance](../Classes/classitk_1_1ParticleDomain.md#function-squareddistance)


Squared Distance between locations is used for computing sigma. 


### function GetLowerBound

```cpp
inline virtual const PointType & GetLowerBound() const override
```


**Reimplements**: [itk::ParticleDomain::GetLowerBound](../Classes/classitk_1_1ParticleDomain.md#function-getlowerbound)


Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md). 


### function GetUpperBound

```cpp
inline virtual const PointType & GetUpperBound() const override
```


**Reimplements**: [itk::ParticleDomain::GetUpperBound](../Classes/classitk_1_1ParticleDomain.md#function-getupperbound)


Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the [PowerOfTwoPointTree](../Classes/classitk_1_1PowerOfTwoPointTree.md). 


### function GetZeroCrossingPoint

```cpp
inline virtual PointType GetZeroCrossingPoint() const override
```


**Reimplements**: [itk::ParticleDomain::GetZeroCrossingPoint](../Classes/classitk_1_1ParticleDomain.md#function-getzerocrossingpoint)


Get any valid point on the domain. This is used to place the first particle. 


### function GetSurfaceArea

```cpp
inline virtual double GetSurfaceArea() const override
```


**Reimplements**: [itk::ParticleDomain::GetSurfaceArea](../Classes/classitk_1_1ParticleDomain.md#function-getsurfacearea)


Use for neighborhood radius. 


### function DeleteImages

```cpp
inline virtual void DeleteImages() override
```


**Reimplements**: [itk::ParticleDomain::DeleteImages](../Classes/classitk_1_1ParticleDomain.md#function-deleteimages)


### function DeletePartialDerivativeImages

```cpp
inline virtual void DeletePartialDerivativeImages() override
```


**Reimplements**: [itk::ParticleDomain::DeletePartialDerivativeImages](../Classes/classitk_1_1ParticleDomain.md#function-deletepartialderivativeimages)


### function InvalidateParticlePosition

```cpp
virtual void InvalidateParticlePosition(
    int idx
) const override
```


**Reimplements**: [itk::ParticleDomain::InvalidateParticlePosition](../Classes/classitk_1_1ParticleDomain.md#function-invalidateparticleposition)


### function GetPositionAfterSplit

```cpp
virtual PointType GetPositionAfterSplit(
    const PointType & pt,
    const vnl_vector_fixed< double, 3 > & random,
    double epsilon
) const override
```


**Reimplements**: [itk::ParticleDomain::GetPositionAfterSplit](../Classes/classitk_1_1ParticleDomain.md#function-getpositionaftersplit)


## Protected Functions Documentation

### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const override
```


-------------------------------

Updated on 2021-11-14 at 17:52:09 +0000