---
title: shapeworks::GroupPvalueJob

---

# shapeworks::GroupPvalueJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-grouppvaluejob)**([ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) stats) |
| virtual void | **[run](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-name)**() override<br>get the name of the job  |
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
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**()<br>get a message to display when the job is complete  |
| virtual QString | **[get_abort_message](../Classes/classshapeworks_1_1Job.md#function-get-abort-message)**()<br>get a message to display when the job is aborted  |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**()<br>start the timer  |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**()<br>how much time has elapsed since the timer was started  |
| void | **[set_complete](../Classes/classshapeworks_1_1Job.md#function-set-complete)**(bool complete)<br>set the job as complete  |
| bool | **[is_complete](../Classes/classshapeworks_1_1Job.md#function-is-complete)**() const<br>is the job complete?  |
| void | **[abort](../Classes/classshapeworks_1_1Job.md#function-abort)**()<br>abort the job  |
| bool | **[is_aborted](../Classes/classshapeworks_1_1Job.md#function-is-aborted)**() const<br>was the job aborted?  |


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

run the job 

**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function get_group_pvalues

```cpp
Eigen::VectorXf get_group_pvalues()
```


-------------------------------

Updated on 2023-10-24 at 22:16:01 +0000