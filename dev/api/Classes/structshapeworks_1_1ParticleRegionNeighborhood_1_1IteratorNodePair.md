---
title: shapeworks::ParticleRegionNeighborhood::IteratorNodePair

---

# shapeworks::ParticleRegionNeighborhood::IteratorNodePair



 [More...](#detailed-description)


`#include <ParticleRegionNeighborhood.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [PointTreeType::NodePointerType](../Classes/classshapeworks_1_1PowerOfTwoPointTree.md#typedef-nodepointertype) | **[NodePointerType](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#typedef-nodepointertype)**  |
| typedef PointTreeType::PointListType::iterator | **[IteratorType](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#typedef-iteratortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**() |
| | **[~IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-~iteratornodepair)**() |
| | **[IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**(const IteratorType & p, const NodePointerType & n) |
| | **[IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-iteratornodepair)**(const [IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & o) |
| const [IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & | **[operator=](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#function-operator=)**(const [IteratorNodePair](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md) & o) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| IteratorType | **[Iterator](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#variable-iterator)**  |
| NodePointerType | **[NodePointer](../Classes/structshapeworks_1_1ParticleRegionNeighborhood_1_1IteratorNodePair.md#variable-nodepointer)**  |

## Detailed Description

```cpp
struct shapeworks::ParticleRegionNeighborhood::IteratorNodePair;
```


Map for direct reference of PointIndexPairs and Tree nodes from the PointTree. This is used for fast removal or modification of point values. 

## Public Types Documentation

### typedef NodePointerType

```cpp
typedef PointTreeType::NodePointerType shapeworks::ParticleRegionNeighborhood::IteratorNodePair::NodePointerType;
```


### typedef IteratorType

```cpp
typedef PointTreeType::PointListType::iterator shapeworks::ParticleRegionNeighborhood::IteratorNodePair::IteratorType;
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

Updated on 2023-10-25 at 15:40:27 +0000