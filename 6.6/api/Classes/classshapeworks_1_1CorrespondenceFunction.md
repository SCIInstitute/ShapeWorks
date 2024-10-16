---
title: shapeworks::CorrespondenceFunction
summary: Correspondence term. 

---

# shapeworks::CorrespondenceFunction



Correspondence term. 


`#include <CorrespondenceFunction.h>`

Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) | **[Self](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-constpointer)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Superclass](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-superclass)**  |
| typedef ShapeDataType::DataType | **[DataType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-datatype)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[SetShapeData](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setshapedata)**(ShapeDataType * s)<br>Access the shape matrix.  |
| ShapeDataType * | **[GetShapeData](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getshapedata)**() |
| const ShapeDataType * | **[GetShapeData](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getshapedata)**() const |
| void | **[SetShapeGradient](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setshapegradient)**(ShapeGradientType * s)<br>Access the shape gradient matrix.  |
| ShapeGradientType * | **[GetShapeGradient](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getshapegradient)**() |
| const ShapeGradientType * | **[GetShapeGradient](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getshapegradient)**() const |
| virtual [VectorType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual [VectorType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const |
| virtual double | **[Energy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) const |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-afteriteration)**() |
| void | **[SetMinimumVarianceDecay](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setminimumvariancedecay)**(double initial_value, double final_value, double time_period) |
| void | **[SetMinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setminimumvariance)**(double d) |
| double | **[GetMinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getminimumvariance)**() const |
| bool | **[GetHoldMinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getholdminimumvariance)**() const |
| void | **[SetHoldMinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setholdminimumvariance)**(bool b) |
| void | **[SetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setrecomputecovarianceinterval)**(int i) |
| int | **[GetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getrecomputecovarianceinterval)**() const |
| void | **[SetAttributeScales](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setdomainspershape)**(int i) |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-getdomainspershape)**() const |
| void | **[SetAttributesPerDomain](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setattributesperdomain)**(const std::vector< int > & i) |
| void | **[UseMeanEnergy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-usemeanenergy)**() |
| void | **[UseEntropy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-useentropy)**() |
| void | **[SetXYZ](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setxyz)**(int i, bool val) |
| void | **[SetNormals](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-setnormals)**(int i, bool val) |
| bool | **[CheckForNans](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-checkfornans)**(vnl_matrix_type mat) |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-correspondencefunction)**() |
| virtual | **[~CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-~correspondencefunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-operator=)**(const [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) & ) |
| | **[CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-correspondencefunction)**(const [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) & ) |
| virtual void | **[ComputeUpdates](../Classes/classshapeworks_1_1CorrespondenceFunction.md#function-computeupdates)**(const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| itkTypeMacro([CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md), [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)) typedef shapeworks typedef [shapeworks::ShapeGradientMatrix](../Classes/classshapeworks_1_1ShapeGradientMatrix.md) | **[ShapeGradientType](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-shapegradienttype)**  |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeDataType::Pointer | **[m_ShapeData](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-shapedata)**  |
| ShapeGradientType::Pointer | **[m_ShapeGradient](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-shapegradient)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_PointsUpdate](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-pointsupdate)**  |
| double | **[m_MinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-minimumvariance)**  |
| double | **[m_MinimumEigenValue](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-minimumeigenvalue)**  |
| bool | **[m_HoldMinimumVariance](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-holdminimumvariance)**  |
| int | **[m_RecomputeCovarianceInterval](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-recomputecovarianceinterval)**  |
| double | **[m_MinimumVarianceDecayConstant](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-minimumvariancedecayconstant)**  |
| int | **[m_Counter](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-counter)**  |
| std::vector< double > | **[m_AttributeScales](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-attributescales)**  |
| int | **[m_DomainsPerShape](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-domainspershape)**  |
| std::vector< int > | **[m_AttributesPerDomain](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-attributesperdomain)**  |
| double | **[m_CurrentEnergy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-currentenergy)**  |
| bool | **[m_UseMeanEnergy](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-usemeanenergy)**  |
| std::vector< bool > | **[m_UseXYZ](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-usexyz)**  |
| std::vector< bool > | **[m_UseNormals](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-usenormals)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_points_mean](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-points-mean)**  |
| std::shared_ptr< Eigen::MatrixXd > | **[m_InverseCovMatrix](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-m-inversecovmatrix)**  |
| int | **[num_dims](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-num-dims)**  |
| int | **[num_samples](../Classes/classshapeworks_1_1CorrespondenceFunction.md#variable-num-samples)**  |

## Additional inherited members

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1VectorFunction.md#function-itktypemacro)**([VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) , LightObject ) |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)**() |
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


## Public Types Documentation

### typedef Self

```cpp
typedef CorrespondenceFunction shapeworks::CorrespondenceFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::CorrespondenceFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::CorrespondenceFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef VectorFunction shapeworks::CorrespondenceFunction::Superclass;
```


### typedef DataType

```cpp
typedef ShapeDataType::DataType shapeworks::CorrespondenceFunction::DataType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::CorrespondenceFunction::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystem::PointType shapeworks::CorrespondenceFunction::PointType;
```


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> shapeworks::CorrespondenceFunction::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> shapeworks::CorrespondenceFunction::vnl_matrix_type;
```


## Public Functions Documentation

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
    const ParticleSystem * ,
    double & ,
    double & 
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * ,
    double & maxtimestep
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function Energy

```cpp
inline virtual double Energy(
    unsigned int a,
    unsigned int b,
    const ParticleSystem * c
) const
```


**Reimplements**: [shapeworks::VectorFunction::Energy](../Classes/classshapeworks_1_1VectorFunction.md#function-energy)


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [shapeworks::VectorFunction::BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)


Called before each iteration of a solver. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [shapeworks::VectorFunction::AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)


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
inline virtual VectorFunction::Pointer Clone()
```


**Reimplements**: [shapeworks::VectorFunction::Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)


## Protected Functions Documentation

### function CorrespondenceFunction

```cpp
inline CorrespondenceFunction()
```


### function ~CorrespondenceFunction

```cpp
inline virtual ~CorrespondenceFunction()
```


### function operator=

```cpp
void operator=(
    const CorrespondenceFunction & 
)
```


### function CorrespondenceFunction

```cpp
CorrespondenceFunction(
    const CorrespondenceFunction & 
)
```


### function ComputeUpdates

```cpp
virtual void ComputeUpdates(
    const ParticleSystem * c
)
```


## Public Attributes Documentation

### variable ShapeGradientType

```cpp
itkTypeMacro(CorrespondenceFunction, VectorFunction) typedef shapeworks typedef shapeworks::ShapeGradientMatrix ShapeGradientType;
```


### variable VDimension

```cpp
static constexpr static int VDimension = 3;
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
std::shared_ptr< Eigen::MatrixXd > m_InverseCovMatrix;
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

Updated on 2024-10-16 at 07:18:26 +0000