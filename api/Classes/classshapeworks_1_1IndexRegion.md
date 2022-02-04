---
title: shapeworks::IndexRegion
summary: Indices into a 3d region of memory (can be negative, e.g., for the purpose of padding an image) 

---

# shapeworks::IndexRegion



Indices into a 3d region of memory (can be negative, e.g., for the purpose of padding an image) 


`#include <Region.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md#function-indexregion)**() =default |
| | **[IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md#function-indexregion)**(const [IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) & ) =default |
| | **[IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md#function-indexregion)**(const Dims & dims) |
| | **[IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md#function-indexregion)**(const Coord & _min, const Coord & _max) |
| bool | **[operator==](../Classes/classshapeworks_1_1IndexRegion.md#function-operator==)**(const [IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) & other) const |
| bool | **[valid](../Classes/classshapeworks_1_1IndexRegion.md#function-valid)**() const<br>verified min/max do not create an inverted or an empty region  |
| Coord | **[origin](../Classes/classshapeworks_1_1IndexRegion.md#function-origin)**() const |
| Dims | **[size](../Classes/classshapeworks_1_1IndexRegion.md#function-size)**() const |
| [IndexRegion](../Classes/classshapeworks_1_1IndexRegion.md) & | **[pad](../Classes/classshapeworks_1_1IndexRegion.md#function-pad)**(int padding)<br>uniformly grows or shrinks the region by the specified amount  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| Coord | **[min](../Classes/classshapeworks_1_1IndexRegion.md#variable-min)**  |
| Coord | **[max](../Classes/classshapeworks_1_1IndexRegion.md#variable-max)**  |

## Public Functions Documentation

### function IndexRegion

```cpp
IndexRegion() =default
```


### function IndexRegion

```cpp
IndexRegion(
    const IndexRegion & 
) =default
```


### function IndexRegion

```cpp
inline IndexRegion(
    const Dims & dims
)
```


### function IndexRegion

```cpp
inline IndexRegion(
    const Coord & _min,
    const Coord & _max
)
```


### function operator==

```cpp
inline bool operator==(
    const IndexRegion & other
) const
```


### function valid

```cpp
inline bool valid() const
```

verified min/max do not create an inverted or an empty region 

### function origin

```cpp
inline Coord origin() const
```


### function size

```cpp
inline Dims size() const
```


### function pad

```cpp
IndexRegion & pad(
    int padding
)
```

uniformly grows or shrinks the region by the specified amount 

## Public Attributes Documentation

### variable min

```cpp
Coord min {1000000000, 1000000000, 1000000000};
```


### variable max

```cpp
Coord max {-1000000000, -1000000000, -1000000000};
```


-------------------------------

Updated on 2022-02-04 at 06:14:23 +0000