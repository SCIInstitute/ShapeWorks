---
title: LegacyMeshGenerator

---

# LegacyMeshGenerator





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[LegacyMeshGenerator](../Classes/classLegacyMeshGenerator.md#function-legacymeshgenerator)**() |
| | **[~LegacyMeshGenerator](../Classes/classLegacyMeshGenerator.md#function-~legacymeshgenerator)**() |
| void | **[setNeighborhoodSize](../Classes/classLegacyMeshGenerator.md#function-setneighborhoodsize)**(int size) |
| void | **[setSampleSpacing](../Classes/classLegacyMeshGenerator.md#function-setsamplespacing)**(double spacing) |
| void | **[setUsePowerCrust](../Classes/classLegacyMeshGenerator.md#function-setusepowercrust)**(bool enabled) |
| void | **[setSmoothingAmount](../Classes/classLegacyMeshGenerator.md#function-setsmoothingamount)**(float amount) |
| vtkSmartPointer< vtkPolyData > | **[buildMesh](../Classes/classLegacyMeshGenerator.md#function-buildmesh)**(const Eigen::VectorXd & shape) |

## Public Functions Documentation

### function LegacyMeshGenerator

```cpp
LegacyMeshGenerator()
```


### function ~LegacyMeshGenerator

```cpp
~LegacyMeshGenerator()
```


### function setNeighborhoodSize

```cpp
void setNeighborhoodSize(
    int size
)
```


### function setSampleSpacing

```cpp
void setSampleSpacing(
    double spacing
)
```


### function setUsePowerCrust

```cpp
void setUsePowerCrust(
    bool enabled
)
```


### function setSmoothingAmount

```cpp
void setSmoothingAmount(
    float amount
)
```


### function buildMesh

```cpp
vtkSmartPointer< vtkPolyData > buildMesh(
    const Eigen::VectorXd & shape
)
```


-------------------------------

Updated on 2024-07-12 at 19:19:43 +0000