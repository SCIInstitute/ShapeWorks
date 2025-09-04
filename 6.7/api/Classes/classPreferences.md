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
| enum| **[@4](../Classes/classPreferences.md#enum-@4)** { MAX_RECENT_FILES = 64} |

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
| float | **[get_glyph_size](../Classes/classPreferences.md#function-get-glyph-size)**() |
| void | **[set_glyph_size](../Classes/classPreferences.md#function-set-glyph-size)**(float value) |
| float | **[get_glyph_quality](../Classes/classPreferences.md#function-get-glyph-quality)**() |
| void | **[set_glyph_quality](../Classes/classPreferences.md#function-set-glyph-quality)**(float value) |
| bool | **[get_glyph_auto_size](../Classes/classPreferences.md#function-get-glyph-auto-size)**() |
| void | **[set_glyph_auto_size](../Classes/classPreferences.md#function-set-glyph-auto-size)**(bool value) |
| bool | **[get_glyph_scale_arrows](../Classes/classPreferences.md#function-get-glyph-scale-arrows)**() |
| void | **[set_glyph_scale_arrows](../Classes/classPreferences.md#function-set-glyph-scale-arrows)**(bool value) |
| float | **[get_pca_range](../Classes/classPreferences.md#function-get-pca-range)**() |
| void | **[set_pca_range](../Classes/classPreferences.md#function-set-pca-range)**(float value) |
| int | **[get_pca_steps](../Classes/classPreferences.md#function-get-pca-steps)**() |
| void | **[set_pca_steps](../Classes/classPreferences.md#function-set-pca-steps)**(int value) |
| void | **[set_color_scheme](../Classes/classPreferences.md#function-set-color-scheme)**(int value) |
| int | **[get_color_scheme](../Classes/classPreferences.md#function-get-color-scheme)**() |
| void | **[set_particle_colors](../Classes/classPreferences.md#function-set-particle-colors)**(int value) |
| int | **[get_particle_colors](../Classes/classPreferences.md#function-get-particle-colors)**() |
| void | **[set_color_map](../Classes/classPreferences.md#function-set-color-map)**(int value) |
| int | **[get_color_map](../Classes/classPreferences.md#function-get-color-map)**() |
| void | **[set_discrete_color_mode](../Classes/classPreferences.md#function-set-discrete-color-mode)**(bool value) |
| bool | **[get_discrete_color_mode](../Classes/classPreferences.md#function-get-discrete-color-mode)**() |
| void | **[set_reverse_color_map](../Classes/classPreferences.md#function-set-reverse-color-map)**(bool value) |
| bool | **[get_reverse_color_map](../Classes/classPreferences.md#function-get-reverse-color-map)**() |
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
| QSize | **[get_export_override_size](../Classes/classPreferences.md#function-get-export-override-size)**() |
| void | **[set_export_override_size](../Classes/classPreferences.md#function-set-export-override-size)**(QSize size) |
| bool | **[get_export_override_size_enabled](../Classes/classPreferences.md#function-get-export-override-size-enabled)**() |
| void | **[set_export_override_size_enabled](../Classes/classPreferences.md#function-set-export-override-size-enabled)**(bool enabled) |
| bool | **[get_export_show_orientation_marker](../Classes/classPreferences.md#function-get-export-show-orientation-marker)**() |
| void | **[set_export_show_orientation_marker](../Classes/classPreferences.md#function-set-export-show-orientation-marker)**(bool value) |
| bool | **[get_export_show_color_scale](../Classes/classPreferences.md#function-get-export-show-color-scale)**() |
| void | **[set_export_show_color_scale](../Classes/classPreferences.md#function-set-export-show-color-scale)**(bool value) |
| int | **[get_export_num_pca_images](../Classes/classPreferences.md#function-get-export-num-pca-images)**() |
| void | **[set_export_num_pca_images](../Classes/classPreferences.md#function-set-export-num-pca-images)**(int number) |
| double | **[get_export_pca_range](../Classes/classPreferences.md#function-get-export-pca-range)**() |
| void | **[set_export_pca_range](../Classes/classPreferences.md#function-set-export-pca-range)**(double range) |
| void | **[set_export_pca_modes](../Classes/classPreferences.md#function-set-export-pca-modes)**(QString string) |
| QString | **[get_export_pca_modes](../Classes/classPreferences.md#function-get-export-pca-modes)**() |
| bool | **[get_auto_update_check](../Classes/classPreferences.md#function-get-auto-update-check)**() |
| void | **[set_auto_update_check](../Classes/classPreferences.md#function-set-auto-update-check)**(bool enabled) |
| QDateTime | **[get_update_snooze_until](../Classes/classPreferences.md#function-get-update-snooze-until)**() |
| void | **[set_update_snooze_until](../Classes/classPreferences.md#function-set-update-snooze-until)**(QDateTime date) |
| QString | **[get_device_id](../Classes/classPreferences.md#function-get-device-id)**() |
| bool | **[get_telemetry_enabled](../Classes/classPreferences.md#function-get-telemetry-enabled)**() |
| void | **[set_telemetry_enabled](../Classes/classPreferences.md#function-set-telemetry-enabled)**(bool enabled) |
| bool | **[get_telemetry_asked](../Classes/classPreferences.md#function-get-telemetry-asked)**() |
| void | **[set_telemetry_asked](../Classes/classPreferences.md#function-set-telemetry-asked)**(bool asked) |
| QStringList | **[get_pending_telemetry_events](../Classes/classPreferences.md#function-get-pending-telemetry-events)**() |
| void | **[set_pending_telemetry_events](../Classes/classPreferences.md#function-set-pending-telemetry-events)**(QStringList events) |

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




### enum @4

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


### function get_glyph_scale_arrows

```cpp
bool get_glyph_scale_arrows()
```


### function set_glyph_scale_arrows

```cpp
void set_glyph_scale_arrows(
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


### function set_particle_colors

```cpp
void set_particle_colors(
    int value
)
```


### function get_particle_colors

```cpp
int get_particle_colors()
```


### function set_color_map

```cpp
void set_color_map(
    int value
)
```


### function get_color_map

```cpp
int get_color_map()
```


### function set_discrete_color_mode

```cpp
void set_discrete_color_mode(
    bool value
)
```


### function get_discrete_color_mode

```cpp
bool get_discrete_color_mode()
```


### function set_reverse_color_map

```cpp
void set_reverse_color_map(
    bool value
)
```


### function get_reverse_color_map

```cpp
bool get_reverse_color_map()
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


### function get_export_override_size

```cpp
QSize get_export_override_size()
```


### function set_export_override_size

```cpp
void set_export_override_size(
    QSize size
)
```


### function get_export_override_size_enabled

```cpp
bool get_export_override_size_enabled()
```


### function set_export_override_size_enabled

```cpp
void set_export_override_size_enabled(
    bool enabled
)
```


### function get_export_show_orientation_marker

```cpp
bool get_export_show_orientation_marker()
```


### function set_export_show_orientation_marker

```cpp
void set_export_show_orientation_marker(
    bool value
)
```


### function get_export_show_color_scale

```cpp
bool get_export_show_color_scale()
```


### function set_export_show_color_scale

```cpp
void set_export_show_color_scale(
    bool value
)
```


### function get_export_num_pca_images

```cpp
int get_export_num_pca_images()
```


### function set_export_num_pca_images

```cpp
void set_export_num_pca_images(
    int number
)
```


### function get_export_pca_range

```cpp
double get_export_pca_range()
```


### function set_export_pca_range

```cpp
void set_export_pca_range(
    double range
)
```


### function set_export_pca_modes

```cpp
void set_export_pca_modes(
    QString string
)
```


### function get_export_pca_modes

```cpp
QString get_export_pca_modes()
```


### function get_auto_update_check

```cpp
bool get_auto_update_check()
```


### function set_auto_update_check

```cpp
void set_auto_update_check(
    bool enabled
)
```


### function get_update_snooze_until

```cpp
QDateTime get_update_snooze_until()
```


### function set_update_snooze_until

```cpp
void set_update_snooze_until(
    QDateTime date
)
```


### function get_device_id

```cpp
QString get_device_id()
```


### function get_telemetry_enabled

```cpp
bool get_telemetry_enabled()
```


### function set_telemetry_enabled

```cpp
void set_telemetry_enabled(
    bool enabled
)
```


### function get_telemetry_asked

```cpp
bool get_telemetry_asked()
```


### function set_telemetry_asked

```cpp
void set_telemetry_asked(
    bool asked
)
```


### function get_pending_telemetry_events

```cpp
QStringList get_pending_telemetry_events()
```


### function set_pending_telemetry_events

```cpp
void set_pending_telemetry_events(
    QStringList events
)
```


-------------------------------

Updated on 2025-09-04 at 08:01:26 +0000