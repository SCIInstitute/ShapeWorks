---
title: shapeworks::ShapeEvaluation

---

# shapeworks::ShapeEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeEvaluation](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-shapeevaluation)**() |
| double | **[ComputeCompactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computecompactness)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullCompactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullcompactness)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, std::function< void(float)> progress_callback =nullptr) |
| double | **[ComputeGeneralization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computegeneralization)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullGeneralization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullgeneralization)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, std::function< void(float)> progress_callback =nullptr) |
| double | **[ComputeSpecificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computespecificity)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullSpecificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullspecificity)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & particleSystem, std::function< void(float)> progress_callback =nullptr) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const unsigned | **[VDimension](../Classes/classshapeworks_1_1ShapeEvaluation.md#variable-vdimension)**  |

## Public Functions Documentation

### function ShapeEvaluation

```cpp
ShapeEvaluation()
```


### function ComputeCompactness

```cpp
static double ComputeCompactness(
    const ParticleSystem & particleSystem,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullCompactness

```cpp
static Eigen::VectorXd ComputeFullCompactness(
    const ParticleSystem & particleSystem,
    std::function< void(float)> progress_callback =nullptr
)
```


### function ComputeGeneralization

```cpp
static double ComputeGeneralization(
    const ParticleSystem & particleSystem,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullGeneralization

```cpp
static Eigen::VectorXd ComputeFullGeneralization(
    const ParticleSystem & particleSystem,
    std::function< void(float)> progress_callback =nullptr
)
```


### function ComputeSpecificity

```cpp
static double ComputeSpecificity(
    const ParticleSystem & particleSystem,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullSpecificity

```cpp
static Eigen::VectorXd ComputeFullSpecificity(
    const ParticleSystem & particleSystem,
    std::function< void(float)> progress_callback =nullptr
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static const unsigned VDimension = 3;
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000