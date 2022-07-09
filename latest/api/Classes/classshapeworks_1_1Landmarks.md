---
title: shapeworks::Landmarks
summary: Representation of the project's landmarks. 

---

# shapeworks::Landmarks



Representation of the project's landmarks.  [More...](#detailed-description)


`#include <Landmarks.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Landmarks](../Classes/classshapeworks_1_1Landmarks.md#function-landmarks)**() |
| | **[~Landmarks](../Classes/classshapeworks_1_1Landmarks.md#function-~landmarks)**() |
| int | **[get_number_of_landmarks](../Classes/classshapeworks_1_1Landmarks.md#function-get-number-of-landmarks)**()<br>Return number of landmarks.  |
| [Landmark](../Classes/classshapeworks_1_1Landmark.md) | **[get_landmark](../Classes/classshapeworks_1_1Landmarks.md#function-get-landmark)**(int i)<br>Return the i-th landmark.  |
| void | **[set_landmark](../Classes/classshapeworks_1_1Landmarks.md#function-set-landmark)**(int i, const [Landmark](../Classes/classshapeworks_1_1Landmark.md) & landmark)<br>Set the i-th landmark.  |
| void | **[new_landmark](../Classes/classshapeworks_1_1Landmarks.md#function-new-landmark)**()<br>Create a new landmark.  |

## Detailed Description

```cpp
class shapeworks::Landmarks;
```

Representation of the project's landmarks. 

The [Landmarks](../Classes/classshapeworks_1_1Landmarks.md) class encapsulates a project's set of landmarks. 

## Public Functions Documentation

### function Landmarks

```cpp
Landmarks()
```


### function ~Landmarks

```cpp
~Landmarks()
```


### function get_number_of_landmarks

```cpp
int get_number_of_landmarks()
```

Return number of landmarks. 

### function get_landmark

```cpp
Landmark get_landmark(
    int i
)
```

Return the i-th landmark. 

### function set_landmark

```cpp
void set_landmark(
    int i,
    const Landmark & landmark
)
```

Set the i-th landmark. 

### function new_landmark

```cpp
void new_landmark()
```

Create a new landmark. 

-------------------------------

Updated on 2022-07-09 at 13:02:48 -0600