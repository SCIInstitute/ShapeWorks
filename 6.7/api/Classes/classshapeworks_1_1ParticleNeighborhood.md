---
title: shapeworks::ParticleNeighborhood

---

# shapeworks::ParticleNeighborhood



 [More...](#detailed-description)


`#include <ParticleNeighborhood.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-particleneighborhood)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ps, int domain_id =-1) |
| std::vector< [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) > | **[find_neighborhood_points](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-find-neighborhood-points)**(const itk::Point< double, 3 > & position, int id, std::vector< double > & weights, std::vector< double > & distances, double radius) |
| std::vector< [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) > | **[find_neighborhood_points](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-find-neighborhood-points)**(const itk::Point< double, 3 > & position, int id, std::vector< double > & weights, double radius) |
| std::vector< [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) > | **[find_neighborhood_points](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-find-neighborhood-points)**(const itk::Point< double, 3 > & position, int id, double radius) |
| void | **[set_weighting_enabled](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-set-weighting-enabled)**(bool is_enabled) |
| bool | **[is_weighting_enabled](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-is-weighting-enabled)**() const |
| void | **[set_force_euclidean](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-set-force-euclidean)**(bool is_enabled) |
| bool | **[is_force_euclidean](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-is-force-euclidean)**() const |
| void | **[set_domain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-set-domain)**(ParticleDomain::Pointer domain) |
| ParticleDomain::Pointer | **[get_domain](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-get-domain)**() const |
| void | **[set_domain_id](../Classes/classshapeworks_1_1ParticleNeighborhood.md#function-set-domain-id)**(int id) |

## Detailed Description

```cpp
class shapeworks::ParticleNeighborhood;
```


A [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) is responsible for computing neighborhoods of particles. Given a point position in a domain, and a neighborhood radius, the [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) returns a list of points that are neighbors of that point. 

## Public Functions Documentation

### function ParticleNeighborhood

```cpp
inline explicit ParticleNeighborhood(
    ParticleSystem * ps,
    int domain_id =-1
)
```


### function find_neighborhood_points

```cpp
std::vector< ParticlePointIndexPair > find_neighborhood_points(
    const itk::Point< double, 3 > & position,
    int id,
    std::vector< double > & weights,
    std::vector< double > & distances,
    double radius
)
```


### function find_neighborhood_points

```cpp
std::vector< ParticlePointIndexPair > find_neighborhood_points(
    const itk::Point< double, 3 > & position,
    int id,
    std::vector< double > & weights,
    double radius
)
```


### function find_neighborhood_points

```cpp
std::vector< ParticlePointIndexPair > find_neighborhood_points(
    const itk::Point< double, 3 > & position,
    int id,
    double radius
)
```


### function set_weighting_enabled

```cpp
inline void set_weighting_enabled(
    bool is_enabled
)
```


### function is_weighting_enabled

```cpp
inline bool is_weighting_enabled() const
```


### function set_force_euclidean

```cpp
inline void set_force_euclidean(
    bool is_enabled
)
```


### function is_force_euclidean

```cpp
inline bool is_force_euclidean() const
```


### function set_domain

```cpp
inline void set_domain(
    ParticleDomain::Pointer domain
)
```


### function get_domain

```cpp
inline ParticleDomain::Pointer get_domain() const
```


### function set_domain_id

```cpp
inline void set_domain_id(
    int id
)
```


-------------------------------

Updated on 2025-05-28 at 23:15:54 +0000