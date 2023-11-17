---
title: shapeworks::StatsGroupLDAJob

---

# shapeworks::StatsGroupLDAJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StatsGroupLDAJob](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-statsgroupldajob)**() |
| void | **[set_stats](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-set-stats)**([ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) stats) |
| virtual void | **[run](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-name)**() override<br>get the name of the job  |
| Eigen::VectorXf | **[get_group_pvalues](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-get-group-pvalues)**() |
| void | **[plot](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-plot)**(JKQTPlotter * plot, QString group_1_name, QString group_2_name) |

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

### function StatsGroupLDAJob

```cpp
StatsGroupLDAJob()
```


### function set_stats

```cpp
void set_stats(
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


### function plot

```cpp
void plot(
    JKQTPlotter * plot,
    QString group_1_name,
    QString group_2_name
)
```


-------------------------------

Updated on 2023-11-17 at 08:26:06 +0000