---
title: shapeworks::ParticleSystemEvaluation

---

# shapeworks::ParticleSystemEvaluation





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-particlesystemevaluation)**(const std::vector< std::string > & paths) |
| | **[ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-particlesystemevaluation)**(const Eigen::MatrixXd & matrix) |
| const Eigen::MatrixXd & | **[Particles](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-particles)**() const |
| const std::vector< std::string > & | **[Paths](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-paths)**() const |
| int | **[N](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-n)**() const<br>Number of samples.  |
| int | **[D](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-d)**() const<br>Dimensions (e.g. x/y/z * number of particles)  |
| bool | **[ExactCompare](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-exactcompare)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & other) const |
| bool | **[EvaluationCompare](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-evaluationcompare)**(const [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) & other) const |
| bool | **[ReadParticleFile](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#function-readparticlefile)**(std::string filename, Eigen::VectorXd & points) |

## Friends

|                | Name           |
| -------------- | -------------- |
| struct | **[SharedCommandData](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md#friend-sharedcommanddata)**  |

## Public Functions Documentation

### function ParticleSystemEvaluation

```cpp
ParticleSystemEvaluation(
    const std::vector< std::string > & paths
)
```


### function ParticleSystemEvaluation

```cpp
ParticleSystemEvaluation(
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

Number of samples. 

### function D

```cpp
inline int D() const
```

Dimensions (e.g. x/y/z * number of particles) 

### function ExactCompare

```cpp
bool ExactCompare(
    const ParticleSystemEvaluation & other
) const
```


### function EvaluationCompare

```cpp
bool EvaluationCompare(
    const ParticleSystemEvaluation & other
) const
```


### function ReadParticleFile

```cpp
static bool ReadParticleFile(
    std::string filename,
    Eigen::VectorXd & points
)
```


## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData(
    SharedCommandData 
);
```


-------------------------------

Updated on 2024-03-20 at 15:59:39 +0000