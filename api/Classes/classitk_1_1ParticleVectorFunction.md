---
title: itk::ParticleVectorFunction

---

# itk::ParticleVectorFunction



 [More...](#detailed-description)


`#include <itkParticleVectorFunction.h>`

Inherits from LightObject

Inherited by [itk::ParticleDualVectorFunction< VDimension >](../Classes/classitk_1_1ParticleDualVectorFunction.md), [itk::ParticleEnsembleEntropyFunction< VDimension >](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md), [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md), [itk::ParticleDualVectorFunction< Dimension >](../Classes/classitk_1_1ParticleDualVectorFunction.md), [itk::ParticleEnsembleEntropyFunction< Dimension >](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md), [itk::ParticleEntropyGradientFunction< ImageType::PixelType, Dimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleEntropyGradientFunction< ImageType::PixelType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleMeshBasedGeneralEntropyGradientFunction< Dimension >](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) | **[Self](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-constpointer)**  |
| typedef LightObject | **[Superclass](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-superclass)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype)**  |
| typedef vnl_vector_fixed< double, VDimension > | **[VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleVectorFunction.md#function-itktypemacro)**([ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) , LightObject ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleVectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * , double & maxtimestep) const =0 |
| virtual [VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * , double & maxtimestep, double & energy) const =0 |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleVectorFunction.md#function-energy)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * ) const =0 |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleVectorFunction.md#function-resetbuffers)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeiteration)**() |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * ) |
| virtual void | **[SetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * p) |
| virtual [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * | **[GetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-getparticlesystem)**() const |
| virtual void | **[SetDomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| virtual int | **[GetDomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#function-getdomainnumber)**() const |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleVectorFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-particlevectorfunction)**() |
| virtual | **[~ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-~particlevectorfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleVectorFunction.md#function-operator=)**(const [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) & ) |
| | **[ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-particlevectorfunction)**(const [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * | **[m_ParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#variable-m-particlesystem)**  |
| unsigned int | **[m_DomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#variable-m-domainnumber)**  |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticleVectorFunction;
```


This is the base class defining the API for a function that takes a particle system, domain, and location index as arguments and returns a vector-valued result. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleVectorFunction itk::ParticleVectorFunction< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleVectorFunction< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleVectorFunction< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef LightObject itk::ParticleVectorFunction< VDimension >::Superclass;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleVectorFunction< VDimension >::ParticleSystemType;
```


Type of particle system. 


### typedef VectorType

```cpp
typedef vnl_vector_fixed<double, VDimension> itk::ParticleVectorFunction< VDimension >::VectorType;
```


Type of vectors. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleVectorFunction ,
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
    const ParticleSystemType * ,
    double & maxtimestep
) const =0
```


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleEnsembleEntropyFunction::Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate), [itk::ParticleEnsembleEntropyFunction::Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate), [itk::ParticleDualVectorFunction::Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate), [itk::ParticleDualVectorFunction::Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * ,
    double & maxtimestep,
    double & energy
) const =0
```


**Reimplemented by**: [itk::ParticleDualVectorFunction::Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate), [itk::ParticleDualVectorFunction::Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleEnsembleEntropyFunction::Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate), [itk::ParticleEnsembleEntropyFunction::Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate), [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate)


### function Energy

```cpp
virtual double Energy(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
) const =0
```


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleEnsembleEntropyFunction::Energy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-energy), [itk::ParticleEnsembleEntropyFunction::Energy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-energy), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-energy), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-energy), [itk::ParticleModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleOmegaGradientFunction::Energy](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-energy), [itk::ParticleOmegaGradientFunction::Energy](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-energy), [itk::ParticleDualVectorFunction::Energy](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-energy), [itk::ParticleDualVectorFunction::Energy](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-energy), [itk::ParticleEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-energy), [itk::ParticleEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-energy), [itk::ParticleEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-energy)


### function ResetBuffers

```cpp
inline virtual void ResetBuffers()
```


**Reimplemented by**: [itk::ParticleEntropyGradientFunction::ResetBuffers](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-resetbuffers), [itk::ParticleEntropyGradientFunction::ResetBuffers](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-resetbuffers), [itk::ParticleEntropyGradientFunction::ResetBuffers](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-resetbuffers)


May be called by the solver class. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-afteriteration), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-afteriteration), [itk::ParticleCurvatureEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration), [itk::ParticleCurvatureEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration), [itk::ParticleCurvatureEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration), [itk::ParticleCurvatureEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration), [itk::ParticleDualVectorFunction::AfterIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-afteriteration), [itk::ParticleDualVectorFunction::AfterIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-afteriteration), [itk::ParticleEnsembleEntropyFunction::AfterIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-afteriteration), [itk::ParticleEnsembleEntropyFunction::AfterIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-afteriteration), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-afteriteration), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-afteriteration), [itk::ParticleModifiedCotangentEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-afteriteration), [itk::ParticleModifiedCotangentEntropyGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-afteriteration), [itk::ParticleOmegaGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-afteriteration), [itk::ParticleOmegaGradientFunction::AfterIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-afteriteration)


This method is called by a solver after each iteration. Subclasses may or may not implement this method. 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleCurvatureEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleCurvatureEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleCurvatureEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleCurvatureEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleDualVectorFunction::BeforeIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeiteration), [itk::ParticleDualVectorFunction::BeforeIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeiteration), [itk::ParticleEnsembleEntropyFunction::BeforeIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-beforeiteration), [itk::ParticleEnsembleEntropyFunction::BeforeIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-beforeiteration), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleModifiedCotangentEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleModifiedCotangentEntropyGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeiteration), [itk::ParticleOmegaGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeiteration), [itk::ParticleOmegaGradientFunction::BeforeIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeiteration)


