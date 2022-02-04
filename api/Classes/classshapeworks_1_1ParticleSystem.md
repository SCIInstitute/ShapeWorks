---
title: shapeworks::ParticleSystem

---

# shapeworks::ParticleSystem





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md#function-particlesystem)**(const std::vector< std::string > & paths) |
| | **[ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md#function-particlesystem)**(const Eigen::MatrixXd & matrix) |
| const Eigen::MatrixXd & | **[Particles](../Classes/classshapeworks_1_1ParticleSystem.md#function-particles)**() const |
| const std::vector< std::string > & | **[Paths](../Classes/classshapeworks_1_1ParticleSystem.md#function-paths)**() const |
| int | **[N](../Classes/classshapeworks_1_1ParticleSystem.md#function-n)**() const |
| int | **[D](../Classes/classshapeworks_1_1ParticleSystem.md#function-d)**() const |
| bool | **[ExactCompare](../Classes/classshapeworks_1_1ParticleSystem.md#function-exactcompare)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & other) const |
| bool | **[EvaluationCompare](../Classes/classshapeworks_1_1ParticleSystem.md#function-evaluationcompare)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & other) const |

## Friends

|                | Name           |
| -------------- | -------------- |
| struct | **[SharedCommandData](../Classes/classshapeworks_1_1ParticleSystem.md#friend-sharedcommanddata)**  |

## Public Functions Documentation

### function ParticleSystem

```cpp
ParticleSystem(
    const std::vector< std::string > & paths
)
```


### function ParticleSystem

```cpp
ParticleSystem(
    const Eigen::MatrixXd & matrix
)
```


### function Particles

```cpp
inline const Eigen::MatrixXd & Particles() const
```


### function Paths

```cpp
inline const std::vector< std::string > & Paths() const
```


### function N

```cpp
inline int N() const
```


### function D

```cpp
inline int D() const
```


### function ExactCompare

```cpp
bool ExactCompare(
    const ParticleSystem & other
) const
```


### function EvaluationCompare

```cpp
bool EvaluationCompare(
    const ParticleSystem & other
) const
```


## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData();
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000