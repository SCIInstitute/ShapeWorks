---
title: CustomSurfaceReconstructionFilter

---

# CustomSurfaceReconstructionFilter





Inherits from vtkImageAlgorithm

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[vtkTypeMacro](../Classes/classCustomSurfaceReconstructionFilter.md#function-vtktypemacro)**([CustomSurfaceReconstructionFilter](../Classes/classCustomSurfaceReconstructionFilter.md) , vtkImageAlgorithm ) |
| void | **[PrintSelf](../Classes/classCustomSurfaceReconstructionFilter.md#function-printself)**(ostream & os, vtkIndent indent) |
| | **[vtkGetMacro](../Classes/classCustomSurfaceReconstructionFilter.md#function-vtkgetmacro)**(NeighborhoodSize , int ) |
| | **[vtkSetMacro](../Classes/classCustomSurfaceReconstructionFilter.md#function-vtksetmacro)**(NeighborhoodSize , int ) |
| | **[vtkGetMacro](../Classes/classCustomSurfaceReconstructionFilter.md#function-vtkgetmacro)**(SampleSpacing , double ) |
| | **[vtkSetMacro](../Classes/classCustomSurfaceReconstructionFilter.md#function-vtksetmacro)**(SampleSpacing , double ) |
| [CustomSurfaceReconstructionFilter](../Classes/classCustomSurfaceReconstructionFilter.md) * | **[New](../Classes/classCustomSurfaceReconstructionFilter.md#function-new)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[CustomSurfaceReconstructionFilter](../Classes/classCustomSurfaceReconstructionFilter.md#function-customsurfacereconstructionfilter)**() |
| | **[~CustomSurfaceReconstructionFilter](../Classes/classCustomSurfaceReconstructionFilter.md#function-~customsurfacereconstructionfilter)**() |
| virtual int | **[RequestInformation](../Classes/classCustomSurfaceReconstructionFilter.md#function-requestinformation)**(vtkInformation * , vtkInformationVector ** , vtkInformationVector * ) |
| virtual int | **[RequestData](../Classes/classCustomSurfaceReconstructionFilter.md#function-requestdata)**(vtkInformation * , vtkInformationVector ** , vtkInformationVector * ) |
| virtual int | **[FillInputPortInformation](../Classes/classCustomSurfaceReconstructionFilter.md#function-fillinputportinformation)**(int , vtkInformation * ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[NeighborhoodSize](../Classes/classCustomSurfaceReconstructionFilter.md#variable-neighborhoodsize)**  |
| double | **[SampleSpacing](../Classes/classCustomSurfaceReconstructionFilter.md#variable-samplespacing)**  |

## Public Functions Documentation

### function vtkTypeMacro

```cpp
vtkTypeMacro(
    CustomSurfaceReconstructionFilter ,
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


### function vtkGetMacro

```cpp
vtkGetMacro(
    NeighborhoodSize ,
    int 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    NeighborhoodSize ,
    int 
)
```


### function vtkGetMacro

```cpp
vtkGetMacro(
    SampleSpacing ,
    double 
)
```


### function vtkSetMacro

```cpp
vtkSetMacro(
    SampleSpacing ,
    double 
)
```


### function New

```cpp
static CustomSurfaceReconstructionFilter * New()
```


## Protected Functions Documentation

### function CustomSurfaceReconstructionFilter

```cpp
CustomSurfaceReconstructionFilter()
```


### function ~CustomSurfaceReconstructionFilter

```cpp
inline ~CustomSurfaceReconstructionFilter()
```


### function RequestInformation

```cpp
virtual int RequestInformation(
    vtkInformation * ,
    vtkInformationVector ** ,
    vtkInformationVector * 
)
```


### function RequestData

```cpp
virtual int RequestData(
    vtkInformation * ,
    vtkInformationVector ** ,
    vtkInformationVector * 
)
```


### function FillInputPortInformation

```cpp
virtual int FillInputPortInformation(
    int ,
    vtkInformation * 
)
```


## Protected Attributes Documentation

### variable NeighborhoodSize

```cpp
int NeighborhoodSize;
```


### variable SampleSpacing

```cpp
double SampleSpacing;
```


-------------------------------

Updated on 2022-03-05 at 23:20:34 +0000