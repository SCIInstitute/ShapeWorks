---
title: shapeworks::Analyze
summary: High level analyze API. 

---

# shapeworks::Analyze



High level analyze API.  [More...](#detailed-description)


`#include <Analyze.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[AlignmentType](../Classes/classshapeworks_1_1Analyze.md#enum-alignmenttype)** { Global = -2, Local = -1} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Analyze](../Classes/classshapeworks_1_1Analyze.md#function-analyze)**(ProjectHandle project) |
| void | **[run_offline_analysis](../Classes/classshapeworks_1_1Analyze.md#function-run-offline-analysis)**(std::string outfile, float range, float steps)<br>Run offline analysis, saving results to outfile.  |
| ShapeList | **[get_shapes](../Classes/classshapeworks_1_1Analyze.md#function-get-shapes)**()<br>Return the list of shapes.  |
| int | **[get_num_subjects](../Classes/classshapeworks_1_1Analyze.md#function-get-num-subjects)**()<br>Return the number of subjects.  |
| int | **[get_num_modes](../Classes/classshapeworks_1_1Analyze.md#function-get-num-modes)**()<br>Return the number of modes.  |
| int | **[get_num_particles](../Classes/classshapeworks_1_1Analyze.md#function-get-num-particles)**()<br>Return the number of particles.  |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_mean_shape_points](../Classes/classshapeworks_1_1Analyze.md#function-get-mean-shape-points)**()<br>Return the mean shape.  |
| ShapeHandle | **[get_mean_shape](../Classes/classshapeworks_1_1Analyze.md#function-get-mean-shape)**()<br>Return the mean shape.  |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_group_shape_particles](../Classes/classshapeworks_1_1Analyze.md#function-get-group-shape-particles)**(double ratio) |
| ShapeHandle | **[get_group_shape](../Classes/classshapeworks_1_1Analyze.md#function-get-group-shape)**(double ratio) |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_shape_points](../Classes/classshapeworks_1_1Analyze.md#function-get-shape-points)**(int mode, double value)<br>Return the particles for a given mode and value.  |
| ShapeHandle | **[get_mode_shape](../Classes/classshapeworks_1_1Analyze.md#function-get-mode-shape)**(int mode, double value)<br>Return the shape for a given mode and value.  |
| bool | **[groups_active](../Classes/classshapeworks_1_1Analyze.md#function-groups-active)**() |
| ShapeHandle | **[create_shape_from_points](../Classes/classshapeworks_1_1Analyze.md#function-create-shape-from-points)**([Particles](../Classes/classshapeworks_1_1Particles.md) points) |
| Eigen::VectorXd | **[get_subject_features](../Classes/classshapeworks_1_1Analyze.md#function-get-subject-features)**(int subject, std::string feature_name) |
| void | **[set_group_selection](../Classes/classshapeworks_1_1Analyze.md#function-set-group-selection)**(std::string group_name, std::string group1, std::string group2) |
| [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) | **[get_local_particle_system](../Classes/classshapeworks_1_1Analyze.md#function-get-local-particle-system)**(int domain) |

## Detailed Description

```cpp
class shapeworks::Analyze;
```

High level analyze API. 

The [Analyze](../Classes/classshapeworks_1_1Analyze.md) class operates on a [Project](../Classes/classshapeworks_1_1Project.md). 

## Public Types Documentation

### enum AlignmentType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Global | -2|   |
| Local | -1|   |




## Public Functions Documentation

### function Analyze

```cpp
Analyze(
    ProjectHandle project
)
```


### function run_offline_analysis

```cpp
void run_offline_analysis(
    std::string outfile,
    float range,
    float steps
)
```

Run offline analysis, saving results to outfile. 

### function get_shapes

```cpp
ShapeList get_shapes()
```

Return the list of shapes. 

### function get_num_subjects

```cpp
int get_num_subjects()
```

Return the number of subjects. 

### function get_num_modes

```cpp
int get_num_modes()
```

Return the number of modes. 

### function get_num_particles

```cpp
int get_num_particles()
```

Return the number of particles. 

### function get_mean_shape_points

```cpp
Particles get_mean_shape_points()
```

Return the mean shape. 

### function get_mean_shape

```cpp
ShapeHandle get_mean_shape()
```

Return the mean shape. 

### function get_group_shape_particles

```cpp
Particles get_group_shape_particles(
    double ratio
)
```


### function get_group_shape

```cpp
ShapeHandle get_group_shape(
    double ratio
)
```


### function get_shape_points

```cpp
Particles get_shape_points(
    int mode,
    double value
)
```

Return the particles for a given mode and value. 

### function get_mode_shape

```cpp
ShapeHandle get_mode_shape(
    int mode,
    double value
)
```

Return the shape for a given mode and value. 

### function groups_active

```cpp
inline bool groups_active()
```


### function create_shape_from_points

```cpp
ShapeHandle create_shape_from_points(
    Particles points
)
```


### function get_subject_features

```cpp
Eigen::VectorXd get_subject_features(
    int subject,
    std::string feature_name
)
```


### function set_group_selection

```cpp
void set_group_selection(
    std::string group_name,
    std::string group1,
    std::string group2
)
```


### function get_local_particle_system

```cpp
ParticleSystemEvaluation get_local_particle_system(
    int domain
)
```


-------------------------------

Updated on 2024-09-04 at 22:59:22 +0000