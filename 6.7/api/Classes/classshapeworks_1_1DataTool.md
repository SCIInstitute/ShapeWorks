---
title: shapeworks::DataTool

---

# shapeworks::DataTool





Inherits from QWidget

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[delete_button_clicked](../Classes/classshapeworks_1_1DataTool.md#slot-delete-button-clicked)**() |
| void | **[delete_landmarks_clicked](../Classes/classshapeworks_1_1DataTool.md#slot-delete-landmarks-clicked)**() |
| void | **[set_placing_button_clicked](../Classes/classshapeworks_1_1DataTool.md#slot-set-placing-button-clicked)**(int id) |
| void | **[handle_landmark_mode_changed](../Classes/classshapeworks_1_1DataTool.md#slot-handle-landmark-mode-changed)**() |
| void | **[handle_constraints_mode_changed](../Classes/classshapeworks_1_1DataTool.md#slot-handle-constraints-mode-changed)**() |
| void | **[constraints_table_right_click](../Classes/classshapeworks_1_1DataTool.md#slot-constraints-table-right-click)**(const QPoint & point) |
| void | **[data_table_right_click](../Classes/classshapeworks_1_1DataTool.md#slot-data-table-right-click)**(const QPoint & point) |
| void | **[copy_ffc_clicked](../Classes/classshapeworks_1_1DataTool.md#slot-copy-ffc-clicked)**() |
| void | **[table_selection_changed](../Classes/classshapeworks_1_1DataTool.md#slot-table-selection-changed)**() |
| void | **[subject_notes_changed](../Classes/classshapeworks_1_1DataTool.md#slot-subject-notes-changed)**() |
| void | **[table_data_edited](../Classes/classshapeworks_1_1DataTool.md#slot-table-data-edited)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[import_button_clicked](../Classes/classshapeworks_1_1DataTool.md#signal-import-button-clicked)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DataTool](../Classes/classshapeworks_1_1DataTool.md#function-datatool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~DataTool](../Classes/classshapeworks_1_1DataTool.md#function-~datatool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1DataTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>Set the pointer to the session.  |
| void | **[activate](../Classes/classshapeworks_1_1DataTool.md#function-activate)**()<br>activate this tool  |
| void | **[disable_actions](../Classes/classshapeworks_1_1DataTool.md#function-disable-actions)**() |
| void | **[enable_actions](../Classes/classshapeworks_1_1DataTool.md#function-enable-actions)**() |
| void | **[update_table](../Classes/classshapeworks_1_1DataTool.md#function-update-table)**(bool clean =true) |
| void | **[update_landmark_table](../Classes/classshapeworks_1_1DataTool.md#function-update-landmark-table)**() |
| void | **[landmark_domain_changed](../Classes/classshapeworks_1_1DataTool.md#function-landmark-domain-changed)**() |
| void | **[constraints_domain_changed](../Classes/classshapeworks_1_1DataTool.md#function-constraints-domain-changed)**() |
| void | **[delete_planes_clicked](../Classes/classshapeworks_1_1DataTool.md#function-delete-planes-clicked)**() |
| void | **[delete_ffc_clicked](../Classes/classshapeworks_1_1DataTool.md#function-delete-ffc-clicked)**() |
| void | **[update_notes](../Classes/classshapeworks_1_1DataTool.md#function-update-notes)**() |
| std::string | **[get_notes](../Classes/classshapeworks_1_1DataTool.md#function-get-notes)**() |
| void | **[store_data](../Classes/classshapeworks_1_1DataTool.md#function-store-data)**() |

## Public Slots Documentation

### slot delete_button_clicked

```cpp
void delete_button_clicked()
```


### slot delete_landmarks_clicked

```cpp
void delete_landmarks_clicked()
```


### slot set_placing_button_clicked

```cpp
void set_placing_button_clicked(
    int id
)
```


### slot handle_landmark_mode_changed

```cpp
void handle_landmark_mode_changed()
```


### slot handle_constraints_mode_changed

```cpp
void handle_constraints_mode_changed()
```


### slot constraints_table_right_click

```cpp
void constraints_table_right_click(
    const QPoint & point
)
```


### slot data_table_right_click

```cpp
void data_table_right_click(
    const QPoint & point
)
```


### slot copy_ffc_clicked

```cpp
void copy_ffc_clicked()
```


### slot table_selection_changed

```cpp
void table_selection_changed()
```


### slot subject_notes_changed

```cpp
void subject_notes_changed()
```


### slot table_data_edited

```cpp
void table_data_edited()
```


## Public Signals Documentation

### signal import_button_clicked

```cpp
void import_button_clicked()
```


## Public Functions Documentation

### function DataTool

```cpp
DataTool(
    Preferences & prefs
)
```


### function ~DataTool

```cpp
~DataTool()
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

### function disable_actions

```cpp
void disable_actions()
```


### function enable_actions

```cpp
void enable_actions()
```


### function update_table

```cpp
void update_table(
    bool clean =true
)
```


### function update_landmark_table

```cpp
void update_landmark_table()
```


### function landmark_domain_changed

```cpp
void landmark_domain_changed()
```


### function constraints_domain_changed

```cpp
void constraints_domain_changed()
```


### function delete_planes_clicked

```cpp
void delete_planes_clicked()
```


### function delete_ffc_clicked

```cpp
void delete_ffc_clicked()
```


### function update_notes

```cpp
void update_notes()
```


### function get_notes

```cpp
std::string get_notes()
```


### function store_data

```cpp
void store_data()
```


-------------------------------

Updated on 2025-06-03 at 17:15:45 +0000