---
title: itk::ParticleModifiedCotangentEntropyGradientFunction

---

# itk::ParticleModifiedCotangentEntropyGradientFunction



 [More...](#detailed-description)


`#include <itkParticleModifiedCotangentEntropyGradientFunction.h>`

Inherits from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)< TGradientNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-superclass)**  |
| typedef Superclass::GradientNumericType | **[GradientNumericType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-gradientvectortype)**  |
| typedef [ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)< TGradientNumericType >::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-itktypemacro)**([ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md) , [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * c, double & d) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * ) |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * c) const |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-beforeiteration)**() |
| double | **[ComputeModifiedCotangent](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-computemodifiedcotangent)**(double rij, unsigned int d) const |
| double | **[ComputeModifiedCotangentDerivative](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-computemodifiedcotangentderivative)**(double rij, unsigned int d) const |
| void | **[ClearGlobalSigma](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clearglobalsigma)**() |
| void | **[SetGlobalSigma](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-setglobalsigma)**(std::vector< double > i) |
| void | **[SetGlobalSigma](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-setglobalsigma)**(double i) |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-particlemodifiedcotangententropygradientfunction)**() |
| virtual | **[~ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-~particlemodifiedcotangententropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-operator=)**(const [ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md) & ) |
| | **[ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-particlemodifiedcotangententropygradientfunction)**(const [ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< double > | **[m_GlobalSigma](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#variable-m-globalsigma)**  |

## Additional inherited members

**Public Types inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)< double, VDimension > | **[SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype)**  |

**Public Functions inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-resetbuffers)**() |
| virtual double | **[EstimateSigma](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-estimatesigma)**(unsigned int idx, const typename ParticleSystemType::PointVectorType & neighborhood, const [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md) * domain, const std::vector< double > & weights, const PointType & pos, double initial_sigma, double precision, int & err) const |
| TGradientNumericType | **[AngleCoefficient](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-anglecoefficient)**(const GradientVectorType & , const GradientVectorType & ) const |
| void | **[SetMinimumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-setminimumneighborhoodradius)**(double s) |
| double | **[GetMinimumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getminimumneighborhoodradius)**() const |
| void | **[SetMaximumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-setmaximumneighborhoodradius)**(double s) |
| double | **[GetMaximumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getmaximumneighborhoodradius)**() const |
| void | **[SetFlatCutoff](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-setflatcutoff)**(double s) |
| double | **[GetFlatCutoff](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getflatcutoff)**() const |
| void | **[SetNeighborhoodToSigmaRatio](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-setneighborhoodtosigmaratio)**(double s) |
| double | **[GetNeighborhoodToSigmaRatio](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getneighborhoodtosigmaratio)**() const |
| void | **[SetSpatialSigmaCache](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-setspatialsigmacache)**([SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype) * s) |
| [SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype) * | **[GetSpatialSigmaCache](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getspatialsigmacache)**() |
| const [SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype) * | **[GetSpatialSigmaCache](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-getspatialsigmacache)**() const |
| void | **[ComputeAngularWeights](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-computeangularweights)**(const PointType & , int , const typename ParticleSystemType::PointVectorType & , const [ParticleDomain](../Classes/classitk_1_1ParticleDomain.md) * , std::vector< double > & ) const |

**Protected Functions inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-particleentropygradientfunction)**() |
| virtual | **[~ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-~particleentropygradientfunction)**() |
| | **[ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-particleentropygradientfunction)**(const [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) & ) |

**Protected Attributes inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| double | **[m_MinimumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-minimumneighborhoodradius)**  |
| double | **[m_MaximumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-maximumneighborhoodradius)**  |
| double | **[m_FlatCutoff](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-flatcutoff)**  |
| double | **[m_NeighborhoodToSigmaRatio](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-neighborhoodtosigmaratio)**  |
| SigmaCacheType::Pointer | **[m_SpatialSigmaCache](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-spatialsigmacache)**  |

**Public Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleVectorFunction.md#function-resetbuffers)**() |
| virtual void | **[SetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * p) |
| virtual [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * | **[GetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-getparticlesystem)**() const |
| virtual void | **[SetDomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| virtual int | **[GetDomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#function-getdomainnumber)**() const |

**Protected Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-particlevectorfunction)**() |
| virtual | **[~ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-~particlevectorfunction)**() |
| | **[ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md#function-particlevectorfunction)**(const [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) & ) |

**Protected Attributes inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * | **[m_ParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#variable-m-particlesystem)**  |
| unsigned int | **[m_DomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#variable-m-domainnumber)**  |


## Detailed Description

```cpp
template <class TGradientNumericType ,
unsigned int VDimension>
class itk::ParticleModifiedCotangentEntropyGradientFunction;
```


This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. The change in normals associated with the position is also taken into account, providing an adaptive sampling of the surface with respect to both position and extrinsic surface curvature.

This function depend on modified cotangent potential as defined in Meyer's thesis rather than Gaussian potential (Cates's thesis).

Modified potential only depend on a global sigma based on target number of particles in the domain 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleModifiedCotangentEntropyGradientFunction itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


### typedef VnlMatrixType

```cpp
typedef ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::VnlMatrixType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleModifiedCotangentEntropyGradientFunction ,
    ParticleEntropyGradientFunction 
)
```


### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Dimensionality of the domain of the particle system. 


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int a,
    unsigned int b,
    const ParticleSystemType * c,
    double & d
) const
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * ,
    double & ,
    double & 
) const
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate)


### function BeforeEvaluate

```cpp
inline virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
)
```


**Reimplements**: [itk::ParticleVectorFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystemType * c
) const
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-energy)


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [itk::ParticleVectorFunction::AfterIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-afteriteration)


This method is called by a solver after each iteration. Subclasses may or may not implement this method. 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [itk::ParticleVectorFunction::BeforeIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeiteration)


This method is called by a solver before each iteration. Subclasses may or may not implement this method. 


### function ComputeModifiedCotangent

```cpp
inline double ComputeModifiedCotangent(
    double rij,
    unsigned int d
) const
```


### function ComputeModifiedCotangentDerivative

```cpp
inline double ComputeModifiedCotangentDerivative(
    double rij,
    unsigned int d
) const
```


### function ClearGlobalSigma

```cpp
inline void ClearGlobalSigma()
```


### function SetGlobalSigma

```cpp
inline void SetGlobalSigma(
    std::vector< double > i
)
```


### function SetGlobalSigma

```cpp
inline void SetGlobalSigma(
    double i
)
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleModifiedCotangentEntropyGradientFunction

```cpp
inline ParticleModifiedCotangentEntropyGradientFunction()
```


### function ~ParticleModifiedCotangentEntropyGradientFunction

```cpp
inline virtual ~ParticleModifiedCotangentEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleModifiedCotangentEntropyGradientFunction & 
)
```


### function ParticleModifiedCotangentEntropyGradientFunction

```cpp
ParticleModifiedCotangentEntropyGradientFunction(
    const ParticleModifiedCotangentEntropyGradientFunction & 
)
```


## Protected Attributes Documentation

### variable m_GlobalSigma

```cpp
std::vector< double > m_GlobalSigma;
```


-------------------------------

Updated on 2022-01-27 at 02:24:31 +0000