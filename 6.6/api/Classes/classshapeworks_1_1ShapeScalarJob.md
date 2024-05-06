---
title: shapeworks::ShapeScalarJob

---

# shapeworks::ShapeScalarJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum class| **[JobType](../Classes/classshapeworks_1_1ShapeScalarJob.md#enum-jobtype)** { Find_Components, MSE_Plot, Predict} |
| enum class| **[Direction](../Classes/classshapeworks_1_1ShapeScalarJob.md#enum-direction)** { To_Shape, To_Scalar} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeScalarJob](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-shapescalarjob)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, QString target_feature, Eigen::MatrixXd target_particles, JobType job_type) |
| virtual void | **[run](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-name)**() override<br>get the name of the job  |
| QPixmap | **[get_plot](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-get-plot)**() |
| void | **[set_number_of_components](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-set-number-of-components)**(int num_components) |
| void | **[set_number_of_folds](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-set-number-of-folds)**(int num_folds) |
| void | **[set_max_number_of_components](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-set-max-number-of-components)**(int num) |
| Eigen::VectorXd | **[get_prediction](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-get-prediction)**() |
| void | **[set_direction](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-set-direction)**(Direction direction) |
| Eigen::VectorXd | **[predict_scalars](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-predict-scalars)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, QString target_feature, Eigen::MatrixXd target_particles) |
| Eigen::VectorXd | **[predict_shape](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-predict-shape)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, QString target_feature, Eigen::MatrixXd target_particles) |
| void | **[clear_model](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-clear-model)**() |

## Additional inherited members

**Public Signals inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double , QString  ="") |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

**Public Functions inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**()<br>get a message to display when the job is complete  |
| virtual QString | **[get_abort_message](../Classes/classshapeworks_1_1Job.md#function-get-abort-message)**()<br>get a message to display when the job is aborted  |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**()<br>start the timer  |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**()<br>how much time has elapsed since the timer was started  |
| void | **[set_complete](../Classes/classshapeworks_1_1Job.md#function-set-complete)**(bool complete)<br>set the job as complete  |
| bool | **[is_complete](../Classes/classshapeworks_1_1Job.md#function-is-complete)**() const<br>is the job complete?  |
| void | **[abort](../Classes/classshapeworks_1_1Job.md#function-abort)**()<br>abort the job  |
| bool | **[is_aborted](../Classes/classshapeworks_1_1Job.md#function-is-aborted)**() const<br>was the job aborted?  |
| void | **[set_quiet_mode](../Classes/classshapeworks_1_1Job.md#function-set-quiet-mode)**(bool quiet)<br>set to quiet mode (no progress messages)  |
| bool | **[get_quiet_mode](../Classes/classshapeworks_1_1Job.md#function-get-quiet-mode)**()<br>get quiet mode  |


## Public Types Documentation

### enum JobType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Find_Components | |   |
| MSE_Plot | |   |
| Predict | |   |




### enum Direction

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| To_Shape | |   |
| To_Scalar | |   |




## Public Functions Documentation

### function ShapeScalarJob

```cpp
ShapeScalarJob(
    QSharedPointer< Session > session,
    QString target_feature,
    Eigen::MatrixXd target_particles,
    JobType job_type
)
```


### function run

```cpp
virtual void run() override
```

run the job 

**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function get_plot

```cpp
QPixmap get_plot()
```


### function set_number_of_components

```cpp
inline void set_number_of_components(
    int num_components
)
```


### function set_number_of_folds

```cpp
inline void set_number_of_folds(
    int num_folds
)
```


### function set_max_number_of_components

```cpp
inline void set_max_number_of_components(
    int num
)
```


### function get_prediction

```cpp
inline Eigen::VectorXd get_prediction()
```


### function set_direction

```cpp
inline void set_direction(
    Direction direction
)
```


### function predict_scalars

```cpp
static Eigen::VectorXd predict_scalars(
    QSharedPointer< Session > session,
    QString target_feature,
    Eigen::MatrixXd target_particles
)
```


### function predict_shape

```cpp
static Eigen::VectorXd predict_shape(
    QSharedPointer< Session > session,
    QString target_feature,
    Eigen::MatrixXd target_particles
)
```


### function clear_model

```cpp
static inline void clear_model()
```


-------------------------------

Updated on 2024-05-06 at 23:55:35 +0000