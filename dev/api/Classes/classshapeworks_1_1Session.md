---
title: shapeworks::Session
summary: Representation of a session. 

---

# shapeworks::Session



Representation of a session.  [More...](#detailed-description)


`#include <Session.h>`

Inherits from QObject, QEnableSharedFromThis< Session >

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[set_feature_auto_scale](../Classes/classshapeworks_1_1Session.md#slot-set-feature-auto-scale)**(bool value) |
| void | **[set_landmark_drag_mode](../Classes/classshapeworks_1_1Session.md#slot-set-landmark-drag-mode)**(bool mode) |
| bool | **[get_landmark_drag_mode](../Classes/classshapeworks_1_1Session.md#slot-get-landmark-drag-mode)**() |
| void | **[handle_clear_cache](../Classes/classshapeworks_1_1Session.md#slot-handle-clear-cache)**() |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1Session.md#slot-handle-new-mesh)**() |
| void | **[handle_thread_complete](../Classes/classshapeworks_1_1Session.md#slot-handle-thread-complete)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[data_changed](../Classes/classshapeworks_1_1Session.md#signal-data-changed)**()<br>signal that the data has changed  |
| void | **[points_changed](../Classes/classshapeworks_1_1Session.md#signal-points-changed)**() |
| void | **[landmarks_changed](../Classes/classshapeworks_1_1Session.md#signal-landmarks-changed)**() |
| void | **[planes_changed](../Classes/classshapeworks_1_1Session.md#signal-planes-changed)**() |
| void | **[ffc_changed](../Classes/classshapeworks_1_1Session.md#signal-ffc-changed)**() |
| void | **[update_display](../Classes/classshapeworks_1_1Session.md#signal-update-display)**() |
| void | **[new_mesh](../Classes/classshapeworks_1_1Session.md#signal-new-mesh)**() |
| void | **[feature_range_changed](../Classes/classshapeworks_1_1Session.md#signal-feature-range-changed)**() |
| void | **[update_view_mode](../Classes/classshapeworks_1_1Session.md#signal-update-view-mode)**() |
| void | **[image_slice_settings_changed](../Classes/classshapeworks_1_1Session.md#signal-image-slice-settings-changed)**() |
| void | **[ffc_paint_mode_changed](../Classes/classshapeworks_1_1Session.md#signal-ffc-paint-mode-changed)**() |
| void | **[repaint](../Classes/classshapeworks_1_1Session.md#signal-repaint)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Session](../Classes/classshapeworks_1_1Session.md#function-session)**(QWidget * parent, [Preferences](../Classes/classPreferences.md) & prefs)<br>constructor  |
| | **[~Session](../Classes/classshapeworks_1_1Session.md#function-~session)**()<br>destructor  |
| void | **[set_parent](../Classes/classshapeworks_1_1Session.md#function-set-parent)**(QWidget * parent)<br>set QWidget parent  |
| bool | **[save_project](../Classes/classshapeworks_1_1Session.md#function-save-project)**(QString filename)<br>save project to file  |
| bool | **[load_project](../Classes/classshapeworks_1_1Session.md#function-load-project)**(QString filename)<br>load project from file  |
| bool | **[load_light_project](../Classes/classshapeworks_1_1Session.md#function-load-light-project)**(QString filename)<br>read a lightweight project file  |
| bool | **[load_xl_project](../Classes/classshapeworks_1_1Session.md#function-load-xl-project)**(QString filename)<br>load a shapeworks project  |
| void | **[set_project_path](../Classes/classshapeworks_1_1Session.md#function-set-project-path)**(QString relative_path) |
| std::shared_ptr< [shapeworks::Project](../Classes/classshapeworks_1_1Project.md) > | **[get_project](../Classes/classshapeworks_1_1Session.md#function-get-project)**() |
| void | **[load_original_files](../Classes/classshapeworks_1_1Session.md#function-load-original-files)**(std::vector< std::string > filenames)<br>import files  |
| void | **[load_groomed_files](../Classes/classshapeworks_1_1Session.md#function-load-groomed-files)**(std::vector< std::string > file_names, double iso, int domains_per_shape)<br>load groomed files  |
| bool | **[load_point_files](../Classes/classshapeworks_1_1Session.md#function-load-point-files)**(std::vector< std::string > local, std::vector< std::string > world, int domains_per_shape) |
| bool | **[update_particles](../Classes/classshapeworks_1_1Session.md#function-update-particles)**(std::vector< [Particles](../Classes/classshapeworks_1_1Particles.md) > particles) |
| int | **[get_num_particles](../Classes/classshapeworks_1_1Session.md#function-get-num-particles)**()<br>Return the total number of particles for all domains, combined.  |
| [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[get_local_particle_system](../Classes/classshapeworks_1_1Session.md#function-get-local-particle-system)**(int domain) |
| void | **[update_procrustes_transforms](../Classes/classshapeworks_1_1Session.md#function-update-procrustes-transforms)**(std::vector< std::vector< std::vector< double > > > transforms) |
| bool | **[is_light_project](../Classes/classshapeworks_1_1Session.md#function-is-light-project)**() |
| bool | **[get_groomed_present](../Classes/classshapeworks_1_1Session.md#function-get-groomed-present)**() |
| void | **[remove_shapes](../Classes/classshapeworks_1_1Session.md#function-remove-shapes)**(QList< int > list)<br>remove shapes  |
| ShapeList | **[get_shapes](../Classes/classshapeworks_1_1Session.md#function-get-shapes)**()<br>return all shapes  |
| void | **[calculate_reconstructed_samples](../Classes/classshapeworks_1_1Session.md#function-calculate-reconstructed-samples)**() |
| QString | **[get_filename](../Classes/classshapeworks_1_1Session.md#function-get-filename)**()<br>get the filename  |
| QString | **[get_display_name](../Classes/classshapeworks_1_1Session.md#function-get-display-name)**()<br>get file display name  |
| bool | **[original_present](../Classes/classshapeworks_1_1Session.md#function-original-present)**() |
| bool | **[groomed_present](../Classes/classshapeworks_1_1Session.md#function-groomed-present)**() |
| bool | **[particles_present](../Classes/classshapeworks_1_1Session.md#function-particles-present)**() |
| bool | **[groups_available](../Classes/classshapeworks_1_1Session.md#function-groups-available)**() |
| int | **[get_num_shapes](../Classes/classshapeworks_1_1Session.md#function-get-num-shapes)**() |
| int | **[get_domains_per_shape](../Classes/classshapeworks_1_1Session.md#function-get-domains-per-shape)**() |
| std::string | **[get_default_feature_map](../Classes/classshapeworks_1_1Session.md#function-get-default-feature-map)**() |
| std::shared_ptr< [MeshManager](../Classes/classshapeworks_1_1MeshManager.md) > | **[get_mesh_manager](../Classes/classshapeworks_1_1Session.md#function-get-mesh-manager)**() |
| [shapeworks::Parameters](../Classes/classshapeworks_1_1Parameters.md) & | **[parameters](../Classes/classshapeworks_1_1Session.md#function-parameters)**() |
| std::vector< DomainType > | **[get_groomed_domain_types](../Classes/classshapeworks_1_1Session.md#function-get-groomed-domain-types)**() |
| double | **[update_auto_glyph_size](../Classes/classshapeworks_1_1Session.md#function-update-auto-glyph-size)**() |
| double | **[get_auto_glyph_size](../Classes/classshapeworks_1_1Session.md#function-get-auto-glyph-size)**() |
| void | **[clear_particles](../Classes/classshapeworks_1_1Session.md#function-clear-particles)**()<br>clear particles from session (e.g. groom start, optimize start)  |
| bool | **[get_feature_auto_scale](../Classes/classshapeworks_1_1Session.md#function-get-feature-auto-scale)**() |
| double | **[get_feature_range_max](../Classes/classshapeworks_1_1Session.md#function-get-feature-range-max)**() |
| double | **[get_feature_range_min](../Classes/classshapeworks_1_1Session.md#function-get-feature-range-min)**() |
| void | **[set_feature_range](../Classes/classshapeworks_1_1Session.md#function-set-feature-range)**(double min, double max) |
| void | **[set_feature_range_min](../Classes/classshapeworks_1_1Session.md#function-set-feature-range-min)**(double value) |
| void | **[set_feature_range_max](../Classes/classshapeworks_1_1Session.md#function-set-feature-range-max)**(double value) |
| void | **[handle_ctrl_click](../Classes/classshapeworks_1_1Session.md#function-handle-ctrl-click)**([PickResult](../Classes/classshapeworks_1_1PickResult.md) result) |
| void | **[trigger_landmarks_changed](../Classes/classshapeworks_1_1Session.md#function-trigger-landmarks-changed)**() |
| void | **[trigger_planes_changed](../Classes/classshapeworks_1_1Session.md#function-trigger-planes-changed)**() |
| void | **[trigger_ffc_changed](../Classes/classshapeworks_1_1Session.md#function-trigger-ffc-changed)**() |
| void | **[set_active_landmark_domain](../Classes/classshapeworks_1_1Session.md#function-set-active-landmark-domain)**(int id) |
| int | **[get_active_landmark_domain](../Classes/classshapeworks_1_1Session.md#function-get-active-landmark-domain)**() |
| void | **[set_placing_landmark](../Classes/classshapeworks_1_1Session.md#function-set-placing-landmark)**(int id) |
| int | **[get_placing_landmark](../Classes/classshapeworks_1_1Session.md#function-get-placing-landmark)**() |
| void | **[set_landmarks_active](../Classes/classshapeworks_1_1Session.md#function-set-landmarks-active)**(bool active) |
| bool | **[get_landmarks_active](../Classes/classshapeworks_1_1Session.md#function-get-landmarks-active)**() |
| void | **[set_planes_active](../Classes/classshapeworks_1_1Session.md#function-set-planes-active)**(bool active) |
| bool | **[get_planes_active](../Classes/classshapeworks_1_1Session.md#function-get-planes-active)**() |
| void | **[set_show_landmark_labels](../Classes/classshapeworks_1_1Session.md#function-set-show-landmark-labels)**(bool show) |
| bool | **[get_show_landmark_labels](../Classes/classshapeworks_1_1Session.md#function-get-show-landmark-labels)**() |
| void | **[set_show_planes](../Classes/classshapeworks_1_1Session.md#function-set-show-planes)**(bool show) |
| bool | **[get_show_planes](../Classes/classshapeworks_1_1Session.md#function-get-show-planes)**() |
| bool | **[should_show_planes](../Classes/classshapeworks_1_1Session.md#function-should-show-planes)**() |
| void | **[set_show_landmarks](../Classes/classshapeworks_1_1Session.md#function-set-show-landmarks)**(bool show) |
| bool | **[get_show_landmarks](../Classes/classshapeworks_1_1Session.md#function-get-show-landmarks)**() |
| bool | **[set_image_name](../Classes/classshapeworks_1_1Session.md#function-set-image-name)**(std::string image_name) |
| std::string | **[get_image_name](../Classes/classshapeworks_1_1Session.md#function-get-image-name)**() |
| void | **[set_image_axis](../Classes/classshapeworks_1_1Session.md#function-set-image-axis)**(QString axis) |
| Axis | **[get_image_axis](../Classes/classshapeworks_1_1Session.md#function-get-image-axis)**() |
| void | **[set_image_3d_mode](../Classes/classshapeworks_1_1Session.md#function-set-image-3d-mode)**(bool mode) |
| bool | **[get_image_3d_mode](../Classes/classshapeworks_1_1Session.md#function-get-image-3d-mode)**() |
| void | **[set_image_share_window_and_level](../Classes/classshapeworks_1_1Session.md#function-set-image-share-window-and-level)**(bool enabled) |
| bool | **[get_image_share_window_and_level](../Classes/classshapeworks_1_1Session.md#function-get-image-share-window-and-level)**() |
| void | **[set_image_sync_slice](../Classes/classshapeworks_1_1Session.md#function-set-image-sync-slice)**(bool enabled) |
| bool | **[get_image_sync_slice](../Classes/classshapeworks_1_1Session.md#function-get-image-sync-slice)**() |
| bool | **[has_constraints](../Classes/classshapeworks_1_1Session.md#function-has-constraints)**() |
| void | **[set_loading](../Classes/classshapeworks_1_1Session.md#function-set-loading)**(bool loading) |
| bool | **[is_loading](../Classes/classshapeworks_1_1Session.md#function-is-loading)**() |
| void | **[set_tool_state](../Classes/classshapeworks_1_1Session.md#function-set-tool-state)**(std::string state) |
| std::string | **[get_tool_state](../Classes/classshapeworks_1_1Session.md#function-get-tool-state)**() |
| bool | **[is_analysis_mode](../Classes/classshapeworks_1_1Session.md#function-is-analysis-mode)**() |
| void | **[set_ffc_paint_active](../Classes/classshapeworks_1_1Session.md#function-set-ffc-paint-active)**(bool enabled) |
| bool | **[get_ffc_paint_active](../Classes/classshapeworks_1_1Session.md#function-get-ffc-paint-active)**() |
| void | **[set_ffc_paint_mode_inclusive](../Classes/classshapeworks_1_1Session.md#function-set-ffc-paint-mode-inclusive)**(bool inclusive) |
| bool | **[get_ffc_paint_mode_inclusive](../Classes/classshapeworks_1_1Session.md#function-get-ffc-paint-mode-inclusive)**() |
| void | **[set_ffc_paint_size](../Classes/classshapeworks_1_1Session.md#function-set-ffc-paint-size)**(double size) |
| double | **[get_ffc_paint_size](../Classes/classshapeworks_1_1Session.md#function-get-ffc-paint-size)**() |
| bool | **[get_show_good_bad_particles](../Classes/classshapeworks_1_1Session.md#function-get-show-good-bad-particles)**() |
| void | **[set_show_good_bad_particles](../Classes/classshapeworks_1_1Session.md#function-set-show-good-bad-particles)**(bool enabled) |
| bool | **[get_show_difference_vectors](../Classes/classshapeworks_1_1Session.md#function-get-show-difference-vectors)**() |
| void | **[set_show_difference_vectors](../Classes/classshapeworks_1_1Session.md#function-set-show-difference-vectors)**(bool enabled) |
| bool | **[should_difference_vectors_show](../Classes/classshapeworks_1_1Session.md#function-should-difference-vectors-show)**() |
| std::vector< bool > | **[get_good_bad_particles](../Classes/classshapeworks_1_1Session.md#function-get-good-bad-particles)**() |
| void | **[set_good_bad_particles](../Classes/classshapeworks_1_1Session.md#function-set-good-bad-particles)**(const std::vector< bool > & good_bad) |
| void | **[set_difference_particles](../Classes/classshapeworks_1_1Session.md#function-set-difference-particles)**([Particles](../Classes/classshapeworks_1_1Particles.md) particles) |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_difference_particles](../Classes/classshapeworks_1_1Session.md#function-get-difference-particles)**() |
| void | **[set_compare_settings](../Classes/classshapeworks_1_1Session.md#function-set-compare-settings)**([CompareSettings](../Classes/classshapeworks_1_1CompareSettings.md) settings) |
| [CompareSettings](../Classes/classshapeworks_1_1CompareSettings.md) | **[get_compare_settings](../Classes/classshapeworks_1_1Session.md#function-get-compare-settings)**() |
| void | **[trigger_repaint](../Classes/classshapeworks_1_1Session.md#function-trigger-repaint)**() |
| void | **[set_display_mode](../Classes/classshapeworks_1_1Session.md#function-set-display-mode)**(DisplayMode mode)<br>set display mode (original, groomed, reconstructed)  |
| DisplayMode | **[get_display_mode](../Classes/classshapeworks_1_1Session.md#function-get-display-mode)**()<br>return the current display mode  |
| bool | **[is_supported_file_format](../Classes/classshapeworks_1_1Session.md#function-is-supported-file-format)**(std::string filename) |
| Point3 | **[get_point](../Classes/classshapeworks_1_1Session.md#function-get-point)**(const Eigen::VectorXd & points, int i) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[DATA_C](../Classes/classshapeworks_1_1Session.md#variable-data-c)**  |
| const std::string | **[GROOM_C](../Classes/classshapeworks_1_1Session.md#variable-groom-c)**  |
| const std::string | **[OPTIMIZE_C](../Classes/classshapeworks_1_1Session.md#variable-optimize-c)**  |
| const std::string | **[ANALYSIS_C](../Classes/classshapeworks_1_1Session.md#variable-analysis-c)**  |
| const std::string | **[DEEPSSM_C](../Classes/classshapeworks_1_1Session.md#variable-deepssm-c)**  |

## Detailed Description

```cpp
class shapeworks::Session;
```

Representation of a session. 

The [Session](../Classes/classshapeworks_1_1Session.md) class encapsulates everything about a session/project. 

## Public Slots Documentation

### slot set_feature_auto_scale

```cpp
void set_feature_auto_scale(
    bool value
)
```


### slot set_landmark_drag_mode

```cpp
void set_landmark_drag_mode(
    bool mode
)
```


### slot get_landmark_drag_mode

```cpp
bool get_landmark_drag_mode()
```


### slot handle_clear_cache

```cpp
void handle_clear_cache()
```


### slot handle_new_mesh

```cpp
void handle_new_mesh()
```


### slot handle_thread_complete

```cpp
void handle_thread_complete()
```


## Public Signals Documentation

### signal data_changed

```cpp
void data_changed()
```

signal that the data has changed 

### signal points_changed

```cpp
void points_changed()
```


### signal landmarks_changed

```cpp
void landmarks_changed()
```


### signal planes_changed

```cpp
void planes_changed()
```


### signal ffc_changed

```cpp
void ffc_changed()
```


### signal update_display

```cpp
void update_display()
```


### signal new_mesh

```cpp
void new_mesh()
```


### signal feature_range_changed

```cpp
void feature_range_changed()
```


### signal update_view_mode

```cpp
void update_view_mode()
```


### signal image_slice_settings_changed

```cpp
void image_slice_settings_changed()
```


### signal ffc_paint_mode_changed

```cpp
void ffc_paint_mode_changed()
```


### signal repaint

```cpp
void repaint()
```


## Public Functions Documentation

### function Session

```cpp
Session(
    QWidget * parent,
    Preferences & prefs
)
```

constructor 

### function ~Session

```cpp
~Session()
```

destructor 

### function set_parent

```cpp
void set_parent(
    QWidget * parent
)
```

set QWidget parent 

### function save_project

```cpp
bool save_project(
    QString filename
)
```

save project to file 

### function load_project

```cpp
bool load_project(
    QString filename
)
```

load project from file 

### function load_light_project

```cpp
bool load_light_project(
    QString filename
)
```

read a lightweight project file 

### function load_xl_project

```cpp
bool load_xl_project(
    QString filename
)
```

load a shapeworks project 

### function set_project_path

```cpp
void set_project_path(
    QString relative_path
)
```


### function get_project

```cpp
std::shared_ptr< shapeworks::Project > get_project()
```


### function load_original_files

```cpp
void load_original_files(
    std::vector< std::string > filenames
)
```

import files 

### function load_groomed_files

```cpp
void load_groomed_files(
    std::vector< std::string > file_names,
    double iso,
    int domains_per_shape
)
```

load groomed files 

### function load_point_files

```cpp
bool load_point_files(
    std::vector< std::string > local,
    std::vector< std::string > world,
    int domains_per_shape
)
```


### function update_particles

```cpp
bool update_particles(
    std::vector< Particles > particles
)
```


### function get_num_particles

```cpp
int get_num_particles()
```

Return the total number of particles for all domains, combined. 

### function get_local_particle_system

```cpp
ParticleSystem get_local_particle_system(
    int domain
)
```


### function update_procrustes_transforms

```cpp
void update_procrustes_transforms(
    std::vector< std::vector< std::vector< double > > > transforms
)
```


### function is_light_project

```cpp
bool is_light_project()
```


### function get_groomed_present

```cpp
bool get_groomed_present()
```


### function remove_shapes

```cpp
void remove_shapes(
    QList< int > list
)
```

remove shapes 

### function get_shapes

```cpp
ShapeList get_shapes()
```

return all shapes 

### function calculate_reconstructed_samples

```cpp
void calculate_reconstructed_samples()
```


### function get_filename

```cpp
QString get_filename()
```

get the filename 

### function get_display_name

```cpp
QString get_display_name()
```

get file display name 

### function original_present

```cpp
bool original_present()
```


### function groomed_present

```cpp
bool groomed_present()
```


### function particles_present

```cpp
bool particles_present()
```


### function groups_available

```cpp
bool groups_available()
```


### function get_num_shapes

```cpp
int get_num_shapes()
```


### function get_domains_per_shape

```cpp
int get_domains_per_shape()
```


### function get_default_feature_map

```cpp
std::string get_default_feature_map()
```


### function get_mesh_manager

```cpp
inline std::shared_ptr< MeshManager > get_mesh_manager()
```


### function parameters

```cpp
shapeworks::Parameters & parameters()
```


### function get_groomed_domain_types

```cpp
std::vector< DomainType > get_groomed_domain_types()
```


### function update_auto_glyph_size

```cpp
double update_auto_glyph_size()
```


### function get_auto_glyph_size

```cpp
double get_auto_glyph_size()
```


### function clear_particles

```cpp
void clear_particles()
```

clear particles from session (e.g. groom start, optimize start) 

### function get_feature_auto_scale

```cpp
bool get_feature_auto_scale()
```


### function get_feature_range_max

```cpp
double get_feature_range_max()
```


### function get_feature_range_min

```cpp
double get_feature_range_min()
```


### function set_feature_range

```cpp
void set_feature_range(
    double min,
    double max
)
```


### function set_feature_range_min

```cpp
void set_feature_range_min(
    double value
)
```


### function set_feature_range_max

```cpp
void set_feature_range_max(
    double value
)
```


### function handle_ctrl_click

```cpp
void handle_ctrl_click(
    PickResult result
)
```


### function trigger_landmarks_changed

```cpp
void trigger_landmarks_changed()
```


### function trigger_planes_changed

```cpp
void trigger_planes_changed()
```


### function trigger_ffc_changed

```cpp
void trigger_ffc_changed()
```


### function set_active_landmark_domain

```cpp
void set_active_landmark_domain(
    int id
)
```


### function get_active_landmark_domain

```cpp
int get_active_landmark_domain()
```


### function set_placing_landmark

```cpp
void set_placing_landmark(
    int id
)
```


### function get_placing_landmark

```cpp
int get_placing_landmark()
```


### function set_landmarks_active

```cpp
void set_landmarks_active(
    bool active
)
```


### function get_landmarks_active

```cpp
bool get_landmarks_active()
```


### function set_planes_active

```cpp
void set_planes_active(
    bool active
)
```


### function get_planes_active

```cpp
bool get_planes_active()
```


### function set_show_landmark_labels

```cpp
void set_show_landmark_labels(
    bool show
)
```


### function get_show_landmark_labels

```cpp
bool get_show_landmark_labels()
```


### function set_show_planes

```cpp
void set_show_planes(
    bool show
)
```


### function get_show_planes

```cpp
bool get_show_planes()
```


### function should_show_planes

```cpp
bool should_show_planes()
```


### function set_show_landmarks

```cpp
void set_show_landmarks(
    bool show
)
```


### function get_show_landmarks

```cpp
bool get_show_landmarks()
```


### function set_image_name

```cpp
bool set_image_name(
    std::string image_name
)
```


### function get_image_name

```cpp
std::string get_image_name()
```


### function set_image_axis

```cpp
void set_image_axis(
    QString axis
)
```


### function get_image_axis

```cpp
Axis get_image_axis()
```


### function set_image_3d_mode

```cpp
void set_image_3d_mode(
    bool mode
)
```


### function get_image_3d_mode

```cpp
bool get_image_3d_mode()
```


### function set_image_share_window_and_level

```cpp
void set_image_share_window_and_level(
    bool enabled
)
```


### function get_image_share_window_and_level

```cpp
bool get_image_share_window_and_level()
```


### function set_image_sync_slice

```cpp
void set_image_sync_slice(
    bool enabled
)
```


### function get_image_sync_slice

```cpp
bool get_image_sync_slice()
```


### function has_constraints

```cpp
bool has_constraints()
```


### function set_loading

```cpp
void set_loading(
    bool loading
)
```


### function is_loading

```cpp
bool is_loading()
```


### function set_tool_state

```cpp
void set_tool_state(
    std::string state
)
```


### function get_tool_state

```cpp
std::string get_tool_state()
```


### function is_analysis_mode

```cpp
bool is_analysis_mode()
```


### function set_ffc_paint_active

```cpp
void set_ffc_paint_active(
    bool enabled
)
```


### function get_ffc_paint_active

```cpp
bool get_ffc_paint_active()
```


### function set_ffc_paint_mode_inclusive

```cpp
void set_ffc_paint_mode_inclusive(
    bool inclusive
)
```


### function get_ffc_paint_mode_inclusive

```cpp
bool get_ffc_paint_mode_inclusive()
```


### function set_ffc_paint_size

```cpp
void set_ffc_paint_size(
    double size
)
```


### function get_ffc_paint_size

```cpp
double get_ffc_paint_size()
```


### function get_show_good_bad_particles

```cpp
bool get_show_good_bad_particles()
```


### function set_show_good_bad_particles

```cpp
void set_show_good_bad_particles(
    bool enabled
)
```


### function get_show_difference_vectors

```cpp
bool get_show_difference_vectors()
```


### function set_show_difference_vectors

```cpp
void set_show_difference_vectors(
    bool enabled
)
```


### function should_difference_vectors_show

```cpp
bool should_difference_vectors_show()
```


### function get_good_bad_particles

```cpp
std::vector< bool > get_good_bad_particles()
```


### function set_good_bad_particles

```cpp
void set_good_bad_particles(
    const std::vector< bool > & good_bad
)
```


### function set_difference_particles

```cpp
inline void set_difference_particles(
    Particles particles
)
```


### function get_difference_particles

```cpp
inline Particles get_difference_particles()
```


### function set_compare_settings

```cpp
void set_compare_settings(
    CompareSettings settings
)
```


### function get_compare_settings

```cpp
CompareSettings get_compare_settings()
```


### function trigger_repaint

```cpp
void trigger_repaint()
```


### function set_display_mode

```cpp
void set_display_mode(
    DisplayMode mode
)
```

set display mode (original, groomed, reconstructed) 

### function get_display_mode

```cpp
DisplayMode get_display_mode()
```

return the current display mode 

### function is_supported_file_format

```cpp
static bool is_supported_file_format(
    std::string filename
)
```


### function get_point

```cpp
static Point3 get_point(
    const Eigen::VectorXd & points,
    int i
)
```


## Public Attributes Documentation

### variable DATA_C

```cpp
static const std::string DATA_C;
```


### variable GROOM_C

```cpp
static const std::string GROOM_C;
```


### variable OPTIMIZE_C

```cpp
static const std::string OPTIMIZE_C;
```


### variable ANALYSIS_C

```cpp
static const std::string ANALYSIS_C;
```


### variable DEEPSSM_C

```cpp
static const std::string DEEPSSM_C;
```


-------------------------------

Updated on 2022-11-04 at 00:00:04 +0000