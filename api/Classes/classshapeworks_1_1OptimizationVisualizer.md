---
title: shapeworks::OptimizationVisualizer

---

# shapeworks::OptimizationVisualizer





## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[AddMesh](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-addmesh)**(vtkPolyData * mesh, std::shared_ptr< trimesh::TriMesh > tmesh) |
| void | **[IterationCallback](../Classes/classshapeworks_1_1OptimizationVisualizer.md#function-iterationcallback)**([itk::ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< 3 > * particleSystem) |
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
    itk::ParticleSystem< 3 > * particleSystem
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

Updated on 2022-01-10 at 16:27:26 +0000