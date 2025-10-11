---
title: shapeworks::ShapeWorksVtkOutputWindow
summary: Implementation of vtkOutputWindow to capture and display VTK error messages. 

---

# shapeworks::ShapeWorksVtkOutputWindow



Implementation of vtkOutputWindow to capture and display VTK error messages. 


`#include <ShapeWorksVtkOutputWindow.h>`

Inherits from QObject, vtkOutputWindow

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [ShapeWorksVtkOutputWindow](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md) * | **[New](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-vtktypemacro)**([ShapeWorksVtkOutputWindow](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md) , vtkOutputWindow ) |
| | **[ShapeWorksVtkOutputWindow](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-shapeworksvtkoutputwindow)**() |
| void | **[DisplayErrorText](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-displayerrortext)**(const char * text) override |
| void | **[DisplayWarningText](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-displaywarningtext)**(const char * text) override |
| void | **[DisplayGenericWarningText](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-displaygenericwarningtext)**(const char * text) override |
| void | **[DisplayDebugText](../Classes/classshapeworks_1_1ShapeWorksVtkOutputWindow.md#function-displaydebugtext)**(const char * text) override |

## Public Functions Documentation

### function New

```cpp
static ShapeWorksVtkOutputWindow * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    ShapeWorksVtkOutputWindow ,
    vtkOutputWindow 
)
```


### function ShapeWorksVtkOutputWindow

```cpp
ShapeWorksVtkOutputWindow()
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

Updated on 2025-10-11 at 18:16:33 +0000