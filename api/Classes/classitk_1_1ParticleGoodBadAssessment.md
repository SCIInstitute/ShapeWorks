---
title: itk::ParticleGoodBadAssessment

---

# itk::ParticleGoodBadAssessment



 [More...](#detailed-description)

Inherits from Object

## Public Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[IdxCompare](../Classes/structitk_1_1ParticleGoodBadAssessment_1_1IdxCompare.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleGoodBadAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md) | **[Self](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleGoodBadAssessment.md) > | **[Pointer](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleGoodBadAssessment.md) > | **[ConstPointer](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleGoodBadAssessment.md) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-constweakpointer)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-particlesystemtype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-pointtype)**  |
| typedef [ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)< TGradientNumericType > | **[DomainType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-domaintype)**  |
| typedef DomainType::VnlVectorType | **[NormalType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-normaltype)**  |
| typedef [ParticleMeanCurvatureAttribute](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md)< TGradientNumericType, VDimension > | **[MeanCurvatureCacheType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-meancurvaturecachetype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleGoodBadAssessment.md) ) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-getdomainspershape)**() const |
| void | **[SetEpsilon](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-setepsilon)**(double i) |
| void | **[SetCriterionAngle](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-setcriterionangle)**(double a) |
| void | **[SetPerformAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-setperformassessment)**(bool b) |
| std::vector< std::vector< int > > | **[RunAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-runassessment)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-particlesystemtype) * m_ParticleSystem, [MeanCurvatureCacheType](../Classes/classitk_1_1ParticleMeanCurvatureAttribute.md) * m_MeanCurvatureCache) |
| vnl_matrix< double > | **[computeParticlesNormals](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-computeparticlesnormals)**(int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleGoodBadAssessment.md#typedef-particlesystemtype) * m_ParticleSystem) |
| std::vector< int > | **[sortIdcs](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-sortidcs)**(std::vector< double > v) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGoodBadAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-particlegoodbadassessment)**() |
| virtual | **[~ParticleGoodBadAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md#function-~particlegoodbadassessment)**() |

## Detailed Description

```cpp
template <class TGradientNumericType ,
unsigned int VDimension>
class itk::ParticleGoodBadAssessment;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleGoodBadAssessment itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::Self;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::ConstWeakPointer;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::ParticleSystemType;
```


Particle system typedefs. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::PointType;
```


### typedef DomainType

```cpp
typedef ParticleImageDomainWithGradients<TGradientNumericType> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::DomainType;
```


Type of the domain. 


### typedef NormalType

```cpp
typedef DomainType::VnlVectorType itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::NormalType;
```


### typedef MeanCurvatureCacheType

```cpp
typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> itk::ParticleGoodBadAssessment< TGradientNumericType, VDimension >::MeanCurvatureCacheType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
inline itkNewMacro(
    Self 
)
```


Method for creation through the object factory. Set/Get the number of Domains in each shape. 


### function GetDomainsPerShape

```cpp
inline int GetDomainsPerShape() const
```


### function SetEpsilon

```cpp
inline void SetEpsilon(
    double i
)
```


### function SetCriterionAngle

```cpp
inline void SetCriterionAngle(
    double a
)
```


### function SetPerformAssessment

```cpp
inline void SetPerformAssessment(
    bool b
)
```


### function RunAssessment

```cpp
std::vector< std::vector< int > > RunAssessment(
    const ParticleSystemType * m_ParticleSystem,
    MeanCurvatureCacheType * m_MeanCurvatureCache
)
```


Performs good/bad points assessment and reports the bad positions of the particle system. 


### function computeParticlesNormals

```cpp
vnl_matrix< double > computeParticlesNormals(
    int d,
    const ParticleSystemType * m_ParticleSystem
)
```


### function sortIdcs

```cpp
inline std::vector< int > sortIdcs(
    std::vector< double > v
)
```


## Protected Functions Documentation

### function ParticleGoodBadAssessment

```cpp
inline ParticleGoodBadAssessment()
```


### function ~ParticleGoodBadAssessment

```cpp
inline virtual ~ParticleGoodBadAssessment()
```


-------------------------------

Updated on 2021-12-07 at 17:10:23 +0000