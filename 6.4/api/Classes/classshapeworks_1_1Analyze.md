---
title: shapeworks::Analyze
summary: High level analyze API. 

---

# shapeworks::Analyze



High level analyze API.  [More...](#detailed-description)


`#include <Analyze.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Analyze](../Classes/classshapeworks_1_1Analyze.md#function-analyze)**(ProjectHandle project) |
| void | **[run_offline_analysis](../Classes/classshapeworks_1_1Analyze.md#function-run-offline-analysis)**(std::string outfile) |
| ShapeList | **[get_shapes](../Classes/classshapeworks_1_1Analyze.md#function-get-shapes)**() |
| int | **[get_num_modes](../Classes/classshapeworks_1_1Analyze.md#function-get-num-modes)**() |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_mean_shape_points](../Classes/classshapeworks_1_1Analyze.md#function-get-mean-shape-points)**() |
| ShapeHandle | **[get_mean_shape](../Classes/classshapeworks_1_1Analyze.md#function-get-mean-shape)**() |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_shape_points](../Classes/classshapeworks_1_1Analyze.md#function-get-shape-points)**(int mode, double value) |
| ShapeHandle | **[get_mode_shape](../Classes/classshapeworks_1_1Analyze.md#function-get-mode-shape)**(int mode, double value) |
| bool | **[groups_active](../Classes/classshapeworks_1_1Analyze.md#function-groups-active)**() |
| ShapeHandle | **[create_shape_from_points](../Classes/classshapeworks_1_1Analyze.md#function-create-shape-from-points)**([Particles](../Classes/classshapeworks_1_1Particles.md) points) |

## Detailed Description

```cpp
class shapeworks::Analyze;
```

High level analyze API. 

The [Analyze](../Classes/classshapeworks_1_1Analyze.md) class operates on a [Project](../Classes/classshapeworks_1_1Project.md). 

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
    std::string outfile
)
```


### function get_shapes

```cpp
ShapeList get_shapes()
```


### function get_num_modes

```cpp
int get_num_modes()
```


### function get_mean_shape_points

```cpp
Particles get_mean_shape_points()
```


### function get_mean_shape

```cpp
ShapeHandle get_mean_shape()
```


### function get_shape_points

```cpp
Particles get_shape_points(
    int mode,
    double value
)
```


### function get_mode_shape

```cpp
ShapeHandle get_mode_shape(
    int mode,
    double value
)
```


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


-------------------------------

Updated on 2022-11-16 at 06:00:34 +0000