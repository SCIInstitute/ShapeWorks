---
title: shapeworks::SliceView
summary: SliceView. 

---

# shapeworks::SliceView



[SliceView]().  [More...](#detailed-description)


`#include <SliceView.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[SliceChange](../Classes/classshapeworks_1_1SliceView.md#enum-slicechange)** { Down, Up} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[SliceView](../Classes/classshapeworks_1_1SliceView.md#function-sliceview)**([Viewer](../Classes/classshapeworks_1_1Viewer.md) * viewer) |
| void | **[set_volume](../Classes/classshapeworks_1_1SliceView.md#function-set-volume)**(std::shared_ptr< [Image](../Classes/classshapeworks_1_1Image.md) > volume) |
| void | **[add_mesh](../Classes/classshapeworks_1_1SliceView.md#function-add-mesh)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[clear_meshes](../Classes/classshapeworks_1_1SliceView.md#function-clear-meshes)**() |
| void | **[set_orientation](../Classes/classshapeworks_1_1SliceView.md#function-set-orientation)**(int orientation) |
| bool | **[is_image_loaded](../Classes/classshapeworks_1_1SliceView.md#function-is-image-loaded)**() |
| void | **[update_renderer](../Classes/classshapeworks_1_1SliceView.md#function-update-renderer)**() |
| void | **[update_camera](../Classes/classshapeworks_1_1SliceView.md#function-update-camera)**() |
| Point | **[handle_key](../Classes/classshapeworks_1_1SliceView.md#function-handle-key)**(std::string key) |
| void | **[change_slice](../Classes/classshapeworks_1_1SliceView.md#function-change-slice)**(SliceChange change) |
| Point | **[get_slice_position](../Classes/classshapeworks_1_1SliceView.md#function-get-slice-position)**() |
| void | **[set_slice_position](../Classes/classshapeworks_1_1SliceView.md#function-set-slice-position)**(Point point) |
| void | **[set_brightness_and_contrast](../Classes/classshapeworks_1_1SliceView.md#function-set-brightness-and-contrast)**(double brightness, double contrast) |
| double | **[get_spacing](../Classes/classshapeworks_1_1SliceView.md#function-get-spacing)**() |
| int | **[get_orientation_index](../Classes/classshapeworks_1_1SliceView.md#function-get-orientation-index)**() |
| void | **[update_particles](../Classes/classshapeworks_1_1SliceView.md#function-update-particles)**() |
| [Viewer](../Classes/classshapeworks_1_1Viewer.md) * | **[get_viewer](../Classes/classshapeworks_1_1SliceView.md#function-get-viewer)**() |
| bool | **[should_point_show](../Classes/classshapeworks_1_1SliceView.md#function-should-point-show)**(double x, double y, double z) |
| int | **[get_slice_number](../Classes/classshapeworks_1_1SliceView.md#function-get-slice-number)**() |

## Detailed Description

```cpp
class shapeworks::SliceView;
```

[SliceView](). 

Provide slice viewing to the [Viewer](../Classes/classshapeworks_1_1Viewer.md)

## Public Types Documentation

### enum SliceChange

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Down | |   |
| Up | |   |




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
    std::shared_ptr< Image > volume
)
```


### function add_mesh

```cpp
void add_mesh(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function clear_meshes

```cpp
void clear_meshes()
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
Point handle_key(
    std::string key
)
```


### function change_slice

```cpp
void change_slice(
    SliceChange change
)
```


### function get_slice_position

```cpp
Point get_slice_position()
```


### function set_slice_position

```cpp
void set_slice_position(
    Point point
)
```


### function set_brightness_and_contrast

```cpp
void set_brightness_and_contrast(
    double brightness,
    double contrast
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


### function get_slice_number

```cpp
int get_slice_number()
```


-------------------------------

Updated on 2024-05-02 at 21:06:12 +0000