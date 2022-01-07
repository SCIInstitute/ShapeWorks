---
title: itk::powstruct

---

# itk::powstruct



 [More...](#detailed-description)


`#include <itkPowerOfTwoPointTree.h>`

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const int | **[c](../Classes/structitk_1_1powstruct.md#variable-c)**  |

## Detailed Description

```cpp
template <int a,
int b>
struct itk::powstruct;
```


Compute pow(a,b)=c at compile time. 

## Public Attributes Documentation

### variable c

```cpp
static const int c = a * powstruct<a,b-1>::c;
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000