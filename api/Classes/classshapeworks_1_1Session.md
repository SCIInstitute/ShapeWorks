---
title: shapeworks::Session
summary: Representation of a session. 

---

# shapeworks::Session



Representation of a session.  [More...](#detailed-description)


`#include <Session.h>`

Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[set_feature_auto_scale](../Classes/classshapeworks_1_1Session.md#slot-set-feature-auto-scale)**(bool value) |
| void | **[handle_clear_cache](../Classes/classshapeworks_1_1Session.md#slot-handle-clear-cache)**() |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1Session.md#slot-handle-new-mesh)**() |
| void | **[handle_message](../Classes/classshapeworks_1_1Session.md#slot-handle-message)**(QString s) |
| void | **[handle_thread_complete](../Classes/classshapeworks_1_1Session.md#slot-handle-thread-complete)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[data_changed](../Classes/classshapeworks_1_1Session.md#signal-data-changed)**()<br>signal that the data has changed  |
| void | **[points_changed](../Classes/classshapeworks_1_1Session.md#signal-points-changed)**() |
| void | **[update_display](../Classes/classshapeworks_1_1Session.md#signal-update-display)**() |
| void | **[new_mesh](../Classes/classshapeworks_1_1Session.md#signal-new-mesh)**() |
| void | **[message](../Classes/classshapeworks_1_1Session.md#signal-message)**(QString ) |
| void | **[error](../Classes/classshapeworks_1_1Session.md#signal-error)**(QString ) |
| void | **[feature_range_changed](../Classes/classshapeworks_1_1Session.md#signal-feature-range-changed)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Session](../Classes/classshapeworks_1_1Session.md#function-session)**(QWidget * parent, [Preferences](../Classes/classPreferences.md) & prefs)<br>constructor  |
| | **[~Session](../Classes/classshapeworks_1_1Session.md#function-~session)**()<br>destructor  |
| void | **[set_parent](../Classes/classshapeworks_1_1Session.md#function-set-parent)**(QWidget * parent)<br>set QWidget parent  |
| bool | **[save_project](../Classes/classshapeworks_1_1Session.md#function-save-project)**(std::string filename)<br>save project to file  |
| bool | **[load_project](../Classes/classshapeworks_1_1Session.md#function-load-project)**(QString filename)<br>load project from file  |
| bool | **[load_light_project](../Classes/classshapeworks_1_1Session.md#function-load-light-project)**(QString filename)<br>read a lightweight project file  |
| bool | **[load_xl_project](../Classes/classshapeworks_1_1Session.md#function-load-xl-project)**(QString filename)<br>load a shapeworks project  |
| void | **[set_project_path](../Classes/classshapeworks_1_1Session.md#function-set-project-path)**(QString relative_path) |
| std::shared_ptr< [shapeworks::Project](../Classes/classshapeworks_1_1Project.md) > | **[get_project](../Classes/classshapeworks_1_1Session.md#function-get-project)**() |
| void | **[load_original_files](../Classes/classshapeworks_1_1Session.md#function-load-original-files)**(std::vector< std::string > filenames)<br>import files  |
| void | **[load_groomed_files](../Classes/classshapeworks_1_1Session.md#function-load-groomed-files)**(std::vector< std::string > file_names, double iso, int domains_per_shape)<br>load groomed files  |
| bool | **[load_point_files](../Classes/classshapeworks_1_1Session.md#function-load-point-files)**(std::vector< std::string > local, std::vector< std::string > world, int domains_per_shape) |
| bool | **[update_particles](../Classes/classshapeworks_1_1Session.md#function-update-particles)**(std::vector< [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) > particles) |
| void | **[update_procrustes_transforms](../Classes/classshapeworks_1_1Session.md#function-update-procrustes-transforms)**(std::vector< std::vector< std::vector< double >>> transforms) |
| bool | **[is_light_project](../Classes/classshapeworks_1_1Session.md#function-is-light-project)**() |
| bool | **[get_groomed_present](../Classes/classshapeworks_1_1Session.md#function-get-groomed-present)**() |
| void | **[remove_shapes](../Classes/classshapeworks_1_1Session.md#function-remove-shapes)**(QList< int > list)<br>remove shapes  |
| QVector< QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) > > | **[get_shapes](../Classes/classshapeworks_1_1Session.md#function-get-shapes)**()<br>return all shapes  |
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
| QSharedPointer< [MeshManager](../Classes/classshapeworks_1_1MeshManager.md) > | **[get_mesh_manager](../Classes/classshapeworks_1_1Session.md#function-get-mesh-manager)**() |
| [shapeworks::Parameters](../Classes/classshapeworks_1_1Parameters.md) & | **[parameters](../Classes/classshapeworks_1_1Session.md#function-parameters)**() |
| std::vector< DomainType > | **[get_domain_types](../Classes/classshapeworks_1_1Session.md#function-get-domain-types)**() |
| double | **[update_auto_glyph_size](../Classes/classshapeworks_1_1Session.md#function-update-auto-glyph-size)**() |
| double | **[get_auto_glyph_size](../Classes/classshapeworks_1_1Session.md#function-get-auto-glyph-size)**() |
| void | **[clear_particles](../Classes/classshapeworks_1_1Session.md#function-clear-particles)**()<br>clear particles from session (e.g. groom start, optimize start)  |
| bool | **[get_feature_auto_scale](../Classes/classshapeworks_1_1Session.md#function-get-feature-auto-scale)**() |
| double | **[get_feature_range_max](../Classes/classshapeworks_1_1Session.md#function-get-feature-range-max)**() |
| double | **[get_feature_range_min](../Classes/classshapeworks_1_1Session.md#function-get-feature-range-min)**() |
| void | **[set_feature_range](../Classes/classshapeworks_1_1Session.md#function-set-feature-range)**(double min, double max) |
| void | **[set_feature_range_min](../Classes/classshapeworks_1_1Session.md#function-set-feature-range-min)**(double value) |
| void | **[set_feature_range_max](../Classes/classshapeworks_1_1Session.md#function-set-feature-range-max)**(double value) |
| bool | **[is_supported_file_format](../Classes/classshapeworks_1_1Session.md#function-is-supported-file-format)**(std::string filename) |
| Point3 | **[get_point](../Classes/classshapeworks_1_1Session.md#function-get-point)**(const vnl_vector< double > & points, int i) |

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


### slot handle_clear_cache

```cpp
void handle_clear_cache()
```


### slot handle_new_mesh

```cpp
void handle_new_mesh()
```


### slot handle_message

```cpp
void handle_message(
    QString s
)
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


### signal update_display

```cpp
void update_display()
```


### signal new_mesh

```cpp
void new_mesh()
```


### signal message

```cpp
void message(
    QString 
)
```


### signal error

```cpp
void error(
    QString 
)
```


### signal feature_range_changed

```cpp
void feature_range_changed()
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
    std::string filename
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
    std::vector< StudioParticles > particles
)
```


### function update_procrustes_transforms

```cpp
void update_procrustes_transforms(
    std::vector< std::vector< std::vector< double >>> transforms
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
QVector< QSharedPointer< Shape > > get_shapes()
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
inline QSharedPointer< MeshManager > get_mesh_manager()
```


### function parameters

```cpp
shapeworks::Parameters & parameters()
```


### function get_domain_types

```cpp
std::vector< DomainType > get_domain_types()
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


### function is_supported_file_format

```cpp
static bool is_supported_file_format(
    std::string filename
)
```


### function get_point

```cpp
static Point3 get_point(
    const vnl_vector< double > & points,
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

Updated on 2021-11-19 at 20:38:59 +0000