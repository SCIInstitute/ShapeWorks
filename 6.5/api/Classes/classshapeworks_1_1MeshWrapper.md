---
title: shapeworks::MeshWrapper

---

# shapeworks::MeshWrapper





Inherited by [shapeworks::VtkMeshWrapper](../Classes/classshapeworks_1_1VtkMeshWrapper.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleDomain::PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[PointType](../Classes/classshapeworks_1_1MeshWrapper.md#typedef-pointtype)**  |
| typedef ParticleDomain::GradNType | **[GradNType](../Classes/classshapeworks_1_1MeshWrapper.md#typedef-gradntype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| virtual double | **[ComputeDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-computedistance)**(const PointType & pointa, int idx_a, const PointType & pointb, int idx_b, vnl_vector_fixed< double, 3 > * out_grad =nullptr) const =0 |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-iswithindistance)**(const PointType & a, int idx_a, const PointType & b, int idx_b, double test_dist, double & dist) const =0 |
| virtual PointType | **[GeodesicWalk](../Classes/classshapeworks_1_1MeshWrapper.md#function-geodesicwalk)**(PointType pointa, int idx, vnl_vector_fixed< double, DIMENSION > vector) const =0 |
| virtual PointType | **[GetPointOnMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-getpointonmesh)**() const =0 |
| virtual const PointType & | **[GetMeshLowerBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshlowerbound)**() const =0 |
| virtual const PointType & | **[GetMeshUpperBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshupperbound)**() const =0 |
| virtual vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshWrapper.md#function-projectvectortosurfacetangent)**(const PointType & pointa, int idx, vnl_vector_fixed< double, DIMENSION > & vector) const =0 |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplenormalatpoint)**(PointType p, int idx) const =0 |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplegradnatpoint)**(PointType p, int idx) const =0 |
| virtual PointType | **[SnapToMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-snaptomesh)**(PointType pointa, int idx) const =0 |
| virtual void | **[InvalidateParticle](../Classes/classshapeworks_1_1MeshWrapper.md#function-invalidateparticle)**(int idx) |
| virtual bool | **[IsGeodesicsEnabled](../Classes/classshapeworks_1_1MeshWrapper.md#function-isgeodesicsenabled)**() const =0 |
| virtual vtkSmartPointer< vtkPolyData > | **[GetPolydata](../Classes/classshapeworks_1_1MeshWrapper.md#function-getpolydata)**() const =0 |

## Public Types Documentation

### typedef PointType

```cpp
typedef ParticleDomain::PointType shapeworks::MeshWrapper::PointType;
```


### typedef GradNType

```cpp
typedef ParticleDomain::GradNType shapeworks::MeshWrapper::GradNType;
```


## Public Functions Documentation

### function ComputeDistance

```cpp
virtual double ComputeDistance(
    const PointType & pointa,
    int idx_a,
    const PointType & pointb,
    int idx_b,
    vnl_vector_fixed< double, 3 > * out_grad =nullptr
) const =0
```


### function IsWithinDistance

```cpp
virtual bool IsWithinDistance(
    const PointType & a,
    int idx_a,
    const PointType & b,
    int idx_b,
    double test_dist,
    double & dist
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::IsWithinDistance](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-iswithindistance)


### function GeodesicWalk

```cpp
virtual PointType GeodesicWalk(
    PointType pointa,
    int idx,
    vnl_vector_fixed< double, DIMENSION > vector
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::GeodesicWalk](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-geodesicwalk)


### function GetPointOnMesh

```cpp
virtual PointType GetPointOnMesh() const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::GetPointOnMesh](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getpointonmesh)


### function GetMeshLowerBound

```cpp
virtual const PointType & GetMeshLowerBound() const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::GetMeshLowerBound](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getmeshlowerbound)


### function GetMeshUpperBound

```cpp
virtual const PointType & GetMeshUpperBound() const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::GetMeshUpperBound](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getmeshupperbound)


### function ProjectVectorToSurfaceTangent

```cpp
virtual vnl_vector_fixed< double, DIMENSION > ProjectVectorToSurfaceTangent(
    const PointType & pointa,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & vector
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-projectvectortosurfacetangent)


### function SampleNormalAtPoint

```cpp
virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    PointType p,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::SampleNormalAtPoint](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-samplenormalatpoint)


### function SampleGradNAtPoint

```cpp
virtual GradNType SampleGradNAtPoint(
    PointType p,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::SampleGradNAtPoint](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-samplegradnatpoint)


### function SnapToMesh

```cpp
virtual PointType SnapToMesh(
    PointType pointa,
    int idx
) const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::SnapToMesh](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-snaptomesh)


### function InvalidateParticle

```cpp
inline virtual void InvalidateParticle(
    int idx
)
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::InvalidateParticle](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-invalidateparticle)


### function IsGeodesicsEnabled

```cpp
virtual bool IsGeodesicsEnabled() const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::IsGeodesicsEnabled](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-isgeodesicsenabled)


### function GetPolydata

```cpp
virtual vtkSmartPointer< vtkPolyData > GetPolydata() const =0
```


**Reimplemented by**: [shapeworks::VtkMeshWrapper::GetPolydata](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getpolydata)


-------------------------------

Updated on 2023-05-09 at 19:46:35 +0000