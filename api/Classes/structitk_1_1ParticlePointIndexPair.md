---
title: itk::ParticlePointIndexPair

---

# itk::ParticlePointIndexPair



 [More...](#detailed-description)


`#include <itkParticlePointIndexPair.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef itk::Point< double, VDimension > | **[PointType](../Classes/structitk_1_1ParticlePointIndexPair.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**() |
| | **[ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**(const PointType & p, unsigned int i =0) |
| | **[ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md#function-particlepointindexpair)**(const [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md) & o) |
| const [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md) & | **[operator=](../Classes/structitk_1_1ParticlePointIndexPair.md#function-operator=)**(const [ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md) & o) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| PointType | **[Point](../Classes/structitk_1_1ParticlePointIndexPair.md#variable-point)**  |
| unsigned int | **[Index](../Classes/structitk_1_1ParticlePointIndexPair.md#variable-index)**  |

## Detailed Description

```cpp
template <unsigned int VDimension>
struct itk::ParticlePointIndexPair;
```


Struct containing a Point and an index value associated with a point. This object is used mainly by itkParticleNeighborhood 

## Public Types Documentation

### typedef PointType

```cpp
typedef itk::Point<double, VDimension> itk::ParticlePointIndexPair< VDimension >::PointType;
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

Updated on 2021-12-13 at 17:35:57 +0000