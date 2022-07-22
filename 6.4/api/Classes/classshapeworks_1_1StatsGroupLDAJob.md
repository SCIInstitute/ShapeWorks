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
| virtual void | **[run](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-name)**() override |
| Eigen::VectorXf | **[get_group_pvalues](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-get-group-pvalues)**() |
| void | **[plot](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-plot)**(JKQTPlotter * plot, QString group_1_name, QString group_2_name) |

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


### function plot

```cpp
void plot(
    JKQTPlotter * plot,
    QString group_1_name,
    QString group_2_name
)
```


-------------------------------

Updated on 2022-07-22 at 21:16:04 +0000