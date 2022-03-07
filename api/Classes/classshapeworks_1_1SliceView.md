---
title: shapeworks::SliceView
summary: SliceView. 

---

# shapeworks::SliceView



[SliceView]().  [More...](#detailed-description)


`#include <SliceView.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[SliceView](../Classes/classshapeworks_1_1SliceView.md#function-sliceview)**([Viewer](../Classes/classshapeworks_1_1Viewer.md) * viewer) |
| void | **[set_volume](../Classes/classshapeworks_1_1SliceView.md#function-set-volume)**(vtkSmartPointer< vtkImageData > volume) |
| void | **[set_mesh](../Classes/classshapeworks_1_1SliceView.md#function-set-mesh)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[set_orientation](../Classes/classshapeworks_1_1SliceView.md#function-set-orientation)**(int orientation) |
| bool | **[is_image_loaded](../Classes/classshapeworks_1_1SliceView.md#function-is-image-loaded)**() |
| void | **[update_renderer](../Classes/classshapeworks_1_1SliceView.md#function-update-renderer)**() |
| void | **[update_camera](../Classes/classshapeworks_1_1SliceView.md#function-update-camera)**() |
| void | **[handle_key](../Classes/classshapeworks_1_1SliceView.md#function-handle-key)**(std::string key) |
| void | **[set_window_and_level](../Classes/classshapeworks_1_1SliceView.md#function-set-window-and-level)**(double window, double level) |
| double | **[get_spacing](../Classes/classshapeworks_1_1SliceView.md#function-get-spacing)**() |
| int | **[get_orientation_index](../Classes/classshapeworks_1_1SliceView.md#function-get-orientation-index)**() |
| void | **[update_particles](../Classes/classshapeworks_1_1SliceView.md#function-update-particles)**() |
| [Viewer](../Classes/classshapeworks_1_1Viewer.md) * | **[get_viewer](../Classes/classshapeworks_1_1SliceView.md#function-get-viewer)**() |
| bool | **[should_point_show](../Classes/classshapeworks_1_1SliceView.md#function-should-point-show)**(double x, double y, double z) |

## Detailed Description

```cpp
class shapeworks::SliceView;
```

[SliceView](). 

Provide slice viewing to the [Viewer](../Classes/classshapeworks_1_1Viewer.md)

## Public Functions Documentation

### function SliceView

```cpp
SliceView(
    Viewer * viewer
)
```


### function set_volume

```cpp
void set_volume(
    vtkSmartPointer< vtkImageData > volume
)
```


### function set_mesh

```cpp
void set_mesh(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function set_orientation

```cpp
void set_orientation(
    int orientation
)
```


### function is_image_loaded

```cpp
bool is_image_loaded()
```


### function update_renderer

```cpp
void update_renderer()
```


### function update_camera

```cpp
void update_camera()
```


### function handle_key

```cpp
void handle_key(
    std::string key
)
```


### function set_window_and_level

```cpp
void set_window_and_level(
    double window,
    double level
)
```


### function get_spacing

```cpp
double get_spacing()
```


### function get_orientation_index

```cpp
int get_orientation_index()
```


### function update_particles

```cpp
void update_particles()
```


### function get_viewer

```cpp
Viewer * get_viewer()
```


### function should_point_show

```cpp
bool should_point_show(
    double x,
    double y,
    double z
)
```


-------------------------------

Updated on 2022-03-07 at 00:21:27 +0000