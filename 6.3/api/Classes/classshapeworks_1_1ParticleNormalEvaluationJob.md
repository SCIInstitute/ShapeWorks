---
title: shapeworks::ParticleNormalEvaluationJob

---

# shapeworks::ParticleNormalEvaluationJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[result_ready](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#signal-result-ready)**(std::vector< bool > good_bad) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleNormalEvaluationJob](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-particlenormalevaluationjob)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session, double max_angle_degrees) |
| virtual void | **[run](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-name)**() override |
| std::vector< bool > | **[get_good_bad](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-get-good-bad)**() |

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


## Public Signals Documentation

### signal result_ready

```cpp
void result_ready(
    std::vector< bool > good_bad
)
```


## Public Functions Documentation

### function ParticleNormalEvaluationJob

```cpp
ParticleNormalEvaluationJob(
    QSharedPointer< Session > session,
    double max_angle_degrees
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


### function get_good_bad

```cpp
std::vector< bool > get_good_bad()
```


-------------------------------

Updated on 2022-06-11 at 02:39:24 +0000