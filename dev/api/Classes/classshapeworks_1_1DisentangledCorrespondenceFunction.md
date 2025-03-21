---
title: shapeworks::DisentangledCorrespondenceFunction

---

# shapeworks::DisentangledCorrespondenceFunction



 [More...](#detailed-description)


`#include <DisentangledCorrespondenceFunction.h>`

Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) | **[Self](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-constpointer)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Superclass](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-superclass)**  |
| typedef [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) | **[ShapeMatrixType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-shapematrixtype)**  |
| typedef [ShapeMatrixType::DataType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-datatype) | **[DataType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-datatype)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-vnl-matrix-type)**  |
| typedef std::shared_ptr< std::vector< vnl_matrix_type > > | **[shared_vnl_matrix_array_type](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-shared-vnl-matrix-array-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-itktypemacro)**([DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) , [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual [VectorType](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const |
| virtual double | **[Energy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) const |
| void | **[WriteModes](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-writemodes)**(const std::string & , int ) const |
| void | **[SetShapeMatrix](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-setshapematrix)**([ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * s) |
| [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getshapematrix)**() |
| const [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getshapematrix)**() const |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-afteriteration)**() |
| void | **[SetMinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-setminimumvariance)**(double d) |
| double | **[GetMinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getminimumvariance)**() const |
| void | **[SetMinimumVarianceDecay](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-setminimumvariancedecay)**(double initial_value, double final_value, double time_period) |
| bool | **[GetMinimumVarianceDecayConstant](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getminimumvariancedecayconstant)**() const |
| void | **[PrintShapeMatrix](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-printshapematrix)**() |
| void | **[UseMeanEnergy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-usemeanenergy)**() |
| void | **[UseEntropy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-useentropy)**() |
| bool | **[GetHoldMinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getholdminimumvariance)**() const |
| void | **[SetHoldMinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-setholdminimumvariance)**(bool b) |
| void | **[SetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-setrecomputecovarianceinterval)**(int i) |
| int | **[GetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-getrecomputecovarianceinterval)**() const |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-disentangledcorrespondencefunction)**() |
| virtual | **[~DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-~disentangledcorrespondencefunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-operator=)**(const [DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) & ) |
| | **[DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-disentangledcorrespondencefunction)**(const [DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) & ) |
| virtual void | **[ComputeCovarianceMatrices](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-computecovariancematrices)**() |
| void | **[Initialize](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#function-initialize)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeMatrixType::Pointer | **[m_ShapeMatrix](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-shapematrix)**  |
| double | **[m_MinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-minimumvariance)**  |
| double | **[m_MinimumEigenValue](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-minimumeigenvalue)**  |
| std::vector< double > | **[m_MinimumEigenValue_time_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-minimumeigenvalue-time-cohort)**  |
| std::vector< double > | **[m_MinimumEigenValue_shape_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-minimumeigenvalue-shape-cohort)**  |
| double | **[m_CurrentEnergy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-currentenergy)**  |
| bool | **[m_HoldMinimumVariance](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-holdminimumvariance)**  |
| double | **[m_MinimumVarianceDecayConstant](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-minimumvariancedecayconstant)**  |
| int | **[m_RecomputeCovarianceInterval](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-recomputecovarianceinterval)**  |
| int | **[m_Counter](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-counter)**  |
| bool | **[m_UseMeanEnergy](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-usemeanenergy)**  |
| std::shared_ptr< std::vector< Eigen::MatrixXd > > | **[m_InverseCovMatrices_time_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-inversecovmatrices-time-cohort)**  |
| std::shared_ptr< std::vector< Eigen::MatrixXd > > | **[m_InverseCovMatrices_shape_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-inversecovmatrices-shape-cohort)**  |
| shared_vnl_matrix_array_type | **[m_points_mean_time_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-points-mean-time-cohort)**  |
| shared_vnl_matrix_array_type | **[m_points_mean_shape_cohort](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-points-mean-shape-cohort)**  |
| shared_vnl_matrix_array_type | **[m_Time_PointsUpdate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-time-pointsupdate)**  |
| shared_vnl_matrix_array_type | **[m_Shape_PointsUpdate](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md#variable-m-shape-pointsupdate)**  |

## Additional inherited members

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
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


## Detailed Description

```cpp
class shapeworks::DisentangledCorrespondenceFunction;
```


This class implements the Correspondence Term where the entropy computation is disentangled across time and shape domain, and builds a Sapatiotemporal SSM. 

## Public Types Documentation

### typedef Self

```cpp
typedef DisentangledCorrespondenceFunction shapeworks::DisentangledCorrespondenceFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::DisentangledCorrespondenceFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::DisentangledCorrespondenceFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef VectorFunction shapeworks::DisentangledCorrespondenceFunction::Superclass;
```


### typedef ShapeMatrixType

```cpp
typedef LegacyShapeMatrix shapeworks::DisentangledCorrespondenceFunction::ShapeMatrixType;
```


### typedef DataType

```cpp
typedef ShapeMatrixType::DataType shapeworks::DisentangledCorrespondenceFunction::DataType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::DisentangledCorrespondenceFunction::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystem::PointType shapeworks::DisentangledCorrespondenceFunction::PointType;
```


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> shapeworks::DisentangledCorrespondenceFunction::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> shapeworks::DisentangledCorrespondenceFunction::vnl_matrix_type;
```


### typedef shared_vnl_matrix_array_type

```cpp
typedef std::shared_ptr<std::vector<vnl_matrix_type> > shapeworks::DisentangledCorrespondenceFunction::shared_vnl_matrix_array_type;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    DisentangledCorrespondenceFunction ,
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


### function WriteModes

```cpp
void WriteModes(
    const std::string & ,
    int 
) const
```


Write the first n modes to +- 3 std dev and the mean of the model described by the covariance matrix. The string argument is a prefix to the file names. 


### function SetShapeMatrix

```cpp
inline void SetShapeMatrix(
    ShapeMatrixType * s
)
```


Access the shape matrix. 


### function GetShapeMatrix

```cpp
inline ShapeMatrixType * GetShapeMatrix()
```


### function GetShapeMatrix

```cpp
inline const ShapeMatrixType * GetShapeMatrix() const
```


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


### function SetMinimumVarianceDecay

```cpp
inline void SetMinimumVarianceDecay(
    double initial_value,
    double final_value,
    double time_period
)
```


### function GetMinimumVarianceDecayConstant

```cpp
inline bool GetMinimumVarianceDecayConstant() const
```


### function PrintShapeMatrix

```cpp
inline void PrintShapeMatrix()
```


### function UseMeanEnergy

```cpp
inline void UseMeanEnergy()
```


### function UseEntropy

```cpp
inline void UseEntropy()
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


### function Clone

```cpp
inline virtual VectorFunction::Pointer Clone()
```


**Reimplements**: [shapeworks::VectorFunction::Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)


## Protected Functions Documentation

### function DisentangledCorrespondenceFunction

```cpp
inline DisentangledCorrespondenceFunction()
```


### function ~DisentangledCorrespondenceFunction

```cpp
inline virtual ~DisentangledCorrespondenceFunction()
```


### function operator=

```cpp
void operator=(
    const DisentangledCorrespondenceFunction & 
)
```


### function DisentangledCorrespondenceFunction

```cpp
DisentangledCorrespondenceFunction(
    const DisentangledCorrespondenceFunction & 
)
```


### function ComputeCovarianceMatrices

```cpp
virtual void ComputeCovarianceMatrices()
```


### function Initialize

```cpp
inline void Initialize()
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


## Protected Attributes Documentation

### variable m_ShapeMatrix

```cpp
ShapeMatrixType::Pointer m_ShapeMatrix;
```


### variable m_MinimumVariance

```cpp
double m_MinimumVariance;
```


### variable m_MinimumEigenValue

```cpp
double m_MinimumEigenValue;
```


### variable m_MinimumEigenValue_time_cohort

```cpp
std::vector< double > m_MinimumEigenValue_time_cohort;
```


### variable m_MinimumEigenValue_shape_cohort

```cpp
std::vector< double > m_MinimumEigenValue_shape_cohort;
```


### variable m_CurrentEnergy

```cpp
double m_CurrentEnergy;
```


### variable m_HoldMinimumVariance

```cpp
bool m_HoldMinimumVariance;
```


### variable m_MinimumVarianceDecayConstant

```cpp
double m_MinimumVarianceDecayConstant;
```


### variable m_RecomputeCovarianceInterval

```cpp
int m_RecomputeCovarianceInterval;
```


### variable m_Counter

```cpp
int m_Counter;
```


### variable m_UseMeanEnergy

```cpp
bool m_UseMeanEnergy;
```


### variable m_InverseCovMatrices_time_cohort

```cpp
std::shared_ptr< std::vector< Eigen::MatrixXd > > m_InverseCovMatrices_time_cohort;
```


### variable m_InverseCovMatrices_shape_cohort

```cpp
std::shared_ptr< std::vector< Eigen::MatrixXd > > m_InverseCovMatrices_shape_cohort;
```


### variable m_points_mean_time_cohort

```cpp
shared_vnl_matrix_array_type m_points_mean_time_cohort;
```


### variable m_points_mean_shape_cohort

```cpp
shared_vnl_matrix_array_type m_points_mean_shape_cohort;
```


### variable m_Time_PointsUpdate

```cpp
shared_vnl_matrix_array_type m_Time_PointsUpdate;
```


### variable m_Shape_PointsUpdate

```cpp
shared_vnl_matrix_array_type m_Shape_PointsUpdate;
```


-------------------------------

Updated on 2025-03-21 at 16:07:30 +0000