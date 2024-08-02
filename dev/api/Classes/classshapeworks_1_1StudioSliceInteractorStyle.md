---
title: shapeworks::StudioSliceInteractorStyle

---

# shapeworks::StudioSliceInteractorStyle





Inherits from vtkInteractorStyleImage

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioSliceInteractorStyle](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md) * | **[New](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-vtktypemacro)**([StudioSliceInteractorStyle](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md) , vtkInteractorStyleImage ) |
| void | **[set_lightbox](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-set-lightbox)**([Lightbox](../Classes/classshapeworks_1_1Lightbox.md) * lightbox) |
| | **[StudioSliceInteractorStyle](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-studiosliceinteractorstyle)**() |
| virtual | **[~StudioSliceInteractorStyle](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-~studiosliceinteractorstyle)**() |
| void | **[OnLeftButtonDown](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-onleftbuttondown)**() override |
| void | **[OnRightButtonDown](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-onrightbuttondown)**() override |
| void | **[OnMouseWheelForward](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-onmousewheelforward)**() override |
| void | **[OnMouseWheelBackward](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-onmousewheelbackward)**() override |
| void | **[OnKeyDown](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-onkeydown)**() override |
| void | **[WindowLevel](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-windowlevel)**() override |
| void | **[StartWindowLevel](../Classes/classshapeworks_1_1StudioSliceInteractorStyle.md#function-startwindowlevel)**() override |

## Public Functions Documentation

### function New

```cpp
static StudioSliceInteractorStyle * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    StudioSliceInteractorStyle ,
    vtkInteractorStyleImage 
)
```


### function set_lightbox

```cpp
void set_lightbox(
    Lightbox * lightbox
)
```


### function StudioSliceInteractorStyle

```cpp
StudioSliceInteractorStyle()
```


### function ~StudioSliceInteractorStyle

```cpp
virtual ~StudioSliceInteractorStyle()
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


### function WindowLevel

```cpp
void WindowLevel() override
```


### function StartWindowLevel

```cpp
void StartWindowLevel() override
```


-------------------------------

Updated on 2024-08-02 at 05:19:10 +0000