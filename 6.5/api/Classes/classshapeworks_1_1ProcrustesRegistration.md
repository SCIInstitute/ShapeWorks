---
title: shapeworks::ProcrustesRegistration

---

# shapeworks::ProcrustesRegistration





## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ProcrustesRegistration](../Classes/classshapeworks_1_1ProcrustesRegistration.md) > | **[Pointer](../Classes/classshapeworks_1_1ProcrustesRegistration.md#using-pointer)**  |
| using [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[ParticleSystemType](../Classes/classshapeworks_1_1ProcrustesRegistration.md#using-particlesystemtype)**  |
| using [ParticleSystemType::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ProcrustesRegistration.md#using-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetParticleSystem](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| [ParticleSystemType](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-getparticlesystem)**() const |
| [ParticleSystemType](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-getparticlesystem)**() |
| void | **[RunRegistration](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-runregistration)**(int i) |
| void | **[RunRegistration](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-runregistration)**()<br>Run registration on all domains.  |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-setdomainspershape)**(int i)<br>Set/Get the number of Domains in each shape. Procrustes will be performed separately for each domain.  |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-getdomainspershape)**() const |
| bool | **[GetScaling](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-getscaling)**() const |
| void | **[SetScaling](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-setscaling)**(bool scaling) |
| bool | **[GetRotationTranslation](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-getrotationtranslation)**() const |
| void | **[SetRotationTranslation](../Classes/classshapeworks_1_1ProcrustesRegistration.md#function-setrotationtranslation)**(bool rotationTranslation) |

## Public Types Documentation

### using Pointer

```cpp
using shapeworks::ProcrustesRegistration::Pointer =  std::shared_ptr<ProcrustesRegistration>;
```


### using ParticleSystemType

```cpp
using shapeworks::ProcrustesRegistration::ParticleSystemType =  ParticleSystem;
```


### using PointType

```cpp
using shapeworks::ProcrustesRegistration::PointType =  ParticleSystemType::PointType;
```


## Public Functions Documentation

### function SetParticleSystem

```cpp
inline void SetParticleSystem(
    ParticleSystemType * p
)
```


### function GetParticleSystem

```cpp
inline ParticleSystemType * GetParticleSystem() const
```


### function GetParticleSystem

```cpp
inline ParticleSystemType * GetParticleSystem()
```


### function RunRegistration

```cpp
void RunRegistration(
    int i
)
```


Performs a procrustes registration and modifies the transforms of the particle system accordingly. Assumes m_ParticleSystem has been set to point to a valid object. 


### function RunRegistration

```cpp
void RunRegistration()
```

Run registration on all domains. 

### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    int i
)
```

Set/Get the number of Domains in each shape. Procrustes will be performed separately for each domain. 

### function GetDomainsPerShape

```cpp
inline int GetDomainsPerShape() const
```


### function GetScaling

```cpp
inline bool GetScaling() const
```


### function SetScaling

```cpp
inline void SetScaling(
    bool scaling
)
```


### function GetRotationTranslation

```cpp
inline bool GetRotationTranslation() const
```


### function SetRotationTranslation

```cpp
inline void SetRotationTranslation(
    bool rotationTranslation
)
```


-------------------------------

Updated on 2023-07-05 at 18:53:20 +0000