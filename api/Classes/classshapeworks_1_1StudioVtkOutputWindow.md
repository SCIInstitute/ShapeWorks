---
title: shapeworks::StudioVtkOutputWindow
summary: Implementation of vtkOutputWindow to capture and display VTK error messages. 

---

# shapeworks::StudioVtkOutputWindow



Implementation of vtkOutputWindow to capture and display VTK error messages. 


`#include <StudioVtkOutputWindow.h>`

Inherits from QObject, vtkOutputWindow

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[warning](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#signal-warning)**(QString message) |
| void | **[error](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#signal-error)**(QString message) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md) * | **[New](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-vtktypemacro)**([StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md) , vtkOutputWindow ) |
| | **[StudioVtkOutputWindow](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-studiovtkoutputwindow)**() |
| void | **[DisplayErrorText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displayerrortext)**(const char * text) override |
| void | **[DisplayWarningText](../Classes/classshapeworks_1_1StudioVtkOutputWindow.md#function-displaywarningtext)**(const char * text) override |

## Public Signals Documentation

### signal warning

```cpp
void warning(
    QString message
)
```


### signal error

```cpp
void error(
    QString message
)
```


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


-------------------------------

Updated on 2022-03-03 at 07:50:36 +0000