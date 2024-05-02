---
title: shapeworks::SamplingFunction
summary: This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. 

---

# shapeworks::SamplingFunction



This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution.  [More...](#detailed-description)


`#include <SamplingFunction.h>`

Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

Inherited by [shapeworks::CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef float | **[TGradientNumericType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-tgradientnumerictype)**  |
| typedef [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) | **[Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-constpointer)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Superclass](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-superclass)**  |
| typedef TGradientNumericType | **[GradientNumericType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-gradientnumerictype)**  |
| typedef [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< double > | **[SigmaCacheType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-sigmacachetype)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-pointtype)**  |
| typedef vnl_vector_fixed< TGradientNumericType, VDimension > | **[GradientVectorType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-gradientvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1SamplingFunction.md#function-itktypemacro)**([SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) , [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1SamplingFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1SamplingFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const |
| virtual [VectorType](../Classes/classshapeworks_1_1SamplingFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1SamplingFunction.md#function-evaluate)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system, double & maxdt, double & energy) const |
| virtual double | **[Energy](../Classes/classshapeworks_1_1SamplingFunction.md#function-energy)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) const |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1SamplingFunction.md#function-resetbuffers)**() |
| virtual double | **[EstimateSigma](../Classes/classshapeworks_1_1SamplingFunction.md#function-estimatesigma)**(unsigned int idx, const typename ParticleSystem::PointVectorType & neighborhood, const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * domain, const std::vector< double > & weights, const PointType & pos, double initial_sigma, double precision, int & err) const |
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
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1SamplingFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-samplingfunction)**() |
| virtual | **[~SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-~samplingfunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1SamplingFunction.md#function-operator=)**(const [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) & ) |
| | **[SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md#function-samplingfunction)**(const [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) & ) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1SamplingFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_MinimumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-minimumneighborhoodradius)**  |
| double | **[m_MaximumNeighborhoodRadius](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-maximumneighborhoodradius)**  |
| double | **[m_FlatCutoff](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-flatcutoff)**  |
| double | **[m_NeighborhoodToSigmaRatio](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-neighborhoodtosigmaratio)**  |
| SigmaCacheType::Pointer | **[m_SpatialSigmaCache](../Classes/classshapeworks_1_1SamplingFunction.md#variable-m-spatialsigmacache)**  |

## Additional inherited members

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)**() |
| virtual void | **[BeforeEvaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) |
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

This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. 

Please note that [CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) is currently being used in all cases the curvature part is generally disabled though as the Rho value is always set to 0

The following description is an excerpt from

J Cates, P T Fletcher, M Styner, M Shenton, R Whitaker. [Shape](../Classes/classshapeworks_1_1Shape.md) Modeling and Analysis with Entropy-Based Particle Systems. Information Processing in Medical Imaging IPMI 2007, LNCS 4584, pp. 333�345, 2007.

We treat a surface as a subset of \(\Re^d\), where \(d=2\) or \(d=3\) depending whether we are processing curves in the plane or surfaces in a volume, refspectively. The method we describe here deals with smooth, closed manifolds of codimension one, and we will refer to such manifolds as {_surfaces}_. We sample a surface \({\cal S} \subset \Re^d\) using a discrete set of \(N\) points that are considered random variables \(Z = (X_1, X_2, \ldots, X_N)\) drawn from a probability density function (PDF), \(p(X)\). We denote a realization of this PDF with lower case, and thus we have \(z = (x_1, x_2,\ldots, x_N)\), where \(z \in {\cal S}^N\). The probability of a realization \(x\) is \(p(X = x)\), which we denote simply as \(p(x)\).

The amount of information contained in such a random sampling is, in the limit, the differential entropy of the PDF, which is \(H[X] = -\int_S p(x) \log p(x) dx = -E\{\log p(X)\}\), where \(E\{ \cdot \}\) is the expectation. When we have a sufficient number of points sampled from \(p\), we can approximate the expectation by the sample mean, which gives \(H[X] \approx - (1/N)\sum_{i} \log p(x_i)\). We must also estimate \(p(x_i)\). Density functions on surfaces can be quite complex, and so we use a nonparametric, Parzen windowing estimation of this density using the particles themselves. Thus we have \[ p(x_i) \approx \frac{1}{N(N-1)} \sum^N_{j=1, j \neq i} G(x_i - x_j, \sigma_i), \] where \(G(x_i - x_j, \sigma_i)\) is a \(d\)-dimensional, isotropic Gaussian with standard deviation \(\sigma_i\). The cost function \(C\), is therefore an approximation of (negative) entropy: \[ -H[X] \approx C(x_1, \dots, x_N) = \sum_{i} \log \frac{1}{N(N-1)} \sum_{j \neq i} G(x_i - x_j, \sigma_i). \]

## Public Types Documentation

### typedef TGradientNumericType

```cpp
typedef float shapeworks::SamplingFunction::TGradientNumericType;
```


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


### typedef Superclass

```cpp
typedef VectorFunction shapeworks::SamplingFunction::Superclass;
```


### typedef GradientNumericType

```cpp
typedef TGradientNumericType shapeworks::SamplingFunction::GradientNumericType;
```


Data type representing individual gradient components. 


### typedef SigmaCacheType

```cpp
typedef GenericContainerArray<double> shapeworks::SamplingFunction::SigmaCacheType;
```


Cache type for the sigma values. 


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::SamplingFunction::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystem::PointType shapeworks::SamplingFunction::PointType;
```


### typedef GradientVectorType

```cpp
typedef vnl_vector_fixed<TGradientNumericType, VDimension> shapeworks::SamplingFunction::GradientVectorType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    SamplingFunction ,
    VectorFunction 
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
virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


**Reimplemented by**: [shapeworks::CurvatureSamplingFunction::Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system,
    double & maxdt,
    double & energy
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


**Reimplemented by**: [shapeworks::CurvatureSamplingFunction::Evaluate](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-evaluate)


### function Energy

```cpp
inline virtual double Energy(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
) const
```


**Reimplements**: [shapeworks::VectorFunction::Energy](../Classes/classshapeworks_1_1VectorFunction.md#function-energy)


**Reimplemented by**: [shapeworks::CurvatureSamplingFunction::Energy](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-energy)


### function ResetBuffers

```cpp
inline virtual void ResetBuffers()
```


**Reimplements**: [shapeworks::VectorFunction::ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)


May be called by the solver class. 


### function EstimateSigma

```cpp
virtual double EstimateSigma(
    unsigned int idx,
    const typename ParticleSystem::PointVectorType & neighborhood,
    const shapeworks::ParticleDomain * domain,
    const std::vector< double > & weights,
    const PointType & pos,
    double initial_sigma,
    double precision,
    int & err
) const
```


Estimate the best sigma for Parzen windowing in a given neighborhood. The best sigma is the sigma that maximizes probability at the given point 


### function AngleCoefficient

```cpp
TGradientNumericType AngleCoefficient(
    const GradientVectorType & ,
    const GradientVectorType & 
) const
```


Returns a weighting coefficient based on the angle between two vectors. Weights smoothly approach zero as the angle between two normals approaches 90 degrees. 


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


### function ComputeAngularWeights

```cpp
void ComputeAngularWeights(
    const PointType & ,
    int ,
    const typename ParticleSystem::PointVectorType & ,
    const shapeworks::ParticleDomain * ,
    std::vector< double > & 
) const
```


Compute a set of weights based on the difference in the normals of a central point and each of its neighbors. Difference of > 90 degrees results in a weight of 0. 


### function Clone

```cpp
inline virtual VectorFunction::Pointer Clone()
```


**Reimplements**: [shapeworks::VectorFunction::Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)


**Reimplemented by**: [shapeworks::CurvatureSamplingFunction::Clone](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md#function-clone)


## Protected Functions Documentation

### function SamplingFunction

```cpp
inline SamplingFunction()
```


### function ~SamplingFunction

```cpp
inline virtual ~SamplingFunction()
```


### function operator=

```cpp
void operator=(
    const SamplingFunction & 
)
```


### function SamplingFunction

```cpp
SamplingFunction(
    const SamplingFunction & 
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
```


## Protected Attributes Documentation

### variable m_MinimumNeighborhoodRadius

```cpp
double m_MinimumNeighborhoodRadius;
```


### variable m_MaximumNeighborhoodRadius

```cpp
double m_MaximumNeighborhoodRadius;
```


### variable m_FlatCutoff

```cpp
double m_FlatCutoff;
```


### variable m_NeighborhoodToSigmaRatio

```cpp
double m_NeighborhoodToSigmaRatio;
```


### variable m_SpatialSigmaCache

```cpp
SigmaCacheType::Pointer m_SpatialSigmaCache;
```


-------------------------------

Updated on 2024-05-02 at 13:57:28 +0000