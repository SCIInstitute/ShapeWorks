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
| int | **[N](../Classes/classshapeworks_1_1ParticleSystem.md#function-n)**() const<br>Number of samples.  |
| int | **[D](../Classes/classshapeworks_1_1ParticleSystem.md#function-d)**() const<br>Dimensions (e.g. x/y/z * number of particles)  |
| bool | **[ExactCompare](../Classes/classshapeworks_1_1ParticleSystem.md#function-exactcompare)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & other) const |
| bool | **[EvaluationCompare](../Classes/classshapeworks_1_1ParticleSystem.md#function-evaluationcompare)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) & other) const |
| bool | **[ReadParticleFile](../Classes/classshapeworks_1_1ParticleSystem.md#function-readparticlefile)**(std::string filename, Eigen::VectorXd & points) |

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

Number of samples. 

### function D

```cpp
inline int D() const
```

Dimensions (e.g. x/y/z * number of particles) 

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

Updated on 2023-02-27 at 19:53:44 +0000