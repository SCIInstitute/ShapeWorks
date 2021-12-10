---
title: itk::ParticleProcrustesRegistration
summary: Class for replacing bad particles during optimization Author - Praful Agrawal Date - June 05, 2017. 

---

# itk::ParticleProcrustesRegistration



Class for replacing bad particles during optimization Author - Praful Agrawal Date - June 05, 2017.  [More...](#detailed-description)


`#include <itkParticleGoodBadAssessment.h>`

Inherits from DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md) | **[Self](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-particlesystemtype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-itktypemacro)**([ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md) , DataObject ) |
| void | **[SetParticleSystem](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-particlesystemtype) * p) |
| const [ParticleSystemType](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-particlesystemtype) * | **[GetParticleSystem](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-getparticlesystem)**() const |
| [ParticleSystemType](../Classes/classitk_1_1ParticleProcrustesRegistration.md#typedef-particlesystemtype) * | **[GetParticleSystem](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-getparticlesystem)**() |
| void | **[IterationCallback](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-iterationcallback)**(itk::Object * , const itk::EventObject & ) |
| void | **[RunRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-runregistration)**(int i) |
| void | **[RunRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-runregistration)**() |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-getdomainspershape)**() const |
| bool | **[GetScaling](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-getscaling)**() const |
| void | **[ScalingOn](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-scalingon)**() |
| void | **[ScalingOff](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-scalingoff)**() |
| bool | **[GetRotationTranslation](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-getrotationtranslation)**() const |
| void | **[RotationTranslationOn](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-rotationtranslationon)**() |
| void | **[RotationTranslationOff](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-rotationtranslationoff)**() |
| void | **[SetFixedScales](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-setfixedscales)**(const std::vector< double > v) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-particleprocrustesregistration)**() |
| virtual | **[~ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-~particleprocrustesregistration)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleProcrustesRegistration.md#function-printself)**(std::ostream & os, Indent indent) const |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticleProcrustesRegistration;
```

Class for replacing bad particles during optimization Author - Praful Agrawal Date - June 05, 2017. 
## Public Types Documentation

### typedef Self

```cpp
typedef ParticleProcrustesRegistration itk::ParticleProcrustesRegistration< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject itk::ParticleProcrustesRegistration< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleProcrustesRegistration< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleProcrustesRegistration< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleProcrustesRegistration< VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleProcrustesRegistration< VDimension >::ParticleSystemType;
```


Particle system typedefs. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleProcrustesRegistration< VDimension >::PointType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleProcrustesRegistration ,
    DataObject 
)
```


Run-time type information (and related methods). 


### function SetParticleSystem

```cpp
inline void SetParticleSystem(
    ParticleSystemType * p
)
```


Set/Get the target particle system. 


### function GetParticleSystem

```cpp
inline const ParticleSystemType * GetParticleSystem() const
```


### function GetParticleSystem

```cpp
inline ParticleSystemType * GetParticleSystem()
```


### function IterationCallback

```cpp
inline void IterationCallback(
    itk::Object * ,
    const itk::EventObject & 
)
```


Callback suitable for adding as an observer of itk object iteration events. 


### function RunRegistration

```cpp
void RunRegistration(
    int i
)
```


Performs a procrustes registration and modifies the transforms of the particle system accordingly. Assumes m_ParticleSystem has been set to point to a valid object. 


### function RunRegistration

```cpp
inline void RunRegistration()
```


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


### function ScalingOn

```cpp
inline void ScalingOn()
```


### function ScalingOff

```cpp
inline void ScalingOff()
```


### function GetRotationTranslation

```cpp
inline bool GetRotationTranslation() const
```


### function RotationTranslationOn

```cpp
inline void RotationTranslationOn()
```


### function RotationTranslationOff

```cpp
inline void RotationTranslationOff()
```


### function SetFixedScales

```cpp
inline void SetFixedScales(
    const std::vector< double > v
)
```


## Protected Functions Documentation

### function ParticleProcrustesRegistration

```cpp
inline ParticleProcrustesRegistration()
```


### function ~ParticleProcrustesRegistration

```cpp
inline virtual ~ParticleProcrustesRegistration()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2021-12-10 at 00:32:11 +0000