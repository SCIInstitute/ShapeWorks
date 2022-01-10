---
title: shapeworks::Job

---

# shapeworks::Job





Inherits from QObject

Inherited by [shapeworks::DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md), [shapeworks::GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md), [shapeworks::ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md)

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[error_message](../Classes/classshapeworks_1_1Job.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1Job.md#signal-warning-message)**(QString ) |
| void | **[progress](../Classes/classshapeworks_1_1Job.md#signal-progress)**(double ) |
| void | **[message](../Classes/classshapeworks_1_1Job.md#signal-message)**(QString ) |
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

### signal error_message

```cpp
void error_message(
    QString 
)
```


### signal warning_message

```cpp
void warning_message(
    QString 
)
```


### signal progress

```cpp
void progress(
    double 
)
```


### signal message

```cpp
void message(
    QString 
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


**Reimplemented by**: [shapeworks::ShapeEvaluationJob::run](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-run), [shapeworks::DeepSSMJob::run](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run), [shapeworks::GroupPvalueJob::run](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-run)


### function name

```cpp
virtual QString name() =0
```


**Reimplemented by**: [shapeworks::ShapeEvaluationJob::name](../Classes/classshapeworks_1_1ShapeEvaluationJob.md#function-name), [shapeworks::DeepSSMJob::name](../Classes/classshapeworks_1_1DeepSSMJob.md#function-name), [shapeworks::GroupPvalueJob::name](../Classes/classshapeworks_1_1GroupPvalueJob.md#function-name)


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

Updated on 2022-01-10 at 16:27:26 +0000