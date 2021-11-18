---
title: shapeworks::Lightbox
summary: Display multiple Viewers in a tiled display. 

---

# shapeworks::Lightbox



Display multiple Viewers in a tiled display.  [More...](#detailed-description)


`#include <Lightbox.h>`

Inherits from QObject

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[handle_timer_callback](../Classes/classshapeworks_1_1Lightbox.md#slot-handle-timer-callback)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Lightbox](../Classes/classshapeworks_1_1Lightbox.md#function-lightbox)**() |
| | **[~Lightbox](../Classes/classshapeworks_1_1Lightbox.md#function-~lightbox)**() |
| void | **[set_shapes](../Classes/classshapeworks_1_1Lightbox.md#function-set-shapes)**(QVector< QSharedPointer< [Shape](../Classes/classshapeworks_1_1Shape.md) >> shapes) |
| QVector< ShapeHandle > | **[get_shapes](../Classes/classshapeworks_1_1Lightbox.md#function-get-shapes)**() |
| void | **[set_interactor](../Classes/classshapeworks_1_1Lightbox.md#function-set-interactor)**(vtkRenderWindowInteractor * interactor) |
| void | **[set_render_window](../Classes/classshapeworks_1_1Lightbox.md#function-set-render-window)**(vtkRenderWindow * render_window) |
| void | **[set_tile_layout](../Classes/classshapeworks_1_1Lightbox.md#function-set-tile-layout)**(int width, int height) |
| void | **[setup_renderers](../Classes/classshapeworks_1_1Lightbox.md#function-setup-renderers)**() |
| int | **[get_num_rows](../Classes/classshapeworks_1_1Lightbox.md#function-get-num-rows)**() |
| int | **[get_num_rows_visible](../Classes/classshapeworks_1_1Lightbox.md#function-get-num-rows-visible)**() |
| void | **[set_start_row](../Classes/classshapeworks_1_1Lightbox.md#function-set-start-row)**(int row) |
| ViewerList | **[get_viewers](../Classes/classshapeworks_1_1Lightbox.md#function-get-viewers)**() |
| void | **[redraw](../Classes/classshapeworks_1_1Lightbox.md#function-redraw)**() |
| void | **[handle_pick](../Classes/classshapeworks_1_1Lightbox.md#function-handle-pick)**(int * click_pos, bool one) |
| void | **[set_glyph_lut](../Classes/classshapeworks_1_1Lightbox.md#function-set-glyph-lut)**(vtkSmartPointer< vtkLookupTable > lut) |
| void | **[set_visualizer](../Classes/classshapeworks_1_1Lightbox.md#function-set-visualizer)**([Visualizer](../Classes/classshapeworks_1_1Visualizer.md) * visualizer) |
| bool | **[render_window_ready](../Classes/classshapeworks_1_1Lightbox.md#function-render-window-ready)**() |
| void | **[clear_renderers](../Classes/classshapeworks_1_1Lightbox.md#function-clear-renderers)**() |
| std::array< double, 3 > | **[initPos](../Classes/classshapeworks_1_1Lightbox.md#function-initpos)**() |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1Lightbox.md#function-handle-new-mesh)**() |
| void | **[reset_camera](../Classes/classshapeworks_1_1Lightbox.md#function-reset-camera)**() |
| void | **[reset_camera_clipping_range](../Classes/classshapeworks_1_1Lightbox.md#function-reset-camera-clipping-range)**() |
| void | **[set_orientation_marker](../Classes/classshapeworks_1_1Lightbox.md#function-set-orientation-marker)**(Preferences::OrientationMarkerType type, Preferences::OrientationMarkerCorner corner) |
| void | **[set_orientation_marker_viewport](../Classes/classshapeworks_1_1Lightbox.md#function-set-orientation-marker-viewport)**() |
| void | **[update_feature_range](../Classes/classshapeworks_1_1Lightbox.md#function-update-feature-range)**() |

## Detailed Description

```cpp
class shapeworks::Lightbox;
```

Display multiple Viewers in a tiled display. 

The LightBox class displays multiple Viewers in a tiled display 

## Public Slots Documentation

### slot handle_timer_callback

```cpp
void handle_timer_callback()
```


## Public Functions Documentation

### function Lightbox

```cpp
Lightbox()
```


### function ~Lightbox

```cpp
~Lightbox()
```


### function set_shapes

```cpp
void set_shapes(
    QVector< QSharedPointer< Shape >> shapes
)
```


### function get_shapes

```cpp
QVector< ShapeHandle > get_shapes()
```


### function set_interactor

```cpp
void set_interactor(
    vtkRenderWindowInteractor * interactor
)
```


### function set_render_window

```cpp
void set_render_window(
    vtkRenderWindow * render_window
)
```


### function set_tile_layout

```cpp
void set_tile_layout(
    int width,
    int height
)
```


### function setup_renderers

```cpp
void setup_renderers()
```


### function get_num_rows

```cpp
int get_num_rows()
```


### function get_num_rows_visible

```cpp
int get_num_rows_visible()
```


### function set_start_row

```cpp
void set_start_row(
    int row
)
```


### function get_viewers

```cpp
ViewerList get_viewers()
```


### function redraw

```cpp
void redraw()
```


### function handle_pick

```cpp
void handle_pick(
    int * click_pos,
    bool one
)
```


### function set_glyph_lut

```cpp
void set_glyph_lut(
    vtkSmartPointer< vtkLookupTable > lut
)
```


### function set_visualizer

```cpp
void set_visualizer(
    Visualizer * visualizer
)
```


### function render_window_ready

```cpp
inline bool render_window_ready()
```


### function clear_renderers

```cpp
void clear_renderers()
```


### function initPos

```cpp
std::array< double, 3 > initPos()
```


### function handle_new_mesh

```cpp
void handle_new_mesh()
```


### function reset_camera

```cpp
void reset_camera()
```


### function reset_camera_clipping_range

```cpp
void reset_camera_clipping_range()
```


### function set_orientation_marker

```cpp
void set_orientation_marker(
    Preferences::OrientationMarkerType type,
    Preferences::OrientationMarkerCorner corner
)
```


### function set_orientation_marker_viewport

```cpp
void set_orientation_marker_viewport()
```


### function update_feature_range

```cpp
void update_feature_range()
```


-------------------------------

Updated on 2021-11-18 at 01:30:45 +0000