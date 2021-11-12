---
title: itk::ParticleEvent

---

# itk::ParticleEvent





Inherits from EventObject

Inherited by [itk::ParticleEventWithIndex](../Classes/classitk_1_1ParticleEventWithIndex.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) | **[Self](../Classes/classitk_1_1ParticleEvent.md#typedef-self)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-particleevent)**() |
| virtual | **[~ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-~particleevent)**() |
| int | **[GetThreadID](../Classes/classitk_1_1ParticleEvent.md#function-getthreadid)**() const |
| void | **[SetThreadID](../Classes/classitk_1_1ParticleEvent.md#function-setthreadid)**(int i) |
| | **[ParticleEvent](../Classes/classitk_1_1ParticleEvent.md#function-particleevent)**(const [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) & v) |
| const [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) & | **[operator=](../Classes/classitk_1_1ParticleEvent.md#function-operator=)**(const [ParticleEvent](../Classes/classitk_1_1ParticleEvent.md) & v) |
| virtual const char * | **[GetEventName](../Classes/classitk_1_1ParticleEvent.md#function-geteventname)**() const |
| virtual bool | **[CheckEvent](../Classes/classitk_1_1ParticleEvent.md#function-checkevent)**(const ::itk::EventObject * e) const |
| ::itk::EventObject * | **[MakeObject](../Classes/classitk_1_1ParticleEvent.md#function-makeobject)**() const |

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleEvent itk::ParticleEvent::Self;
```


## Public Functions Documentation

### function ParticleEvent

```cpp
inline ParticleEvent()
```


### function ~ParticleEvent

```cpp
inline virtual ~ParticleEvent()
```


### function GetThreadID

```cpp
inline int GetThreadID() const
```


Get/Set the thread responsible for handling this particular event. 


### function SetThreadID

```cpp
inline void SetThreadID(
    int i
)
```


### function ParticleEvent

```cpp
inline ParticleEvent(
    const ParticleEvent & v
)
```


Copy constructor and operator= 


### function operator=

```cpp
inline const ParticleEvent & operator=(
    const ParticleEvent & v
)
```


### function GetEventName

```cpp
inline virtual const char * GetEventName() const
```


Standard ITK event members. 


### function CheckEvent

```cpp
inline virtual bool CheckEvent(
    const ::itk::EventObject * e
) const
```


### function MakeObject

```cpp
inline ::itk::EventObject * MakeObject() const
```


-------------------------------

Updated on 2021-11-12 at 04:35:43 +0000