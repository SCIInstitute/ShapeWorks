---
title: shapeworks::ShapeEvaluation

---

# shapeworks::ShapeEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeEvaluation](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-shapeevaluation)**() |
| double | **[compute_compactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-compactness)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, int num_modes, const std::string & save_to ="") |
| Eigen::VectorXd | **[compute_full_compactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-full-compactness)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, std::function< void(float)> progress_callback =nullptr) |
| double | **[compute_generalization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-generalization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, int num_modes, const std::string & save_to ="", bool surface_distance_mode =false) |
| Eigen::VectorXd | **[compute_full_generalization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-full-generalization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, std::function< void(float)> progress_callback =nullptr, std::function< bool()> check_abort =nullptr, bool surface_distance_mode =false) |
| double | **[compute_specificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-specificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, int num_mode, const std::string & save_to ="", bool surface_distance_mode =false) |
| Eigen::VectorXd | **[compute_full_specificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-full-specificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, std::function< void(float)> progress_callback =nullptr, std::function< bool()> check_abort =nullptr, bool surface_distance_mode =false) |

## Public Functions Documentation

### function ShapeEvaluation

```cpp
ShapeEvaluation()
```


### function compute_compactness

```cpp
static double compute_compactness(
    const ParticleSystemEvaluation & particle_system,
    int num_modes,
    const std::string & save_to =""
)
```


### function compute_full_compactness

```cpp
static Eigen::VectorXd compute_full_compactness(
    const ParticleSystemEvaluation & particle_system,
    std::function< void(float)> progress_callback =nullptr
)
```


### function compute_generalization

```cpp
static double compute_generalization(
    const ParticleSystemEvaluation & particle_system,
    int num_modes,
    const std::string & save_to ="",
    bool surface_distance_mode =false
)
```


### function compute_full_generalization

```cpp
static Eigen::VectorXd compute_full_generalization(
    const ParticleSystemEvaluation & particle_system,
    std::function< void(float)> progress_callback =nullptr,
    std::function< bool()> check_abort =nullptr,
    bool surface_distance_mode =false
)
```


### function compute_specificity

```cpp
static double compute_specificity(
    const ParticleSystemEvaluation & particle_system,
    int num_mode,
    const std::string & save_to ="",
    bool surface_distance_mode =false
)
```


### function compute_full_specificity

```cpp
static Eigen::VectorXd compute_full_specificity(
    const ParticleSystemEvaluation & particle_system,
    std::function< void(float)> progress_callback =nullptr,
    std::function< bool()> check_abort =nullptr,
    bool surface_distance_mode =false
)
```


-------------------------------

Updated on 2025-10-13 at 18:47:49 +0000