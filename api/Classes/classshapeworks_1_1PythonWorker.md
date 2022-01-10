---
title: shapeworks::PythonWorker

---

# shapeworks::PythonWorker





Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| bool | **[init](../Classes/classshapeworks_1_1PythonWorker.md#slot-init)**() |
| void | **[start_job](../Classes/classshapeworks_1_1PythonWorker.md#slot-start-job)**(QSharedPointer< [Job](../Classes/classshapeworks_1_1Job.md) > job) |
| void | **[finalize_python](../Classes/classshapeworks_1_1PythonWorker.md#slot-finalize-python)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[result_ready](../Classes/classshapeworks_1_1PythonWorker.md#signal-result-ready)**() |
| void | **[error_message](../Classes/classshapeworks_1_1PythonWorker.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1PythonWorker.md#signal-warning-message)**(QString ) |
| void | **[message](../Classes/classshapeworks_1_1PythonWorker.md#signal-message)**(QString ) |
| void | **[finished](../Classes/classshapeworks_1_1PythonWorker.md#signal-finished)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[PythonWorker](../Classes/classshapeworks_1_1PythonWorker.md#function-pythonworker)**() |
| | **[~PythonWorker](../Classes/classshapeworks_1_1PythonWorker.md#function-~pythonworker)**() |
| void | **[set_vtk_output_window](../Classes/classshapeworks_1_1PythonWorker.md#function-set-vtk-output-window)**(vtkSmartPointer< [StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md) > output_window) |
| void | **[run_job](../Classes/classshapeworks_1_1PythonWorker.md#function-run-job)**(QSharedPointer< [Job](../Classes/classshapeworks_1_1Job.md) > job) |
| void | **[incoming_python_message](../Classes/classshapeworks_1_1PythonWorker.md#function-incoming-python-message)**(std::string message_string) |
| void | **[incoming_python_progress](../Classes/classshapeworks_1_1PythonWorker.md#function-incoming-python-progress)**(double value) |
| void | **[end_python](../Classes/classshapeworks_1_1PythonWorker.md#function-end-python)**() |
| void | **[abort_job](../Classes/classshapeworks_1_1PythonWorker.md#function-abort-job)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static const char * | **[python_api_version](../Classes/classshapeworks_1_1PythonWorker.md#variable-python-api-version)**  |

## Public Slots Documentation

### slot init

```cpp
bool init()
```


### slot start_job

```cpp
void start_job(
    QSharedPointer< Job > job
)
```


### slot finalize_python

```cpp
void finalize_python()
```


## Public Signals Documentation

### signal result_ready

```cpp
void result_ready()
```


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

### function PythonWorker

```cpp
PythonWorker()
```


### function ~PythonWorker

```cpp
~PythonWorker()
```


### function set_vtk_output_window

```cpp
void set_vtk_output_window(
    vtkSmartPointer< StudioVtkOutputWindow > output_window
)
```


### function run_job

```cpp
void run_job(
    QSharedPointer< Job > job
)
```


### function incoming_python_message

```cpp
void incoming_python_message(
    std::string message_string
)
```


### function incoming_python_progress

```cpp
void incoming_python_progress(
    double value
)
```


### function end_python

```cpp
void end_python()
```


### function abort_job

```cpp
void abort_job()
```


## Public Attributes Documentation

### variable python_api_version

```cpp
static constexpr static const char * python_api_version = "6.2";
```


-------------------------------

Updated on 2022-01-10 at 16:27:26 +0000