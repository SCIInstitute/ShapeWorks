---
title: Preferences
summary: Application preferences. 

---

# Preferences



Application preferences.  [More...](#detailed-description)


`#include <Preferences.h>`

Inherits from QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[OrientationMarkerType](../Classes/classPreferences.md#enum-orientationmarkertype)** { medical = 0, triad = 1, none = 2} |
| enum| **[OrientationMarkerCorner](../Classes/classPreferences.md#enum-orientationmarkercorner)** { upper_right = 0, lower_right = 1, lower_left = 2, upper_left = 3} |
| enum| **[@0](../Classes/classPreferences.md#enum-@0)** { MAX_RECENT_FILES = 64} |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[color_scheme_changed](../Classes/classPreferences.md#signal-color-scheme-changed)**(int newIndex) |
| void | **[glyph_properties_changed](../Classes/classPreferences.md#signal-glyph-properties-changed)**() |
| void | **[threading_changed_signal](../Classes/classPreferences.md#signal-threading-changed-signal)**() |
| void | **[sliders_changed_signal](../Classes/classPreferences.md#signal-sliders-changed-signal)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Preferences](../Classes/classPreferences.md#function-preferences)**() |
| void | **[restore_defaults](../Classes/classPreferences.md#function-restore-defaults)**() |
| void | **[add_recent_file](../Classes/classPreferences.md#function-add-recent-file)**(QString file, QString path) |
| QStringList | **[get_recent_files](../Classes/classPreferences.md#function-get-recent-files)**() |
| QStringList | **[get_recent_paths](../Classes/classPreferences.md#function-get-recent-paths)**() |
| bool | **[not_saved](../Classes/classPreferences.md#function-not-saved)**() |
| void | **[set_saved](../Classes/classPreferences.md#function-set-saved)**(bool saved =true) |
| QByteArray | **[get_window_geometry](../Classes/classPreferences.md#function-get-window-geometry)**() |
| void | **[set_window_geometry](../Classes/classPreferences.md#function-set-window-geometry)**(QByteArray geometry) |
| QByteArray | **[get_window_state](../Classes/classPreferences.md#function-get-window-state)**() |
| void | **[set_window_state](../Classes/classPreferences.md#function-set-window-state)**(QByteArray state) |
| QString | **[get_last_directory](../Classes/classPreferences.md#function-get-last-directory)**() |
| void | **[set_last_directory](../Classes/classPreferences.md#function-set-last-directory)**(QString value) |
| bool | **[get_cache_enabled](../Classes/classPreferences.md#function-get-cache-enabled)**() |
| void | **[set_cache_enabled](../Classes/classPreferences.md#function-set-cache-enabled)**(bool value) |
| bool | **[get_parallel_enabled](../Classes/classPreferences.md#function-get-parallel-enabled)**() |
| void | **[set_parallel_enabled](../Classes/classPreferences.md#function-set-parallel-enabled)**(bool value) |
| int | **[get_memory_cache_percent](../Classes/classPreferences.md#function-get-memory-cache-percent)**() |
| void | **[set_memory_cache_percent](../Classes/classPreferences.md#function-set-memory-cache-percent)**(int value) |
| int | **[get_num_threads](../Classes/classPreferences.md#function-get-num-threads)**() |
| void | **[set_num_threads](../Classes/classPreferences.md#function-set-num-threads)**(int num_threads) |
| float | **[get_cache_epsilon](../Classes/classPreferences.md#function-get-cache-epsilon)**() |
| void | **[set_cache_epsilon](../Classes/classPreferences.md#function-set-cache-epsilon)**(float value) |
| float | **[get_glyph_size](../Classes/classPreferences.md#function-get-glyph-size)**() |
| void | **[set_glyph_size](../Classes/classPreferences.md#function-set-glyph-size)**(float value) |
| float | **[get_glyph_quality](../Classes/classPreferences.md#function-get-glyph-quality)**() |
| void | **[set_glyph_quality](../Classes/classPreferences.md#function-set-glyph-quality)**(float value) |
| bool | **[get_glyph_auto_size](../Classes/classPreferences.md#function-get-glyph-auto-size)**() |
| void | **[set_glyph_auto_size](../Classes/classPreferences.md#function-set-glyph-auto-size)**(bool value) |
| float | **[get_pca_range](../Classes/classPreferences.md#function-get-pca-range)**() |
| void | **[set_pca_range](../Classes/classPreferences.md#function-set-pca-range)**(float value) |
| int | **[get_pca_steps](../Classes/classPreferences.md#function-get-pca-steps)**() |
| void | **[set_pca_steps](../Classes/classPreferences.md#function-set-pca-steps)**(int value) |
| void | **[set_color_scheme](../Classes/classPreferences.md#function-set-color-scheme)**(int value) |
| int | **[get_color_scheme](../Classes/classPreferences.md#function-get-color-scheme)**() |
| bool | **[get_center_checked](../Classes/classPreferences.md#function-get-center-checked)**() |
| void | **[set_center_checked](../Classes/classPreferences.md#function-set-center-checked)**(bool value) |
| int | **[get_geodesic_cache_multiplier](../Classes/classPreferences.md#function-get-geodesic-cache-multiplier)**() |
| void | **[set_geodesic_cache_multiplier](../Classes/classPreferences.md#function-set-geodesic-cache-multiplier)**(int value) |
| OrientationMarkerType | **[get_orientation_marker_type](../Classes/classPreferences.md#function-get-orientation-marker-type)**() |
| void | **[set_orientation_marker_type](../Classes/classPreferences.md#function-set-orientation-marker-type)**(OrientationMarkerType type) |
| OrientationMarkerCorner | **[get_orientation_marker_corner](../Classes/classPreferences.md#function-get-orientation-marker-corner)**() |
| void | **[set_orientation_marker_corner](../Classes/classPreferences.md#function-set-orientation-marker-corner)**(OrientationMarkerCorner corner) |
| QString | **[get_groom_file_template](../Classes/classPreferences.md#function-get-groom-file-template)**() |
| void | **[set_groom_file_template](../Classes/classPreferences.md#function-set-groom-file-template)**(QString groom_file_template) |
| QString | **[get_optimize_file_template](../Classes/classPreferences.md#function-get-optimize-file-template)**() |
| void | **[set_optimize_file_template](../Classes/classPreferences.md#function-set-optimize-file-template)**(QString optimize_file_template) |

## Detailed Description

```cpp
class Preferences;
```

Application preferences. 

The [Preferences](../Classes/classPreferences.md) singleton controls all preferences for the application. Values persist via the QSettings class. 

## Public Types Documentation

### enum OrientationMarkerType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| medical | 0|   |
| triad | 1|   |
| none | 2|   |




### enum OrientationMarkerCorner

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| upper_right | 0|   |
| lower_right | 1|   |
| lower_left | 2|   |
| upper_left | 3|   |




### enum @0

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| MAX_RECENT_FILES | 64|   |




## Public Signals Documentation

### signal color_scheme_changed

```cpp
void color_scheme_changed(
    int newIndex
)
```


### signal glyph_properties_changed

```cpp
void glyph_properties_changed()
```


### signal threading_changed_signal

```cpp
void threading_changed_signal()
```


### signal sliders_changed_signal

```cpp
void sliders_changed_signal()
```


## Public Functions Documentation

### function Preferences

```cpp
Preferences()
```


### function restore_defaults

```cpp
void restore_defaults()
```


### function add_recent_file

```cpp
void add_recent_file(
    QString file,
    QString path
)
```


### function get_recent_files

```cpp
QStringList get_recent_files()
```


### function get_recent_paths

```cpp
QStringList get_recent_paths()
```


### function not_saved

```cpp
bool not_saved()
```


### function set_saved

```cpp
void set_saved(
    bool saved =true
)
```


### function get_window_geometry

```cpp
QByteArray get_window_geometry()
```


### function set_window_geometry

```cpp
void set_window_geometry(
    QByteArray geometry
)
```


### function get_window_state

```cpp
QByteArray get_window_state()
```


### function set_window_state

```cpp
void set_window_state(
    QByteArray state
)
```


### function get_last_directory

```cpp
QString get_last_directory()
```


### function set_last_directory

```cpp
void set_last_directory(
    QString value
)
```


### function get_cache_enabled

```cpp
bool get_cache_enabled()
```


### function set_cache_enabled

```cpp
void set_cache_enabled(
    bool value
)
```


### function get_parallel_enabled

```cpp
bool get_parallel_enabled()
```


### function set_parallel_enabled

```cpp
void set_parallel_enabled(
    bool value
)
```


### function get_memory_cache_percent

```cpp
int get_memory_cache_percent()
```


### function set_memory_cache_percent

```cpp
void set_memory_cache_percent(
    int value
)
```


### function get_num_threads

```cpp
int get_num_threads()
```


### function set_num_threads

```cpp
void set_num_threads(
    int num_threads
)
```


### function get_cache_epsilon

```cpp
float get_cache_epsilon()
```


### function set_cache_epsilon

```cpp
void set_cache_epsilon(
    float value
)
```


### function get_glyph_size

```cpp
float get_glyph_size()
```


### function set_glyph_size

```cpp
void set_glyph_size(
    float value
)
```


### function get_glyph_quality

```cpp
float get_glyph_quality()
```


### function set_glyph_quality

```cpp
void set_glyph_quality(
    float value
)
```


### function get_glyph_auto_size

```cpp
bool get_glyph_auto_size()
```


### function set_glyph_auto_size

```cpp
void set_glyph_auto_size(
    bool value
)
```


### function get_pca_range

```cpp
float get_pca_range()
```


### function set_pca_range

```cpp
void set_pca_range(
    float value
)
```


### function get_pca_steps

```cpp
int get_pca_steps()
```


### function set_pca_steps

```cpp
void set_pca_steps(
    int value
)
```


### function set_color_scheme

```cpp
void set_color_scheme(
    int value
)
```


### function get_color_scheme

```cpp
int get_color_scheme()
```


### function get_center_checked

```cpp
bool get_center_checked()
```


### function set_center_checked

```cpp
void set_center_checked(
    bool value
)
```


### function get_geodesic_cache_multiplier

```cpp
int get_geodesic_cache_multiplier()
```


### function set_geodesic_cache_multiplier

```cpp
void set_geodesic_cache_multiplier(
    int value
)
```


### function get_orientation_marker_type

```cpp
OrientationMarkerType get_orientation_marker_type()
```


### function set_orientation_marker_type

```cpp
void set_orientation_marker_type(
    OrientationMarkerType type
)
```


### function get_orientation_marker_corner

```cpp
OrientationMarkerCorner get_orientation_marker_corner()
```


### function set_orientation_marker_corner

```cpp
void set_orientation_marker_corner(
    OrientationMarkerCorner corner
)
```


### function get_groom_file_template

```cpp
QString get_groom_file_template()
```


### function set_groom_file_template

```cpp
void set_groom_file_template(
    QString groom_file_template
)
```


### function get_optimize_file_template

```cpp
QString get_optimize_file_template()
```


### function set_optimize_file_template

```cpp
void set_optimize_file_template(
    QString optimize_file_template
)
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000