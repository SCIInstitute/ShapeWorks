---
title: shapeworks::ParticleNormalEvaluation

---

# shapeworks::ParticleNormalEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| std::vector< double > | **[evaluate_particle_normals](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md#function-evaluate-particle-normals)**(const Eigen::MatrixXd & particles, const Eigen::MatrixXd & normals) |
| std::vector< bool > | **[threshold_particle_normals](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md#function-threshold-particle-normals)**(std::vector< double > angles, double max_angle_degrees) |
| Eigen::MatrixXd | **[compute_particle_normals](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md#function-compute-particle-normals)**(const Eigen::MatrixXd & particles, std::vector< std::shared_ptr< [MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md) > > meshes)<br>Compute normals at particle positions.  |

## Public Functions Documentation

### function evaluate_particle_normals

```cpp
static std::vector< double > evaluate_particle_normals(
    const Eigen::MatrixXd & particles,
    const Eigen::MatrixXd & normals
)
```


### function threshold_particle_normals

```cpp
static std::vector< bool > threshold_particle_normals(
    std::vector< double > angles,
    double max_angle_degrees
)
```


### function compute_particle_normals

```cpp
static Eigen::MatrixXd compute_particle_normals(
    const Eigen::MatrixXd & particles,
    std::vector< std::shared_ptr< MeshWrapper > > meshes
)
```

Compute normals at particle positions. 

-------------------------------

Updated on 2024-10-09 at 17:00:45 +0000