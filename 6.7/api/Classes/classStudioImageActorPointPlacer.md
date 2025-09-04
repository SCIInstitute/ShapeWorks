---
title: StudioImageActorPointPlacer

---

# StudioImageActorPointPlacer





Inherits from vtkImageActorPointPlacer

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [StudioImageActorPointPlacer](../Classes/classStudioImageActorPointPlacer.md) * | **[New](../Classes/classStudioImageActorPointPlacer.md#function-new)**() |
| | **[vtkTypeMacro](../Classes/classStudioImageActorPointPlacer.md#function-vtktypemacro)**([StudioImageActorPointPlacer](../Classes/classStudioImageActorPointPlacer.md) , vtkImageActorPointPlacer ) |
| void | **[PrintSelf](../Classes/classStudioImageActorPointPlacer.md#function-printself)**(ostream & os, vtkIndent indent) override |
| int | **[ComputeWorldPosition](../Classes/classStudioImageActorPointPlacer.md#function-computeworldposition)**(vtkRenderer * renderer, double displayPos[2], double worldPos[3], double worldOrient[9]) override |
| int | **[ValidateWorldPosition](../Classes/classStudioImageActorPointPlacer.md#function-validateworldposition)**(double worldPos[3]) override |
| int | **[UpdateWorldPosition](../Classes/classStudioImageActorPointPlacer.md#function-updateworldposition)**(vtkRenderer * renderer, double worldPos[3], double worldOrient[9]) override |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[StudioImageActorPointPlacer](../Classes/classStudioImageActorPointPlacer.md#function-studioimageactorpointplacer)**() |
| | **[~StudioImageActorPointPlacer](../Classes/classStudioImageActorPointPlacer.md#function-~studioimageactorpointplacer)**() override |

## Public Functions Documentation

### function New

```cpp
static StudioImageActorPointPlacer * New()
```


### function vtkTypeMacro

```cpp
vtkTypeMacro(
    StudioImageActorPointPlacer ,
    vtkImageActorPointPlacer 
)
```


### function PrintSelf

```cpp
void PrintSelf(
    ostream & os,
    vtkIndent indent
) override
```


### function ComputeWorldPosition

```cpp
int ComputeWorldPosition(
    vtkRenderer * renderer,
    double displayPos[2],
    double worldPos[3],
    double worldOrient[9]
) override
```


### function ValidateWorldPosition

```cpp
int ValidateWorldPosition(
    double worldPos[3]
) override
```


### function UpdateWorldPosition

```cpp
int UpdateWorldPosition(
    vtkRenderer * renderer,
    double worldPos[3],
    double worldOrient[9]
) override
```


## Protected Functions Documentation

### function StudioImageActorPointPlacer

```cpp
StudioImageActorPointPlacer()
```


### function ~StudioImageActorPointPlacer

```cpp
~StudioImageActorPointPlacer() override
```


-------------------------------

Updated on 2025-09-04 at 08:01:26 +0000