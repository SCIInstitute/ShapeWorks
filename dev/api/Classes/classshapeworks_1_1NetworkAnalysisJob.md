---
title: shapeworks::NetworkAnalysisJob

---

# shapeworks::NetworkAnalysisJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[NetworkAnalysisJob](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-networkanalysisjob)**(std::shared_ptr< [Project](../Classes/classshapeworks_1_1Project.md) > project, std::string target_group, std::string target_feature) |
| virtual void | **[run](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-run)**() override<br>run the job  |
| virtual QString | **[name](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-name)**() override<br>get the name of the job  |
| void | **[set_pvalue_of_interest](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-pvalue-of-interest)**(double pvalue_of_interest) |
| void | **[set_pvalue_threshold](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-pvalue-threshold)**(double pvalue_threshold) |
| void | **[set_num_iterations](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-num-iterations)**(int num_iterations) |
| Eigen::VectorXd | **[get_tvalues](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-get-tvalues)**() |
| Eigen::VectorXd | **[get_spm_values](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-get-spm-values)**() |

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

### function NetworkAnalysisJob

```cpp
NetworkAnalysisJob(
    std::shared_ptr< Project > project,
    std::string target_group,
    std::string target_feature
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


### function set_pvalue_of_interest

```cpp
void set_pvalue_of_interest(
    double pvalue_of_interest
)
```


### function set_pvalue_threshold

```cpp
void set_pvalue_threshold(
    double pvalue_threshold
)
```


### function set_num_iterations

```cpp
void set_num_iterations(
    int num_iterations
)
```


### function get_tvalues

```cpp
Eigen::VectorXd get_tvalues()
```


### function get_spm_values

```cpp
Eigen::VectorXd get_spm_values()
```


-------------------------------

Updated on 2025-10-10 at 18:16:00 +0000