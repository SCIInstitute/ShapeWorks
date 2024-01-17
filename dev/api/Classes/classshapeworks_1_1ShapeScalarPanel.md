---
title: shapeworks::ShapeScalarPanel

---

# shapeworks::ShapeScalarPanel





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[run_clicked](../Classes/classshapeworks_1_1ShapeScalarPanel.md#slot-run-clicked)**() |
| void | **[handle_job_progress](../Classes/classshapeworks_1_1ShapeScalarPanel.md#slot-handle-job-progress)**(int progress) |
| void | **[handle_job_complete](../Classes/classshapeworks_1_1ShapeScalarPanel.md#slot-handle-job-complete)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classshapeworks_1_1ShapeScalarPanel.md#signal-update-view)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeScalarPanel](../Classes/classshapeworks_1_1ShapeScalarPanel.md#function-shapescalarpanel)**(QWidget * parent =0) |
| | **[~ShapeScalarPanel](../Classes/classshapeworks_1_1ShapeScalarPanel.md#function-~shapescalarpanel)**() |
| void | **[set_session](../Classes/classshapeworks_1_1ShapeScalarPanel.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the session  |
| void | **[reset](../Classes/classshapeworks_1_1ShapeScalarPanel.md#function-reset)**() |

## Public Slots Documentation

### slot run_clicked

```cpp
void run_clicked()
```


### slot handle_job_progress

```cpp
void handle_job_progress(
    int progress
)
```


### slot handle_job_complete

```cpp
void handle_job_complete()
```


## Public Signals Documentation

### signal update_view

```cpp
void update_view()
```


## Public Functions Documentation

### function ShapeScalarPanel

```cpp
ShapeScalarPanel(
    QWidget * parent =0
)
```


### function ~ShapeScalarPanel

```cpp
~ShapeScalarPanel()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

set the pointer to the session 

### function reset

```cpp
void reset()
```


-------------------------------

Updated on 2024-01-17 at 16:55:15 +0000