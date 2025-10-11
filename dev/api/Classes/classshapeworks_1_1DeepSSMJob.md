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
| enum class| **[JobType](../Classes/classshapeworks_1_1DeepSSMJob.md#enum-jobtype)** { DeepSSM_PrepType = 0, DeepSSM_AugmentationType = 1, DeepSSM_TrainingType = 2, DeepSSM_TestingType = 3} |
| enum| **[PrepStep](../Classes/classshapeworks_1_1DeepSSMJob.md#enum-prepstep)** { NOT_STARTED = 0, GROOM_TRAINING = 1, OPTIMIZE_TRAINING = 2, OPTIMIZE_VALIDATION = 3, GROOM_IMAGES = 4, DONE = 5} |
| enum class| **[SplitType](../Classes/classshapeworks_1_1DeepSSMJob.md#enum-splittype)** { TRAIN, VAL, TEST} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md#function-deepssmjob)**(std::shared_ptr< [Project](../Classes/classshapeworks_1_1Project.md) > project, DeepSSMJob::JobType tool_mode, DeepSSMJob::PrepStep prep_step =DeepSSMJob::NOT_STARTED) |
| | **[~DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md#function-~deepssmjob)**() |
| virtual void | **[run](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1DeepSSMJob.md#function-name)**() override<br>get the name of the job  |
| void | **[run_prep](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-prep)**() |
| void | **[run_augmentation](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-augmentation)**() |
| void | **[run_training](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-training)**() |
| void | **[run_testing](../Classes/classshapeworks_1_1DeepSSMJob.md#function-run-testing)**() |
| void | **[python_message](../Classes/classshapeworks_1_1DeepSSMJob.md#function-python-message)**(std::string str) |
| void | **[set_num_dataloader_workers](../Classes/classshapeworks_1_1DeepSSMJob.md#function-set-num-dataloader-workers)**(int num_workers) |
| int | **[get_num_dataloader_workers](../Classes/classshapeworks_1_1DeepSSMJob.md#function-get-num-dataloader-workers)**() |
| void | **[set_prep_step](../Classes/classshapeworks_1_1DeepSSMJob.md#function-set-prep-step)**(DeepSSMJob::PrepStep step) |
| std::vector< int > | **[get_split](../Classes/classshapeworks_1_1DeepSSMJob.md#function-get-split)**(ProjectHandle project, DeepSSMJob::SplitType split_type) |

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


## Detailed Description

```cpp
class shapeworks::DeepSSMJob;
```

Qt Wrapper for DeepSSM. 

The [DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md) class wraps the functionality for DeepSSM as a Studio [Job](../Classes/classshapeworks_1_1Job.md) object 

## Public Types Documentation

### enum JobType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| DeepSSM_PrepType | 0|   |
| DeepSSM_AugmentationType | 1|   |
| DeepSSM_TrainingType | 2|   |
| DeepSSM_TestingType | 3|   |




### enum PrepStep

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| NOT_STARTED | 0|   |
| GROOM_TRAINING | 1|   |
| OPTIMIZE_TRAINING | 2|   |
| OPTIMIZE_VALIDATION | 3|   |
| GROOM_IMAGES | 4|   |
| DONE | 5|   |




### enum SplitType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| TRAIN | |   |
| VAL | |   |
| TEST | |   |




## Public Functions Documentation

### function DeepSSMJob

```cpp
DeepSSMJob(
    std::shared_ptr< Project > project,
    DeepSSMJob::JobType tool_mode,
    DeepSSMJob::PrepStep prep_step =DeepSSMJob::NOT_STARTED
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

run the job 

**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```

get the name of the job 

**Reimplements**: [shapeworks::Job::name](../Classes/classshapeworks_1_1Job.md#function-name)


### function run_prep

```cpp
void run_prep()
```


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


### function set_num_dataloader_workers

```cpp
void set_num_dataloader_workers(
    int num_workers
)
```


### function get_num_dataloader_workers

```cpp
int get_num_dataloader_workers()
```


### function set_prep_step

```cpp
inline void set_prep_step(
    DeepSSMJob::PrepStep step
)
```


### function get_split

```cpp
static std::vector< int > get_split(
    ProjectHandle project,
    DeepSSMJob::SplitType split_type
)
```


-------------------------------

Updated on 2025-10-11 at 18:16:33 +0000