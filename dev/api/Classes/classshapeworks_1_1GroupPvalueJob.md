---
title: shapeworks::GroupPvalueJob

---

# shapeworks::GroupPvalueJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-grouppvaluejob)**([ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) stats) |
| virtual void | **[run](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-name)**() override |
| Eigen::VectorXf | **[get_group_pvalues](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-get-group-pvalues)**() |

## Additional inherited members

**Public Signals inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double ) |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

**Public Functions inherited from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**() |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**() |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**() |


## Public Functions Documentation

### function GroupPvalueJob

```cpp
GroupPvalueJob(
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


### function get_group_pvalues

```cpp
Eigen::VectorXf get_group_pvalues()
```


-------------------------------

Updated on 2022-11-16 at 05:48:47 +0000