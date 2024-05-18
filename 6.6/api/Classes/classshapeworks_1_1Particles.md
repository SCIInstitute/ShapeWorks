---
title: shapeworks::Particles
summary: Representation of correspondence points for a shape including multiple domains. 

---

# shapeworks::Particles



Representation of correspondence points for a shape including multiple domains.  [More...](#detailed-description)


`#include <Particles.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Particles](../Classes/classshapeworks_1_1Particles.md#function-particles)**() =default |
| void | **[set_local_particles](../Classes/classshapeworks_1_1Particles.md#function-set-local-particles)**(int domain, const std::vector< itk::Point< double > > & particles) |
| void | **[set_world_particles](../Classes/classshapeworks_1_1Particles.md#function-set-world-particles)**(int domain, const std::vector< itk::Point< double > > & particles) |
| void | **[set_local_particles](../Classes/classshapeworks_1_1Particles.md#function-set-local-particles)**(int domain, Eigen::VectorXd particles) |
| void | **[set_world_particles](../Classes/classshapeworks_1_1Particles.md#function-set-world-particles)**(int domain, Eigen::VectorXd particles) |
| std::vector< Eigen::VectorXd > | **[get_local_particles](../Classes/classshapeworks_1_1Particles.md#function-get-local-particles)**() const |
| std::vector< Eigen::VectorXd > | **[get_world_particles](../Classes/classshapeworks_1_1Particles.md#function-get-world-particles)**() const |
| Eigen::VectorXd | **[get_local_particles](../Classes/classshapeworks_1_1Particles.md#function-get-local-particles)**(int domain) |
| Eigen::VectorXd | **[get_world_particles](../Classes/classshapeworks_1_1Particles.md#function-get-world-particles)**(int domain) |
| Eigen::VectorXd | **[get_raw_world_particles](../Classes/classshapeworks_1_1Particles.md#function-get-raw-world-particles)**(int domain)<br>Get untransformed original world particles from optimizer.  |
| Eigen::VectorXd | **[get_combined_local_particles](../Classes/classshapeworks_1_1Particles.md#function-get-combined-local-particles)**() const |
| Eigen::VectorXd | **[get_combined_global_particles](../Classes/classshapeworks_1_1Particles.md#function-get-combined-global-particles)**() const |
| void | **[set_combined_global_particles](../Classes/classshapeworks_1_1Particles.md#function-set-combined-global-particles)**(const Eigen::VectorXd & particles) |
| std::vector< itk::Point< double > > | **[get_local_points](../Classes/classshapeworks_1_1Particles.md#function-get-local-points)**(int domain) |
| std::vector< itk::Point< double > > | **[get_world_points](../Classes/classshapeworks_1_1Particles.md#function-get-world-points)**(int domain) |
| int | **[get_domain_for_combined_id](../Classes/classshapeworks_1_1Particles.md#function-get-domain-for-combined-id)**(int id)<br>Return which domain a particle belongs to when they are concatenated together.  |
| void | **[set_transform](../Classes/classshapeworks_1_1Particles.md#function-set-transform)**(vtkSmartPointer< vtkTransform > transform) |
| void | **[set_procrustes_transforms](../Classes/classshapeworks_1_1Particles.md#function-set-procrustes-transforms)**(const std::vector< vtkSmartPointer< vtkTransform > > & transforms) |
| void | **[set_alignment_type](../Classes/classshapeworks_1_1Particles.md#function-set-alignment-type)**(int alignment) |
| Eigen::VectorXd | **[get_difference_vectors](../Classes/classshapeworks_1_1Particles.md#function-get-difference-vectors)**(const [Particles](../Classes/classshapeworks_1_1Particles.md) & other) const |
| int | **[get_number_of_domains](../Classes/classshapeworks_1_1Particles.md#function-get-number-of-domains)**() |
| int | **[get_total_number_of_particles](../Classes/classshapeworks_1_1Particles.md#function-get-total-number-of-particles)**() |
| void | **[save_particles_file](../Classes/classshapeworks_1_1Particles.md#function-save-particles-file)**(std::string filename, const Eigen::VectorXd & points) |

## Detailed Description

```cpp
class shapeworks::Particles;
```

Representation of correspondence points for a shape including multiple domains. 

The StudioParticles class encapsulates the correspondence points for a shape, including multiple domains, local and global points 

## Public Functions Documentation

### function Particles

```cpp
Particles() =default
```


### function set_local_particles

```cpp
void set_local_particles(
    int domain,
    const std::vector< itk::Point< double > > & particles
)
```


### function set_world_particles

```cpp
void set_world_particles(
    int domain,
    const std::vector< itk::Point< double > > & particles
)
```


### function set_local_particles

```cpp
void set_local_particles(
    int domain,
    Eigen::VectorXd particles
)
```


### function set_world_particles

```cpp
void set_world_particles(
    int domain,
    Eigen::VectorXd particles
)
```


### function get_local_particles

```cpp
std::vector< Eigen::VectorXd > get_local_particles() const
```


### function get_world_particles

```cpp
std::vector< Eigen::VectorXd > get_world_particles() const
```


### function get_local_particles

```cpp
Eigen::VectorXd get_local_particles(
    int domain
)
```


### function get_world_particles

```cpp
Eigen::VectorXd get_world_particles(
    int domain
)
```


### function get_raw_world_particles

```cpp
Eigen::VectorXd get_raw_world_particles(
    int domain
)
```

Get untransformed original world particles from optimizer. 

### function get_combined_local_particles

```cpp
Eigen::VectorXd get_combined_local_particles() const
```


### function get_combined_global_particles

```cpp
Eigen::VectorXd get_combined_global_particles() const
```


### function set_combined_global_particles

```cpp
void set_combined_global_particles(
    const Eigen::VectorXd & particles
)
```


### function get_local_points

```cpp
std::vector< itk::Point< double > > get_local_points(
    int domain
)
```


### function get_world_points

```cpp
std::vector< itk::Point< double > > get_world_points(
    int domain
)
```


### function get_domain_for_combined_id

```cpp
int get_domain_for_combined_id(
    int id
)
```

Return which domain a particle belongs to when they are concatenated together. 

### function set_transform

```cpp
void set_transform(
    vtkSmartPointer< vtkTransform > transform
)
```


### function set_procrustes_transforms

```cpp
void set_procrustes_transforms(
    const std::vector< vtkSmartPointer< vtkTransform > > & transforms
)
```


### function set_alignment_type

```cpp
void set_alignment_type(
    int alignment
)
```


### function get_difference_vectors

```cpp
Eigen::VectorXd get_difference_vectors(
    const Particles & other
) const
```


### function get_number_of_domains

```cpp
int get_number_of_domains()
```


### function get_total_number_of_particles

```cpp
int get_total_number_of_particles()
```


### function save_particles_file

```cpp
static void save_particles_file(
    std::string filename,
    const Eigen::VectorXd & points
)
```


-------------------------------

Updated on 2024-05-18 at 07:02:35 +0000