---
title: shapeworks::LegacyCorrespondenceFunction

---

# shapeworks::LegacyCorrespondenceFunction





Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) | **[Self](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-constpointer)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Superclass](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-superclass)**  |
| typedef [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) | **[ShapeMatrixType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-shapematrixtype)**  |
| typedef [ShapeMatrixType::DataType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-datatype) | **[DataType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-datatype)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-vectortype)**  |
| typedef [ParticleSystem::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-itktypemacro)**([LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) , [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & , double & ) const |
| virtual [VectorType](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-evaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * , double & maxtimestep) const |
| virtual double | **[Energy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-energy)**(unsigned int a, unsigned int b, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * c) const |
| void | **[WriteModes](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-writemodes)**(const std::string & , int ) const |
| void | **[SetShapeMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-setshapematrix)**([ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * s) |
| [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getshapematrix)**() |
| const [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getshapematrix)**() const |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-beforeiteration)**() |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-afteriteration)**() |
| void | **[SetMinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-setminimumvariance)**(double d) |
| double | **[GetMinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getminimumvariance)**() const |
| void | **[SetMinimumVarianceDecay](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-setminimumvariancedecay)**(double initial_value, double final_value, double time_period) |
| bool | **[GetMinimumVarianceDecayConstant](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getminimumvariancedecayconstant)**() const |
| void | **[PrintShapeMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-printshapematrix)**() |
| void | **[UseMeanEnergy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-usemeanenergy)**() |
| void | **[UseEntropy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-useentropy)**() |
| bool | **[GetHoldMinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getholdminimumvariance)**() const |
| void | **[SetHoldMinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-setholdminimumvariance)**(bool b) |
| void | **[SetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-setrecomputecovarianceinterval)**(int i) |
| int | **[GetRecomputeCovarianceInterval](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-getrecomputecovarianceinterval)**() const |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-legacycorrespondencefunction)**() |
| virtual | **[~LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-~legacycorrespondencefunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-operator=)**(const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) & ) |
| | **[LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-legacycorrespondencefunction)**(const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) & ) |
| virtual void | **[ComputeCovarianceMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#function-computecovariancematrix)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeMatrixType::Pointer | **[m_ShapeMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-shapematrix)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_PointsUpdate](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-pointsupdate)**  |
| double | **[m_MinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-minimumvariance)**  |
| double | **[m_MinimumEigenValue](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-minimumeigenvalue)**  |
| double | **[m_CurrentEnergy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-currentenergy)**  |
| bool | **[m_HoldMinimumVariance](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-holdminimumvariance)**  |
| double | **[m_MinimumVarianceDecayConstant](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-minimumvariancedecayconstant)**  |
| int | **[m_RecomputeCovarianceInterval](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-recomputecovarianceinterval)**  |
| int | **[m_Counter](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-counter)**  |
| bool | **[m_UseMeanEnergy](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-usemeanenergy)**  |
| std::shared_ptr< vnl_matrix_type > | **[m_points_mean](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-points-mean)**  |
| std::shared_ptr< Eigen::MatrixXd > | **[m_InverseCovMatrix](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md#variable-m-inversecovmatrix)**  |

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


## Public Types Documentation

### typedef Self

```cpp
typedef LegacyCorrespondenceFunction shapeworks::LegacyCorrespondenceFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::LegacyCorrespondenceFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::LegacyCorrespondenceFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef VectorFunction shapeworks::LegacyCorrespondenceFunction::Superclass;
```


### typedef ShapeMatrixType

```cpp
typedef LegacyShapeMatrix shapeworks::LegacyCorrespondenceFunction::ShapeMatrixType;
```


### typedef DataType

```cpp
typedef ShapeMatrixType::DataType shapeworks::LegacyCorrespondenceFunction::DataType;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::LegacyCorrespondenceFunction::VectorType;
```


Vector & Point types. 


### typedef PointType

```cpp
typedef ParticleSystem::PointType shapeworks::LegacyCorrespondenceFunction::PointType;
```


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> shapeworks::LegacyCorrespondenceFunction::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> shapeworks::LegacyCorrespondenceFunction::vnl_matrix_type;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    LegacyCorrespondenceFunction ,
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

### function LegacyCorrespondenceFunction

```cpp
inline LegacyCorrespondenceFunction()
```


### function ~LegacyCorrespondenceFunction

```cpp
inline virtual ~LegacyCorrespondenceFunction()
```


### function operator=

```cpp
void operator=(
    const LegacyCorrespondenceFunction & 
)
```


### function LegacyCorrespondenceFunction

```cpp
LegacyCorrespondenceFunction(
    const LegacyCorrespondenceFunction & 
)
```


### function ComputeCovarianceMatrix

```cpp
virtual void ComputeCovarianceMatrix()
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
std::shared_ptr< Eigen::MatrixXd > m_InverseCovMatrix;
```


-------------------------------

Updated on 2025-03-09 at 20:10:14 +0000