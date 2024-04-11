---
title: shapeworks::CurvatureSamplingFunction
summary: Please note: This is the sampling function currently being used. 

---

# shapeworks::CurvatureSamplingFunction



Please note: This is the sampling function currently being used.  [More...](#detailed-description)


`#include <CurvatureSamplingFunction.h>`

Inherits from [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md), [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Protected Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[CrossDomainNeighborhood](../Classes/structshapeworks_1_1CurvatureSamplingFunction_1_1CrossDomainNeighborhood.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef float | **[TGradientNumericType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-tgradientnumerictype)**  |
| typedef [CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) | **[Self](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-constpointer)**  |
| typedef [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) | **[Superclass](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-superclass)**  |
| typedef [Superclass::GradientNumericType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-gradientnumerictype) | **[GradientNumericType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-gradientvectortype)**  |
| typedef [MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md)< TGradientNumericType, VDimension > | **[MeanCurvatureCacheType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-meancurvaturecachetype)**  |
| typedef [shapeworks::ImageDomainWithCurvature](../Classes/classshapeworks_1_1ImageDomainWithCurvature.md)< TGradientNumericType >::VnlMatrixType | **[VnlMatrixType](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-itktypemacro)**([CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) , [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual VectorType | **[Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const |
| virtual VectorType | **[Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) |
| virtual double | **[Energy](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) const |
| double | **[ComputeKappa](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-computekappa)**(double mc, unsigned int d) const |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-beforeiteration)**() |
| virtual double | **[EstimateSigma](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-estimatesigma)**(unsigned int idx, unsigned int dom, const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * domain, const PointType & pos, double initial_sigma, double precision, int & err, double & avgKappa) const |
| void | **[SetMeanCurvatureCache](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-setmeancurvaturecache)**([MeanCurvatureCacheType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) * s) |
| [MeanCurvatureCacheType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) * | **[GetMeanCurvatureCache](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-getmeancurvaturecache)**() |
| const [MeanCurvatureCacheType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) * | **[GetMeanCurvatureCache](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-getmeancurvaturecache)**() const |
| void | **[SetRho](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-setrho)**(double g) |
| double | **[GetRho](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-getrho)**() const |
| void | **[SetSharedBoundaryWeight](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-setsharedboundaryweight)**(double w) |
| double | **[GetSharedBoundaryWeight](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-getsharedboundaryweight)**() const |
| void | **[SetSharedBoundaryEnabled](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-setsharedboundaryenabled)**(bool enabled) |
| bool | **[GetSharedBoundaryEnabled](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-getsharedboundaryenabled)**() const |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-curvaturesamplingfunction)**() |
| virtual | **[~CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-~curvaturesamplingfunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-operator=)**(const [CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) & ) |
| | **[CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-curvaturesamplingfunction)**(const [CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) & ) |
| void | **[UpdateNeighborhood](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-updateneighborhood)**(const PointType & pos, int idx, int d, double radius, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| MeanCurvatureCacheType::Pointer | **[m_MeanCurvatureCache](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-meancurvaturecache)**  |
| unsigned int | **[m_Counter](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-counter)**  |
| double | **[m_Rho](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-rho)**  |
| double | **[m_avgKappa](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-avgkappa)**  |
| bool | **[m_IsSharedBoundaryEnabled](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-issharedboundaryenabled)**  |
| double | **[m_SharedBoundaryWeight](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-sharedboundaryweight)**  |
| double | **[m_CurrentSigma](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-currentsigma)**  |
| std::vector< [CrossDomainNeighborhood](../Classes/structshapeworks_1_1CurvatureSamplingFunction_1_1CrossDomainNeighborhood.md) > | **[m_CurrentNeighborhood](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-currentneighborhood)**  |
| float | **[m_MaxMoveFactor](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#variable-m-maxmovefactor)**  |

## Additional inherited members

**Public Types inherited from [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< double > | **[SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-sigmacachetype)**  |

**Public Functions inherited from [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1SamplingFunction.md#function-resetbuffers)**() |
| TGradientNumericType | **[AngleCoefficient](../Classes/classshapeworks_1_1SamplingFunction.md#function-anglecoefficient)**(const GradientVectorType & , const GradientVectorType & ) const |
| void | **[SetMinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-setminimumneighborhoodradius)**(double s) |
| double | **[GetMinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-getminimumneighborhoodradius)**() const |
| void | **[SetMaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-setmaximumneighborhoodradius)**(double s) |
| double | **[GetMaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#function-getmaximumneighborhoodradius)**() const |
| void | **[SetFlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#function-setflatcutoff)**(double s) |
| double | **[GetFlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#function-getflatcutoff)**() const |
| void | **[SetNeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#function-setneighborhoodtosigmaratio)**(double s) |
| double | **[GetNeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#function-getneighborhoodtosigmaratio)**() const |
| void | **[SetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-setspatialsigmacache)**([SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-sigmacachetype) * s) |
| [SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-sigmacachetype) * | **[GetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-getspatialsigmacache)**() |
| const [SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-sigmacachetype) * | **[GetSpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#function-getspatialsigmacache)**() const |
| void | **[ComputeAngularWeights](../Classes/classshapeworks_1_1SamplingFunction.md#function-computeangularweights)**(const PointType & , int , const typename ParticleSystem::PointVectorType & , const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * , std::vector< double > & ) const |

**Protected Functions inherited from [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-samplingfunction)**() |
| virtual | **[~SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-~samplingfunction)**() |
| | **[SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-samplingfunction)**(const [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) & ) |

**Protected Attributes inherited from [shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)**

|                | Name           |
| -------------- | -------------- |
| double | **[m_MinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-minimumneighborhoodradius)**  |
| double | **[m_MaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-maximumneighborhoodradius)**  |
| double | **[m_FlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-flatcutoff)**  |
| double | **[m_NeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-neighborhoodtosigmaratio)**  |
| SigmaCacheType::Pointer | **[m_SpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-spatialsigmacache)**  |

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)**() |
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
class shapeworks::CurvatureSamplingFunction;
```

Please note: This is the sampling function currently being used. 

This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. The change in normals associated with the position is also taken into account, providing an adaptive sampling of the surface with respect to both position and extrinsic surface curvature. 

## Public Types Documentation

### typedef TGradientNumericType

```cpp
typedef float shapeworks::CurvatureSamplingFunction::TGradientNumericType;
```


### typedef Self

```cpp
typedef CurvatureSamplingFunction shapeworks::CurvatureSamplingFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::CurvatureSamplingFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::CurvatureSamplingFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef SamplingFunction shapeworks::CurvatureSamplingFunction::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType shapeworks::CurvatureSamplingFunction::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::CurvatureSamplingFunction::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType shapeworks::CurvatureSamplingFunction::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType shapeworks::CurvatureSamplingFunction::GradientVectorType;
```


### typedef MeanCurvatureCacheType

```cpp
typedef MeanCurvatureContainer<TGradientNumericType, VDimension> shapeworks::CurvatureSamplingFunction::MeanCurvatureCacheType;
```


### typedef VnlMatrixType

```cpp
typedef shapeworks::ImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType shapeworks::CurvatureSamplingFunction::VnlMatrixType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    CurvatureSamplingFunction ,
    SamplingFunction 
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
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep
) const
```


**Reimplements**: [shapeworks::SamplingFunction::Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & ,
    double & 
) const
```


**Reimplements**: [shapeworks::SamplingFunction::Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)


### function BeforeEvaluate

```cpp
virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
)
```


**Reimplements**: [shapeworks::VectorFunction::BeforeEvaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystem * c
) const
```


**Reimplements**: [shapeworks::SamplingFunction::Energy](../Classes/classshapeworks_1_1SamplingFunction.md#function-energy)


### function ComputeKappa

```cpp
inline double ComputeKappa(
    double mc,
    unsigned int d
) const
```


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [shapeworks::VectorFunction::AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)


This method is called by a solver after each iteration. Subclasses may or may not implement this method. 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [shapeworks::VectorFunction::BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)


This method is called by a solver before each iteration. Subclasses may or may not implement this method. 


### function EstimateSigma

```cpp
virtual double EstimateSigma(
    unsigned int idx,
    unsigned int dom,
    const shapeworks::ParticleDomain * domain,
    const PointType & pos,
    double initial_sigma,
    double precision,
    int & err,
    double & avgKappa
) const
```


Estimate the best sigma for Parzen windowing in a given neighborhood. The best sigma is the sigma that maximizes probability at the given point 


### function SetMeanCurvatureCache

```cpp
inline void SetMeanCurvatureCache(
    MeanCurvatureCacheType * s
)
```


### function GetMeanCurvatureCache

```cpp
inline MeanCurvatureCacheType * GetMeanCurvatureCache()
```


### function GetMeanCurvatureCache

```cpp
inline const MeanCurvatureCacheType * GetMeanCurvatureCache() const
```


### function SetRho

```cpp
inline void SetRho(
    double g
)
```


### function GetRho

```cpp
inline double GetRho() const
```


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


### function Clone

```cpp
inline virtual VectorFunction::Pointer Clone()
```


**Reimplements**: [shapeworks::SamplingFunction::Clone](../Classes/classshapeworks_1_1SamplingFunction.md#function-clone)


## Protected Functions Documentation

### function CurvatureSamplingFunction

```cpp
inline CurvatureSamplingFunction()
```


### function ~CurvatureSamplingFunction

```cpp
inline virtual ~CurvatureSamplingFunction()
```


### function operator=

```cpp
void operator=(
    const CurvatureSamplingFunction & 
)
```


### function CurvatureSamplingFunction

```cpp
CurvatureSamplingFunction(
    const CurvatureSamplingFunction & 
)
```


### function UpdateNeighborhood

```cpp
void UpdateNeighborhood(
    const PointType & pos,
    int idx,
    int d,
    double radius,
    const ParticleSystem * system
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
```


## Protected Attributes Documentation

### variable m_MeanCurvatureCache

```cpp
MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;
```


### variable m_Counter

```cpp
unsigned int m_Counter;
```


### variable m_Rho

```cpp
double m_Rho;
```


### variable m_avgKappa

```cpp
double m_avgKappa;
```


### variable m_IsSharedBoundaryEnabled

```cpp
bool m_IsSharedBoundaryEnabled {false};
```


### variable m_SharedBoundaryWeight

```cpp
double m_SharedBoundaryWeight {1.0};
```


### variable m_CurrentSigma

```cpp
double m_CurrentSigma;
```


### variable m_CurrentNeighborhood

```cpp
std::vector< CrossDomainNeighborhood > m_CurrentNeighborhood;
```


### variable m_MaxMoveFactor

```cpp
float m_MaxMoveFactor = 0;
```


-------------------------------

Updated on 2024-04-11 at 17:30:13 +0000