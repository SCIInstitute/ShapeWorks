---
title: shapeworks::Worker

---

# shapeworks::Worker





Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[process](../Classes/classshapeworks_1_1Worker.md#slot-process)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[error_message](../Classes/classshapeworks_1_1Worker.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1Worker.md#signal-warning-message)**(QString ) |
| void | **[status_message](../Classes/classshapeworks_1_1Worker.md#signal-status-message)**(QString ) |
| void | **[progress](../Classes/classshapeworks_1_1Worker.md#signal-progress)**(int ) |
| void | **[finished](../Classes/classshapeworks_1_1Worker.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Worker](../Classes/classshapeworks_1_1Worker.md#function-worker)**() |
| | **[~Worker](../Classes/classshapeworks_1_1Worker.md#function-~worker)**() |
| void | **[run_job](../Classes/classshapeworks_1_1Worker.md#function-run-job)**(QSharedPointer< [Job](../Classes/classshapeworks_1_1Job.md) > job) |
| void | **[abort_job](../Classes/classshapeworks_1_1Worker.md#function-abort-job)**() |
| [Worker](../Classes/classshapeworks_1_1Worker.md) * | **[create_worker](../Classes/classshapeworks_1_1Worker.md#function-create-worker)**() |

## Public Slots Documentation

### slot process

```cpp
void process()
```


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


### signal status_message

```cpp
void status_message(
    QString 
)
```


### signal progress

```cpp
void progress(
    int 
)
```


### signal finished

```cpp
void finished()
```


## Public Functions Documentation

### function Worker

```cpp
Worker()
```


### function ~Worker

```cpp
~Worker()
```


### function run_job

```cpp
void run_job(
    QSharedPointer< Job > job
)
```


### function abort_job

```cpp
void abort_job()
```


### function create_worker

```cpp
static Worker * create_worker()
```


-------------------------------

Updated on 2022-02-02 at 18:18:09 +0000