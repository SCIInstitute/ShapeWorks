---
title: shapeworks::StudioInteractorStyle

---

# shapeworks::StudioInteractorStyle





Inherits from vtkInteractorStyleTrackballCamera

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioInteractorStyle](../Classes/classshapeworks_1_1StudioInteractorStyle.md) * | **[New](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-vtktypemacro)**([StudioInteractorStyle](../Classes/classshapeworks_1_1StudioInteractorStyle.md) , vtkInteractorStyleTrackballCamera ) |
| void | **[set_lightbox](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-set-lightbox)**([Lightbox](../Classes/classshapeworks_1_1Lightbox.md) * lightbox) |
| | **[StudioInteractorStyle](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-studiointeractorstyle)**() |
| virtual | **[~StudioInteractorStyle](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-~studiointeractorstyle)**() |
| void | **[OnLeftButtonDown](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-onleftbuttondown)**() override |
| void | **[OnRightButtonDown](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-onrightbuttondown)**() override |
| void | **[OnMouseWheelForward](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-onmousewheelforward)**() override |
| void | **[OnMouseWheelBackward](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-onmousewheelbackward)**() override |
| void | **[OnKeyDown](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-onkeydown)**() override |
| void | **[Dolly](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-dolly)**() override |
| void | **[Rotate](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-rotate)**() override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[Dolly](../Classes/classshapeworks_1_1StudioInteractorStyle.md#function-dolly)**(double factor) override |

## Public Functions Documentation

### function New

```cpp
static StudioInteractorStyle * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    StudioInteractorStyle ,
    vtkInteractorStyleTrackballCamera 
)
```


### function set_lightbox

```cpp
void set_lightbox(
    Lightbox * lightbox
)
```


### function StudioInteractorStyle

```cpp
StudioInteractorStyle()
```


### function ~StudioInteractorStyle

```cpp
virtual ~StudioInteractorStyle()
```


### function OnLeftButtonDown

```cpp
void OnLeftButtonDown() override
```


### function OnRightButtonDown

```cpp
void OnRightButtonDown() override
```


### function OnMouseWheelForward

```cpp
void OnMouseWheelForward() override
```


### function OnMouseWheelBackward

```cpp
void OnMouseWheelBackward() override
```


### function OnKeyDown

```cpp
void OnKeyDown() override
```


### function Dolly

```cpp
void Dolly() override
```


### function Rotate

```cpp
void Rotate() override
```


## Protected Functions Documentation

### function Dolly

```cpp
void Dolly(
    double factor
) override
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000