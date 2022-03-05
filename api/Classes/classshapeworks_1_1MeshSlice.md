---
title: shapeworks::MeshSlice
summary: MeshSlice. 

---

# shapeworks::MeshSlice



[MeshSlice]().  [More...](#detailed-description)


`#include <MeshSlice.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshSlice](../Classes/classshapeworks_1_1MeshSlice.md#function-meshslice)**([Viewer](../Classes/classshapeworks_1_1Viewer.md) * viewer) |
| void | **[set_volume](../Classes/classshapeworks_1_1MeshSlice.md#function-set-volume)**(vtkSmartPointer< vtkImageData > volume) |
| bool | **[is_image_loaded](../Classes/classshapeworks_1_1MeshSlice.md#function-is-image-loaded)**() |
| void | **[update_renderer](../Classes/classshapeworks_1_1MeshSlice.md#function-update-renderer)**() |
| void | **[update_camera](../Classes/classshapeworks_1_1MeshSlice.md#function-update-camera)**() |
| void | **[handle_key](../Classes/classshapeworks_1_1MeshSlice.md#function-handle-key)**(std::string key) |

## Detailed Description

```cpp
class shapeworks::MeshSlice;
```

[MeshSlice](). 

Provide slice viewing to the [Viewer](../Classes/classshapeworks_1_1Viewer.md)

## Public Functions Documentation

### function MeshSlice

```cpp
MeshSlice(
    Viewer * viewer
)
```


### function set_volume

```cpp
void set_volume(
    vtkSmartPointer< vtkImageData > volume
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


-------------------------------

Updated on 2022-03-05 at 23:20:33 +0000