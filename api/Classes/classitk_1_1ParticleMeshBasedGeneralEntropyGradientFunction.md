---
title: itk::ParticleMeshBasedGeneralEntropyGradientFunction

---

# itk::ParticleMeshBasedGeneralEntropyGradientFunction



 [More...](#detailed-description)

Inherits from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md) | **[Self](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-constpointer)**  |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-superclass)**  |
| typedef [ParticleGeneralShapeGradientMatrix](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md)< double, VDimension > | **[ShapeGradientType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-shapegradienttype)**  |
| typedef ShapeDataType::DataType | **[DataType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-datatype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-self) ) |
| void | **[SetShapeData](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setshapedata)**([ShapeDataType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-shapedatatype) * s) |
| [ShapeDataType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-shapedatatype) * | **[GetShapeData](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getshapedata)**() |
| const [ShapeDataType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-shapedatatype) * | **[GetShapeData](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getshapedata)**() const |
| void | **[SetShapeGradient](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setshapegradient)**([ShapeGradientType](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md) * s) |
| [ShapeGradientType](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md) * | **[GetShapeGradient](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getshapegradient)**() |
| const [ShapeGradientType](../Classes/classitk_1_1ParticleGeneralShapeGradientMatrix.md) * | **[GetShapeGradient](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getshapegradient)**() const |
| virtual [VectorType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * , double & , double & ) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * , double & maxtimestep) const |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * c) const |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-afteriteration)**() |
| void | **[SetMinimumVarianceDecay](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setminimumvariancedecay)**(double initial_value, double final_value, double time_period) |
| void | **[SetMinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setminimumvariance)**(double d) |
| double | **[GetMinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getminimumvariance)**() const |
| bool | **[GetHoldMinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getholdminimumvariance)**() const |
| void | **[SetHoldMinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setholdminimumvariance)**(bool b) |
| void | **[SetRecomputeCovarianceInterval](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setrecomputecovarianceinterval)**(int i) |
| int | **[GetRecomputeCovarianceInterval](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getrecomputecovarianceinterval)**() const |
| void | **[SetAttributeScales](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetDomainsPerShape](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-getdomainspershape)**() const |
| void | **[SetAttributesPerDomain](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setattributesperdomain)**(const std::vector< int > & i) |
| void | **[UseMeanEnergy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-usemeanenergy)**() |
| void | **[UseEntropy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-useentropy)**() |
| void | **[SetXYZ](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setxyz)**(int i, bool val) |
| void | **[SetNormals](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-setnormals)**(int i, bool val) |
| bool | **[CheckForNans](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-checkfornans)**(vnl_matrix_type mat) |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-particlemeshbasedgeneralentropygradientfunction)**() |
| virtual | **[~ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-~particlemeshbasedgeneralentropygradientfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-operator=)**(const [ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md) & ) |
| | **[ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-particlemeshbasedgeneralentropygradientfunction)**(const [ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md) & ) |
| virtual void | **[ComputeUpdates](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#function-computeupdates)**(const [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * c) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| itkTypeMacro([ParticleMeshBasedGeneralEntropyGradientFunction](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md), [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)) typedef typename [Superclass](../Classes/classitk_1_1ParticleVectorFunction.md) typedef [ParticleGeneralShapeMatrix](../Classes/classitk_1_1ParticleGeneralShapeMatrix.md)< double, VDimension > | **[ShapeDataType](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-shapedatatype)**  |
| unsigned | **[int](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-int)**  |
| unsigned | **[VDimension](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeDataType::Pointer | **[m_ShapeData](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-shapedata)**  |
| ShapeGradientType::Pointer | **[m_ShapeGradient](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-shapegradient)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_PointsUpdate](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-pointsupdate)**  |
| double | **[m_MinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-minimumvariance)**  |
| double | **[m_MinimumEigenValue](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-minimumeigenvalue)**  |
| bool | **[m_HoldMinimumVariance](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-holdminimumvariance)**  |
| int | **[m_RecomputeCovarianceInterval](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-recomputecovarianceinterval)**  |
| double | **[m_MinimumVarianceDecayConstant](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-minimumvariancedecayconstant)**  |
| int | **[m_Counter](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-counter)**  |
| std::vector< double > | **[m_AttributeScales](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-attributescales)**  |
| int | **[m_DomainsPerShape](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-domainspershape)**  |
| std::vector< int > | **[m_AttributesPerDomain](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-attributesperdomain)**  |
| double | **[m_CurrentEnergy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-currentenergy)**  |
| bool | **[m_UseMeanEnergy](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-usemeanenergy)**  |
| std::vector< bool > | **[m_UseXYZ](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-usexyz)**  |
| std::vector< bool > | **[m_UseNormals](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-usenormals)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_points_mean](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-points-mean)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_InverseCovMatrix](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-m-inversecovmatrix)**  |
| int | **[num_dims](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-num-dims)**  |
| int | **[num_samples](../Classes/classitk_1_1ParticleMeshBasedGeneralEntropyGradientFunction.md#variable-num-samples)**  |

## Additional inherited members

**Public Types inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype)**  |

