---
title: shapeworks::SamplingFunction

---

# shapeworks::SamplingFunction



 [More...](#detailed-description)


`#include <SamplingFunction.h>`

Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) | **[Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-constpointer)**  |
| using vnl_vector_fixed< double, 3 > | **[VectorType](../Classes/classshapeworks_1_1SamplingFunction.md#using-vectortype)**  |
| using [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1SamplingFunction.md#using-pointtype)**  |
| using vnl_vector_fixed< float, 3 > | **[GradientVectorType](../Classes/classshapeworks_1_1SamplingFunction.md#using-gradientvectortype)**  |
| using [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< double > | **[SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#using-sigmacachetype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1SamplingFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) ) |
| virtual VectorType | **[Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const override |
| virtual VectorType | **[Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & , double & ) const override |
| virtual void | **[BeforeEvaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) override |
| virtual double | **[Energy](../Classes/classshapeworks_1_1SamplingFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) const override |
| double | **[EstimateSigma](../Classes/classshapeworks_1_1SamplingFunction.md#function-estimatesigma)**(unsigned int idx, unsigned int dom, const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * domain, const PointType & pos, double initial_sigma, double precision, int & err, double & avg_kappa) const |
| void | **[SetSharedBoundaryWeight](../Classes/classshapeworks_1_1SamplingFunction.md#function-setsharedboundaryweight)**(double w) |
| double | **[GetSharedBoundaryWeight](../Classes/classshapeworks_1_1SamplingFunction.md#function-getsharedboundaryweight)**() const |
| void | **[SetSharedBoundaryEnabled](../Classes/classshapeworks_1_1SamplingFunction.md#function-setsharedboundaryenabled)**(bool enabled) |
| bool | **[GetSharedBoundaryEnabled](../Classes/classshapeworks_1_1SamplingFunction.md#function-getsharedboundaryenabled)**() const |
| void | **[SetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-setspatialsigmacache)**([SigmaCacheType](../Classes/classshapeworks_1_1GenericContainerArray.md) * s) |
| [SigmaCacheType](../Classes/classshapeworks_1_1GenericContainerArray.md) * | **[GetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-getspatialsigmacache)**() |
| const [SigmaCacheType](../Classes/classshapeworks_1_1GenericContainerArray.md) * | **[GetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-getspatialsigmacache)**() const |
| void | **[SetMinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-setminimumneighborhoodradius)**(double s) |
| double | **[GetMinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-getminimumneighborhoodradius)**() const |
| void | **[SetMaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-setmaximumneighborhoodradius)**(double s) |
| double | **[GetMaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-getmaximumneighborhoodradius)**() const |
| void | **[SetFlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#function-setflatcutoff)**(double s) |
| double | **[GetFlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#function-getflatcutoff)**() const |
| void | **[SetNeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#function-setneighborhoodtosigmaratio)**(double s) |
| double | **[GetNeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#function-getneighborhoodtosigmaratio)**() const |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1SamplingFunction.md#function-resetbuffers)**() override |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1SamplingFunction.md#function-clone)**() override |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1SamplingFunction.md#variable-vdimension)**  |

## Additional inherited members

**Public Types inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| typedef itk::LightObject | **[Superclass](../Classes/classshapeworks_1_1VectorFunction.md#typedef-superclass)**  |

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1VectorFunction.md#function-itktypemacro)**([VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) , LightObject ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1VectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)**() |
| virtual void | **[SetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-setparticlesystem)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| virtual [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-getparticlesystem)**() const |
| virtual void | **[SetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| virtual int | **[GetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-getdomainnumber)**() const |
| virtual double | **[GetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-getrelativeenergyscaling)**() const |
| virtual void | **[SetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-setrelativeenergyscaling)**(double r) |

**Protected Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-vectorfunction)**() |
| virtual | **[~VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-~vectorfunction)**() |
| | **[VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md#function-vectorfunction)**(const [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) & ) |

**Protected Attributes inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[m_ParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#variable-m-particlesystem)**  |
| unsigned int | **[m_DomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#variable-m-domainnumber)**  |


## Detailed Description

```cpp
class shapeworks::SamplingFunction;
```


This function is responsible for the sampling term of the optimization. E.g. the repulsion force between particles on a single shape 

## Public Types Documentation

### typedef Self

```cpp
typedef SamplingFunction shapeworks::SamplingFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::SamplingFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::SamplingFunction::ConstPointer;
```


### using VectorType

```cpp
using shapeworks::SamplingFunction::VectorType =  vnl_vector_fixed<double, 3>;
```


### using PointType

```cpp
using shapeworks::SamplingFunction::PointType =  ParticleSystem::PointType;
```


### using GradientVectorType

```cpp
using shapeworks::SamplingFunction::GradientVectorType =  vnl_vector_fixed<float, 3>;
```


### using SigmaCacheType

```cpp
using shapeworks::SamplingFunction::SigmaCacheType =  GenericContainerArray<double>;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep
) const override
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & ,
    double & 
) const override
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


### function BeforeEvaluate

```cpp
virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
) override
```


**Reimplements**: [shapeworks::VectorFunction::BeforeEvaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystem * c
) const override
```


**Reimplements**: [shapeworks::VectorFunction::Energy](../Classes/classshapeworks_1_1VectorFunction.md#function-energy)


### function EstimateSigma

```cpp
double EstimateSigma(
    unsigned int idx,
    unsigned int dom,
    const shapeworks::ParticleDomain * domain,
    const PointType & pos,
    double initial_sigma,
    double precision,
    int & err,
    double & avg_kappa
) const
```


Estimate the best sigma for Parzen windowing in a given neighborhood. The best sigma is the sigma that maximizes probability at the given point 


### function SetSharedBoundaryWeight

```cpp
inline void SetSharedBoundaryWeight(
    double w
)
```


### function GetSharedBoundaryWeight

```cpp
inline double GetSharedBoundaryWeight() const
```


### function SetSharedBoundaryEnabled

```cpp
inline void SetSharedBoundaryEnabled(
    bool enabled
)
```


### function GetSharedBoundaryEnabled

```cpp
inline bool GetSharedBoundaryEnabled() const
```


### function SetSpatialSigmaCache

```cpp
inline void SetSpatialSigmaCache(
    SigmaCacheType * s
)
```


Access the cache of sigma values for each particle position. This cache is populated by registering this object as an observer of the correct particle system (see SetParticleSystem). 


### function GetSpatialSigmaCache

```cpp
inline SigmaCacheType * GetSpatialSigmaCache()
```


### function GetSpatialSigmaCache

```cpp
inline const SigmaCacheType * GetSpatialSigmaCache() const
```


### function SetMinimumNeighborhoodRadius

```cpp
inline void SetMinimumNeighborhoodRadius(
    double s
)
```


Minimum radius of the neighborhood of points that are considered in the calculation. The neighborhood is a spherical radius in 3D space. The actual radius used in a calculation may exceed this value, but will not exceed the MaximumNeighborhoodRadius. 


### function GetMinimumNeighborhoodRadius

```cpp
inline double GetMinimumNeighborhoodRadius() const
```


### function SetMaximumNeighborhoodRadius

```cpp
inline void SetMaximumNeighborhoodRadius(
    double s
)
```


Maximum radius of the neighborhood of points that are considered in the calculation. The neighborhood is a spherical radius in 3D space. 


### function GetMaximumNeighborhoodRadius

```cpp
inline double GetMaximumNeighborhoodRadius() const
```


### function SetFlatCutoff

```cpp
inline void SetFlatCutoff(
    double s
)
```


Numerical parameters 


### function GetFlatCutoff

```cpp
inline double GetFlatCutoff() const
```


### function SetNeighborhoodToSigmaRatio

```cpp
inline void SetNeighborhoodToSigmaRatio(
    double s
)
```


### function GetNeighborhoodToSigmaRatio

```cpp
inline double GetNeighborhoodToSigmaRatio() const
```


### function ResetBuffers

```cpp
inline virtual void ResetBuffers() override
```


**Reimplements**: [shapeworks::VectorFunction::ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)


May be called by the solver class. 


### function Clone

```cpp
virtual VectorFunction::Pointer Clone() override
```


**Reimplements**: [shapeworks::VectorFunction::Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
```


-------------------------------

Updated on 2025-08-16 at 16:54:09 +0000