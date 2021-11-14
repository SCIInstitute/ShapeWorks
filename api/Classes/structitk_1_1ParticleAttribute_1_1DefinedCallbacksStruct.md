---
title: itk::ParticleAttribute::DefinedCallbacksStruct

---

# itk::ParticleAttribute::DefinedCallbacksStruct



 [More...](#detailed-description)


`#include <itkParticleAttribute.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DefinedCallbacksStruct](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#function-definedcallbacksstruct)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[Event](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-event)**  |
| bool | **[EventWithIndex](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-eventwithindex)**  |
| bool | **[DomainAddEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-domainaddevent)**  |
| bool | **[TransformSetEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-transformsetevent)**  |
| bool | **[PrefixTransformSetEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-prefixtransformsetevent)**  |
| bool | **[NeighborhoodSetEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-neighborhoodsetevent)**  |
| bool | **[PositionSetEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-positionsetevent)**  |
| bool | **[PositionAddEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-positionaddevent)**  |
| bool | **[PositionRemoveEvent](../Classes/structitk_1_1ParticleAttribute_1_1DefinedCallbacksStruct.md#variable-positionremoveevent)**  |

## Detailed Description

```cpp
struct itk::ParticleAttribute::DefinedCallbacksStruct;
```


Data structure indicating which callback functions are defined by a subclass. The [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) class will reference this structure to determine which callbacks to register. This is just a list of every event defined in itkParticleEvent.h 

## Public Functions Documentation

### function DefinedCallbacksStruct

```cpp
inline DefinedCallbacksStruct()
```


## Public Attributes Documentation

### variable Event

```cpp
bool Event;
```


### variable EventWithIndex

```cpp
bool EventWithIndex;
```


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

Updated on 2021-11-14 at 17:52:09 +0000