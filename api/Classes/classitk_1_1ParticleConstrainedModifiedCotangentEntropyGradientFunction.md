---
title: itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction

---

# itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction



 [More...](#detailed-description)


`#include <itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h>`

Inherits from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)< TGradientNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-superclass)**  |
| typedef [Superclass::GradientNumericType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-gradientnumerictype) | **[GradientNumericType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-gradientvectortype)**  |
| typedef [ParticleImageDomainWithCurvature](../Classes/classitk_1_1ParticleImageDomainWithCurvature.md)< TGradientNumericType >::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-itktypemacro)**([ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md) , [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c, double & d) const |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * ) |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c) const |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-beforeiteration)**() |
| void | **[EstimateGlobalSigma](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-estimateglobalsigma)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * system) |
| double | **[ComputeModifiedCotangent](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-computemodifiedcotangent)**(double rij) const |
| double | **[ComputeModifiedCotangentDerivative](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-computemodifiedcotangentderivative)**(double rij) const |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-getdomainspershape)**() const |
| void | **[SetRunStatus](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-setrunstatus)**(int i) |
| int | **[GetRunStatus](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-getrunstatus)**() const |
| void | **[SetDiagnosticsOutputPrefix](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-setdiagnosticsoutputprefix)**(const std::string s) |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-particleconstrainedmodifiedcotangententropygradientfunction)**() |
| virtual | **[~ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-~particleconstrainedmodifiedcotangententropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-operator=)**(const [ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md) & ) |
| | **[ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#function-particleconstrainedmodifiedcotangententropygradientfunction)**(const [ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| unsigned int | **[m_Counter](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-counter)**  |
| ParticleSystemType::PointVectorType | **[m_CurrentNeighborhood](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-currentneighborhood)**  |
| std::vector< double > | **[m_CurrentWeights](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-currentweights)**  |
| std::string | **[m_diagnostics_prefix](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-diagnostics-prefix)**  |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-domainspershape)**  |
| int | **[m_RunStatus](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-runstatus)**  |
| double | **[m_GlobalSigma](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-globalsigma)**  |
| float | **[m_MaxMoveFactor](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md#variable-m-maxmovefactor)**  |

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
class itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction;
```


This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. The change in normals associated with the position is also taken into account, providing an adaptive sampling of the surface with respect to both position and extrinsic surface curvature.

This function depend on modified cotangent potential as defined in Meyer's thesis rather than Gaussian potential (Cates's thesis).

Modified potential only depend on a global sigma which defined the neighborhood of each particle compared to a sigma per particle in case of Gaussian potential.

This class also constrains the particle distribution according to the provided spheres (in or out) ?!!! 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleConstrainedModifiedCotangentEntropyGradientFunction itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


### typedef VnlMatrixType

```cpp
typedef ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction< TGradientNumericType, VDimension >::VnlMatrixType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleConstrainedModifiedCotangentEntropyGradientFunction ,
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


### function EstimateGlobalSigma

```cpp
void EstimateGlobalSigma(
    const ParticleSystemType * system
)
```


Estimate the best sigma for Parzen windowing. This is almost twice the average distance to the nearest 6 neigbhors (hexagonal packing) 


### function ComputeModifiedCotangent

```cpp
inline double ComputeModifiedCotangent(
    double rij
) const
```


### function ComputeModifiedCotangentDerivative

```cpp
inline double ComputeModifiedCotangentDerivative(
    double rij
) const
```


### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    int i
)
```


### function GetDomainsPerShape

```cpp
inline int GetDomainsPerShape() const
```


### function SetRunStatus

```cpp
inline void SetRunStatus(
    int i
)
```


### function GetRunStatus

```cpp
inline int GetRunStatus() const
```


### function SetDiagnosticsOutputPrefix

```cpp
inline void SetDiagnosticsOutputPrefix(
    const std::string s
)
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleConstrainedModifiedCotangentEntropyGradientFunction

```cpp
inline ParticleConstrainedModifiedCotangentEntropyGradientFunction()
```


### function ~ParticleConstrainedModifiedCotangentEntropyGradientFunction

```cpp
inline virtual ~ParticleConstrainedModifiedCotangentEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleConstrainedModifiedCotangentEntropyGradientFunction & 
)
```


### function ParticleConstrainedModifiedCotangentEntropyGradientFunction

```cpp
ParticleConstrainedModifiedCotangentEntropyGradientFunction(
    const ParticleConstrainedModifiedCotangentEntropyGradientFunction & 
)
```


## Protected Attributes Documentation

### variable m_Counter

```cpp
unsigned int m_Counter;
```


### variable m_CurrentNeighborhood

```cpp
ParticleSystemType::PointVectorType m_CurrentNeighborhood;
```


### variable m_CurrentWeights

```cpp
std::vector< double > m_CurrentWeights;
```


### variable m_diagnostics_prefix

```cpp
std::string m_diagnostics_prefix;
```


### variable m_DomainsPerShape

```cpp
int m_DomainsPerShape;
```


### variable m_RunStatus

```cpp
int m_RunStatus;
```


### variable m_GlobalSigma

```cpp
double m_GlobalSigma;
```


### variable m_MaxMoveFactor

```cpp
float m_MaxMoveFactor;
```


-------------------------------

Updated on 2021-11-23 at 22:14:02 +0000