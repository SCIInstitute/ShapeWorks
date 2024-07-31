---
title: shapeworks::ParticlePointIndexPair

---

# shapeworks::ParticlePointIndexPair



 [More...](#detailed-description)


`#include <ParticlePointIndexPair.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef itk::Point< double, 3 > | **[PointType](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**() |
| | **[ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**(const PointType & p, unsigned int i =0) |
| | **[ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**(const [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) & o) |
| const [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) & | **[operator=](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#function-operator=)**(const [ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md) & o) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| PointType | **[Point](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#variable-point)**  |
| unsigned int | **[Index](../Classes/structshapeworks_1_1ParticlePointIndexPair.md#variable-index)**  |

## Detailed Description

```cpp
struct shapeworks::ParticlePointIndexPair;
```


Struct containing a Point and an index value associated with a point. This object is used mainly by [ParticleNeighborhood.h]

## Public Types Documentation

### typedef PointType

```cpp
typedef itk::Point<double, 3> shapeworks::ParticlePointIndexPair::PointType;
```


## Public Functions Documentation

### function ParticlePointIndexPair

```cpp
inline ParticlePointIndexPair()
```


### function ParticlePointIndexPair

```cpp
inline ParticlePointIndexPair(
    const PointType & p,
    unsigned int i =0
)
```


### function ParticlePointIndexPair

```cpp
inline ParticlePointIndexPair(
    const ParticlePointIndexPair & o
)
```


### function operator=

```cpp
inline const ParticlePointIndexPair & operator=(
    const ParticlePointIndexPair & o
)
```


## Public Attributes Documentation

### variable Point

```cpp
PointType Point;
```


### variable Index

```cpp
unsigned int Index;
```


-------------------------------

Updated on 2024-07-31 at 07:37:36 +0000