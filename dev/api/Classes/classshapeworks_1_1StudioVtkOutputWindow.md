---
title: shapeworks::StudioVtkOutputWindow
summary: Implementation of vtkOutputWindow to capture and display VTK error messages. 

---

# shapeworks::StudioVtkOutputWindow



Implementation of vtkOutputWindow to capture and display VTK error messages. 


`#include <StudioVtkOutputWindow.h>`

Inherits from QObject, vtkOutputWindow

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md) * | **[New](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-vtktypemacro)**([StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md) , vtkOutputWindow ) |
| | **[StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-studiovtkoutputwindow)**() |
| void | **[DisplayErrorText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displayerrortext)**(const char * text) override |
| void | **[DisplayWarningText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displaywarningtext)**(const char * text) override |
| void | **[DisplayGenericWarningText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displaygenericwarningtext)**(const char * text) override |
| void | **[DisplayDebugText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displaydebugtext)**(const char * text) override |

## Public Functions Documentation

### function New

```cpp
static StudioVtkOutputWindow * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    StudioVtkOutputWindow ,
    vtkOutputWindow 
)
```


### function StudioVtkOutputWindow

```cpp
StudioVtkOutputWindow()
```


### function DisplayErrorText

```cpp
void DisplayErrorText(
    const char * text
) override
```


### function DisplayWarningText

```cpp
void DisplayWarningText(
    const char * text
) override
```


### function DisplayGenericWarningText

```cpp
void DisplayGenericWarningText(
    const char * text
) override
```


### function DisplayDebugText

```cpp
void DisplayDebugText(
    const char * text
) override
```


-------------------------------

Updated on 2024-07-31 at 00:01:36 +0000