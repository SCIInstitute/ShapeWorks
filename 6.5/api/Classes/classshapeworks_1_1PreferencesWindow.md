---
title: shapeworks::PreferencesWindow
summary: Qt UI dialog to control preferences. 

---

# shapeworks::PreferencesWindow



Qt UI dialog to control preferences.  [More...](#detailed-description)


`#include <PreferencesWindow.h>`

Inherits from QDialog

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[on_mesh_cache_enabled_stateChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-mesh-cache-enabled-statechanged)**(int state) |
| void | **[on_mesh_cache_memory_valueChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-mesh-cache-memory-valuechanged)**(int value) |
| void | **[on_color_scheme_currentIndexChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-color-scheme-currentindexchanged)**(int index) |
| void | **[on_pca_range_valueChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-pca-range-valuechanged)**(double value) |
| void | **[on_pca_steps_valueChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-pca-steps-valuechanged)**(int value) |
| void | **[on_parallel_enabled_toggled](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-parallel-enabled-toggled)**(bool b) |
| void | **[on_num_threads_valueChanged](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-on-num-threads-valuechanged)**(int i) |
| void | **[accept](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-accept)**() override |
| void | **[restore_defaults](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-restore-defaults)**() |
| void | **[save_to_preferences](../Classes/classshapeworks_1_1PreferencesWindow.md#slot-save-to-preferences)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[clear_cache](../Classes/classshapeworks_1_1PreferencesWindow.md#signal-clear-cache)**() |
| void | **[update_view](../Classes/classshapeworks_1_1PreferencesWindow.md#signal-update-view)**() |
| void | **[slider_update](../Classes/classshapeworks_1_1PreferencesWindow.md#signal-slider-update)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[PreferencesWindow](../Classes/classshapeworks_1_1PreferencesWindow.md#function-preferenceswindow)**(QWidget * parent, [Preferences](../Classes/classPreferences.md) & prefs) |
| void | **[set_values_from_preferences](../Classes/classshapeworks_1_1PreferencesWindow.md#function-set-values-from-preferences)**() |
| void | **[closeEvent](../Classes/classshapeworks_1_1PreferencesWindow.md#function-closeevent)**(QCloseEvent * event) override |

## Detailed Description

```cpp
class shapeworks::PreferencesWindow;
```

Qt UI dialog to control preferences. 

The PreferenceWindow provides controls over preferences for the application. It is synchronized with the singleton Preference object 

## Public Slots Documentation

### slot on_mesh_cache_enabled_stateChanged

```cpp
void on_mesh_cache_enabled_stateChanged(
    int state
)
```


### slot on_mesh_cache_memory_valueChanged

```cpp
void on_mesh_cache_memory_valueChanged(
    int value
)
```


### slot on_color_scheme_currentIndexChanged

```cpp
void on_color_scheme_currentIndexChanged(
    int index
)
```


### slot on_pca_range_valueChanged

```cpp
void on_pca_range_valueChanged(
    double value
)
```


### slot on_pca_steps_valueChanged

```cpp
void on_pca_steps_valueChanged(
    int value
)
```


### slot on_parallel_enabled_toggled

```cpp
void on_parallel_enabled_toggled(
    bool b
)
```


### slot on_num_threads_valueChanged

```cpp
void on_num_threads_valueChanged(
    int i
)
```


### slot accept

```cpp
void accept() override
```


### slot restore_defaults

```cpp
void restore_defaults()
```


### slot save_to_preferences

```cpp
void save_to_preferences()
```


## Public Signals Documentation

### signal clear_cache

```cpp
void clear_cache()
```


### signal update_view

```cpp
void update_view()
```


### signal slider_update

```cpp
void slider_update()
```


## Public Functions Documentation

### function PreferencesWindow

```cpp
PreferencesWindow(
    QWidget * parent,
    Preferences & prefs
)
```


### function set_values_from_preferences

```cpp
void set_values_from_preferences()
```


### function closeEvent

```cpp
void closeEvent(
    QCloseEvent * event
) override
```


-------------------------------

Updated on 2024-01-17 at 16:55:15 +0000