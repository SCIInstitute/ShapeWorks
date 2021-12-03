---
title: itk::ParticleOmegaGradientFunction

---

# itk::ParticleOmegaGradientFunction



 [More...](#detailed-description)


`#include <itkParticleOmegaGradientFunction.h>`

Inherits from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)< TGradientNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-superclass)**  |
| typedef Superclass::GradientNumericType | **[GradientNumericType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-gradientvectortype)**  |
| typedef [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)< TGradientNumericType, VDimension > | **[MeanCurvatureCacheType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-meancurvaturecachetype)**  |
| typedef [ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)< TGradientNumericType >::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-itktypemacro)**([ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md) , [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * c, double & d) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * ) |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) * c) const |
| double | **[ComputeKappa](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-computekappa)**(double mc, unsigned int d, double planeDist) const |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-beforeiteration)**() |
| virtual double | **[EstimateSigma](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-estimatesigma)**(unsigned int , unsigned int , const typename ParticleSystemType::PointVectorType & , const std::vector< double > & , const PointType & , double , double , int & err, double & , unsigned int , unsigned int ) const |
| void | **[SetMeanCurvatureCache](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-setmeancurvaturecache)**([MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * s) |
| [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-getmeancurvaturecache)**() |
| const [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-getmeancurvaturecache)**() const |
| void | **[SetRho](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-setrho)**(double g) |
| double | **[GetRho](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-getrho)**() const |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-particleomegagradientfunction)**() |
| virtual | **[~ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-~particleomegagradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-operator=)**(const [ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md) & ) |
| | **[ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#function-particleomegagradientfunction)**(const [ParticleOmegaGradientFunction](../Classes/classitk_1_1ParticleOmegaGradientFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| MeanCurvatureCacheType::Pointer | **[m_MeanCurvatureCache](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-meancurvaturecache)**  |
| unsigned int | **[m_Counter](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-counter)**  |
| double | **[m_Rho](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-rho)**  |
| double | **[m_avgKappa](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-avgkappa)**  |
| double | **[m_CurrentSigma](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-currentsigma)**  |
| ParticleSystemType::PointVectorType | **[m_CurrentNeighborhood](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-currentneighborhood)**  |
| std::vector< double > | **[m_CurrentWeights](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-currentweights)**  |
| std::vector< itk::Point< double, VDimension > > | **[planePts](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-planepts)**  |
| std::vector< itk::Point< double, VDimension > > | **[spherePts](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-spherepts)**  |
| std::vector< double > | **[CToP](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-ctop)**  |
| float | **[m_MaxMoveFactor](../Classes/classitk_1_1ParticleOmegaGradientFunction.md#variable-m-maxmovefactor)**  |

## Additional inherited members

**Public Types inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleContainerArrayAttribute](../Classes/classitk_1_1ParticleContainerArrayAttribute.md)< double, VDimension > | **[SigmaCacheType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-sigmacachetype)**  |

**Public Functions inherited from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-resetbuffers)**() |
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
class itk::ParticleOmegaGradientFunction;
```


This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. The change in normals associated with the position is also taken into account, providing an adaptive sampling of the surface with respect to both position and extrinsic surface curvature. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleOmegaGradientFunction itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


### typedef MeanCurvatureCacheType

```cpp
typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::MeanCurvatureCacheType;
```


### typedef VnlMatrixType

```cpp
typedef ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType itk::ParticleOmegaGradientFunction< TGradientNumericType, VDimension >::VnlMatrixType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleOmegaGradientFunction ,
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
virtual void BeforeEvaluate(
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


### function ComputeKappa

```cpp
inline double ComputeKappa(
    double mc,
    unsigned int d,
    double planeDist
) const
```


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


### function EstimateSigma

```cpp
virtual double EstimateSigma(
    unsigned int ,
    unsigned int ,
    const typename ParticleSystemType::PointVectorType & ,
    const std::vector< double > & ,
    const PointType & ,
    double ,
    double ,
    int & err,
    double & ,
    unsigned int ,
    unsigned int 
) const
```


Estimate the best sigma for Parzen windowing in a given neighborhood. The best sigma is the sigma that maximizes probability at the given point 


### function SetMeanCurvatureCache

```cpp
inline void SetMeanCurvatureCache(
    MeanCurvatureCacheType * s
)
```


Access the cache of curvature-based weight values for each particle position. 


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


Set/Get the parameters in the kappa function. 


### function GetRho

```cpp
inline double GetRho() const
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleOmegaGradientFunction

```cpp
inline ParticleOmegaGradientFunction()
```


### function ~ParticleOmegaGradientFunction

```cpp
inline virtual ~ParticleOmegaGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleOmegaGradientFunction & 
)
```


### function ParticleOmegaGradientFunction

```cpp
ParticleOmegaGradientFunction(
    const ParticleOmegaGradientFunction & 
)
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


### variable m_CurrentSigma

```cpp
double m_CurrentSigma;
```


### variable m_CurrentNeighborhood

```cpp
ParticleSystemType::PointVectorType m_CurrentNeighborhood;
```


### variable m_CurrentWeights

```cpp
std::vector< double > m_CurrentWeights;
```


### variable planePts

```cpp
std::vector< itk::Point< double, VDimension > > planePts;
```


### variable spherePts

```cpp
std::vector< itk::Point< double, VDimension > > spherePts;
```


### variable CToP

```cpp
std::vector< double > CToP;
```


### variable m_MaxMoveFactor

```cpp
float m_MaxMoveFactor;
```


-------------------------------

Updated on 2021-12-03 at 02:18:10 +0000