---
title: shapeworks::ParticleEvent

---

# shapeworks::ParticleEvent





Inherits from itk::EventObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md) | **[Self](../Classes/classshapeworks_1_1ParticleEvent.md#typedef-self)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md#function-particleevent)**() |
| virtual | **[~ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md#function-~particleevent)**() |
| | **[ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md#function-particleevent)**(const [ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md) & v) |
| const [ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md) & | **[operator=](../Classes/classshapeworks_1_1ParticleEvent.md#function-operator=)**(const [ParticleEvent](../Classes/classshapeworks_1_1ParticleEvent.md) & v) |
| virtual const char * | **[GetEventName](../Classes/classshapeworks_1_1ParticleEvent.md#function-geteventname)**() const |
| virtual bool | **[CheckEvent](../Classes/classshapeworks_1_1ParticleEvent.md#function-checkevent)**(const ::itk::EventObject * e) const |
| ::itk::EventObject * | **[MakeObject](../Classes/classshapeworks_1_1ParticleEvent.md#function-makeobject)**() const |
| void | **[SetPositionIndex](../Classes/classshapeworks_1_1ParticleEvent.md#function-setpositionindex)**(int i) |
| int | **[GetPositionIndex](../Classes/classshapeworks_1_1ParticleEvent.md#function-getpositionindex)**() const |
| void | **[SetDomainIndex](../Classes/classshapeworks_1_1ParticleEvent.md#function-setdomainindex)**(int i) |
| int | **[GetDomainIndex](../Classes/classshapeworks_1_1ParticleEvent.md#function-getdomainindex)**() const |

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleEvent shapeworks::ParticleEvent::Self;
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

Updated on 2023-11-09 at 17:05:56 +0000