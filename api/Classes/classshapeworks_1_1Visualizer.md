---
title: shapeworks::Visualizer
summary: Controls display of objects in viewers. 

---

# shapeworks::Visualizer



Controls display of objects in viewers.  [More...](#detailed-description)


`#include <Visualizer.h>`

Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[update_viewer_properties](../Classes/classshapeworks_1_1Visualizer.md#slot-update-viewer-properties)**()<br>update viewer properties (e.g. glyph size, quality, etc)  |
| void | **[handle_feature_range_changed](../Classes/classshapeworks_1_1Visualizer.md#slot-handle-feature-range-changed)**() |
| void | **[handle_image_slice_settings_changed](../Classes/classshapeworks_1_1Visualizer.md#slot-handle-image-slice-settings-changed)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Visualizer](../Classes/classshapeworks_1_1Visualizer.md#function-visualizer)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~Visualizer](../Classes/classshapeworks_1_1Visualizer.md#function-~visualizer)**() |
| void | **[set_lightbox](../Classes/classshapeworks_1_1Visualizer.md#function-set-lightbox)**(LightboxHandle lightbox)<br>set the lightbox  |
| void | **[set_session](../Classes/classshapeworks_1_1Visualizer.md#function-set-session)**(SessionHandle session)<br>set the session  |
| void | **[set_display_mode](../Classes/classshapeworks_1_1Visualizer.md#function-set-display-mode)**(std::string mode)<br>set display mode (original, groomed, reconstructed)  |
| std::string | **[get_display_mode](../Classes/classshapeworks_1_1Visualizer.md#function-get-display-mode)**()<br>return the current display mode  |
| void | **[set_center](../Classes/classshapeworks_1_1Visualizer.md#function-set-center)**(bool center)<br>turn automatic centering on/off  |
| bool | **[get_center](../Classes/classshapeworks_1_1Visualizer.md#function-get-center)**()<br>get centering on/off  |
| void | **[set_alignment_domain](../Classes/classshapeworks_1_1Visualizer.md#function-set-alignment-domain)**(int domain)<br>set the alignment domain  |
| int | **[get_alignment_domain](../Classes/classshapeworks_1_1Visualizer.md#function-get-alignment-domain)**()<br>get the current alignment domain  |
| void | **[set_show_glyphs](../Classes/classshapeworks_1_1Visualizer.md#function-set-show-glyphs)**(bool show)<br>turn on/off glyph display  |
| void | **[set_show_surface](../Classes/classshapeworks_1_1Visualizer.md#function-set-show-surface)**(bool show)<br>turn on/off surface display  |
| void | **[set_show_landmarks](../Classes/classshapeworks_1_1Visualizer.md#function-set-show-landmarks)**(bool show)<br>turn on/off landmark display  |
| void | **[display_samples](../Classes/classshapeworks_1_1Visualizer.md#function-display-samples)**()<br>update the display using the current settings  |
| void | **[update_samples](../Classes/classshapeworks_1_1Visualizer.md#function-update-samples)**() |
| void | **[update_landmarks](../Classes/classshapeworks_1_1Visualizer.md#function-update-landmarks)**() |
| void | **[update_planes](../Classes/classshapeworks_1_1Visualizer.md#function-update-planes)**() |
| void | **[display_sample](../Classes/classshapeworks_1_1Visualizer.md#function-display-sample)**(int i) |
| void | **[display_shape](../Classes/classshapeworks_1_1Visualizer.md#function-display-shape)**(ShapeHandle shape) |
| void | **[display_shapes](../Classes/classshapeworks_1_1Visualizer.md#function-display-shapes)**(QVector< QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) >> shapes) |
| void | **[set_selected_point_one](../Classes/classshapeworks_1_1Visualizer.md#function-set-selected-point-one)**(int id) |
| void | **[set_selected_point_two](../Classes/classshapeworks_1_1Visualizer.md#function-set-selected-point-two)**(int id) |
| void | **[set_mean](../Classes/classshapeworks_1_1Visualizer.md#function-set-mean)**(const Eigen::VectorXd & mean) |
| void | **[reset_camera](../Classes/classshapeworks_1_1Visualizer.md#function-reset-camera)**() |
| void | **[update_lut](../Classes/classshapeworks_1_1Visualizer.md#function-update-lut)**() |
| [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) | **[get_current_shape](../Classes/classshapeworks_1_1Visualizer.md#function-get-current-shape)**() |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1Visualizer.md#function-handle-new-mesh)**() |
| vtkSmartPointer< vtkPolyData > | **[get_current_mesh](../Classes/classshapeworks_1_1Visualizer.md#function-get-current-mesh)**() |
| std::vector< vtkSmartPointer< vtkPolyData > > | **[get_current_meshes_transformed](../Classes/classshapeworks_1_1Visualizer.md#function-get-current-meshes-transformed)**() |
| const std::string & | **[get_feature_map](../Classes/classshapeworks_1_1Visualizer.md#function-get-feature-map)**() const<br>Get the currently selected feature map.  |
| void | **[set_uniform_feature_range](../Classes/classshapeworks_1_1Visualizer.md#function-set-uniform-feature-range)**(bool value)<br>Set if we are using a uniform feature range.  |
| bool | **[get_uniform_feature_range](../Classes/classshapeworks_1_1Visualizer.md#function-get-uniform-feature-range)**(void )<br>Return if we are using a uniform feature range.  |
| void | **[set_feature_map](../Classes/classshapeworks_1_1Visualizer.md#function-set-feature-map)**(const std::string & feature_map)<br>Set the currently selected feature map.  |
| void | **[clear_viewers](../Classes/classshapeworks_1_1Visualizer.md#function-clear-viewers)**()<br>clear out the viewers  |
| void | **[reset_feature_range](../Classes/classshapeworks_1_1Visualizer.md#function-reset-feature-range)**()<br>Reset the feature range (e.g. for a new feature)  |
| double * | **[get_feature_range](../Classes/classshapeworks_1_1Visualizer.md#function-get-feature-range)**()<br>Get the current feature range.  |
| double * | **[get_feature_raw_range](../Classes/classshapeworks_1_1Visualizer.md#function-get-feature-raw-range)**()<br>Get the current raw feature range.  |
| bool | **[get_feature_range_valid](../Classes/classshapeworks_1_1Visualizer.md#function-get-feature-range-valid)**()<br>Return if the feature range is valid or not.  |
| void | **[update_feature_range](../Classes/classshapeworks_1_1Visualizer.md#function-update-feature-range)**(double * range)<br>Update the feature range with a given range.  |
| vtkSmartPointer< vtkTransform > | **[get_transform](../Classes/classshapeworks_1_1Visualizer.md#function-get-transform)**(QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) > shape, int alignment_domain, int domain)<br>Request the transform for a given shape and domain.  |
| void | **[set_opacities](../Classes/classshapeworks_1_1Visualizer.md#function-set-opacities)**(std::vector< float > opacities)<br>Set domain opacities.  |
| std::vector< float > | **[get_opacities](../Classes/classshapeworks_1_1Visualizer.md#function-get-opacities)**()<br>Get domain opacities.  |
| double | **[get_current_glyph_size](../Classes/classshapeworks_1_1Visualizer.md#function-get-current-glyph-size)**()<br>Get the current glyph size.  |
| void | **[handle_ctrl_click](../Classes/classshapeworks_1_1Visualizer.md#function-handle-ctrl-click)**([PickResult](../Classes/classshapeworks_1_1PickResult.md) result)<br>Handle ctrl click.  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[MODE_ORIGINAL_C](../Classes/classshapeworks_1_1Visualizer.md#variable-mode-original-c)**  |
| const std::string | **[MODE_GROOMED_C](../Classes/classshapeworks_1_1Visualizer.md#variable-mode-groomed-c)**  |
| const std::string | **[MODE_RECONSTRUCTION_C](../Classes/classshapeworks_1_1Visualizer.md#variable-mode-reconstruction-c)**  |

## Detailed Description

```cpp
class shapeworks::Visualizer;
```

Controls display of objects in viewers. 

The [Visualizer](../Classes/classshapeworks_1_1Visualizer.md) class maintains state and control over viewer properties. Additionally, it performs all shape-based statistical functions. 

## Public Slots Documentation

### slot update_viewer_properties

```cpp
void update_viewer_properties()
```

update viewer properties (e.g. glyph size, quality, etc) 

### slot handle_feature_range_changed

```cpp
void handle_feature_range_changed()
```


### slot handle_image_slice_settings_changed

```cpp
void handle_image_slice_settings_changed()
```


## Public Functions Documentation

### function Visualizer

```cpp
Visualizer(
    Preferences & prefs
)
```


### function ~Visualizer

```cpp
~Visualizer()
```


### function set_lightbox

```cpp
void set_lightbox(
    LightboxHandle lightbox
)
```

set the lightbox 

### function set_session

```cpp
void set_session(
    SessionHandle session
)
```

set the session 

### function set_display_mode

```cpp
void set_display_mode(
    std::string mode
)
```

set display mode (original, groomed, reconstructed) 

### function get_display_mode

```cpp
std::string get_display_mode()
```

return the current display mode 

### function set_center

```cpp
void set_center(
    bool center
)
```

turn automatic centering on/off 

### function get_center

```cpp
bool get_center()
```

get centering on/off 

### function set_alignment_domain

```cpp
void set_alignment_domain(
    int domain
)
```

set the alignment domain 

### function get_alignment_domain

```cpp
int get_alignment_domain()
```

get the current alignment domain 

### function set_show_glyphs

```cpp
void set_show_glyphs(
    bool show
)
```

turn on/off glyph display 

### function set_show_surface

```cpp
void set_show_surface(
    bool show
)
```

turn on/off surface display 

### function set_show_landmarks

```cpp
void set_show_landmarks(
    bool show
)
```

turn on/off landmark display 

### function display_samples

```cpp
void display_samples()
```

update the display using the current settings 

### function update_samples

```cpp
void update_samples()
```


### function update_landmarks

```cpp
void update_landmarks()
```


### function update_planes

```cpp
void update_planes()
```


### function display_sample

```cpp
void display_sample(
    int i
)
```


### function display_shape

```cpp
void display_shape(
    ShapeHandle shape
)
```


### function display_shapes

```cpp
void display_shapes(
    QVector< QSharedPointer< Shape >> shapes
)
```


### function set_selected_point_one

```cpp
void set_selected_point_one(
    int id
)
```


### function set_selected_point_two

```cpp
void set_selected_point_two(
    int id
)
```


### function set_mean

```cpp
void set_mean(
    const Eigen::VectorXd & mean
)
```


### function reset_camera

```cpp
void reset_camera()
```


### function update_lut

```cpp
void update_lut()
```


### function get_current_shape

```cpp
StudioParticles get_current_shape()
```


### function handle_new_mesh

```cpp
void handle_new_mesh()
```


### function get_current_mesh

```cpp
vtkSmartPointer< vtkPolyData > get_current_mesh()
```


### function get_current_meshes_transformed

```cpp
std::vector< vtkSmartPointer< vtkPolyData > > get_current_meshes_transformed()
```


### function get_feature_map

```cpp
const std::string & get_feature_map() const
```

Get the currently selected feature map. 

### function set_uniform_feature_range

```cpp
void set_uniform_feature_range(
    bool value
)
```

Set if we are using a uniform feature range. 

### function get_uniform_feature_range

```cpp
bool get_uniform_feature_range(
    void 
)
```

Return if we are using a uniform feature range. 

### function set_feature_map

```cpp
void set_feature_map(
    const std::string & feature_map
)
```

Set the currently selected feature map. 

### function clear_viewers

```cpp
void clear_viewers()
```

clear out the viewers 

### function reset_feature_range

```cpp
void reset_feature_range()
```

Reset the feature range (e.g. for a new feature) 

### function get_feature_range

```cpp
double * get_feature_range()
```

Get the current feature range. 

### function get_feature_raw_range

```cpp
double * get_feature_raw_range()
```

Get the current raw feature range. 

### function get_feature_range_valid

```cpp
bool get_feature_range_valid()
```

Return if the feature range is valid or not. 

### function update_feature_range

```cpp
void update_feature_range(
    double * range
)
```

Update the feature range with a given range. 

### function get_transform

```cpp
vtkSmartPointer< vtkTransform > get_transform(
    QSharedPointer< Shape > shape,
    int alignment_domain,
    int domain
)
```

Request the transform for a given shape and domain. 

### function set_opacities

```cpp
void set_opacities(
    std::vector< float > opacities
)
```

Set domain opacities. 

### function get_opacities

```cpp
std::vector< float > get_opacities()
```

Get domain opacities. 

### function get_current_glyph_size

```cpp
double get_current_glyph_size()
```

Get the current glyph size. 

### function handle_ctrl_click

```cpp
void handle_ctrl_click(
    PickResult result
)
```

Handle ctrl click. 

## Public Attributes Documentation

### variable MODE_ORIGINAL_C

```cpp
static const std::string MODE_ORIGINAL_C;
```


### variable MODE_GROOMED_C

```cpp
static const std::string MODE_GROOMED_C;
```


### variable MODE_RECONSTRUCTION_C

```cpp
static const std::string MODE_RECONSTRUCTION_C;
```


-------------------------------

Updated on 2022-03-07 at 00:21:28 +0000