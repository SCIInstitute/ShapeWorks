---
title: shapeworks::PlaneWidget
summary: PlaneWidget. 

---

# shapeworks::PlaneWidget



[PlaneWidget]().  [More...](#detailed-description)


`#include <PlaneWidget.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[PlaneWidget](../Classes/classshapeworks_1_1PlaneWidget.md#function-planewidget)**([Viewer](../Classes/classshapeworks_1_1Viewer.md) * viewer) |
| | **[~PlaneWidget](../Classes/classshapeworks_1_1PlaneWidget.md#function-~planewidget)**() |
| void | **[update](../Classes/classshapeworks_1_1PlaneWidget.md#function-update)**()<br>Update the widget from shape data.  |
| void | **[store_positions](../Classes/classshapeworks_1_1PlaneWidget.md#function-store-positions)**()<br>Store positions back to shape data (this is called by callback)  |
| void | **[update_glyph_properties](../Classes/classshapeworks_1_1PlaneWidget.md#function-update-glyph-properties)**()<br>Update the sizes and detail of control spheres.  |
| void | **[clear_planes](../Classes/classshapeworks_1_1PlaneWidget.md#function-clear-planes)**()<br>Clear point handles and planes.  |
| void | **[handle_right_click](../Classes/classshapeworks_1_1PlaneWidget.md#function-handle-right-click)**(int domain, int plane, int point)<br>Handle a right click on a particular point.  |
| void | **[delete_plane](../Classes/classshapeworks_1_1PlaneWidget.md#function-delete-plane)**(int domain, int plane_id)<br>Delete a particular plane.  |
| void | **[flip_plane](../Classes/classshapeworks_1_1PlaneWidget.md#function-flip-plane)**(int domain, int plane_id)<br>Flip the normal for a given plane.  |
| void | **[apply_plane](../Classes/classshapeworks_1_1PlaneWidget.md#function-apply-plane)**(int domain, int plane_id)<br>Apply a plane from one shape to all others (e.g. copy and paste)  |
| void | **[set_plane_offset](../Classes/classshapeworks_1_1PlaneWidget.md#function-set-plane-offset)**(int domain, int plane_id, int offset)<br>Set an offset for a given plane.  |
| void | **[finalize_plane_offset](../Classes/classshapeworks_1_1PlaneWidget.md#function-finalize-plane-offset)**(int domain, int plane_id)<br>Finalize the offset for a given plane.  |

## Detailed Description

```cpp
class shapeworks::PlaneWidget;
```

[PlaneWidget](). 

Widget to display and manipulate constraint planes 

## Public Functions Documentation

### function PlaneWidget

```cpp
PlaneWidget(
    Viewer * viewer
)
```


### function ~PlaneWidget

```cpp
~PlaneWidget()
```


### function update

```cpp
void update()
```

Update the widget from shape data. 

### function store_positions

```cpp
void store_positions()
```

Store positions back to shape data (this is called by callback) 

### function update_glyph_properties

```cpp
void update_glyph_properties()
```

Update the sizes and detail of control spheres. 

### function clear_planes

```cpp
void clear_planes()
```

Clear point handles and planes. 

### function handle_right_click

```cpp
void handle_right_click(
    int domain,
    int plane,
    int point
)
```

Handle a right click on a particular point. 

### function delete_plane

```cpp
void delete_plane(
    int domain,
    int plane_id
)
```

Delete a particular plane. 

### function flip_plane

```cpp
void flip_plane(
    int domain,
    int plane_id
)
```

Flip the normal for a given plane. 

### function apply_plane

```cpp
void apply_plane(
    int domain,
    int plane_id
)
```

Apply a plane from one shape to all others (e.g. copy and paste) 

### function set_plane_offset

```cpp
void set_plane_offset(
    int domain,
    int plane_id,
    int offset
)
```

Set an offset for a given plane. 

### function finalize_plane_offset

```cpp
void finalize_plane_offset(
    int domain,
    int plane_id
)
```

Finalize the offset for a given plane. 

-------------------------------

Updated on 2022-03-07 at 00:21:27 +0000