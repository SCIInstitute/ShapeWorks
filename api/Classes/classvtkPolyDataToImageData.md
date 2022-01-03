---
title: vtkPolyDataToImageData

---

# vtkPolyDataToImageData





Inherits from vtkImageAlgorithm

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[vtkTypeMacro](../Classes/classvtkPolyDataToImageData.md#function-vtktypemacro)**([vtkPolyDataToImageData](../Classes/classvtkPolyDataToImageData.md) , vtkImageAlgorithm ) |
| void | **[PrintSelf](../Classes/classvtkPolyDataToImageData.md#function-printself)**(ostream & os, vtkIndent indent) |
| | **[vtkSetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtksetmacro)**(Border , unsigned int ) |
| | **[vtkGetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtkgetmacro)**(Border , unsigned int ) |
| | **[vtkSetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtksetmacro)**(Spacing , double ) |
| | **[vtkGetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtkgetmacro)**(Spacing , double ) |
| | **[vtkSetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtksetmacro)**(SampleDistance , double ) |
| | **[vtkGetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtkgetmacro)**(SampleDistance , double ) |
| | **[vtkSetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtksetmacro)**(InitialBallSize , unsigned int ) |
| | **[vtkGetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtkgetmacro)**(InitialBallSize , unsigned int ) |
| | **[vtkSetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtksetmacro)**(FinalBallSize , unsigned int ) |
| | **[vtkGetMacro](../Classes/classvtkPolyDataToImageData.md#function-vtkgetmacro)**(FinalBallSize , unsigned int ) |
| virtual int | **[FillInputPortInformation](../Classes/classvtkPolyDataToImageData.md#function-fillinputportinformation)**(int port, vtkInformation * info) |
| [vtkPolyDataToImageData](../Classes/classvtkPolyDataToImageData.md) * | **[New](../Classes/classvtkPolyDataToImageData.md#function-new)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[vtkPolyDataToImageData](../Classes/classvtkPolyDataToImageData.md#function-vtkpolydatatoimagedata)**() |
| | **[~vtkPolyDataToImageData](../Classes/classvtkPolyDataToImageData.md#function-~vtkpolydatatoimagedata)**() |
| virtual int | **[RequestInformation](../Classes/classvtkPolyDataToImageData.md#function-requestinformation)**(vtkInformation * request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector) |
| virtual int | **[RequestData](../Classes/classvtkPolyDataToImageData.md#function-requestdata)**(vtkInformation * request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector) |
| virtual int | **[RequestUpdateExtent](../Classes/classvtkPolyDataToImageData.md#function-requestupdateextent)**(vtkInformation * request, vtkInformationVector ** inputVector, vtkInformationVector * outputVector) |

## Public Functions Documentation

### function vtkTypeMacro

```cpp
vtkTypeMacro(
    vtkPolyDataToImageData ,
    vtkImageAlgorithm 
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
    Border ,
    unsigned int 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    Border ,
    unsigned int 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    Spacing ,
    double 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    Spacing ,
    double 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    SampleDistance ,
    double 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    SampleDistance ,
    double 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    InitialBallSize ,
    unsigned int 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    InitialBallSize ,
    unsigned int 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    FinalBallSize ,
    unsigned int 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    FinalBallSize ,
    unsigned int 
)
```


### function FillInputPortInformation

```cpp
virtual int FillInputPortInformation(
    int port,
    vtkInformation * info
)
```


### function New

```cpp
static vtkPolyDataToImageData * New()
```


## Protected Functions Documentation

### function vtkPolyDataToImageData

```cpp
vtkPolyDataToImageData()
```


### function ~vtkPolyDataToImageData

```cpp
inline ~vtkPolyDataToImageData()
```


### function RequestInformation

```cpp
virtual int RequestInformation(
    vtkInformation * request,
    vtkInformationVector ** inputVector,
    vtkInformationVector * outputVector
)
```


### function RequestData

```cpp
virtual int RequestData(
    vtkInformation * request,
    vtkInformationVector ** inputVector,
    vtkInformationVector * outputVector
)
```


### function RequestUpdateExtent

```cpp
virtual int RequestUpdateExtent(
    vtkInformation * request,
    vtkInformationVector ** inputVector,
    vtkInformationVector * outputVector
)
```


-------------------------------

Updated on 2022-01-03 at 16:16:10 +0000