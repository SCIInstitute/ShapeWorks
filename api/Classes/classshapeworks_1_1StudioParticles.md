---
title: shapeworks::StudioParticles
summary: Representation of correspondence points for a shape including multiple domains. 

---

# shapeworks::StudioParticles



Representation of correspondence points for a shape including multiple domains.  [More...](#detailed-description)


`#include <StudioParticles.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md#function-studioparticles)**() |
| void | **[set_local_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-set-local-particles)**(int domain, std::vector< itk::Point< double >> particles) |
| void | **[set_world_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-set-world-particles)**(int domain, std::vector< itk::Point< double >> particles) |
| void | **[set_local_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-set-local-particles)**(int domain, Eigen::VectorXd particles) |
| void | **[set_world_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-set-world-particles)**(int domain, Eigen::VectorXd particles) |
| std::vector< Eigen::VectorXd > | **[get_local_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-local-particles)**() |
| std::vector< Eigen::VectorXd > | **[get_world_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-world-particles)**() |
| Eigen::VectorXd | **[get_local_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-local-particles)**(int domain) |
| Eigen::VectorXd | **[get_world_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-world-particles)**(int domain) |
| Eigen::VectorXd | **[get_raw_world_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-raw-world-particles)**(int domain)<br>Get untransformed original world particles from optimizer.  |
| Eigen::VectorXd | **[get_combined_local_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-combined-local-particles)**() |
| Eigen::VectorXd | **[get_combined_global_particles](../Classes/classshapeworks_1_1StudioParticles.md#function-get-combined-global-particles)**() |
| std::vector< itk::Point< double > > | **[get_local_points](../Classes/classshapeworks_1_1StudioParticles.md#function-get-local-points)**(int domain) |
| std::vector< itk::Point< double > > | **[get_world_points](../Classes/classshapeworks_1_1StudioParticles.md#function-get-world-points)**(int domain) |
| int | **[get_domain_for_combined_id](../Classes/classshapeworks_1_1StudioParticles.md#function-get-domain-for-combined-id)**(int id)<br>Return which domain a particle belongs to when they are concatenated together.  |
| void | **[set_transform](../Classes/classshapeworks_1_1StudioParticles.md#function-set-transform)**(vtkSmartPointer< vtkTransform > transform) |
| void | **[set_procrustes_transforms](../Classes/classshapeworks_1_1StudioParticles.md#function-set-procrustes-transforms)**(std::vector< vtkSmartPointer< vtkTransform >> transforms) |

## Detailed Description

```cpp
class shapeworks::StudioParticles;
```

Representation of correspondence points for a shape including multiple domains. 

The [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) class encapsulates the correspondence points for a shape, including multiple domains, local and global points 

## Public Functions Documentation

### function StudioParticles

```cpp
StudioParticles()
```


### function set_local_particles

```cpp
void set_local_particles(
    int domain,
    std::vector< itk::Point< double >> particles
)
```


### function set_world_particles

```cpp
void set_world_particles(
    int domain,
    std::vector< itk::Point< double >> particles
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
std::vector< Eigen::VectorXd > get_local_particles()
```


### function get_world_particles

```cpp
std::vector< Eigen::VectorXd > get_world_particles()
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
Eigen::VectorXd get_combined_local_particles()
```


### function get_combined_global_particles

```cpp
Eigen::VectorXd get_combined_global_particles()
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
    std::vector< vtkSmartPointer< vtkTransform >> transforms
)
```


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000