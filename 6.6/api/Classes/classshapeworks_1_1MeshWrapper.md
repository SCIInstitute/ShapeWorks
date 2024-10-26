---
title: shapeworks::MeshWrapper

---

# shapeworks::MeshWrapper





## Public Types

|                | Name           |
| -------------- | -------------- |
| using [ParticleDomain::PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[PointType](../Classes/classshapeworks_1_1MeshWrapper.md#using-pointtype)**  |
| using ParticleDomain::GradNType | **[GradNType](../Classes/classshapeworks_1_1MeshWrapper.md#using-gradntype)**  |
| using vnl_vector_fixed< float, 3 > | **[NormalType](../Classes/classshapeworks_1_1MeshWrapper.md#using-normaltype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorType](../Classes/classshapeworks_1_1MeshWrapper.md#using-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md#function-meshwrapper)**(vtkSmartPointer< vtkPolyData > mesh, bool geodesics_enabled =false, size_t geodesics_cache_multiplier_size =0) |
| | **[~MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md#function-~meshwrapper)**() =default |
| double | **[ComputeDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-computedistance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, VectorType * out_grad =nullptr) const |
| bool | **[IsWithinDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-iswithindistance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, double test_dist, double & dist) const |
| PointType | **[GeodesicWalk](../Classes/classshapeworks_1_1MeshWrapper.md#function-geodesicwalk)**(PointType p, int idx, VectorType vector) const |
| VectorType | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshWrapper.md#function-projectvectortosurfacetangent)**(const PointType & pointa, int idx, VectorType & vector) const |
| NormalType | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplenormalatpoint)**(PointType p, int idx =-1) const |
| GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1MeshWrapper.md#function-samplegradnatpoint)**(PointType p, int idx) const |
| PointType | **[SnapToMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-snaptomesh)**(PointType pointa, int idx) const |
| PointType | **[GetPointOnMesh](../Classes/classshapeworks_1_1MeshWrapper.md#function-getpointonmesh)**() const |
| const PointType & | **[GetMeshLowerBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshlowerbound)**() const |
| const PointType & | **[GetMeshUpperBound](../Classes/classshapeworks_1_1MeshWrapper.md#function-getmeshupperbound)**() const |
| virtual void | **[InvalidateParticle](../Classes/classshapeworks_1_1MeshWrapper.md#function-invalidateparticle)**(int idx) |
| vtkSmartPointer< vtkPolyData > | **[GetPolydata](../Classes/classshapeworks_1_1MeshWrapper.md#function-getpolydata)**() const |
| bool | **[IsGeodesicsEnabled](../Classes/classshapeworks_1_1MeshWrapper.md#function-isgeodesicsenabled)**() const |

## Public Types Documentation

### using PointType

```cpp
using shapeworks::MeshWrapper::PointType =  ParticleDomain::PointType;
```


### using GradNType

```cpp
using shapeworks::MeshWrapper::GradNType =  ParticleDomain::GradNType;
```


### using NormalType

```cpp
using shapeworks::MeshWrapper::NormalType =  vnl_vector_fixed<float, 3>;
```


### using VectorType

```cpp
using shapeworks::MeshWrapper::VectorType =  vnl_vector_fixed<double, 3>;
```


## Public Functions Documentation

### function MeshWrapper

```cpp
explicit MeshWrapper(
    vtkSmartPointer< vtkPolyData > mesh,
    bool geodesics_enabled =false,
    size_t geodesics_cache_multiplier_size =0
)
```


### function ~MeshWrapper

```cpp
~MeshWrapper() =default
```


### function ComputeDistance

```cpp
double ComputeDistance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    VectorType * out_grad =nullptr
) const
```


### function IsWithinDistance

```cpp
bool IsWithinDistance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    double test_dist,
    double & dist
) const
```


### function GeodesicWalk

```cpp
PointType GeodesicWalk(
    PointType p,
    int idx,
    VectorType vector
) const
```


### function ProjectVectorToSurfaceTangent

```cpp
VectorType ProjectVectorToSurfaceTangent(
    const PointType & pointa,
    int idx,
    VectorType & vector
) const
```


### function SampleNormalAtPoint

```cpp
NormalType SampleNormalAtPoint(
    PointType p,
    int idx =-1
) const
```


### function SampleGradNAtPoint

```cpp
GradNType SampleGradNAtPoint(
    PointType p,
    int idx
) const
```


### function SnapToMesh

```cpp
PointType SnapToMesh(
    PointType pointa,
    int idx
) const
```


### function GetPointOnMesh

```cpp
PointType GetPointOnMesh() const
```


### function GetMeshLowerBound

```cpp
inline const PointType & GetMeshLowerBound() const
```


### function GetMeshUpperBound

```cpp
inline const PointType & GetMeshUpperBound() const
```


### function InvalidateParticle

```cpp
virtual void InvalidateParticle(
    int idx
)
```


### function GetPolydata

```cpp
inline vtkSmartPointer< vtkPolyData > GetPolydata() const
```


### function IsGeodesicsEnabled

```cpp
inline bool IsGeodesicsEnabled() const
```


-------------------------------

Updated on 2024-10-26 at 19:00:30 +0000