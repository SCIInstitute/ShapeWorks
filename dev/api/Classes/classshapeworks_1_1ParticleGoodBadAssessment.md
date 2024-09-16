---
title: shapeworks::ParticleGoodBadAssessment
summary: Performs good/bad points assessment and reports the bad positions of the particle system. 

---

# shapeworks::ParticleGoodBadAssessment



Performs good/bad points assessment and reports the bad positions of the particle system. 


`#include <ParticleGoodBadAssessment.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using [MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md)< float, 3 > | **[MeanCurvatureCacheType](../Classes/classshapeworks_1_1ParticleGoodBadAssessment.md#using-meancurvaturecachetype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[set_domains_per_shape](../Classes/classshapeworks_1_1ParticleGoodBadAssessment.md#function-set-domains-per-shape)**(int i) |
| void | **[set_criterion_angle](../Classes/classshapeworks_1_1ParticleGoodBadAssessment.md#function-set-criterion-angle)**(double a) |
| std::vector< std::vector< int > > | **[run_assessment](../Classes/classshapeworks_1_1ParticleGoodBadAssessment.md#function-run-assessment)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ps, [MeanCurvatureCacheType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) * mean_curvature_cache) |

## Public Types Documentation

### using MeanCurvatureCacheType

```cpp
using shapeworks::ParticleGoodBadAssessment::MeanCurvatureCacheType =  MeanCurvatureContainer<float, 3>;
```


## Public Functions Documentation

### function set_domains_per_shape

```cpp
inline void set_domains_per_shape(
    int i
)
```


### function set_criterion_angle

```cpp
inline void set_criterion_angle(
    double a
)
```


### function run_assessment

```cpp
std::vector< std::vector< int > > run_assessment(
    const ParticleSystem * ps,
    MeanCurvatureCacheType * mean_curvature_cache
)
```


-------------------------------

Updated on 2024-09-16 at 07:25:46 +0000