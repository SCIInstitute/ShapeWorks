---
title: shapeworks::StudioHandleWidget

---

# shapeworks::StudioHandleWidget





Inherits from vtkHandleWidget

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioHandleWidget](../Classes/classshapeworks_1_1StudioHandleWidget.md) * | **[New](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-new)**() |
| | **[StudioHandleWidget](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-studiohandlewidget)**() |
| void | **[set_domain](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-set-domain)**(int domain) |
| void | **[set_plane](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-set-plane)**(int plane) |
| void | **[set_point](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-set-point)**(int point) |
| void | **[set_plane_widget](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-set-plane-widget)**([PlaneWidget](../Classes/classshapeworks_1_1PlaneWidget.md) * plane_widget) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[RightClickSelectAction](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-rightclickselectaction)**(vtkAbstractWidget * w) |
| void | **[CustomMoveAction](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-custommoveaction)**(vtkAbstractWidget * w) |
| void | **[CustomSelectAction](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-customselectaction)**(vtkAbstractWidget * w) |
| void | **[CustomEndSelectAction](../Classes/classshapeworks_1_1StudioHandleWidget.md#function-customendselectaction)**(vtkAbstractWidget * w) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[domain_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-domain-)**  |
| int | **[plane_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-plane-)**  |
| int | **[point_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-point-)**  |
| [PlaneWidget](../Classes/classshapeworks_1_1PlaneWidget.md) * | **[plane_widget_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-plane-widget-)**  |
| bool | **[shift_active_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-shift-active-)**  |
| int | **[start_y_](../Classes/classshapeworks_1_1StudioHandleWidget.md#variable-start-y-)**  |

## Public Functions Documentation

### function New

```cpp
static StudioHandleWidget * New()
```


### function StudioHandleWidget

```cpp
StudioHandleWidget()
```


### function set_domain

```cpp
inline void set_domain(
    int domain
)
```


### function set_plane

```cpp
inline void set_plane(
    int plane
)
```


### function set_point

```cpp
inline void set_point(
    int point
)
```


### function set_plane_widget

```cpp
inline void set_plane_widget(
    PlaneWidget * plane_widget
)
```


## Protected Functions Documentation

### function RightClickSelectAction

```cpp
static void RightClickSelectAction(
    vtkAbstractWidget * w
)
```


### function CustomMoveAction

```cpp
static void CustomMoveAction(
    vtkAbstractWidget * w
)
```


### function CustomSelectAction

```cpp
static void CustomSelectAction(
    vtkAbstractWidget * w
)
```


### function CustomEndSelectAction

```cpp
static void CustomEndSelectAction(
    vtkAbstractWidget * w
)
```


## Protected Attributes Documentation

### variable domain_

```cpp
int domain_ = -1;
```


### variable plane_

```cpp
int plane_ = -1;
```


### variable point_

```cpp
int point_ = -1;
```


### variable plane_widget_

```cpp
PlaneWidget * plane_widget_;
```


### variable shift_active_

```cpp
bool shift_active_ = false;
```


### variable start_y_

```cpp
int start_y_ = 0;
```


-------------------------------

Updated on 2022-03-07 at 00:21:28 +0000