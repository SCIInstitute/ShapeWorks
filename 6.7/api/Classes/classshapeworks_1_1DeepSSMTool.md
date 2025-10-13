---
title: shapeworks::DeepSSMTool

---

# shapeworks::DeepSSMTool





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[run_clicked](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-run-clicked)**() |
| void | **[run_prep_clicked](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-run-prep-clicked)**(int step) |
| void | **[restore_defaults](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-restore-defaults)**() |
| void | **[handle_thread_complete](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-handle-thread-complete)**() |
| void | **[handle_progress](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-handle-progress)**(int val, QString message) |
| void | **[handle_error](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-handle-error)**(QString msg) |
| void | **[tab_changed](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-tab-changed)**(int tab) |
| void | **[update_panels](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-update-panels)**() |
| void | **[update_split](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-update-split)**() |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-handle-new-mesh)**() |
| void | **[training_fine_tuning_changed](../Classes/classshapeworks_1_1DeepSSMTool.md#slot-training-fine-tuning-changed)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classshapeworks_1_1DeepSSMTool.md#signal-update-view)**() |
| void | **[progress](../Classes/classshapeworks_1_1DeepSSMTool.md#signal-progress)**(int ) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DeepSSMTool](../Classes/classshapeworks_1_1DeepSSMTool.md#function-deepssmtool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~DeepSSMTool](../Classes/classshapeworks_1_1DeepSSMTool.md#function-~deepssmtool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1DeepSSMTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the session  |
| void | **[set_app](../Classes/classshapeworks_1_1DeepSSMTool.md#function-set-app)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app)<br>set the pointer to the application  |
| bool | **[is_active](../Classes/classshapeworks_1_1DeepSSMTool.md#function-is-active)**()<br>Return if this tool is active.  |
| void | **[load_params](../Classes/classshapeworks_1_1DeepSSMTool.md#function-load-params)**() |
| void | **[store_params](../Classes/classshapeworks_1_1DeepSSMTool.md#function-store-params)**() |
| void | **[shutdown](../Classes/classshapeworks_1_1DeepSSMTool.md#function-shutdown)**() |
| ShapeList | **[get_shapes](../Classes/classshapeworks_1_1DeepSSMTool.md#function-get-shapes)**() |
| void | **[resizeEvent](../Classes/classshapeworks_1_1DeepSSMTool.md#function-resizeevent)**(QResizeEvent * event) override |
| std::string | **[get_display_feature](../Classes/classshapeworks_1_1DeepSSMTool.md#function-get-display-feature)**() |

## Public Slots Documentation

### slot run_clicked

```cpp
void run_clicked()
```


### slot run_prep_clicked

```cpp
void run_prep_clicked(
    int step
)
```


### slot restore_defaults

```cpp
void restore_defaults()
```


### slot handle_thread_complete

```cpp
void handle_thread_complete()
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
    QString msg
)
```


### slot tab_changed

```cpp
void tab_changed(
    int tab
)
```


### slot update_panels

```cpp
void update_panels()
```


### slot update_split

```cpp
void update_split()
```


### slot handle_new_mesh

```cpp
void handle_new_mesh()
```


### slot training_fine_tuning_changed

```cpp
void training_fine_tuning_changed()
```


## Public Signals Documentation

### signal update_view

```cpp
void update_view()
```


### signal progress

```cpp
void progress(
    int 
)
```


## Public Functions Documentation

### function DeepSSMTool

```cpp
DeepSSMTool(
    Preferences & prefs
)
```


### function ~DeepSSMTool

```cpp
~DeepSSMTool()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

set the pointer to the session 

### function set_app

```cpp
void set_app(
    ShapeWorksStudioApp * app
)
```

set the pointer to the application 

### function is_active

```cpp
bool is_active()
```

Return if this tool is active. 

### function load_params

```cpp
void load_params()
```


### function store_params

```cpp
void store_params()
```


### function shutdown

```cpp
void shutdown()
```


### function get_shapes

```cpp
ShapeList get_shapes()
```


### function resizeEvent

```cpp
void resizeEvent(
    QResizeEvent * event
) override
```


### function get_display_feature

```cpp
std::string get_display_feature()
```


-------------------------------

Updated on 2025-10-13 at 18:47:49 +0000