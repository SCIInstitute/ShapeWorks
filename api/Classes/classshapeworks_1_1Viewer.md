---
title: shapeworks::Viewer
summary: 3D Viewer

---

# shapeworks::Viewer



3D [Viewer]() [More...](#detailed-description)


`#include <Viewer.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Viewer](../Classes/classshapeworks_1_1Viewer.md#function-viewer)**() |
| | **[~Viewer](../Classes/classshapeworks_1_1Viewer.md#function-~viewer)**() =default |
| void | **[set_renderer](../Classes/classshapeworks_1_1Viewer.md#function-set-renderer)**(vtkSmartPointer< vtkRenderer > renderer) |
| vtkSmartPointer< vtkRenderer > | **[get_renderer](../Classes/classshapeworks_1_1Viewer.md#function-get-renderer)**() |
| void | **[display_shape](../Classes/classshapeworks_1_1Viewer.md#function-display-shape)**(QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) > shape) |
| void | **[clear_viewer](../Classes/classshapeworks_1_1Viewer.md#function-clear-viewer)**() |
| void | **[reset_camera](../Classes/classshapeworks_1_1Viewer.md#function-reset-camera)**(std::array< double, 3 > c) |
| void | **[set_glyph_size_and_quality](../Classes/classshapeworks_1_1Viewer.md#function-set-glyph-size-and-quality)**(double size, double quality) |
| double | **[get_glyph_size](../Classes/classshapeworks_1_1Viewer.md#function-get-glyph-size)**() |
| double | **[get_glyph_quality](../Classes/classshapeworks_1_1Viewer.md#function-get-glyph-quality)**() |
| void | **[set_session](../Classes/classshapeworks_1_1Viewer.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session) |
| QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > | **[get_session](../Classes/classshapeworks_1_1Viewer.md#function-get-session)**() |
| void | **[set_show_glyphs](../Classes/classshapeworks_1_1Viewer.md#function-set-show-glyphs)**(bool show) |
| void | **[set_show_surface](../Classes/classshapeworks_1_1Viewer.md#function-set-show-surface)**(bool show) |
| void | **[set_show_landmarks](../Classes/classshapeworks_1_1Viewer.md#function-set-show-landmarks)**(bool show) |
| bool | **[get_show_landmarks](../Classes/classshapeworks_1_1Viewer.md#function-get-show-landmarks)**() |
| void | **[update_points](../Classes/classshapeworks_1_1Viewer.md#function-update-points)**() |
| void | **[update_glyph_properties](../Classes/classshapeworks_1_1Viewer.md#function-update-glyph-properties)**() |
| int | **[handle_pick](../Classes/classshapeworks_1_1Viewer.md#function-handle-pick)**(int * click_pos) |
| [PickResult](../Classes/classshapeworks_1_1PickResult.md) | **[handle_ctrl_click](../Classes/classshapeworks_1_1Viewer.md#function-handle-ctrl-click)**(int * click_pos) |
| void | **[set_selected_point](../Classes/classshapeworks_1_1Viewer.md#function-set-selected-point)**(int id) |
| void | **[set_lut](../Classes/classshapeworks_1_1Viewer.md#function-set-lut)**(vtkSmartPointer< vtkLookupTable > lut) |
| void | **[set_loading_screen](../Classes/classshapeworks_1_1Viewer.md#function-set-loading-screen)**(vtkSmartPointer< vtkImageData > loading_screen) |
| void | **[set_color_scheme](../Classes/classshapeworks_1_1Viewer.md#function-set-color-scheme)**(int i) |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1Viewer.md#function-handle-new-mesh)**() |
| bool | **[is_viewer_ready](../Classes/classshapeworks_1_1Viewer.md#function-is-viewer-ready)**() |
| void | **[set_visualizer](../Classes/classshapeworks_1_1Viewer.md#function-set-visualizer)**([Visualizer](../Classes/classshapeworks_1_1Visualizer.md) * visualizer) |
| void | **[update_feature_range](../Classes/classshapeworks_1_1Viewer.md#function-update-feature-range)**(double * range) |
| void | **[update_opacities](../Classes/classshapeworks_1_1Viewer.md#function-update-opacities)**() |
| QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) > | **[get_shape](../Classes/classshapeworks_1_1Viewer.md#function-get-shape)**() |
| void | **[update_landmarks](../Classes/classshapeworks_1_1Viewer.md#function-update-landmarks)**() |
| std::vector< vtkSmartPointer< vtkActor > > | **[get_surface_actors](../Classes/classshapeworks_1_1Viewer.md#function-get-surface-actors)**() |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_meshes](../Classes/classshapeworks_1_1Viewer.md#function-get-meshes)**() |
| vtkSmartPointer< vtkTransform > | **[get_transform](../Classes/classshapeworks_1_1Viewer.md#function-get-transform)**(int alignment_domain, int domain) |
| vtkSmartPointer< vtkTransform > | **[get_landmark_transform](../Classes/classshapeworks_1_1Viewer.md#function-get-landmark-transform)**(int domain) |

## Detailed Description

```cpp
class shapeworks::Viewer;
```

3D [Viewer]()

The [Viewer](../Classes/classshapeworks_1_1Viewer.md) class encapsulates all the functionality for visualizing a single subject/shape 

## Public Functions Documentation

### function Viewer

```cpp
Viewer()
```


### function ~Viewer

```cpp
~Viewer() =default
```


### function set_renderer

```cpp
void set_renderer(
    vtkSmartPointer< vtkRenderer > renderer
)
```


### function get_renderer

```cpp
vtkSmartPointer< vtkRenderer > get_renderer()
```


### function display_shape

```cpp
void display_shape(
    QSharedPointer< Shape > shape
)
```


### function clear_viewer

```cpp
void clear_viewer()
```


### function reset_camera

```cpp
void reset_camera(
    std::array< double, 3 > c
)
```


### function set_glyph_size_and_quality

```cpp
void set_glyph_size_and_quality(
    double size,
    double quality
)
```


### function get_glyph_size

```cpp
double get_glyph_size()
```


### function get_glyph_quality

```cpp
double get_glyph_quality()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```


### function get_session

```cpp
QSharedPointer< Session > get_session()
```


### function set_show_glyphs

```cpp
void set_show_glyphs(
    bool show
)
```


### function set_show_surface

```cpp
void set_show_surface(
    bool show
)
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


### function update_points

```cpp
void update_points()
```


### function update_glyph_properties

```cpp
void update_glyph_properties()
```


### function handle_pick

```cpp
int handle_pick(
    int * click_pos
)
```


### function handle_ctrl_click

```cpp
PickResult handle_ctrl_click(
    int * click_pos
)
```


### function set_selected_point

```cpp
void set_selected_point(
    int id
)
```


### function set_lut

```cpp
void set_lut(
    vtkSmartPointer< vtkLookupTable > lut
)
```


### function set_loading_screen

```cpp
void set_loading_screen(
    vtkSmartPointer< vtkImageData > loading_screen
)
```


### function set_color_scheme

```cpp
void set_color_scheme(
    int i
)
```


### function handle_new_mesh

```cpp
void handle_new_mesh()
```


### function is_viewer_ready

```cpp
bool is_viewer_ready()
```


### function set_visualizer

```cpp
void set_visualizer(
    Visualizer * visualizer
)
```


### function update_feature_range

```cpp
void update_feature_range(
    double * range
)
```


### function update_opacities

```cpp
void update_opacities()
```


### function get_shape

```cpp
QSharedPointer< Shape > get_shape()
```


### function update_landmarks

```cpp
void update_landmarks()
```


### function get_surface_actors

```cpp
std::vector< vtkSmartPointer< vtkActor > > get_surface_actors()
```


### function get_meshes

```cpp
MeshGroup get_meshes()
```


### function get_transform

```cpp
vtkSmartPointer< vtkTransform > get_transform(
    int alignment_domain,
    int domain
)
```


### function get_landmark_transform

```cpp
vtkSmartPointer< vtkTransform > get_landmark_transform(
    int domain
)
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000