---
title: shapeworks::powstruct

---

# shapeworks::powstruct



 [More...](#detailed-description)


`#include <PowerOfTwoPointTree.h>`

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const int | **[c](../Classes/structshapeworks_1_1powstruct.md#variable-c)**  |

## Detailed Description

```cpp
template <int a,
int b>
struct shapeworks::powstruct;
```


Compute pow(a,b)=c at compile time. 

## Public Attributes Documentation

### variable c

```cpp
static const int c = a * powstruct<a, b - 1>::c;
```


-------------------------------

Updated on 2024-08-07 at 19:44:45 +0000