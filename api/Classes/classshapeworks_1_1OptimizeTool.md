---
title: shapeworks::OptimizeTool

---

# shapeworks::OptimizeTool





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[on_run_optimize_button_clicked](../Classes/classshapeworks_1_1OptimizeTool.md#slot-on-run-optimize-button-clicked)**()<br>Run optimize tool.  |
| void | **[on_restoreDefaults_clicked](../Classes/classshapeworks_1_1OptimizeTool.md#slot-on-restoredefaults-clicked)**() |
| void | **[handle_optimize_complete](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-optimize-complete)**() |
| void | **[handle_optimize_failed](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-optimize-failed)**() |
| void | **[handle_progress](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-progress)**(int val, QString message) |
| void | **[handle_error](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-error)**(QString ) |
| void | **[handle_warning](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-warning)**(QString ) |
| void | **[handle_message](../Classes/classshapeworks_1_1OptimizeTool.md#slot-handle-message)**(QString ) |
| void | **[update_ui_elements](../Classes/classshapeworks_1_1OptimizeTool.md#slot-update-ui-elements)**() |
| bool | **[validate_inputs](../Classes/classshapeworks_1_1OptimizeTool.md#slot-validate-inputs)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[optimize_start](../Classes/classshapeworks_1_1OptimizeTool.md#signal-optimize-start)**() |
| void | **[optimize_complete](../Classes/classshapeworks_1_1OptimizeTool.md#signal-optimize-complete)**() |
| void | **[error_message](../Classes/classshapeworks_1_1OptimizeTool.md#signal-error-message)**(QString ) |
| void | **[warning_message](../Classes/classshapeworks_1_1OptimizeTool.md#signal-warning-message)**(QString ) |
| void | **[progress](../Classes/classshapeworks_1_1OptimizeTool.md#signal-progress)**(int ) |
| void | **[message](../Classes/classshapeworks_1_1OptimizeTool.md#signal-message)**(QString ) |
| void | **[status](../Classes/classshapeworks_1_1OptimizeTool.md#signal-status)**(QString ) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[OptimizeTool](../Classes/classshapeworks_1_1OptimizeTool.md#function-optimizetool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~OptimizeTool](../Classes/classshapeworks_1_1OptimizeTool.md#function-~optimizetool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1OptimizeTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the project  |
| void | **[activate](../Classes/classshapeworks_1_1OptimizeTool.md#function-activate)**()<br>activate this tool  |
| void | **[load_params](../Classes/classshapeworks_1_1OptimizeTool.md#function-load-params)**()<br>Load params from project.  |
| void | **[store_params](../Classes/classshapeworks_1_1OptimizeTool.md#function-store-params)**()<br>Store params to project.  |
| void | **[enable_actions](../Classes/classshapeworks_1_1OptimizeTool.md#function-enable-actions)**()<br>Enable action buttons.  |
| void | **[disable_actions](../Classes/classshapeworks_1_1OptimizeTool.md#function-disable-actions)**()<br>Disable action buttons.  |
| void | **[shutdown_threads](../Classes/classshapeworks_1_1OptimizeTool.md#function-shutdown-threads)**()<br>shut down any running threads  |

## Public Slots Documentation

### slot on_run_optimize_button_clicked

```cpp
void on_run_optimize_button_clicked()
```

Run optimize tool. 

### slot on_restoreDefaults_clicked

```cpp
void on_restoreDefaults_clicked()
```


### slot handle_optimize_complete

```cpp
void handle_optimize_complete()
```


### slot handle_optimize_failed

```cpp
void handle_optimize_failed()
```


### slot handle_progress

```cpp
void handle_progress(
    int val,
    QString message
)
```


### slot handle_error

```cpp
void handle_error(
    QString 
)
```


### slot handle_warning

```cpp
void handle_warning(
    QString 
)
```


### slot handle_message

```cpp
void handle_message(
    QString 
)
```


### slot update_ui_elements

```cpp
void update_ui_elements()
```


### slot validate_inputs

```cpp
bool validate_inputs()
```


## Public Signals Documentation

### signal optimize_start

```cpp
void optimize_start()
```


### signal optimize_complete

```cpp
void optimize_complete()
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


### signal progress

```cpp
void progress(
    int 
)
```


### signal message

```cpp
void message(
    QString 
)
```


### signal status

```cpp
void status(
    QString 
)
```


## Public Functions Documentation

### function OptimizeTool

```cpp
OptimizeTool(
    Preferences & prefs
)
```


### function ~OptimizeTool

```cpp
~OptimizeTool()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

set the pointer to the project 

### function activate

```cpp
void activate()
```

activate this tool 

### function load_params

```cpp
void load_params()
```

Load params from project. 

### function store_params

```cpp
void store_params()
```

Store params to project. 

### function enable_actions

```cpp
void enable_actions()
```

Enable action buttons. 

### function disable_actions

```cpp
void disable_actions()
```

Disable action buttons. 

### function shutdown_threads

```cpp
void shutdown_threads()
```

shut down any running threads 

-------------------------------

Updated on 2022-01-07 at 20:14:45 +0000