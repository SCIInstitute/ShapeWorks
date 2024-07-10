---
title: shapeworks::Observer::DefinedCallbacksStruct

---

# shapeworks::Observer::DefinedCallbacksStruct



 [More...](#detailed-description)


`#include <Observer.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DefinedCallbacksStruct](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#function-definedcallbacksstruct)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[DomainAddEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-domainaddevent)**  |
| bool | **[TransformSetEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-transformsetevent)**  |
| bool | **[PrefixTransformSetEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-prefixtransformsetevent)**  |
| bool | **[NeighborhoodSetEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-neighborhoodsetevent)**  |
| bool | **[PositionSetEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-positionsetevent)**  |
| bool | **[PositionAddEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-positionaddevent)**  |
| bool | **[PositionRemoveEvent](../Classes/structshapeworks_1_1Observer_1_1DefinedCallbacksStruct.md#variable-positionremoveevent)**  |

## Detailed Description

```cpp
struct shapeworks::Observer::DefinedCallbacksStruct;
```


Data structure indicating which callback functions are defined by a subclass. The [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) class will reference this structure to determine which callbacks to register. 

## Public Functions Documentation

### function DefinedCallbacksStruct

```cpp
inline DefinedCallbacksStruct()
```


## Public Attributes Documentation

### variable DomainAddEvent

```cpp
bool DomainAddEvent;
```


### variable TransformSetEvent

```cpp
bool TransformSetEvent;
```


### variable PrefixTransformSetEvent

```cpp
bool PrefixTransformSetEvent;
```


### variable NeighborhoodSetEvent

```cpp
bool NeighborhoodSetEvent;
```


### variable PositionSetEvent

```cpp
bool PositionSetEvent;
```


### variable PositionAddEvent

```cpp
bool PositionAddEvent;
```


### variable PositionRemoveEvent

```cpp
bool PositionRemoveEvent;
```


-------------------------------

Updated on 2024-07-10 at 18:20:41 +0000