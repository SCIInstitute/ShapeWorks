---
title: itk::ParticleEventWithIndex

---

# itk::ParticleEventWithIndex



 [More...](#detailed-description)


`#include <itkParticleEvents.h>`

Inherits from [itk::ParticleEvent](../Classes/classitk_1_1ParticleEvent.md), EventObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-particleeventwithindex)**() |
| | **[~ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-~particleeventwithindex)**() |
| | **[ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-particleeventwithindex)**(const [ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md) & v) |
| const [ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md) & | **[operator=](../Classes/classitk_1_1ParticleEventWithIndex.md#function-operator=)**(const [ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md) & v) |
| void | **[SetPositionIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-setpositionindex)**(int i) |
| int | **[GetPositionIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-getpositionindex)**() const |
| void | **[SetDomainIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-setdomainindex)**(int i) |
| int | **[GetDomainIndex](../Classes/classitk_1_1ParticleEventWithIndex.md#function-getdomainindex)**() const |

## Additional inherited members

**Public Types inherited from [itk::ParticleEvent](../Classes/classitk_1_1ParticleEvent.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) | **[Self](../Classes/classitk_1_1ParticleEvent.md#typedef-self)**  |

**Public Functions inherited from [itk::ParticleEvent](../Classes/classitk_1_1ParticleEvent.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-particleevent)**() |
| virtual | **[~ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-~particleevent)**() |
| int | **[GetThreadID](../Classes/classitk_1_1ParticleEvent.md#function-getthreadid)**() const |
| void | **[SetThreadID](../Classes/classitk_1_1ParticleEvent.md#function-setthreadid)**(int i) |
| | **[ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-particleevent)**(const [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) & v) |
| virtual const char * | **[GetEventName](../Classes/classitk_1_1ParticleEvent.md#function-geteventname)**() const |
| virtual bool | **[CheckEvent](../Classes/classitk_1_1ParticleEvent.md#function-checkevent)**(const ::itk::EventObject * e) const |
| ::itk::EventObject * | **[MakeObject](../Classes/classitk_1_1ParticleEvent.md#function-makeobject)**() const |


## Detailed Description

```cpp
class itk::ParticleEventWithIndex;
```


Event that carries Position index and a Domain index information. This is used, for example to indicate which position has changed in the particle system on InvokeEvent. 

## Public Functions Documentation

### function ParticleEventWithIndex

```cpp
inline ParticleEventWithIndex()
```


### function ~ParticleEventWithIndex

```cpp
inline ~ParticleEventWithIndex()
```


### function ParticleEventWithIndex

```cpp
inline ParticleEventWithIndex(
    const ParticleEventWithIndex & v
)
```


Copy constructor and operator=. 


### function operator=

```cpp
inline const ParticleEventWithIndex & operator=(
    const ParticleEventWithIndex & v
)
```


### function SetPositionIndex

```cpp
inline void SetPositionIndex(
    int i
)
```


Get/Set the index value. 


### function GetPositionIndex

```cpp
inline int GetPositionIndex() const
```


### function SetDomainIndex

```cpp
inline void SetDomainIndex(
    int i
)
```


Get/Set the Domain index value. 


### function GetDomainIndex

```cpp
inline int GetDomainIndex() const
```


-------------------------------

Updated on 2022-01-28 at 07:11:43 +0000