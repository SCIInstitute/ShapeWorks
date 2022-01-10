---
title: shapeworks::PhysicalRegion
summary: physical bounds of a 3d region of space 

---

# shapeworks::PhysicalRegion



physical bounds of a 3d region of space 


`#include <Region.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md#function-physicalregion)**(const [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & ) =default |
| | **[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md#function-physicalregion)**(const Point & _min, const Point & _max) |
| | **[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md#function-physicalregion)**() |
| bool | **[operator==](../Classes/classshapeworks_1_1PhysicalRegion.md#function-operator==)**(const [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & other) const |
| bool | **[valid](../Classes/classshapeworks_1_1PhysicalRegion.md#function-valid)**() const<br>verified min/max do not create an inverted or an empty region  |
| Point | **[origin](../Classes/classshapeworks_1_1PhysicalRegion.md#function-origin)**() const |
| Point | **[size](../Classes/classshapeworks_1_1PhysicalRegion.md#function-size)**() const |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & | **[shrink](../Classes/classshapeworks_1_1PhysicalRegion.md#function-shrink)**(const [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & other)<br>shrink this region down to the smallest portions of both  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & | **[expand](../Classes/classshapeworks_1_1PhysicalRegion.md#function-expand)**(const [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & other)<br>expand this region to include the largest portions of both  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & | **[expand](../Classes/classshapeworks_1_1PhysicalRegion.md#function-expand)**(const Point & pt)<br>expand this region to include this point  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) & | **[pad](../Classes/classshapeworks_1_1PhysicalRegion.md#function-pad)**(double padding)<br>uniformly grows or shrinks the region by the specified amount  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| Point | **[min](../Classes/classshapeworks_1_1PhysicalRegion.md#variable-min)**  |
| Point | **[max](../Classes/classshapeworks_1_1PhysicalRegion.md#variable-max)**  |

## Public Functions Documentation

### function PhysicalRegion

```cpp
PhysicalRegion(
    const PhysicalRegion & 
) =default
```


### function PhysicalRegion

```cpp
inline PhysicalRegion(
    const Point & _min,
    const Point & _max
)
```


### function PhysicalRegion

```cpp
inline PhysicalRegion()
```


### function operator==

```cpp
inline bool operator==(
    const PhysicalRegion & other
) const
```


### function valid

```cpp
inline bool valid() const
```

verified min/max do not create an inverted or an empty region 

### function origin

```cpp
inline Point origin() const
```


### function size

```cpp
inline Point size() const
```


### function shrink

```cpp
PhysicalRegion & shrink(
    const PhysicalRegion & other
)
```

shrink this region down to the smallest portions of both 

### function expand

```cpp
PhysicalRegion & expand(
    const PhysicalRegion & other
)
```

expand this region to include the largest portions of both 

### function expand

```cpp
PhysicalRegion & expand(
    const Point & pt
)
```

expand this region to include this point 

### function pad

```cpp
PhysicalRegion & pad(
    double padding
)
```

uniformly grows or shrinks the region by the specified amount 

## Public Attributes Documentation

### variable min

```cpp
Point min;
```


### variable max

```cpp
Point max;
```


-------------------------------

Updated on 2022-01-10 at 16:27:26 +0000