---
title: shapeworks::Groom
summary: High level groom API. 

---

# shapeworks::Groom



High level groom API.  [More...](#detailed-description)


`#include <Groom.h>`

Inherited by [shapeworks::QGroom](../Classes/classshapeworks_1_1QGroom.md)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Groom](../Classes/classshapeworks_1_1Groom.md#function-groom)**(ProjectHandle project) |
| virtual bool | **[run](../Classes/classshapeworks_1_1Groom.md#function-run)**()<br>Run the grooming.  |
| void | **[set_skip_grooming](../Classes/classshapeworks_1_1Groom.md#function-set-skip-grooming)**(bool skip)<br>Set if grooming steps should be skipped.  |
| void | **[abort](../Classes/classshapeworks_1_1Groom.md#function-abort)**()<br>Set abort as soon as possible.  |
| bool | **[get_aborted](../Classes/classshapeworks_1_1Groom.md#function-get-aborted)**()<br>Return if grooming was aborted.  |
| double | **[compute_landmark_distance](../Classes/classshapeworks_1_1Groom.md#function-compute-landmark-distance)**(vtkSmartPointer< vtkPoints > one, vtkSmartPointer< vtkPoints > two)<br>Util to compute square distance between paired landmarks.  |
| vtkSmartPointer< vtkMatrix4x4 > | **[compute_landmark_transform](../Classes/classshapeworks_1_1Groom.md#function-compute-landmark-transform)**(vtkSmartPointer< vtkPoints > source, vtkSmartPointer< vtkPoints > target)<br>Util to compute landmark transform.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[update_progress](../Classes/classshapeworks_1_1Groom.md#function-update-progress)**()<br>call to be overridden by subclasses  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| tbb::atomic< float > | **[progress_](../Classes/classshapeworks_1_1Groom.md#variable-progress-)**  |
| tbb::atomic< int > | **[total_ops_](../Classes/classshapeworks_1_1Groom.md#variable-total-ops-)**  |
| tbb::atomic< int > | **[progress_counter_](../Classes/classshapeworks_1_1Groom.md#variable-progress-counter-)**  |

## Detailed Description

```cpp
class shapeworks::Groom;
```

High level groom API. 

The [Groom](../Classes/classshapeworks_1_1Groom.md) class operates on a [Project](../Classes/classshapeworks_1_1Project.md). It is used by Studio and other tools to perform Grooming operations. 

## Public Functions Documentation

### function Groom

```cpp
Groom(
    ProjectHandle project
)
```


### function run

```cpp
virtual bool run()
```

Run the grooming. 

### function set_skip_grooming

```cpp
void set_skip_grooming(
    bool skip
)
```

Set if grooming steps should be skipped. 

### function abort

```cpp
void abort()
```

Set abort as soon as possible. 

### function get_aborted

```cpp
bool get_aborted()
```

Return if grooming was aborted. 

### function compute_landmark_distance

```cpp
static double compute_landmark_distance(
    vtkSmartPointer< vtkPoints > one,
    vtkSmartPointer< vtkPoints > two
)
```

Util to compute square distance between paired landmarks. 

### function compute_landmark_transform

```cpp
static vtkSmartPointer< vtkMatrix4x4 > compute_landmark_transform(
    vtkSmartPointer< vtkPoints > source,
    vtkSmartPointer< vtkPoints > target
)
```

Util to compute landmark transform. 

## Protected Functions Documentation

### function update_progress

```cpp
inline virtual void update_progress()
```

call to be overridden by subclasses 

**Reimplemented by**: [shapeworks::QGroom::update_progress](../Classes/classshapeworks_1_1QGroom.md#function-update-progress)


## Protected Attributes Documentation

### variable progress_

```cpp
tbb::atomic< float > progress_ = 0;
```


### variable total_ops_

```cpp
tbb::atomic< int > total_ops_ = 0;
```


### variable progress_counter_

```cpp
tbb::atomic< int > progress_counter_ = 0;
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000