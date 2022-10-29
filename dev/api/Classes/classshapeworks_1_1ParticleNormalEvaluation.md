---
title: shapeworks::ParticleNormalEvaluation

---

# shapeworks::ParticleNormalEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| std::vector< bool > | **[evaluate_particle_normals](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md#function-evaluate-particle-normals)**(const Eigen::MatrixXd & particles, const Eigen::MatrixXd & normals, double max_angle_degrees) |
| Eigen::MatrixXd | **[compute_particle_normals](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md#function-compute-particle-normals)**(const Eigen::MatrixXd & particles, std::vector< std::shared_ptr< [VtkMeshWrapper](../Classes/classshapeworks_1_1VtkMeshWrapper.md) > > meshes)<br>Compute normals at particle positions.  |

## Public Functions Documentation

### function evaluate_particle_normals

```cpp
static std::vector< bool > evaluate_particle_normals(
    const Eigen::MatrixXd & particles,
    const Eigen::MatrixXd & normals,
    double max_angle_degrees
)
```


### function compute_particle_normals

```cpp
static Eigen::MatrixXd compute_particle_normals(
    const Eigen::MatrixXd & particles,
    std::vector< std::shared_ptr< VtkMeshWrapper > > meshes
)
```

Compute normals at particle positions. 

-------------------------------

Updated on 2022-10-29 at 22:38:48 +0000