---
title: shapeworks::Surface

---

# shapeworks::Surface





## Public Types

|                | Name           |
| -------------- | -------------- |
| using [ParticleDomain::PointType](../Classes/classshapeworks_1_1ParticleDomain.md#using-pointtype) | **[PointType](../Classes/classshapeworks_1_1Surface.md#using-pointtype)**  |
| using ParticleDomain::GradNType | **[GradNType](../Classes/classshapeworks_1_1Surface.md#using-gradntype)**  |
| using vnl_vector_fixed< float, 3 > | **[NormalType](../Classes/classshapeworks_1_1Surface.md#using-normaltype)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorType](../Classes/classshapeworks_1_1Surface.md#using-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Surface](../Classes/classshapeworks_1_1Surface.md#function-surface)**(vtkSmartPointer< vtkPolyData > mesh, bool geodesics_enabled =false, size_t geodesics_cache_multiplier_size =0) |
| | **[~Surface](../Classes/classshapeworks_1_1Surface.md#function-~surface)**() =default |
| double | **[compute_distance](../Classes/classshapeworks_1_1Surface.md#function-compute-distance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, VectorType * out_grad =nullptr) const |
| bool | **[is_within_distance](../Classes/classshapeworks_1_1Surface.md#function-is-within-distance)**(const PointType & pointa, int idxa, const PointType & pointb, int idxb, double test_dist, double & dist) const |
| PointType | **[geodesic_walk](../Classes/classshapeworks_1_1Surface.md#function-geodesic-walk)**(PointType p, int idx, VectorType vector) const |
| VectorType | **[project_vector_to_surface_tangent](../Classes/classshapeworks_1_1Surface.md#function-project-vector-to-surface-tangent)**(const PointType & pointa, int idx, VectorType & vector) const |
| NormalType | **[sample_normal_at_point](../Classes/classshapeworks_1_1Surface.md#function-sample-normal-at-point)**(PointType p, int idx =-1) const |
| GradNType | **[sample_gradient_normal_at_point](../Classes/classshapeworks_1_1Surface.md#function-sample-gradient-normal-at-point)**(PointType p, int idx) const |
| PointType | **[snap_to_mesh](../Classes/classshapeworks_1_1Surface.md#function-snap-to-mesh)**(PointType pointa, int idx) const |
| PointType | **[get_point_on_mesh](../Classes/classshapeworks_1_1Surface.md#function-get-point-on-mesh)**() const |
| const PointType & | **[get_mesh_lower_bound](../Classes/classshapeworks_1_1Surface.md#function-get-mesh-lower-bound)**() const |
| const PointType & | **[get_mesh_upper_bound](../Classes/classshapeworks_1_1Surface.md#function-get-mesh-upper-bound)**() const |
| virtual void | **[invalidate_particle](../Classes/classshapeworks_1_1Surface.md#function-invalidate-particle)**(int idx) |
| vtkSmartPointer< vtkPolyData > | **[get_polydata](../Classes/classshapeworks_1_1Surface.md#function-get-polydata)**() const |
| bool | **[is_geodesics_enabled](../Classes/classshapeworks_1_1Surface.md#function-is-geodesics-enabled)**() const |

## Public Types Documentation

### using PointType

```cpp
using shapeworks::Surface::PointType =  ParticleDomain::PointType;
```


### using GradNType

```cpp
using shapeworks::Surface::GradNType =  ParticleDomain::GradNType;
```


### using NormalType

```cpp
using shapeworks::Surface::NormalType =  vnl_vector_fixed<float, 3>;
```


### using VectorType

```cpp
using shapeworks::Surface::VectorType =  vnl_vector_fixed<double, 3>;
```


## Public Functions Documentation

### function Surface

```cpp
explicit Surface(
    vtkSmartPointer< vtkPolyData > mesh,
    bool geodesics_enabled =false,
    size_t geodesics_cache_multiplier_size =0
)
```


### function ~Surface

```cpp
~Surface() =default
```


### function compute_distance

```cpp
double compute_distance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    VectorType * out_grad =nullptr
) const
```


### function is_within_distance

```cpp
bool is_within_distance(
    const PointType & pointa,
    int idxa,
    const PointType & pointb,
    int idxb,
    double test_dist,
    double & dist
) const
```


### function geodesic_walk

```cpp
PointType geodesic_walk(
    PointType p,
    int idx,
    VectorType vector
) const
```


### function project_vector_to_surface_tangent

```cpp
VectorType project_vector_to_surface_tangent(
    const PointType & pointa,
    int idx,
    VectorType & vector
) const
```


### function sample_normal_at_point

```cpp
NormalType sample_normal_at_point(
    PointType p,
    int idx =-1
) const
```


### function sample_gradient_normal_at_point

```cpp
GradNType sample_gradient_normal_at_point(
    PointType p,
    int idx
) const
```


### function snap_to_mesh

```cpp
PointType snap_to_mesh(
    PointType pointa,
    int idx
) const
```


### function get_point_on_mesh

```cpp
PointType get_point_on_mesh() const
```


### function get_mesh_lower_bound

```cpp
inline const PointType & get_mesh_lower_bound() const
```


### function get_mesh_upper_bound

```cpp
inline const PointType & get_mesh_upper_bound() const
```


### function invalidate_particle

```cpp
virtual void invalidate_particle(
    int idx
)
```


### function get_polydata

```cpp
inline vtkSmartPointer< vtkPolyData > get_polydata() const
```


### function is_geodesics_enabled

```cpp
inline bool is_geodesics_enabled() const
```


-------------------------------

Updated on 2025-09-20 at 02:22:46 +0000