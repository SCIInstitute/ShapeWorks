---
title: shapeworks::ShapeEvaluation

---

# shapeworks::ShapeEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeEvaluation](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-shapeevaluation)**() |
| double | **[ComputeCompactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computecompactness)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullCompactness](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullcompactness)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, std::function< void(float)> progress_callback =nullptr) |
| double | **[ComputeGeneralization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computegeneralization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullGeneralization](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullgeneralization)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, std::function< void(float)> progress_callback =nullptr) |
| double | **[ComputeSpecificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computespecificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, const int nModes, const std::string & saveTo ="") |
| Eigen::VectorXd | **[ComputeFullSpecificity](../Classes/classshapeworks_1_1ShapeEvaluation.md#function-computefullspecificity)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & ParticleSystemEvaluation, std::function< void(float)> progress_callback =nullptr) |

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
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullCompactness

```cpp
static Eigen::VectorXd ComputeFullCompactness(
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    std::function< void(float)> progress_callback =nullptr
)
```


### function ComputeGeneralization

```cpp
static double ComputeGeneralization(
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullGeneralization

```cpp
static Eigen::VectorXd ComputeFullGeneralization(
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    std::function< void(float)> progress_callback =nullptr
)
```


### function ComputeSpecificity

```cpp
static double ComputeSpecificity(
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    const int nModes,
    const std::string & saveTo =""
)
```


### function ComputeFullSpecificity

```cpp
static Eigen::VectorXd ComputeFullSpecificity(
    const ParticleSystemEvaluation & ParticleSystemEvaluation,
    std::function< void(float)> progress_callback =nullptr
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static const unsigned VDimension = 3;
```


-------------------------------

Updated on 2023-07-13 at 17:18:58 +0000