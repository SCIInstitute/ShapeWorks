---
title: shapeworks::GroomTool

---

# shapeworks::GroomTool





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[on_antialias_checkbox_stateChanged](../Classes/classshapeworks_1_1GroomTool.md#slot-on-antialias-checkbox-statechanged)**(int state) |
| void | **[on_blur_checkbox_stateChanged](../Classes/classshapeworks_1_1GroomTool.md#slot-on-blur-checkbox-statechanged)**(int state) |
| void | **[on_autopad_checkbox_stateChanged](../Classes/classshapeworks_1_1GroomTool.md#slot-on-autopad-checkbox-statechanged)**(int state) |
| void | **[on_skip_button_clicked](../Classes/classshapeworks_1_1GroomTool.md#slot-on-skip-button-clicked)**() |
| void | **[on_restore_defaults_clicked](../Classes/classshapeworks_1_1GroomTool.md#slot-on-restore-defaults-clicked)**() |
| void | **[update_ui](../Classes/classshapeworks_1_1GroomTool.md#slot-update-ui)**() |
| void | **[domain_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-domain-changed)**() |
| void | **[alignment_checkbox_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-alignment-checkbox-changed)**(int state) |
| void | **[alignment_option_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-alignment-option-changed)**(int index) |
| void | **[reflect_checkbox_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-reflect-checkbox-changed)**(int state) |
| void | **[reflect_column_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-reflect-column-changed)**(int index) |
| void | **[reflect_choice_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-reflect-choice-changed)**(int index) |
| void | **[reflect_axis_changed](../Classes/classshapeworks_1_1GroomTool.md#slot-reflect-axis-changed)**(int index) |
| void | **[on_run_groom_button_clicked](../Classes/classshapeworks_1_1GroomTool.md#slot-on-run-groom-button-clicked)**()<br>Run groom tool.  |
| void | **[handle_thread_complete](../Classes/classshapeworks_1_1GroomTool.md#slot-handle-thread-complete)**() |
| void | **[handle_progress](../Classes/classshapeworks_1_1GroomTool.md#slot-handle-progress)**(int val) |
| void | **[handle_error](../Classes/classshapeworks_1_1GroomTool.md#slot-handle-error)**(QString msg) |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[groom_start](../Classes/classshapeworks_1_1GroomTool.md#signal-groom-start)**() |
| void | **[groom_complete](../Classes/classshapeworks_1_1GroomTool.md#signal-groom-complete)**() |
| void | **[error_message](../Classes/classshapeworks_1_1GroomTool.md#signal-error-message)**(QString ) |
| void | **[message](../Classes/classshapeworks_1_1GroomTool.md#signal-message)**(QString ) |
| void | **[progress](../Classes/classshapeworks_1_1GroomTool.md#signal-progress)**(int ) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[GroomTool](../Classes/classshapeworks_1_1GroomTool.md#function-groomtool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~GroomTool](../Classes/classshapeworks_1_1GroomTool.md#function-~groomtool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1GroomTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>Set the pointer to the session.  |
| void | **[activate](../Classes/classshapeworks_1_1GroomTool.md#function-activate)**()<br>activate this tool  |
| void | **[load_params](../Classes/classshapeworks_1_1GroomTool.md#function-load-params)**()<br>Load params from project.  |
| void | **[store_params](../Classes/classshapeworks_1_1GroomTool.md#function-store-params)**()<br>Store params to project.  |
| void | **[disable_actions](../Classes/classshapeworks_1_1GroomTool.md#function-disable-actions)**()<br>Disable action buttons.  |
| void | **[enable_actions](../Classes/classshapeworks_1_1GroomTool.md#function-enable-actions)**()<br>Enable action buttons.  |
| void | **[shutdown_threads](../Classes/classshapeworks_1_1GroomTool.md#function-shutdown-threads)**()<br>shut down any running threads  |

## Public Slots Documentation

### slot on_antialias_checkbox_stateChanged

```cpp
void on_antialias_checkbox_stateChanged(
    int state
)
```


### slot on_blur_checkbox_stateChanged

```cpp
void on_blur_checkbox_stateChanged(
    int state
)
```


### slot on_autopad_checkbox_stateChanged

```cpp
void on_autopad_checkbox_stateChanged(
    int state
)
```


### slot on_skip_button_clicked

```cpp
void on_skip_button_clicked()
```


### slot on_restore_defaults_clicked

```cpp
void on_restore_defaults_clicked()
```


### slot update_ui

```cpp
void update_ui()
```


### slot domain_changed

```cpp
void domain_changed()
```


### slot alignment_checkbox_changed

```cpp
void alignment_checkbox_changed(
    int state
)
```


### slot alignment_option_changed

```cpp
void alignment_option_changed(
    int index
)
```


### slot reflect_checkbox_changed

```cpp
void reflect_checkbox_changed(
    int state
)
```


### slot reflect_column_changed

```cpp
void reflect_column_changed(
    int index
)
```


### slot reflect_choice_changed

```cpp
void reflect_choice_changed(
    int index
)
```


### slot reflect_axis_changed

```cpp
void reflect_axis_changed(
    int index
)
```


### slot on_run_groom_button_clicked

```cpp
void on_run_groom_button_clicked()
```

Run groom tool. 

### slot handle_thread_complete

```cpp
void handle_thread_complete()
```


### slot handle_progress

```cpp
void handle_progress(
    int val
)
```


### slot handle_error

```cpp
void handle_error(
    QString msg
)
```


## Public Signals Documentation

### signal groom_start

```cpp
void groom_start()
```


### signal groom_complete

```cpp
void groom_complete()
```


### signal error_message

```cpp
void error_message(
    QString 
)
```


### signal message

```cpp
void message(
    QString 
)
```


### signal progress

```cpp
void progress(
    int 
)
```


## Public Functions Documentation

### function GroomTool

```cpp
GroomTool(
    Preferences & prefs
)
```


### function ~GroomTool

```cpp
~GroomTool()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

Set the pointer to the session. 

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

### function disable_actions

```cpp
void disable_actions()
```

Disable action buttons. 

### function enable_actions

```cpp
void enable_actions()
```

Enable action buttons. 

### function shutdown_threads

```cpp
void shutdown_threads()
```

shut down any running threads 

-------------------------------

Updated on 2022-01-06 at 21:52:21 +0000