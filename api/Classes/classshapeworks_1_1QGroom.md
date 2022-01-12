---
title: shapeworks::QGroom
summary: Qt Wrapper for Groom. 

---

# shapeworks::QGroom



Qt Wrapper for [Groom](../Classes/classshapeworks_1_1Groom.md).  [More...](#detailed-description)


`#include <QGroom.h>`

Inherits from QObject, [shapeworks::Groom](../Classes/classshapeworks_1_1Groom.md)

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1QGroom.md#signal-progress)**(int ) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[QGroom](../Classes/classshapeworks_1_1QGroom.md#function-qgroom)**(ProjectHandle project) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[update_progress](../Classes/classshapeworks_1_1QGroom.md#function-update-progress)**()<br>call to be overridden by subclasses  |

## Additional inherited members

**Public Functions inherited from [shapeworks::Groom](../Classes/classshapeworks_1_1Groom.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Groom](../Classes/classshapeworks_1_1Groom.md#function-groom)**(ProjectHandle project) |
| virtual bool | **[run](../Classes/classshapeworks_1_1Groom.md#function-run)**()<br>Run the grooming.  |
| void | **[set_skip_grooming](../Classes/classshapeworks_1_1Groom.md#function-set-skip-grooming)**(bool skip)<br>Set if grooming steps should be skipped.  |
| void | **[abort](../Classes/classshapeworks_1_1Groom.md#function-abort)**()<br>Set abort as soon as possible.  |
| bool | **[get_aborted](../Classes/classshapeworks_1_1Groom.md#function-get-aborted)**()<br>Return if grooming was aborted.  |

**Protected Attributes inherited from [shapeworks::Groom](../Classes/classshapeworks_1_1Groom.md)**

|                | Name           |
| -------------- | -------------- |
| tbb::atomic< float > | **[progress_](../Classes/classshapeworks_1_1Groom.md#variable-progress-)**  |
| tbb::atomic< int > | **[total_ops_](../Classes/classshapeworks_1_1Groom.md#variable-total-ops-)**  |
| tbb::atomic< int > | **[progress_counter_](../Classes/classshapeworks_1_1Groom.md#variable-progress-counter-)**  |


## Detailed Description

```cpp
class shapeworks::QGroom;
```

Qt Wrapper for [Groom](../Classes/classshapeworks_1_1Groom.md). 

The [QGroom](../Classes/classshapeworks_1_1QGroom.md) class wraps the [Groom](../Classes/classshapeworks_1_1Groom.md) class to provide a QObject with a progress signal 

## Public Signals Documentation

### signal progress

```cpp
void progress(
    int 
)
```


## Public Functions Documentation

### function QGroom

```cpp
QGroom(
    ProjectHandle project
)
```


## Protected Functions Documentation

### function update_progress

```cpp
virtual void update_progress()
```

call to be overridden by subclasses 

**Reimplements**: [shapeworks::Groom::update_progress](../Classes/classshapeworks_1_1Groom.md#function-update-progress)


-------------------------------

Updated on 2022-01-12 at 19:02:30 +0000