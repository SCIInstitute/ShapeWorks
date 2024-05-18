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
| double | **[compute_generalization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-generalization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, int num_modes, const std::string & save_to ="") |
| Eigen::VectorXd | **[compute_full_generalization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-full-generalization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, std::function< void(float)> progress_callback =nullptr) |
| double | **[compute_specificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-specificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, int num_mode, const std::string & save_to ="") |
| Eigen::VectorXd | **[compute_full_specificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-compute-full-specificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & particle_system, std::function< void(float)> progress_callback =nullptr) |

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
    const std::string & save_to =""
)
```


### function compute_full_generalization

```cpp
static Eigen::VectorXd compute_full_generalization(
    const ParticleSystemEvaluation & particle_system,
    std::function< void(float)> progress_callback =nullptr
)
```


### function compute_specificity

```cpp
static double compute_specificity(
    const ParticleSystemEvaluation & particle_system,
    int num_mode,
    const std::string & save_to =""
)
```


### function compute_full_specificity

```cpp
static Eigen::VectorXd compute_full_specificity(
    const ParticleSystemEvaluation & particle_system,
    std::function< void(float)> progress_callback =nullptr
)
```


-------------------------------

Updated on 2024-05-18 at 07:02:36 +0000