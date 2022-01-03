---
title: shapeworks::VtkMeshWrapper

---

# shapeworks::VtkMeshWrapper





Inherits from [shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using MeshWrapper::PointType | **[PointType](../Classes/classshapeworks_1_1VtkMeshWrapper.md#using-pointtype)**  |
| using MeshWrapper::GradNType | **[GradNType](../Classes/classshapeworks_1_1VtkMeshWrapper.md#using-gradntype)**  |
| using vnl_vector_fixed< float, DIMENSION > | **[NormalType](../Classes/classshapeworks_1_1VtkMeshWrapper.md#using-normaltype)**  |
| using vnl_vector_fixed< double, DIMENSION > | **[VectorType](../Classes/classshapeworks_1_1VtkMeshWrapper.md#using-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[VtkMeshWrapper](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-vtkmeshwrapper)**(vtkSmartPointer< vtkPolyData > mesh, bool geodesics_enabled =false, size_t geodesics_cache_multiplier_size =0) |
| | **[~VtkMeshWrapper](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-~vtkmeshwrapper)**() =default |
| double | **[ComputeDistance](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-computedistance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, VectorType * out_grad =nullptr) const override |
| virtual bool | **[IsWithinDistance](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-iswithindistance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, double test_dist, double & dist) const override |
| virtual PointType | **[GeodesicWalk](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-geodesicwalk)**(PointType p, int idx, VectorType vector) const override |
| virtual VectorType | **[ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-projectvectortosurfacetangent)**(const PointType & pointa, int idx, VectorType & vector) const override |
| virtual NormalType | **[SampleNormalAtPoint](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-samplenormalatpoint)**(PointType p, int idx) const override |
| virtual GradNType | **[SampleGradNAtPoint](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-samplegradnatpoint)**(PointType p, int idx) const override |
| virtual PointType | **[SnapToMesh](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-snaptomesh)**(PointType pointa, int idx) const override |
| virtual PointType | **[GetPointOnMesh](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getpointonmesh)**() const override |
| virtual const PointType & | **[GetMeshLowerBound](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getmeshlowerbound)**() const override |
| virtual const PointType & | **[GetMeshUpperBound](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-getmeshupperbound)**() const override |
| virtual void | **[InvalidateParticle](../Classes/classshapeworks_1_1VtkMeshWrapper.md#function-invalidateparticle)**(int idx) override |

## Public Types Documentation

### using PointType

```cpp
using shapeworks::VtkMeshWrapper::PointType =  MeshWrapper::PointType;
```


### using GradNType

```cpp
using shapeworks::VtkMeshWrapper::GradNType =  MeshWrapper::GradNType;
```


### using NormalType

```cpp
using shapeworks::VtkMeshWrapper::NormalType =  vnl_vector_fixed<float, DIMENSION>;
```


### using VectorType

```cpp
using shapeworks::VtkMeshWrapper::VectorType =  vnl_vector_fixed<double, DIMENSION>;
```


## Public Functions Documentation

### function VtkMeshWrapper

```cpp
explicit VtkMeshWrapper(
    vtkSmartPointer< vtkPolyData > mesh,
    bool geodesics_enabled =false,
    size_t geodesics_cache_multiplier_size =0
)
```


### function ~VtkMeshWrapper

```cpp
~VtkMeshWrapper() =default
```


### function ComputeDistance

```cpp
double ComputeDistance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    VectorType * out_grad =nullptr
) const override
```


### function IsWithinDistance

```cpp
virtual bool IsWithinDistance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    double test_dist,
    double & dist
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::IsWithinDistance](../Classes/classshapeworks_1_1MeshWrapper.md#function-iswithindistance)


### function GeodesicWalk

```cpp
virtual PointType GeodesicWalk(
    PointType p,
    int idx,
    VectorType vector
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::GeodesicWalk](../Classes/classshapeworks_1_1MeshWrapper.md#function-geodesicwalk)


### function ProjectVectorToSurfaceTangent

```cpp
virtual VectorType ProjectVectorToSurfaceTangent(
    const PointType & pointa,
    int idx,
    VectorType & vector
) const override
```


**Reimplements**: [shapeworks::MeshWrapper::ProjectVectorToSurfaceTangent](../Classes/classshapeworks_1_1MeshWrapper.md#function-projectvectortosurfacetangent)


### function SampleNormalAtPoint

```cpp
virtual NormalType SampleNormalAtPoint(
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


### function InvalidateParticle

```cpp
virtual void InvalidateParticle(
    int idx
) override
```


**Reimplements**: [shapeworks::MeshWrapper::InvalidateParticle](../Classes/classshapeworks_1_1MeshWrapper.md#function-invalidateparticle)


-------------------------------

Updated on 2022-01-03 at 16:16:09 +0000