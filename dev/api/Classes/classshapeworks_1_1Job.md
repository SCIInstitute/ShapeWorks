---
title: shapeworks::Job

---

# shapeworks::Job





Inherits from QObject

Inherited by [monailabel::MonaiLabelJob](../Classes/classmonailabel_1_1MonaiLabelJob.md), [shapeworks::DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md), [shapeworks::GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md), [shapeworks::NetworkAnalysisJob](../Classes/classshapeworks_1_1NetworkAnalysisJob.md), [shapeworks::ParticleAreaJob](../Classes/classshapeworks_1_1ParticleAreaJob.md), [shapeworks::ParticleNormalEvaluationJob](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md), [shapeworks::ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md), [shapeworks::ShapeScalarJob](../Classes/classshapeworks_1_1ShapeScalarJob.md), [shapeworks::StatsGroupLDAJob](../Classes/classshapeworks_1_1StatsGroupLDAJob.md)

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double , QString  ="") |
| void | **[finished](../Classes/classshapeworks_1_1Job.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Job](../Classes/classshapeworks_1_1Job.md#function-job)**() |
| virtual | **[~Job](../Classes/classshapeworks_1_1Job.md#function-~job)**() |
| virtual void | **[run](../Classes/classshapeworks_1_1Job.md#function-run)**() =0<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1Job.md#function-name)**() =0<br>get the name of the job  |
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

### signal progress

```cpp
void progress(
    double ,
    QString  =""
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

run the job 

**Reimplemented by**: [shapeworks::ShapeEvaluationJob::run](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-run), [shapeworks::DeepSSMJob::run](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run), [shapeworks::GroupPvalueJob::run](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-run), [shapeworks::NetworkAnalysisJob::run](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-run), [shapeworks::ParticleAreaJob::run](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-run), [shapeworks::ParticleNormalEvaluationJob::run](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-run), [shapeworks::ShapeScalarJob::run](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-run), [shapeworks::StatsGroupLDAJob::run](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-run), [monailabel::MonaiLabelJob::run](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-run)


### function name

```cpp
virtual QString name() =0
```

get the name of the job 

**Reimplemented by**: [shapeworks::ShapeEvaluationJob::name](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-name), [shapeworks::DeepSSMJob::name](../Classes/classshapeworks_1_1DeepSSMJob.md#function-name), [shapeworks::GroupPvalueJob::name](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-name), [shapeworks::NetworkAnalysisJob::name](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-name), [shapeworks::ParticleAreaJob::name](../Classes/classshapeworks_1_1ParticleAreaJob.md#function-name), [shapeworks::ParticleNormalEvaluationJob::name](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md#function-name), [shapeworks::ShapeScalarJob::name](../Classes/classshapeworks_1_1ShapeScalarJob.md#function-name), [shapeworks::StatsGroupLDAJob::name](../Classes/classshapeworks_1_1StatsGroupLDAJob.md#function-name), [monailabel::MonaiLabelJob::name](../Classes/classmonailabel_1_1MonaiLabelJob.md#function-name)


### function get_completion_message

```cpp
virtual QString get_completion_message()
```

get a message to display when the job is complete 

### function get_abort_message

```cpp
virtual QString get_abort_message()
```

get a message to display when the job is aborted 

### function start_timer

```cpp
void start_timer()
```

start the timer 

### function timer_elapsed

```cpp
qint64 timer_elapsed()
```

how much time has elapsed since the timer was started 

### function set_complete

```cpp
inline void set_complete(
    bool complete
)
```

set the job as complete 

### function is_complete

```cpp
inline bool is_complete() const
```

is the job complete? 

### function abort

```cpp
inline void abort()
```

abort the job 

### function is_aborted

```cpp
inline bool is_aborted() const
```

was the job aborted? 

### function set_quiet_mode

```cpp
inline void set_quiet_mode(
    bool quiet
)
```

set to quiet mode (no progress messages) 

### function get_quiet_mode

```cpp
inline bool get_quiet_mode()
```

get quiet mode 

-------------------------------

Updated on 2025-05-19 at 18:59:07 +0000