---
title: shapeworks::ParticleSystemEvaluation

---

# shapeworks::ParticleSystemEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-particlesystemevaluation)**(const std::vector< std::string > & paths)<br>Initialize particle system from a list of paths to particle files.  |
| | **[ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-particlesystemevaluation)**(const Eigen::MatrixXd & matrix, int num_values_per_particle =3)<br>Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)  |
| const Eigen::MatrixXd & | **[get_matrix](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-get-matrix)**() const<br>Get the matrix representation of the particle system.  |
| int | **[get_num_values_per_particle](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-get-num-values-per-particle)**() const<br>Get the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar)  |
| const std::vector< std::string > & | **[get_paths](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-get-paths)**() const<br>Get the paths to the particle files.  |
| int | **[num_samples](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-num-samples)**() const<br>Number of samples.  |
| int | **[num_dims](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-num-dims)**() const<br>Dimensions (e.g. x/y/z * number of particles)  |
| bool | **[exact_compare](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-exact-compare)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & other) const<br>Perform an exact comparison of two particle systems.  |
| bool | **[evaluation_compare](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-evaluation-compare)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & other) const<br>Perform an evaluation comparison of two particle systems.  |
| void | **[set_meshes](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-set-meshes)**(const std::vector< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > & meshes)<br>Set the meshes for each sample (used for some evaluation metrics)  |
| const std::vector< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > & | **[get_meshes](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-get-meshes)**() const<br>Get the meshes for each sample.  |
| bool | **[read_particle_file](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-read-particle-file)**(std::string filename, Eigen::VectorXd & points)<br>Read a particle file into an Eigen vector.  |

## Friends

|                | Name           |
| -------------- | -------------- |
| struct | **[SharedCommandData](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#friend-sharedcommanddata)**  |

## Public Functions Documentation

### function ParticleSystemEvaluation

```cpp
explicit ParticleSystemEvaluation(
    const std::vector< std::string > & paths
)
```

Initialize particle system from a list of paths to particle files. 

### function ParticleSystemEvaluation

```cpp
explicit ParticleSystemEvaluation(
    const Eigen::MatrixXd & matrix,
    int num_values_per_particle =3
)
```

Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples) 

### function get_matrix

```cpp
inline const Eigen::MatrixXd & get_matrix() const
```

Get the matrix representation of the particle system. 

### function get_num_values_per_particle

```cpp
inline int get_num_values_per_particle() const
```

Get the number of values for each particle (e.g. 3 for x/y/z, 4 for x/y/z/scalar) 

### function get_paths

```cpp
inline const std::vector< std::string > & get_paths() const
```

Get the paths to the particle files. 

### function num_samples

```cpp
inline int num_samples() const
```

Number of samples. 

### function num_dims

```cpp
inline int num_dims() const
```

Dimensions (e.g. x/y/z * number of particles) 

### function exact_compare

```cpp
bool exact_compare(
    const ParticleSystemEvaluation & other
) const
```

Perform an exact comparison of two particle systems. 

### function evaluation_compare

```cpp
bool evaluation_compare(
    const ParticleSystemEvaluation & other
) const
```

Perform an evaluation comparison of two particle systems. 

### function set_meshes

```cpp
void set_meshes(
    const std::vector< Mesh > & meshes
)
```

Set the meshes for each sample (used for some evaluation metrics) 

### function get_meshes

```cpp
inline const std::vector< Mesh > & get_meshes() const
```

Get the meshes for each sample. 

### function read_particle_file

```cpp
static bool read_particle_file(
    std::string filename,
    Eigen::VectorXd & points
)
```

Read a particle file into an Eigen vector. 

## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData(
    SharedCommandData 
);
```


-------------------------------

Updated on 2024-08-16 at 05:11:53 +0000