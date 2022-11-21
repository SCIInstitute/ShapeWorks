---
title: shapeworks::Job

---

# shapeworks::Job





Inherits from QObject

Inherited by [shapeworks::DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md), [shapeworks::GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md), [shapeworks::ParticleNormalEvaluationJob](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md), [shapeworks::ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md), [shapeworks::StatsGroupLDAJob](../Classes/classshapeworks_1_1StatsGroupLDAJob.md)

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double ) |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual void | **[run](../Classes/classshapeworks_1_1Job.md#function-run)**() =0 |
| virtual QString | **[name](../Classes/classshapeworks_1_1Job.md#function-name)**() =0 |
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**() |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**() |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**() |

## Public Signals Documentation

### signal progress

```cpp
void progress(
    double 
)
```


### signal finished

```cpp
void finished()
```


## Public Functions Documentation

### function Job

```cpp
Job()
```


### function ~Job

```cpp
virtual ~Job()
```


### function run

```cpp
virtual void run() =0
```


**Reimplemented by**: [shapeworks::ShapeEvaluationJob::run](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-run), [shapeworks::DeepSSMJob::run](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run), [shapeworks::GroupPvalueJob::run](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-run), [shapeworks::ParticleNormalEvaluationJob::run](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-run), [shapeworks::StatsGroupLDAJob::run](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-run)


### function name

```cpp
virtual QString name() =0
```


**Reimplemented by**: [shapeworks::ShapeEvaluationJob::name](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-name), [shapeworks::DeepSSMJob::name](../Classes/classshapeworks_1_1DeepSSMJob.md#function-name), [shapeworks::GroupPvalueJob::name](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-name), [shapeworks::ParticleNormalEvaluationJob::name](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-name), [shapeworks::StatsGroupLDAJob::name](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-name)


### function get_completion_message

```cpp
virtual QString get_completion_message()
```


### function start_timer

```cpp
void start_timer()
```


### function timer_elapsed

```cpp
qint64 timer_elapsed()
```


-------------------------------

Updated on 2022-11-21 at 20:30:36 +0000