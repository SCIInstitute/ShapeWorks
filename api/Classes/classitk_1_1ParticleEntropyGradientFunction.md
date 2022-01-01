---
title: itk::ParticleEntropyGradientFunction
summary: This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. 

---

# itk::ParticleEntropyGradientFunction



This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution.  [More...](#detailed-description)


`#include <itkParticleEntropyGradientFunction.h>`

Inherits from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

Inherited by [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md), [itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md), [itk::ParticleModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md), [itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleOmegaGradientFunction.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-superclass)**  |
| typedef TGradientNumericType | **[GradientNumericType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)< double, VDimension > | **[SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef vnl_vector_fixed< TGradientNumericType, VDimension > | **[GradientVectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-gradientvectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-itktypemacro)**([ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) , [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * , double & ) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * , double & , double & ) const |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-energy)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * ) const |
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
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-particleentropygradientfunction)**() |
| virtual | **[~ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-~particleentropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-operator=)**(const [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) & ) |
| | **[ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-particleentropygradientfunction)**(const [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_MinimumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-minimumneighborhoodradius)**  |
| double | **[m_MaximumNeighborhoodRadius](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-maximumneighborhoodradius)**  |
| double | **[m_FlatCutoff](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-flatcutoff)**  |
| double | **[m_NeighborhoodToSigmaRatio](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-neighborhoodtosigmaratio)**  |
| SigmaCacheType::Pointer | **[m_SpatialSigmaCache](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#variable-m-spatialsigmacache)**  |

## Additional inherited members

**Public Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeiteration)**() |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * ) |
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
class itk::ParticleEntropyGradientFunction;
```

This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. 

The following description is an excerpt from

J Cates, P T Fletcher, M Styner, M Shenton, R Whitaker. Shape Modeling and Analysis with Entropy-Based Particle Systems. Information Processing in Medical Imaging IPMI 2007, LNCS 4584, pp. 333�345, 2007.

We treat a surface as a subset of \(\Re^d\), where \(d=2\) or \(d=3\) depending whether we are processing curves in the plane or surfaces in a volume, refspectively. The method we describe here deals with smooth, closed manifolds of codimension one, and we will refer to such manifolds as {_surfaces}_. We sample a surface \({\cal S} \subset \Re^d\) using a discrete set of \(N\) points that are considered random variables \(Z = (X_1, X_2, \ldots, X_N)\) drawn from a probability density function (PDF), \(p(X)\). We denote a realization of this PDF with lower case, and thus we have \(z = (x_1, x_2,\ldots, x_N)\), where \(z \in {\cal S}^N\). The probability of a realization \(x\) is \(p(X = x)\), which we denote simply as \(p(x)\).

The amount of information contained in such a random sampling is, in the limit, the differential entropy of the PDF, which is \(H[X] = -\int_S p(x) \log p(x) dx = -E\{\log p(X)\}\), where \(E\{ \cdot \}\) is the expectation. When we have a sufficient number of points sampled from \(p\), we can approximate the expectation by the sample mean, which gives \(H[X] \approx - (1/N)\sum_{i} \log p(x_i)\). We must also estimate \(p(x_i)\). Density functions on surfaces can be quite complex, and so we use a nonparametric, Parzen windowing estimation of this density using the particles themselves. Thus we have \[ p(x_i) \approx \frac{1}{N(N-1)} \sum^N_{j=1, j \neq i} G(x_i - x_j, \sigma_i), \] where \(G(x_i - x_j, \sigma_i)\) is a \(d\)-dimensional, isotropic Gaussian with standard deviation \(\sigma_i\). The cost function \(C\), is therefore an approximation of (negative) entropy: \[ -H[X] \approx C(x_1, \dots, x_N) = \sum_{i} \log \frac{1}{N(N-1)} \sum_{j \neq i} G(x_i - x_j, \sigma_i). \]

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleEntropyGradientFunction itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleVectorFunction<VDimension> itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef TGradientNumericType itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Data type representing individual gradient components. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


Type of particle system. 


### typedef SigmaCacheType

```cpp
typedef ParticleContainerArrayAttribute<double, VDimension> itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::SigmaCacheType;
```


Cache type for the sigma values. 


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef vnl_vector_fixed<TGradientNumericType, VDimension> itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleEntropyGradientFunction ,
    ParticleVectorFunction 
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
    const ParticleSystemType * ,
    double & 
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * ,
    double & ,
    double & 
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleCurvatureEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleModifiedCotangentEntropyGradientFunction::Evaluate](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate), [itk::ParticleOmegaGradientFunction::Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate)


### function Energy

```cpp
inline virtual double Energy(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Energy](../Classes/classitk_1_1ParticleVectorFunction.md#function-energy)


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleCurvatureEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy), [itk::ParticleModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleModifiedCotangentEntropyGradientFunction::Energy](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-energy), [itk::ParticleOmegaGradientFunction::Energy](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-energy), [itk::ParticleOmegaGradientFunction::Energy](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-energy)


### function ResetBuffers

```cpp
inline virtual void ResetBuffers()
```


**Reimplements**: [itk::ParticleVectorFunction::ResetBuffers](../Classes/classitk_1_1ParticleVectorFunction.md#function-resetbuffers)


May be called by the solver class. 


### function EstimateSigma

```cpp
virtual double EstimateSigma(
    unsigned int idx,
    const typename ParticleSystemType::PointVectorType & neighborhood,
    const ParticleDomain * domain,
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
    const typename ParticleSystemType::PointVectorType & ,
    const ParticleDomain * ,
    std::vector< double > & 
) const
```


Compute a set of weights based on the difference in the normals of a central point and each of its neighbors. Difference of > 90 degrees results in a weight of 0. 


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleVectorFunction::Clone](../Classes/classitk_1_1ParticleVectorFunction.md#function-clone)


**Reimplemented by**: [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleCurvatureEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-clone), [itk::ParticleCurvatureEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-clone), [itk::ParticleModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleModifiedCotangentEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md#function-clone), [itk::ParticleOmegaGradientFunction::Clone](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-clone), [itk::ParticleOmegaGradientFunction::Clone](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleEntropyGradientFunction

```cpp
inline ParticleEntropyGradientFunction()
```


### function ~ParticleEntropyGradientFunction

```cpp
inline virtual ~ParticleEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleEntropyGradientFunction & 
)
```


### function ParticleEntropyGradientFunction

```cpp
ParticleEntropyGradientFunction(
    const ParticleEntropyGradientFunction & 
)
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

Updated on 2022-01-01 at 17:59:57 +0000