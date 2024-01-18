---
title: shapeworks::VectorFunction

---

# shapeworks::VectorFunction



 [More...](#detailed-description)


`#include <VectorFunction.h>`

Inherits from itk::LightObject

Inherited by [shapeworks::CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md), [shapeworks::DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md), [shapeworks::DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md), [shapeworks::LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md), [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Self](../Classes/classshapeworks_1_1VectorFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1VectorFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1VectorFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1VectorFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1VectorFunction.md#typedef-constpointer)**  |
| typedef itk::LightObject | **[Superclass](../Classes/classshapeworks_1_1VectorFunction.md#typedef-superclass)**  |
| typedef vnl_vector_fixed< double, VDimension > | **[VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1VectorFunction.md#function-itktypemacro)**([VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) , LightObject ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1VectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const =0 |
| virtual [VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep, double & energy) const =0 |
| virtual double | **[Energy](../Classes/classshapeworks_1_1VectorFunction.md#function-energy)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) const =0 |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)**() |
| virtual void | **[BeforeEvaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) |
| virtual void | **[SetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-setparticlesystem)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| virtual [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-getparticlesystem)**() const |
| virtual void | **[SetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| virtual int | **[GetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-getdomainnumber)**() const |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)**() |
| virtual double | **[GetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-getrelativeenergyscaling)**() const |
| virtual void | **[SetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-setrelativeenergyscaling)**(double r) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-vectorfunction)**() |
| virtual | **[~VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-~vectorfunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1VectorFunction.md#function-operator=)**(const [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) & ) |
| | **[VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-vectorfunction)**(const [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) & ) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1VectorFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[m_ParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#variable-m-particlesystem)**  |
| unsigned int | **[m_DomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#variable-m-domainnumber)**  |

## Detailed Description

```cpp
class shapeworks::VectorFunction;
```


This is the base class defining the API for a function that takes a particle system, domain, and location index as arguments and returns a vector-valued result. 

## Public Types Documentation

### typedef Self

```cpp
typedef VectorFunction shapeworks::VectorFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::VectorFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::VectorFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef itk::LightObject shapeworks::VectorFunction::Superclass;
```


### typedef VectorType

```cpp
typedef vnl_vector_fixed<double, VDimension> shapeworks::VectorFunction::VectorType;
```


Type of vectors. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    VectorFunction ,
    LightObject 
)
```


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Method for object allocation through the factory. Dimensionality of the domain of the particle system. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep
) const =0
```


**Reimplemented by**: [shapeworks::CorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-evaluate), [shapeworks::CurvatureSamplingFunction::Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate), [shapeworks::DisentangledCorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-evaluate), [shapeworks::LegacyCorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-evaluate), [shapeworks::SamplingFunction::Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate), [shapeworks::DualVectorFunction::Evaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep,
    double & energy
) const =0
```


**Reimplemented by**: [shapeworks::SamplingFunction::Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate), [shapeworks::DualVectorFunction::Evaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-evaluate), [shapeworks::CorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-evaluate), [shapeworks::CurvatureSamplingFunction::Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate), [shapeworks::DisentangledCorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-evaluate), [shapeworks::LegacyCorrespondenceFunction::Evaluate](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-evaluate)


### function Energy

```cpp
virtual double Energy(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
) const =0
```


**Reimplemented by**: [shapeworks::CorrespondenceFunction::Energy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-energy), [shapeworks::CurvatureSamplingFunction::Energy](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-energy), [shapeworks::DisentangledCorrespondenceFunction::Energy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-energy), [shapeworks::LegacyCorrespondenceFunction::Energy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-energy), [shapeworks::DualVectorFunction::Energy](../Classes/classshapeworks_1_1DualVectorFunction.md#function-energy), [shapeworks::SamplingFunction::Energy](../Classes/classshapeworks_1_1SamplingFunction.md#function-energy)


### function ResetBuffers

```cpp
inline virtual void ResetBuffers()
```


**Reimplemented by**: [shapeworks::SamplingFunction::ResetBuffers](../Classes/classshapeworks_1_1SamplingFunction.md#function-resetbuffers)


May be called by the solver class. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplemented by**: [shapeworks::CorrespondenceFunction::AfterIteration](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-afteriteration), [shapeworks::CurvatureSamplingFunction::AfterIteration](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-afteriteration), [shapeworks::DisentangledCorrespondenceFunction::AfterIteration](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-afteriteration), [shapeworks::DualVectorFunction::AfterIteration](../Classes/classshapeworks_1_1DualVectorFunction.md#function-afteriteration), [shapeworks::LegacyCorrespondenceFunction::AfterIteration](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-afteriteration)


This method is called by a solver after each iteration. Subclasses may or may not implement this method. 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplemented by**: [shapeworks::CorrespondenceFunction::BeforeIteration](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-beforeiteration), [shapeworks::CurvatureSamplingFunction::BeforeIteration](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-beforeiteration), [shapeworks::DisentangledCorrespondenceFunction::BeforeIteration](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-beforeiteration), [shapeworks::DualVectorFunction::BeforeIteration](../Classes/classshapeworks_1_1DualVectorFunction.md#function-beforeiteration), [shapeworks::LegacyCorrespondenceFunction::BeforeIteration](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-beforeiteration)


This method is called by a solver before each iteration. Subclasses may or may not implement this method. 


### function BeforeEvaluate

```cpp
inline virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
)
```


**Reimplemented by**: [shapeworks::DualVectorFunction::BeforeEvaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-beforeevaluate), [shapeworks::CurvatureSamplingFunction::BeforeEvaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function SetParticleSystem

```cpp
inline virtual void SetParticleSystem(
    ParticleSystem * p
)
```


**Reimplemented by**: [shapeworks::DualVectorFunction::SetParticleSystem](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setparticlesystem)


Some subclasses may require a pointer to the particle system and its domain number. These methods set/get those values. 


### function GetParticleSystem

```cpp
inline virtual ParticleSystem * GetParticleSystem() const
```


### function SetDomainNumber

```cpp
inline virtual void SetDomainNumber(
    unsigned int i
)
```


**Reimplemented by**: [shapeworks::DualVectorFunction::SetDomainNumber](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setdomainnumber)


### function GetDomainNumber

```cpp
inline virtual int GetDomainNumber() const
```


### function Clone

```cpp
inline virtual VectorFunction::Pointer Clone()
```


**Reimplemented by**: [shapeworks::CorrespondenceFunction::Clone](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-clone), [shapeworks::CurvatureSamplingFunction::Clone](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-clone), [shapeworks::DisentangledCorrespondenceFunction::Clone](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-clone), [shapeworks::DualVectorFunction::Clone](../Classes/classshapeworks_1_1DualVectorFunction.md#function-clone), [shapeworks::LegacyCorrespondenceFunction::Clone](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-clone), [shapeworks::SamplingFunction::Clone](../Classes/classshapeworks_1_1SamplingFunction.md#function-clone)


### function GetRelativeEnergyScaling

```cpp
inline virtual double GetRelativeEnergyScaling() const
```


**Reimplemented by**: [shapeworks::DualVectorFunction::GetRelativeEnergyScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getrelativeenergyscaling)


### function SetRelativeEnergyScaling

```cpp
inline virtual void SetRelativeEnergyScaling(
    double r
)
```


**Reimplemented by**: [shapeworks::DualVectorFunction::SetRelativeEnergyScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setrelativeenergyscaling)


## Protected Functions Documentation

### function VectorFunction

```cpp
inline VectorFunction()
```


### function ~VectorFunction

```cpp
inline virtual ~VectorFunction()
```


### function operator=

```cpp
void operator=(
    const VectorFunction & 
)
```


### function VectorFunction

```cpp
VectorFunction(
    const VectorFunction & 
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


## Protected Attributes Documentation

### variable m_ParticleSystem

```cpp
ParticleSystem * m_ParticleSystem;
```


### variable m_DomainNumber

```cpp
unsigned int m_DomainNumber;
```


-------------------------------

Updated on 2024-01-18 at 05:07:34 +0000