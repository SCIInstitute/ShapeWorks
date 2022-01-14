---
title: itk::ParticleRegionNeighborhood::IteratorNodePair

---

# itk::ParticleRegionNeighborhood::IteratorNodePair



 [More...](#detailed-description)


`#include <itkParticleRegionNeighborhood.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [PointTreeType::NodePointerType](../Classes/classitk_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) | **[NodePointerType](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#typedef-nodepointertype)**  |
| typedef PointTreeType::PointListType::iterator | **[IteratorType](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#typedef-iteratortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**() |
| | **[~IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-~iteratornodepair)**() |
| | **[IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**(const IteratorType & p, const NodePointerType & n) |
| | **[IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**(const [IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & o) |
| const [IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & | **[operator=](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-operator=)**(const [IteratorNodePair](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & o) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| IteratorType | **[Iterator](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#variable-iterator)**  |
| NodePointerType | **[NodePointer](../Classes/structitk_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#variable-nodepointer)**  |

## Detailed Description

```cpp
struct itk::ParticleRegionNeighborhood::IteratorNodePair;
```


Map for direct reference of PointIndexPairs and Tree nodes from the PointTree. This is used for fast removal or modification of point values. 

## Public Types Documentation

### typedef NodePointerType

```cpp
typedef PointTreeType::NodePointerType itk::ParticleRegionNeighborhood< VDimension >::IteratorNodePair::NodePointerType;
```


### typedef IteratorType

```cpp
typedef PointTreeType::PointListType::iterator itk::ParticleRegionNeighborhood< VDimension >::IteratorNodePair::IteratorType;
```


## Public Functions Documentation

### function IteratorNodePair

```cpp
inline IteratorNodePair()
```


### function ~IteratorNodePair

```cpp
inline ~IteratorNodePair()
```


### function IteratorNodePair

```cpp
inline IteratorNodePair(
    const IteratorType & p,
    const NodePointerType & n
)
```


### function IteratorNodePair

```cpp
inline IteratorNodePair(
    const IteratorNodePair & o
)
```


### function operator=

```cpp
inline const IteratorNodePair & operator=(
    const IteratorNodePair & o
)
```


## Public Attributes Documentation

### variable Iterator

```cpp
IteratorType Iterator;
```


### variable NodePointer

```cpp
NodePointerType NodePointer;
```


-------------------------------

Updated on 2022-01-14 at 02:26:00 +0000