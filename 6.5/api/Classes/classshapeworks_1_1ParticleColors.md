---
title: shapeworks::ParticleColors
summary: Supplies particle colors. 

---

# shapeworks::ParticleColors



Supplies particle colors.  [More...](#detailed-description)


`#include <ParticleColors.h>`

Inherits from QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[ParticleColorsType](../Classes/classshapeworks_1_1ParticleColors.md#enum-particlecolorstype)** { Distinct = 0, Original = 1} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_ENUM](../Classes/classshapeworks_1_1ParticleColors.md#function-q-enum)**(ParticleColorsType ) |
| vtkSmartPointer< vtkLookupTable > | **[construct_distinct](../Classes/classshapeworks_1_1ParticleColors.md#function-construct-distinct)**() |

## Detailed Description

```cpp
class shapeworks::ParticleColors;
```

Supplies particle colors. 

The [ParticleColors](../Classes/classshapeworks_1_1ParticleColors.md) supplies distinct colors for particles 

## Public Types Documentation

### enum ParticleColorsType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Distinct | 0|   |
| Original | 1|   |




## Public Functions Documentation

### function Q_ENUM

```cpp
Q_ENUM(
    ParticleColorsType 
)
```


### function construct_distinct

```cpp
static vtkSmartPointer< vtkLookupTable > construct_distinct()
```


-------------------------------

Updated on 2023-11-09 at 17:05:56 +0000