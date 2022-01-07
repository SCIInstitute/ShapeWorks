---
title: shapeworks::ShapeEvaluationJob

---

# shapeworks::ShapeEvaluationJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum class| **[JobType](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#enum-jobtype)** { CompactnessType, SpecificityType, GeneralizationType} |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[report_progress](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#signal-report-progress)**(shapeworks::ShapeEvaluationJob::JobType job_type, float progress) |
| void | **[result_ready](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#signal-result-ready)**(shapeworks::ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-shapeevaluationjob)**(JobType job_type, [ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) stats) |
| virtual void | **[run](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-name)**() override |

## Additional inherited members

**Public Signals inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[error_message](../Classes/classshapeworks_1_1Job.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1Job.md#signal-warning-message)**(QString ) |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double ) |
| void | **[message](../Classes/classshapeworks_1_1Job.md#signal-message)**(QString ) |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

**Public Functions inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**() |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**() |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**() |


## Public Types Documentation

### enum JobType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| CompactnessType | |   |
| SpecificityType | |   |
| GeneralizationType | |   |




## Public Signals Documentation

### signal report_progress

```cpp
void report_progress(
    shapeworks::ShapeEvaluationJob::JobType job_type,
    float progress
)
```


### signal result_ready

```cpp
void result_ready(
    shapeworks::ShapeEvaluationJob::JobType job_type,
    Eigen::VectorXd data
)
```


## Public Functions Documentation

### function ShapeEvaluationJob

```cpp
ShapeEvaluationJob(
    JobType job_type,
    ParticleShapeStatistics stats
)
```


### function run

```cpp
virtual void run() override
```


**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```


**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


-------------------------------

Updated on 2022-01-07 at 20:14:45 +0000