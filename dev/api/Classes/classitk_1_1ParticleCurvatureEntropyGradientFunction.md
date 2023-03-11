---
title: itk::ParticleCurvatureEntropyGradientFunction

---

# itk::ParticleCurvatureEntropyGradientFunction



 [More...](#detailed-description)


`#include <itkParticleCurvatureEntropyGradientFunction.h>`

Inherits from [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleEntropyGradientFunction< TGradientNumericType, VDimension >](../Classes/classitk_1_1ParticleEntropyGradientFunction.md), [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Protected Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[CrossDomainNeighborhood](../Classes/structitk_1_1ParticleCurvatureEntropyGradientFunction_1_1CrossDomainNeighborhood.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)< TGradientNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-superclass)**  |
| typedef [Superclass::GradientNumericType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-gradientnumerictype) | **[GradientNumericType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-gradientvectortype)**  |
| typedef [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)< TGradientNumericType, VDimension > | **[MeanCurvatureCacheType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-meancurvaturecachetype)**  |
| typedef [shapeworks::ParticleImageDomainWithCurvature](../Classes/classshapeworks_1_1ParticleImageDomainWithCurvature.md)< TGradientNumericType >::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-vnlmatrixtype)**  |
| typedef [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md)< TGradientNumericType, VDimension > | **[Superclass](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-superclass)**  |
| typedef [Superclass::GradientNumericType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-gradientnumerictype) | **[GradientNumericType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-gradientnumerictype)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef Superclass::PointType | **[PointType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef Superclass::GradientVectorType | **[GradientVectorType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-gradientvectortype)**  |
| typedef [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)< TGradientNumericType, VDimension > | **[MeanCurvatureCacheType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-meancurvaturecachetype)**  |
| typedef ParticleImageDomainWithCurvature< TGradientNumericType, VDimension >::VnlMatrixType | **[VnlMatrixType](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itktypemacro)**([ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) , [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c, double & d) const |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * ) |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c) const |
| double | **[ComputeKappa](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-computekappa)**(double mc, unsigned int d) const |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration)**() |
| virtual double | **[EstimateSigma](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-estimatesigma)**(unsigned int idx, unsigned int dom, const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * domain, const PointType & pos, double initial_sigma, double precision, int & err, double & avgKappa) const |
| void | **[SetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setmeancurvaturecache)**([MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * s) |
| [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getmeancurvaturecache)**() |
| const [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getmeancurvaturecache)**() const |
| void | **[SetRho](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setrho)**(double g) |
| double | **[GetRho](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getrho)**() const |
| void | **[SetSharedBoundaryWeight](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setsharedboundaryweight)**(double w) |
| double | **[GetSharedBoundaryWeight](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getsharedboundaryweight)**() const |
| void | **[SetSharedBoundaryEnabled](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setsharedboundaryenabled)**(bool enabled) |
| bool | **[GetSharedBoundaryEnabled](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getsharedboundaryenabled)**() const |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-clone)**() |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itktypemacro)**([ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) , [ParticleEntropyGradientFunction](../Classes/classitk_1_1ParticleEntropyGradientFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c, double & d) const |
| virtual VectorType | **[Evaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * ) |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * c) const |
| double | **[ComputeKappa](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-computekappa)**(double mc, unsigned int d) const |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-beforeiteration)**() |
| virtual double | **[EstimateSigma](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-estimatesigma)**(unsigned int , unsigned int , const typename ParticleSystemType::PointVectorType & , const std::vector< double > & , const PointType & , double , double , int & err, double & ) const |
| void | **[SetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setmeancurvaturecache)**([MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * s) |
| [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getmeancurvaturecache)**() |
| const [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * | **[GetMeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getmeancurvaturecache)**() const |
| void | **[SetRho](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-setrho)**(double g) |
| double | **[GetRho](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-getrho)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-particlecurvatureentropygradientfunction)**() |
| virtual | **[~ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-~particlecurvatureentropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-operator=)**(const [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) & ) |
| | **[ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-particlecurvatureentropygradientfunction)**(const [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) & ) |
| void | **[UpdateNeighborhood](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-updateneighborhood)**(const PointType & pos, int idx, int d, double radius, const [ParticleSystemType](../Classes/classitk_1_1ParticleSystem.md) * system) |
| | **[ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-particlecurvatureentropygradientfunction)**() |
| virtual | **[~ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-~particlecurvatureentropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-operator=)**(const [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) & ) |
| | **[ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#function-particlecurvatureentropygradientfunction)**(const [ParticleCurvatureEntropyGradientFunction](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| MeanCurvatureCacheType::Pointer | **[m_MeanCurvatureCache](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-meancurvaturecache)**  |
| unsigned int | **[m_Counter](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-counter)**  |
| double | **[m_Rho](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-rho)**  |
| double | **[m_avgKappa](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-avgkappa)**  |
| bool | **[m_IsSharedBoundaryEnabled](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-issharedboundaryenabled)**  |
| double | **[m_SharedBoundaryWeight](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-sharedboundaryweight)**  |
| double | **[m_CurrentSigma](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-currentsigma)**  |
| std::vector< [CrossDomainNeighborhood](../Classes/structitk_1_1ParticleCurvatureEntropyGradientFunction_1_1CrossDomainNeighborhood.md) > | **[m_CurrentNeighborhood](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-currentneighborhood)**  |
| float | **[m_MaxMoveFactor](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-maxmovefactor)**  |
| ParticleSystemType::PointVectorType | **[m_CurrentNeighborhood](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-currentneighborhood)**  |
| std::vector< double > | **[m_CurrentWeights](../Classes/classitk_1_1ParticleCurvatureEntropyGradientFunction.md#variable-m-currentweights)**  |

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
| void | **[ComputeAngularWeights](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-computeangularweights)**(const PointType & , int , const typename ParticleSystemType::PointVectorType & , const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * , std::vector< double > & ) const |

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
| void | **[ComputeAngularWeights](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-computeangularweights)**(const PointType & , int , const typename ParticleSystemType::PointVectorType & , const [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) * , std::vector< double > & ) const |

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
| virtual double | **[GetRelativeEnergyScaling](../Classes/classitk_1_1ParticleVectorFunction.md#function-getrelativeenergyscaling)**() const |
| virtual void | **[SetRelativeEnergyScaling](../Classes/classitk_1_1ParticleVectorFunction.md#function-setrelativeenergyscaling)**(double r) |

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
class itk::ParticleCurvatureEntropyGradientFunction;
```


This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution. The change in normals associated with the position is also taken into account, providing an adaptive sampling of the surface with respect to both position and extrinsic surface curvature. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleCurvatureEntropyGradientFunction itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


### typedef MeanCurvatureCacheType

```cpp
typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::MeanCurvatureCacheType;
```


### typedef VnlMatrixType

```cpp
typedef shapeworks::ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::VnlMatrixType;
```


### typedef Self

```cpp
typedef ParticleCurvatureEntropyGradientFunction itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::Superclass;
```


### typedef GradientNumericType

```cpp
typedef Superclass::GradientNumericType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::GradientNumericType;
```


Inherit some parent typedefs. 


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::ParticleSystemType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::VectorType;
```


### typedef PointType

```cpp
typedef Superclass::PointType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::PointType;
```


### typedef GradientVectorType

```cpp
typedef Superclass::GradientVectorType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::GradientVectorType;
```


### typedef MeanCurvatureCacheType

```cpp
typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::MeanCurvatureCacheType;
```


### typedef VnlMatrixType

```cpp
typedef ParticleImageDomainWithCurvature<TGradientNumericType,VDimension>::VnlMatrixType itk::ParticleCurvatureEntropyGradientFunction< TGradientNumericType, VDimension >::VnlMatrixType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleCurvatureEntropyGradientFunction ,
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


### function BeforeEvaluate

```cpp
virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
)
```


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystemType * c
) const
```


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
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleEntropyGradientFunction::Clone](../Classes/classitk_1_1ParticleEntropyGradientFunction.md#function-clone)


### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleCurvatureEntropyGradientFunction ,
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


### function BeforeEvaluate

```cpp
virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
)
```


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystemType * c
) const
```


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
    double & 
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


## Protected Functions Documentation

### function ParticleCurvatureEntropyGradientFunction

```cpp
inline ParticleCurvatureEntropyGradientFunction()
```


### function ~ParticleCurvatureEntropyGradientFunction

```cpp
inline virtual ~ParticleCurvatureEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleCurvatureEntropyGradientFunction & 
)
```


### function ParticleCurvatureEntropyGradientFunction

```cpp
ParticleCurvatureEntropyGradientFunction(
    const ParticleCurvatureEntropyGradientFunction & 
)
```


### function UpdateNeighborhood

```cpp
inline void UpdateNeighborhood(
    const PointType & pos,
    int idx,
    int d,
    double radius,
    const ParticleSystemType * system
)
```


### function ParticleCurvatureEntropyGradientFunction

```cpp
inline ParticleCurvatureEntropyGradientFunction()
```


### function ~ParticleCurvatureEntropyGradientFunction

```cpp
inline virtual ~ParticleCurvatureEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleCurvatureEntropyGradientFunction & 
)
```


### function ParticleCurvatureEntropyGradientFunction

```cpp
ParticleCurvatureEntropyGradientFunction(
    const ParticleCurvatureEntropyGradientFunction & 
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
float m_MaxMoveFactor;
```


### variable m_CurrentNeighborhood

```cpp
ParticleSystemType::PointVectorType m_CurrentNeighborhood;
```


### variable m_CurrentWeights

```cpp
std::vector< double > m_CurrentWeights;
```


-------------------------------

Updated on 2023-03-11 at 06:26:42 +0000