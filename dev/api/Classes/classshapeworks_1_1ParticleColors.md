---
title: shapeworks::ParticleColors

---

# shapeworks::ParticleColors





Inherits from QObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[ParticleColorsType](../Classes/classshapeworks_1_1ParticleColors.md#enum-particlecolorstype)** { Distinct = 0, Original = 1} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_ENUM](../Classes/classshapeworks_1_1ParticleColors.md#function-q-enum)**(ParticleColorsType ) |
| vtkSmartPointer< vtkLookupTable > | **[construct](../Classes/classshapeworks_1_1ParticleColors.md#function-construct)**(ParticleColorsType type, int num) |

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


### function construct

```cpp
static vtkSmartPointer< vtkLookupTable > construct(
    ParticleColorsType type,
    int num
)
```


-------------------------------

Updated on 2022-11-20 at 18:23:10 +0000