This method is called by a solver before each iteration. Subclasses may or may not implement this method. 


### function BeforeEvaluate

```cpp
inline virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
)
```


**Reimplemented by**: [itk::ParticleDualVectorFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeevaluate), [itk::ParticleDualVectorFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeevaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleCurvatureEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleCurvatureEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleCurvatureEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleCurvatureEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate), [itk::ParticleOmegaGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeevaluate), [itk::ParticleOmegaGradientFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function SetParticleSystem

```cpp
inline virtual void SetParticleSystem(
    ParticleSystemType * p
)
```


**Reimplemented by**: [itk::ParticleDualVectorFunction::SetParticleSystem](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setparticlesystem), [itk::ParticleDualVectorFunction::SetParticleSystem](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setparticlesystem)


Some subclasses may require a pointer to the particle system and its domain number. These methods set/get those values. 


### function GetParticleSystem

```cpp
inline virtual ParticleSystemType * GetParticleSystem() const
```


### function SetDomainNumber

```cpp
inline virtual void SetDomainNumber(
    unsigned int i
)
```


**Reimplemented by**: [itk::ParticleDualVectorFunction::SetDomainNumber](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setdomainnumber), [itk::ParticleDualVectorFunction::SetDomainNumber](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setdomainnumber)


### function GetDomainNumber

```cpp
inline virtual int GetDomainNumber() const
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleCurvatureEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-clone), [itk::ParticleCurvatureEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-clone), [itk::ParticleDualVectorFunction::Clone](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-clone), [itk::ParticleDualVectorFunction::Clone](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-clone), [itk::ParticleEnsembleEntropyFunction::Clone](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-clone), [itk::ParticleEnsembleEntropyFunction::Clone](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-clone), [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone), [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone), [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-clone), [itk::ParticleMeshBasedGeneralEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-clone), [itk::ParticleModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleOmegaGradientFunction::Clone](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-clone), [itk::ParticleOmegaGradientFunction::Clone](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleVectorFunction

```cpp
inline ParticleVectorFunction()
```


### function ~ParticleVectorFunction

```cpp
inline virtual ~ParticleVectorFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleVectorFunction & 
)
```


### function ParticleVectorFunction

```cpp
ParticleVectorFunction(
    const ParticleVectorFunction & 
)
```


## Protected Attributes Documentation

### variable m_ParticleSystem

```cpp
ParticleSystemType * m_ParticleSystem;
```


### variable m_DomainNumber

```cpp
unsigned int m_DomainNumber;
```


-------------------------------

Updated on 2021-11-19 at 20:38:58 +0000