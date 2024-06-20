---
title: shapeworks::PaintWidget

---

# shapeworks::PaintWidget





Inherits from vtkAbstractWidget

## Protected Types

|                | Name           |
| -------------- | -------------- |
| enum| **[@5](../Classes/classshapeworks_1_1PaintWidget.md#enum-@5)** { Start, Paint, Erase} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [PaintWidget](../Classes/classshapeworks_1_1PaintWidget.md) * | **[New](../Classes/classshapeworks_1_1PaintWidget.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1PaintWidget.md#function-vtktypemacro)**([PaintWidget](../Classes/classshapeworks_1_1PaintWidget.md) , vtkAbstractWidget ) |
| void | **[PrintSelf](../Classes/classshapeworks_1_1PaintWidget.md#function-printself)**(ostream & os, vtkIndent indent) |
| | **[vtkSetMacro](../Classes/classshapeworks_1_1PaintWidget.md#function-vtksetmacro)**(PointPlacer , vtkPointPlacer * ) |
| | **[vtkGetMacro](../Classes/classshapeworks_1_1PaintWidget.md#function-vtkgetmacro)**(PointPlacer , vtkPointPlacer * ) |
| | **[vtkSetMacro](../Classes/classshapeworks_1_1PaintWidget.md#function-vtksetmacro)**(Renderer , vtkRenderer * ) |
| | **[vtkGetMacro](../Classes/classshapeworks_1_1PaintWidget.md#function-vtkgetmacro)**(Renderer , vtkRenderer * ) |
| void | **[set_viewer](../Classes/classshapeworks_1_1PaintWidget.md#function-set-viewer)**([Viewer](../Classes/classshapeworks_1_1Viewer.md) * viewer) |
| virtual void | **[SetEnabled](../Classes/classshapeworks_1_1PaintWidget.md#function-setenabled)**(int ) |
| void | **[SetRepresentation](../Classes/classshapeworks_1_1PaintWidget.md#function-setrepresentation)**(vtkContourRepresentation * r) |
| void | **[CreateDefaultRepresentation](../Classes/classshapeworks_1_1PaintWidget.md#function-createdefaultrepresentation)**() |
| void | **[update_position](../Classes/classshapeworks_1_1PaintWidget.md#function-update-position)**() |
| virtual void | **[Initialize](../Classes/classshapeworks_1_1PaintWidget.md#function-initialize)**(vtkPolyData * poly, int state =1) |
| virtual void | **[Initialize](../Classes/classshapeworks_1_1PaintWidget.md#function-initialize)**() |
| void | **[set_brush_size](../Classes/classshapeworks_1_1PaintWidget.md#function-set-brush-size)**(double size) |
| double | **[get_brush_size](../Classes/classshapeworks_1_1PaintWidget.md#function-get-brush-size)**() |
| void | **[set_brush_color](../Classes/classshapeworks_1_1PaintWidget.md#function-set-brush-color)**(float r, float g, float b) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[PaintWidget](../Classes/classshapeworks_1_1PaintWidget.md#function-paintwidget)**() |
| | **[~PaintWidget](../Classes/classshapeworks_1_1PaintWidget.md#function-~paintwidget)**() |
| bool | **[use_point_placer](../Classes/classshapeworks_1_1PaintWidget.md#function-use-point-placer)**(double displayPos[2], int newState) |
| void | **[set_cursor](../Classes/classshapeworks_1_1PaintWidget.md#function-set-cursor)**(int requestedShape) |
| void | **[StartPaintAction](../Classes/classshapeworks_1_1PaintWidget.md#function-startpaintaction)**(vtkAbstractWidget * w) |
| void | **[EndPaintAction](../Classes/classshapeworks_1_1PaintWidget.md#function-endpaintaction)**(vtkAbstractWidget * w) |
| void | **[StartEraseAction](../Classes/classshapeworks_1_1PaintWidget.md#function-starteraseaction)**(vtkAbstractWidget * w) |
| void | **[EndEraseAction](../Classes/classshapeworks_1_1PaintWidget.md#function-enderaseaction)**(vtkAbstractWidget * w) |
| void | **[MoveAction](../Classes/classshapeworks_1_1PaintWidget.md#function-moveaction)**(vtkAbstractWidget * w) |
| void | **[LeaveAction](../Classes/classshapeworks_1_1PaintWidget.md#function-leaveaction)**(vtkAbstractWidget * w) |
| void | **[KeyPressAction](../Classes/classshapeworks_1_1PaintWidget.md#function-keypressaction)**(vtkAbstractWidget * w) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[WidgetState](../Classes/classshapeworks_1_1PaintWidget.md#variable-widgetstate)**  |
| vtkPointPlacer * | **[PointPlacer](../Classes/classshapeworks_1_1PaintWidget.md#variable-pointplacer)**  |
| vtkRenderer * | **[Renderer](../Classes/classshapeworks_1_1PaintWidget.md#variable-renderer)**  |
| StudioSphereRepresentation * | **[sphere_cursor_](../Classes/classshapeworks_1_1PaintWidget.md#variable-sphere-cursor-)**  |
| bool | **[mouse_in_window_](../Classes/classshapeworks_1_1PaintWidget.md#variable-mouse-in-window-)**  |
| [Viewer](../Classes/classshapeworks_1_1Viewer.md) * | **[viewer_](../Classes/classshapeworks_1_1PaintWidget.md#variable-viewer-)**  |

## Protected Types Documentation

### enum @5

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Start | |   |
| Paint | |   |
| Erase | |   |




## Public Functions Documentation

### function New

```cpp
static PaintWidget * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    PaintWidget ,
    vtkAbstractWidget 
)
```


### function PrintSelf

```cpp
void PrintSelf(
    ostream & os,
    vtkIndent indent
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    PointPlacer ,
    vtkPointPlacer * 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    PointPlacer ,
    vtkPointPlacer * 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    Renderer ,
    vtkRenderer * 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    Renderer ,
    vtkRenderer * 
)
```


### function set_viewer

```cpp
void set_viewer(
    Viewer * viewer
)
```


### function SetEnabled

```cpp
virtual void SetEnabled(
    int 
)
```


### function SetRepresentation

```cpp
inline void SetRepresentation(
    vtkContourRepresentation * r
)
```


### function CreateDefaultRepresentation

```cpp
void CreateDefaultRepresentation()
```


### function update_position

```cpp
void update_position()
```


### function Initialize

```cpp
virtual void Initialize(
    vtkPolyData * poly,
    int state =1
)
```


### function Initialize

```cpp
inline virtual void Initialize()
```


### function set_brush_size

```cpp
void set_brush_size(
    double size
)
```


### function get_brush_size

```cpp
double get_brush_size()
```


### function set_brush_color

```cpp
void set_brush_color(
    float r,
    float g,
    float b
)
```


## Protected Functions Documentation

### function PaintWidget

```cpp
PaintWidget()
```


### function ~PaintWidget

```cpp
~PaintWidget()
```


### function use_point_placer

```cpp
bool use_point_placer(
    double displayPos[2],
    int newState
)
```


### function set_cursor

```cpp
void set_cursor(
    int requestedShape
)
```


### function StartPaintAction

```cpp
static void StartPaintAction(
    vtkAbstractWidget * w
)
```


### function EndPaintAction

```cpp
static void EndPaintAction(
    vtkAbstractWidget * w
)
```


### function StartEraseAction

```cpp
static void StartEraseAction(
    vtkAbstractWidget * w
)
```


### function EndEraseAction

```cpp
static void EndEraseAction(
    vtkAbstractWidget * w
)
```


### function MoveAction

```cpp
static void MoveAction(
    vtkAbstractWidget * w
)
```


### function LeaveAction

```cpp
static void LeaveAction(
    vtkAbstractWidget * w
)
```


### function KeyPressAction

```cpp
static void KeyPressAction(
    vtkAbstractWidget * w
)
```


## Protected Attributes Documentation

### variable WidgetState

```cpp
int WidgetState;
```


### variable PointPlacer

```cpp
vtkPointPlacer * PointPlacer;
```


### variable Renderer

```cpp
vtkRenderer * Renderer;
```


### variable sphere_cursor_

```cpp
StudioSphereRepresentation * sphere_cursor_;
```


### variable mouse_in_window_

```cpp
bool mouse_in_window_;
```


### variable viewer_

```cpp
Viewer * viewer_;
```


-------------------------------

Updated on 2024-06-20 at 05:15:49 +0000