---
title: shapeworks::ShapeEvaluationJob
summary: The ShapeEvaluationJob class is a worker class that computes shape evaluation metrics of compactness, specificity, and generalization. It runs asynchronously using the Job and Worker interfaces. 

---

# shapeworks::ShapeEvaluationJob



The [ShapeEvaluationJob]() class is a worker class that computes shape evaluation metrics of compactness, specificity, and generalization. It runs asynchronously using the [Job](../Classes/classshapeworks_1_1Job.md) and [Worker]() interfaces. 


`#include <ShapeEvaluationJob.h>`

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
| | **[ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-shapeevaluationjob)**(JobType job_type, [ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) stats, QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session) |
| virtual void | **[run](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-name)**() override<br>get the name of the job  |

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
    ParticleShapeStatistics stats,
    QSharedPointer< Session > session
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


-------------------------------

Updated on 2024-07-14 at 16:33:03 +0000