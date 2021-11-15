---
title: Vis

---

# Vis





## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[visMesh](../Classes/classVis.md#function-vismesh)**(vtkSmartPointer< vtkPolyData > mesh, std::string window_title ="") |
| void | **[visMeshWithNormals](../Classes/classVis.md#function-vismeshwithnormals)**(vtkSmartPointer< vtkPolyData > mesh, int ratio =10, std::string window_title ="") |
| void | **[visMeshWithParticles](../Classes/classVis.md#function-vismeshwithparticles)**(vtkSmartPointer< vtkPolyData > mesh, vtkSmartPointer< vtkPoints > particles, float glyphRadius =2, std::string window_title ="") |
| void | **[visMeshWithParticles](../Classes/classVis.md#function-vismeshwithparticles)**(vtkSmartPointer< vtkPolyData > mesh, vtkSmartPointer< vtkPoints > particles, std::vector< double > glyphRadii, std::string window_title ="") |
| void | **[visMeshWithParticles](../Classes/classVis.md#function-vismeshwithparticles)**(vtkSmartPointer< vtkPolyData > mesh, vtkSmartPointer< vtkPoints > particles, vtkSmartPointer< vtkPoints > particles2, float glyphRadius =2, std::string window_title ="") |
| void | **[visMeshWithParticlesNormals](../Classes/classVis.md#function-vismeshwithparticlesnormals)**(vtkSmartPointer< vtkPolyData > mesh, vtkSmartPointer< vtkPolyData > particlesData, std::string window_title ="") |
| void | **[visParticles](../Classes/classVis.md#function-visparticles)**(vtkSmartPointer< vtkPoints > particles, float glyphRadius =2, std::string window_title ="") |
| void | **[visParticles](../Classes/classVis.md#function-visparticles)**(vtkSmartPointer< vtkPolyData > polydata, float glyphRadius =2, std::string window_title ="") |
| void | **[visParticles](../Classes/classVis.md#function-visparticles)**(vtkSmartPointer< vtkPoints > particles, vtkSmartPointer< vtkPoints > particles2, float glyphRadius =2, std::string window_title ="") |

## Public Functions Documentation

### function visMesh

```cpp
static void visMesh(
    vtkSmartPointer< vtkPolyData > mesh,
    std::string window_title =""
)
```


### function visMeshWithNormals

```cpp
static void visMeshWithNormals(
    vtkSmartPointer< vtkPolyData > mesh,
    int ratio =10,
    std::string window_title =""
)
```


### function visMeshWithParticles

```cpp
static void visMeshWithParticles(
    vtkSmartPointer< vtkPolyData > mesh,
    vtkSmartPointer< vtkPoints > particles,
    float glyphRadius =2,
    std::string window_title =""
)
```


### function visMeshWithParticles

```cpp
static void visMeshWithParticles(
    vtkSmartPointer< vtkPolyData > mesh,
    vtkSmartPointer< vtkPoints > particles,
    std::vector< double > glyphRadii,
    std::string window_title =""
)
```


### function visMeshWithParticles

```cpp
static void visMeshWithParticles(
    vtkSmartPointer< vtkPolyData > mesh,
    vtkSmartPointer< vtkPoints > particles,
    vtkSmartPointer< vtkPoints > particles2,
    float glyphRadius =2,
    std::string window_title =""
)
```


### function visMeshWithParticlesNormals

```cpp
static void visMeshWithParticlesNormals(
    vtkSmartPointer< vtkPolyData > mesh,
    vtkSmartPointer< vtkPolyData > particlesData,
    std::string window_title =""
)
```


### function visParticles

```cpp
static void visParticles(
    vtkSmartPointer< vtkPoints > particles,
    float glyphRadius =2,
    std::string window_title =""
)
```


### function visParticles

```cpp
static void visParticles(
    vtkSmartPointer< vtkPolyData > polydata,
    float glyphRadius =2,
    std::string window_title =""
)
```


### function visParticles

```cpp
static void visParticles(
    vtkSmartPointer< vtkPoints > particles,
    vtkSmartPointer< vtkPoints > particles2,
    float glyphRadius =2,
    std::string window_title =""
)
```


-------------------------------

Updated on 2021-11-15 at 18:13:25 +0000