---
title: shapeworks::TriMeshWrapper

---

# shapeworks::TriMeshWrapper





Inherits from [shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef MeshWrapper::PointType | **[PointType](../Classes/classshapeworks_1_1TriMeshWrapper.md#typedef-pointtype)**  |
| typedef MeshWrapper::GradNType | **[GradNType](../Classes/classshapeworks_1_1TriMeshWrapper.md#typedef-gradntype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[TriMeshWrapper](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-trimeshwrapper)**(std::shared_ptr< trimesh::TriMesh > mesh) |
| | **[~TriMeshWrapper](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-~trimeshwrapper)**() =default |
| virtual double | **[ComputeDistance](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-computedistance)**(const PointType & pointa, int idx_a, const PointType & pointb, int idx_b, vnl_vector_fixed< double, 3 > * out_grad) const override |
| virtual PointType | **[GeodesicWalk](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-geodesicwalk)**(PointType pointa, int idx, vnl_vector_fixed< double, DIMENSION > vector) const override |
| virtual vnl_vector_fixed< double, DIMENSION > | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-projectvectortosurfacetangent)**(const PointType & pointa, int idx, vnl_vector_fixed< double, DIMENSION > & vector) const override |
| virtual vnl_vector_fixed< float, DIMENSION > | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-samplenormalatpoint)**(PointType p, int idx) const override |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-samplegradnatpoint)**(PointType p, int idx) const override |
| virtual PointType | **[SnapToMesh](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-snaptomesh)**(PointType pointa, int idx) const override |
| virtual PointType | **[GetPointOnMesh](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-getpointonmesh)**() const override |
| virtual const PointType & | **[GetMeshLowerBound](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-getmeshlowerbound)**() const override |
| virtual const PointType & | **[GetMeshUpperBound](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-getmeshupperbound)**() const override |
| virtual bool | **[IsGeodesicsEnabled](../Classes/classshapeworks_1_1TriMeshWrapper.md#function-isgeodesicsenabled)**() const override |

## Additional inherited members

**Public Functions inherited from [shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md)**

|                | Name           |
| -------------- | -------------- |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-iswithindistance)**(const PointType & a, int idx_a, const PointType & b, int idx_b, double test_dist, double & dist) const =0 |
| virtual void | **[InvalidateParticle](../Classes/classshapeworks_1_1MeshWrapper.md#function-invalidateparticle)**(int idx) |


## Public Types Documentation

### typedef PointType

```cpp
typedef MeshWrapper::PointType shapeworks::TriMeshWrapper::PointType;
```


### typedef GradNType

```cpp
typedef MeshWrapper::GradNType shapeworks::TriMeshWrapper::GradNType;
```


## Public Functions Documentation

### function TriMeshWrapper

```cpp
explicit TriMeshWrapper(
    std::shared_ptr< trimesh::TriMesh > mesh
)
```


### function ~TriMeshWrapper

```cpp
~TriMeshWrapper() =default
```


### function ComputeDistance

```cpp
virtual double ComputeDistance(
    const PointType & pointa,
    int idx_a,
    const PointType & pointb,
    int idx_b,
    vnl_vector_fixed< double, 3 > * out_grad
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::ComputeDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-computedistance)


### function GeodesicWalk

```cpp
virtual PointType GeodesicWalk(
    PointType pointa,
    int idx,
    vnl_vector_fixed< double, DIMENSION > vector
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::GeodesicWalk](../Classes/classshapeworks_1_1MeshWrapper.md#function-geodesicwalk)


### function ProjectVectorToSurfaceTangent

```cpp
virtual vnl_vector_fixed< double, DIMENSION > ProjectVectorToSurfaceTangent(
    const PointType & pointa,
    int idx,
    vnl_vector_fixed< double, DIMENSION > & vector
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshWrapper.md#function-projectvectortosurfacetangent)


### function SampleNormalAtPoint

```cpp
virtual vnl_vector_fixed< float, DIMENSION > SampleNormalAtPoint(
    PointType p,
    int idx
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::SampleNormalAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplenormalatpoint)


### function SampleGradNAtPoint

```cpp
virtual GradNType SampleGradNAtPoint(
    PointType p,
    int idx
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::SampleGradNAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplegradnatpoint)


### function SnapToMesh

```cpp
virtual PointType SnapToMesh(
    PointType pointa,
    int idx
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::SnapToMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-snaptomesh)


### function GetPointOnMesh

```cpp
virtual PointType GetPointOnMesh() const override
```


**Reimplements**: [shapeworks::MeshWrapper::GetPointOnMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-getpointonmesh)


### function GetMeshLowerBound

```cpp
inline virtual const PointType & GetMeshLowerBound() const override
```


**Reimplements**: [shapeworks::MeshWrapper::GetMeshLowerBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshlowerbound)


### function GetMeshUpperBound

```cpp
inline virtual const PointType & GetMeshUpperBound() const override
```


**Reimplements**: [shapeworks::MeshWrapper::GetMeshUpperBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshupperbound)


### function IsGeodesicsEnabled

```cpp
inline virtual bool IsGeodesicsEnabled() const override
```


**Reimplements**: [shapeworks::MeshWrapper::IsGeodesicsEnabled](../Classes/classshapeworks_1_1MeshWrapper.md#function-isgeodesicsenabled)


-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000