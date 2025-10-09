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
| virtual void | **[run](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-name)**() override<br>get the name of the job  |
| std::vector< bool > | **[get_good_bad](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-get-good-bad)**() |

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

run the job 

**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function get_good_bad

```cpp
std::vector< bool > get_good_bad()
```


-------------------------------

Updated on 2025-10-09 at 04:04:22 +0000