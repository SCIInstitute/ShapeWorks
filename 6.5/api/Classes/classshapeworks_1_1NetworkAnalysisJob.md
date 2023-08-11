---
title: shapeworks::NetworkAnalysisJob

---

# shapeworks::NetworkAnalysisJob





Inherits from [shapeworks::Job](../Classes/classshapeworks_1_1Job.md), QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[NetworkAnalysisJob](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-networkanalysisjob)**(std::shared_ptr< [Project](../Classes/classshapeworks_1_1Project.md) > project, std::string target_group, std::string target_feature) |
| virtual void | **[run](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-run)**() override |
| virtual QString | **[name](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-name)**() override |
| void | **[set_pvalue_of_interest](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-pvalue-of-interest)**(double pvalue_of_interest) |
| void | **[set_pvalue_threshold](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-pvalue-threshold)**(double pvalue_threshold) |
| void | **[set_num_iterations](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-set-num-iterations)**(int num_iterations) |
| Eigen::VectorXf | **[get_tvalues](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-get-tvalues)**() |
| Eigen::VectorXf | **[get_spm_values](../Classes/classshapeworks_1_1NetworkAnalysisJob.md#function-get-spm-values)**() |

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
| virtual QString | **[get_completion_message](../Classes/classshapeworks_1_1Job.md#function-get-completion-message)**() |
| void | **[start_timer](../Classes/classshapeworks_1_1Job.md#function-start-timer)**() |
| qint64 | **[timer_elapsed](../Classes/classshapeworks_1_1Job.md#function-timer-elapsed)**() |


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


**Reimplements**: [shapeworks::Job::run](../Classes/classshapeworks_1_1Job.md#function-run)


### function name

```cpp
virtual QString name() override
```


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
Eigen::VectorXf get_tvalues()
```


### function get_spm_values

```cpp
Eigen::VectorXf get_spm_values()
```


-------------------------------

Updated on 2023-08-11 at 07:52:38 +0000