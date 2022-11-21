---
title: shapeworks::ParticleProcrustesRegistration

---

# shapeworks::ParticleProcrustesRegistration





## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::shared_ptr< [ParticleProcrustesRegistration](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md) > | **[Pointer](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#using-pointer)**  |
| using [itk::ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) | **[ParticleSystemType](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#using-particlesystemtype)**  |
| using [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#using-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetParticleSystem](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * p) |
| [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-getparticlesystem)**() const |
| [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-getparticlesystem)**() |
| void | **[RunRegistration](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-runregistration)**(int i) |
| void | **[RunRegistration](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-runregistration)**()<br>Run registration on all domains.  |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-setdomainspershape)**(int i)<br>Set/Get the number of Domains in each shape. Procrustes will be performed separately for each domain.  |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-getdomainspershape)**() const |
| bool | **[GetScaling](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-getscaling)**() const |
| void | **[SetScaling](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-setscaling)**(bool scaling) |
| bool | **[GetRotationTranslation](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-getrotationtranslation)**() const |
| void | **[SetRotationTranslation](../Classes/classshapeworks_1_1ParticleProcrustesRegistration.md#function-setrotationtranslation)**(bool rotationTranslation) |

## Public Types Documentation

### using Pointer

```cpp
using shapeworks::ParticleProcrustesRegistration::Pointer =  std::shared_ptr<ParticleProcrustesRegistration>;
```


### using ParticleSystemType

```cpp
using shapeworks::ParticleProcrustesRegistration::ParticleSystemType =  itk::ParticleSystem;
```


### using PointType

```cpp
using shapeworks::ParticleProcrustesRegistration::PointType =  ParticleSystemType::PointType;
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

Updated on 2022-11-21 at 20:30:36 +0000