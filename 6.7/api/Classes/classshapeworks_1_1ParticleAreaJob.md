---
title: shapeworks::ParticleAreaJob

---

# shapeworks::ParticleAreaJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleAreaJob](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-particleareajob)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session) |
| virtual void | **[run](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-name)**() override<br>get the name of the job  |
| const Eigen::VectorXd & | **[get_mean_areas](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-get-mean-areas)**() const |
| const Eigen::VectorXd & | **[get_stddev_areas](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-get-stddev-areas)**() const |

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


## Public Functions Documentation

### function ParticleAreaJob

```cpp
ParticleAreaJob(
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
inline virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function get_mean_areas

```cpp
inline const Eigen::VectorXd & get_mean_areas() const
```


### function get_stddev_areas

```cpp
inline const Eigen::VectorXd & get_stddev_areas() const
```


-------------------------------

Updated on 2025-08-22 at 08:23:42 +0000