---
title: shapeworks::DeepSSMJob
summary: Qt Wrapper for DeepSSM. 

---

# shapeworks::DeepSSMJob



Qt Wrapper for DeepSSM.  [More...](#detailed-description)


`#include <DeepSSMJob.h>`

Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum class| **[FileType](../Classes/classshapeworks_1_1DeepSSMJob.md#enum-filetype)** { ID, IMAGE, PARTICLES} |
| enum class| **[SplitType](../Classes/classshapeworks_1_1DeepSSMJob.md#enum-splittype)** { TRAIN, TEST} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md#function-deepssmjob)**(ProjectHandle project, DeepSSMTool::ToolMode tool_mode) |
| | **[~DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md#function-~deepssmjob)**() |
| virtual void | **[run](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1DeepSSMJob.md#function-name)**() override |
| void | **[run_augmentation](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-augmentation)**() |
| void | **[run_training](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-training)**() |
| void | **[run_testing](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-testing)**() |
| void | **[python_message](../Classes/classshapeworks_1_1DeepSSMJob.md#function-python-message)**(std::string str) |
| std::vector< std::string > | **[get_list](../Classes/classshapeworks_1_1DeepSSMJob.md#function-get-list)**(FileType file_type, SplitType split_type) |

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


## Detailed Description

```cpp
class shapeworks::DeepSSMJob;
```

Qt Wrapper for DeepSSM. 

The [DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md) class wraps the functionality for DeepSSM as a Studio [Job](../Classes/classshapeworks_1_1Job.md) object 

## Public Types Documentation

### enum FileType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| ID | |   |
| IMAGE | |   |
| PARTICLES | |   |




### enum SplitType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| TRAIN | |   |
| TEST | |   |




## Public Functions Documentation

### function DeepSSMJob

```cpp
DeepSSMJob(
    ProjectHandle project,
    DeepSSMTool::ToolMode tool_mode
)
```


### function ~DeepSSMJob

```cpp
~DeepSSMJob()
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


### function run_augmentation

```cpp
void run_augmentation()
```


### function run_training

```cpp
void run_training()
```


### function run_testing

```cpp
void run_testing()
```


### function python_message

```cpp
void python_message(
    std::string str
)
```


### function get_list

```cpp
std::vector< std::string > get_list(
    FileType file_type,
    SplitType split_type
)
```


-------------------------------

Updated on 2022-02-16 at 06:47:27 +0000