**Public Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleVectorFunction.md#function-itktypemacro)**([ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) , LightObject ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleVectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleVectorFunction.md#function-resetbuffers)**() |
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
template <unsigned int VDimension>
class itk::ParticleMeshBasedGeneralEntropyGradientFunction;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleMeshBasedGeneralEntropyGradientFunction itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleVectorFunction<VDimension> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::Superclass;
```


### typedef ShapeGradientType

```cpp
typedef ParticleGeneralShapeGradientMatrix<double, VDimension> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::ShapeGradientType;
```


### typedef DataType

```cpp
typedef ShapeDataType::DataType itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::DataType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::PointType;
```


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> itk::ParticleMeshBasedGeneralEntropyGradientFunction< VDimension >::vnl_matrix_type;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. Dimensionality of the domain of the particle system. 


### function SetShapeData

```cpp
inline void SetShapeData(
    ShapeDataType * s
)
```


Access the shape matrix. 


### function GetShapeData

```cpp
inline ShapeDataType * GetShapeData()
```


### function GetShapeData

```cpp
inline const ShapeDataType * GetShapeData() const
```


### function SetShapeGradient

```cpp
inline void SetShapeGradient(
    ShapeGradientType * s
)
```


Access the shape gradient matrix. 


### function GetShapeGradient

```cpp
inline ShapeGradientType * GetShapeGradient()
```


### function GetShapeGradient

```cpp
inline const ShapeGradientType * GetShapeGradient() const
```


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


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * ,
    double & maxtimestep
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystemType * c
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Energy](../Classes/classitk_1_1ParticleVectorFunction.md#function-energy)


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [itk::ParticleVectorFunction::BeforeIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeiteration)


Called before each iteration of a solver. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [itk::ParticleVectorFunction::AfterIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-afteriteration)


Called after each iteration of the solver. 


### function SetMinimumVarianceDecay

```cpp
inline void SetMinimumVarianceDecay(
    double initial_value,
    double final_value,
    double time_period
)
```


### function SetMinimumVariance

```cpp
inline void SetMinimumVariance(
    double d
)
```


### function GetMinimumVariance

```cpp
inline double GetMinimumVariance() const
```


### function GetHoldMinimumVariance

```cpp
inline bool GetHoldMinimumVariance() const
```


### function SetHoldMinimumVariance

```cpp
inline void SetHoldMinimumVariance(
    bool b
)
```


### function SetRecomputeCovarianceInterval

```cpp
inline void SetRecomputeCovarianceInterval(
    int i
)
```


### function GetRecomputeCovarianceInterval

```cpp
inline int GetRecomputeCovarianceInterval() const
```


### function SetAttributeScales

```cpp
inline void SetAttributeScales(
    const std::vector< double > & s
)
```


### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    int i
)
```


