---
title: shapeworks::ParticleAreaPanel

---

# shapeworks::ParticleAreaPanel





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[run_clicked](../Classes/classshapeworks_1_1ParticleAreaPanel.md#slot-run-clicked)**() |
| void | **[show_particle_area_clicked](../Classes/classshapeworks_1_1ParticleAreaPanel.md#slot-show-particle-area-clicked)**() |
| void | **[display_option_changed](../Classes/classshapeworks_1_1ParticleAreaPanel.md#slot-display-option-changed)**() |
| void | **[handle_job_progress](../Classes/classshapeworks_1_1ParticleAreaPanel.md#slot-handle-job-progress)**(int progress) |
| void | **[handle_job_complete](../Classes/classshapeworks_1_1ParticleAreaPanel.md#slot-handle-job-complete)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classshapeworks_1_1ParticleAreaPanel.md#signal-update-view)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleAreaPanel](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-particleareapanel)**(QWidget * parent =0) |
| | **[~ParticleAreaPanel](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-~particleareapanel)**() |
| void | **[set_session](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the session  |
| void | **[reset](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-reset)**() |
| bool | **[get_display_particle_area](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-get-display-particle-area)**() const |
| Eigen::VectorXd | **[get_computed_values](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-get-computed-values)**() const |
| std::string | **[get_computed_value_name](../Classes/classshapeworks_1_1ParticleAreaPanel.md#function-get-computed-value-name)**() const |

## Public Slots Documentation

### slot run_clicked

```cpp
void run_clicked()
```


### slot show_particle_area_clicked

```cpp
void show_particle_area_clicked()
```


### slot display_option_changed

```cpp
void display_option_changed()
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

### function ParticleAreaPanel

```cpp
ParticleAreaPanel(
    QWidget * parent =0
)
```


### function ~ParticleAreaPanel

```cpp
~ParticleAreaPanel()
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


### function get_display_particle_area

```cpp
bool get_display_particle_area() const
```


### function get_computed_values

```cpp
Eigen::VectorXd get_computed_values() const
```


### function get_computed_value_name

```cpp
std::string get_computed_value_name() const
```


-------------------------------

Updated on 2024-04-09 at 20:45:27 +0000