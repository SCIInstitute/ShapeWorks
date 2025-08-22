---
title: shapeworks::OptimizationVisualizer

---

# shapeworks::OptimizationVisualizer





## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[AddMesh](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-addmesh)**(vtkPolyData * mesh, std::shared_ptr< trimesh::TriMesh > tmesh) |
| void | **[IterationCallback](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-iterationcallback)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * particleSystem) |
| void | **[SetWireFrame](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-setwireframe)**(bool enabled) |
| void | **[SetSaveScreenshots](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-setsavescreenshots)**(bool enabled, std::string path) |
| | **[OptimizationVisualizer](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-optimizationvisualizer)**() |
| | **[~OptimizationVisualizer](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-~optimizationvisualizer)**() |

## Public Functions Documentation

### function AddMesh

```cpp
void AddMesh(
    vtkPolyData * mesh,
    std::shared_ptr< trimesh::TriMesh > tmesh
)
```


### function IterationCallback

```cpp
void IterationCallback(
    ParticleSystem * particleSystem
)
```


### function SetWireFrame

```cpp
void SetWireFrame(
    bool enabled
)
```


### function SetSaveScreenshots

```cpp
void SetSaveScreenshots(
    bool enabled,
    std::string path
)
```


### function OptimizationVisualizer

```cpp
inline OptimizationVisualizer()
```


### function ~OptimizationVisualizer

```cpp
inline ~OptimizationVisualizer()
```


-------------------------------

Updated on 2025-08-22 at 08:23:42 +0000