Set/Get the number of domains per shape. 


### function GetDomainsPerShape

```cpp
inline int GetDomainsPerShape() const
```


### function SetAttributesPerDomain

```cpp
inline void SetAttributesPerDomain(
    const std::vector< int > & i
)
```


### function UseMeanEnergy

```cpp
inline void UseMeanEnergy()
```


### function UseEntropy

```cpp
inline void UseEntropy()
```


### function SetXYZ

```cpp
inline void SetXYZ(
    int i,
    bool val
)
```


### function SetNormals

```cpp
inline void SetNormals(
    int i,
    bool val
)
```


### function CheckForNans

```cpp
inline bool CheckForNans(
    vnl_matrix_type mat
)
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleVectorFunction::Clone](../Classes/classitk_1_1ParticleVectorFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleMeshBasedGeneralEntropyGradientFunction

```cpp
inline ParticleMeshBasedGeneralEntropyGradientFunction()
```


### function ~ParticleMeshBasedGeneralEntropyGradientFunction

```cpp
inline virtual ~ParticleMeshBasedGeneralEntropyGradientFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleMeshBasedGeneralEntropyGradientFunction & 
)
```


### function ParticleMeshBasedGeneralEntropyGradientFunction

```cpp
ParticleMeshBasedGeneralEntropyGradientFunction(
    const ParticleMeshBasedGeneralEntropyGradientFunction & 
)
```


### function ComputeUpdates

```cpp
virtual void ComputeUpdates(
    const ParticleSystemType * c
)
```


## Public Attributes Documentation

### variable ShapeDataType

```cpp
itkTypeMacro(ParticleMeshBasedGeneralEntropyGradientFunction, ParticleVectorFunction) typedef typename Superclass typedef ParticleGeneralShapeMatrix< double, VDimension > ShapeDataType;
```


Type of particle system. 


### variable int

```cpp
unsigned int;
```


### variable VDimension

```cpp
unsigned VDimension;
```


## Protected Attributes Documentation

### variable m_ShapeData

```cpp
ShapeDataType::Pointer m_ShapeData;
```


### variable m_ShapeGradient

```cpp
ShapeGradientType::Pointer m_ShapeGradient;
```


### variable m_PointsUpdate

```cpp
std::shared_ptr< vnl_matrix_type > m_PointsUpdate;
```


### variable m_MinimumVariance

```cpp
double m_MinimumVariance;
```


### variable m_MinimumEigenValue

```cpp
double m_MinimumEigenValue;
```


### variable m_HoldMinimumVariance

```cpp
bool m_HoldMinimumVariance;
```


### variable m_RecomputeCovarianceInterval

```cpp
int m_RecomputeCovarianceInterval;
```


### variable m_MinimumVarianceDecayConstant

```cpp
double m_MinimumVarianceDecayConstant;
```


### variable m_Counter

```cpp
int m_Counter;
```


### variable m_AttributeScales

```cpp
std::vector< double > m_AttributeScales;
```


### variable m_DomainsPerShape

```cpp
int m_DomainsPerShape;
```


### variable m_AttributesPerDomain

```cpp
std::vector< int > m_AttributesPerDomain;
```


### variable m_CurrentEnergy

```cpp
double m_CurrentEnergy;
```


### variable m_UseMeanEnergy

```cpp
bool m_UseMeanEnergy;
```


### variable m_UseXYZ

```cpp
std::vector< bool > m_UseXYZ;
```


### variable m_UseNormals

```cpp
std::vector< bool > m_UseNormals;
```


### variable m_points_mean

```cpp
std::shared_ptr< vnl_matrix_type > m_points_mean;
```


### variable m_InverseCovMatrix

```cpp
std::shared_ptr< vnl_matrix_type > m_InverseCovMatrix;
```


### variable num_dims

```cpp
int num_dims;
```


### variable num_samples

```cpp
int num_samples;
```


-------------------------------

Updated on 2022-03-05 at 23:20:33 +0000