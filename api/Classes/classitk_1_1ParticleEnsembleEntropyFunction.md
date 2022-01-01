---
title: itk::ParticleEnsembleEntropyFunction

---

# itk::ParticleEnsembleEntropyFunction



 [More...](#detailed-description)

Inherits from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md) | **[Self](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-constpointer)**  |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-superclass)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-particlesystemtype)**  |
| typedef [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)< double, VDimension > | **[ShapeMatrixType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-shapematrixtype)**  |
| typedef [ShapeMatrixType::DataType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-datatype) | **[DataType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-datatype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-itktypemacro)**([ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md) , [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-particlesystemtype) * , double & , double & ) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-particlesystemtype) * , double & maxtimestep) const |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystemType](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#typedef-particlesystemtype) * c) const |
| void | **[WriteModes](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-writemodes)**(const std::string & , int ) const |
| void | **[SetShapeMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-setshapematrix)**([ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * s) |
| [ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * | **[GetShapeMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getshapematrix)**() |
| const [ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * | **[GetShapeMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getshapematrix)**() const |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-afteriteration)**() |
| void | **[SetMinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-setminimumvariance)**(double d) |
| double | **[GetMinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getminimumvariance)**() const |
| void | **[SetMinimumVarianceDecay](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-setminimumvariancedecay)**(double initial_value, double final_value, double time_period) |
| bool | **[GetMinimumVarianceDecayConstant](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getminimumvariancedecayconstant)**() const |
| void | **[PrintShapeMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-printshapematrix)**() |
| void | **[UseMeanEnergy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-usemeanenergy)**() |
| void | **[UseEntropy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-useentropy)**() |
| bool | **[GetHoldMinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getholdminimumvariance)**() const |
| void | **[SetHoldMinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-setholdminimumvariance)**(bool b) |
| void | **[SetRecomputeCovarianceInterval](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-setrecomputecovarianceinterval)**(int i) |
| int | **[GetRecomputeCovarianceInterval](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-getrecomputecovarianceinterval)**() const |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-particleensembleentropyfunction)**() |
| virtual | **[~ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-~particleensembleentropyfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-operator=)**(const [ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md) & ) |
| | **[ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-particleensembleentropyfunction)**(const [ParticleEnsembleEntropyFunction](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md) & ) |
| virtual void | **[ComputeCovarianceMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#function-computecovariancematrix)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeMatrixType::Pointer | **[m_ShapeMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-shapematrix)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_PointsUpdate](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-pointsupdate)**  |
| double | **[m_MinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-minimumvariance)**  |
| double | **[m_MinimumEigenValue](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-minimumeigenvalue)**  |
| double | **[m_CurrentEnergy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-currentenergy)**  |
| bool | **[m_HoldMinimumVariance](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-holdminimumvariance)**  |
| double | **[m_MinimumVarianceDecayConstant](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-minimumvariancedecayconstant)**  |
| int | **[m_RecomputeCovarianceInterval](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-recomputecovarianceinterval)**  |
| int | **[m_Counter](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-counter)**  |
| bool | **[m_UseMeanEnergy](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-usemeanenergy)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_points_mean](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-points-mean)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_InverseCovMatrix](../Classes/classitk_1_1ParticleEnsembleEntropyFunction.md#variable-m-inversecovmatrix)**  |

## Additional inherited members

**Public Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
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
class itk::ParticleEnsembleEntropyFunction;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleEnsembleEntropyFunction itk::ParticleEnsembleEntropyFunction< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleEnsembleEntropyFunction< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleEnsembleEntropyFunction< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleVectorFunction<VDimension> itk::ParticleEnsembleEntropyFunction< VDimension >::Superclass;
```


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleEnsembleEntropyFunction< VDimension >::ParticleSystemType;
```


Type of particle system. 


### typedef ShapeMatrixType

```cpp
typedef ParticleShapeMatrixAttribute<double, VDimension> itk::ParticleEnsembleEntropyFunction< VDimension >::ShapeMatrixType;
```


### typedef DataType

```cpp
typedef ShapeMatrixType::DataType itk::ParticleEnsembleEntropyFunction< VDimension >::DataType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleEnsembleEntropyFunction< VDimension >::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleEnsembleEntropyFunction< VDimension >::PointType;
```


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> itk::ParticleEnsembleEntropyFunction< VDimension >::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> itk::ParticleEnsembleEntropyFunction< VDimension >::vnl_matrix_type;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleEnsembleEntropyFunction ,
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


**Reimplements**: [itk::ParticleVectorFunction::BeforeIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeiteration)


Called before each iteration of a solver. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [itk::ParticleVectorFunction::AfterIteration](../Classes/classitk_1_1ParticleVectorFunction.md#function-afteriteration)


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
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleVectorFunction::Clone](../Classes/classitk_1_1ParticleVectorFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleEnsembleEntropyFunction

```cpp
inline ParticleEnsembleEntropyFunction()
```


### function ~ParticleEnsembleEntropyFunction

```cpp
inline virtual ~ParticleEnsembleEntropyFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleEnsembleEntropyFunction & 
)
```


### function ParticleEnsembleEntropyFunction

```cpp
ParticleEnsembleEntropyFunction(
    const ParticleEnsembleEntropyFunction & 
)
```


### function ComputeCovarianceMatrix

```cpp
virtual void ComputeCovarianceMatrix()
```


## Protected Attributes Documentation

### variable m_ShapeMatrix

```cpp
ShapeMatrixType::Pointer m_ShapeMatrix;
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


### variable m_points_mean

```cpp
std::shared_ptr< vnl_matrix_type > m_points_mean;
```


### variable m_InverseCovMatrix

```cpp
std::shared_ptr< vnl_matrix_type > m_InverseCovMatrix;
```


-------------------------------

Updated on 2022-01-01 at 17:29:12 +0000