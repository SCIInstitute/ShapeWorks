---
title: shapeworks::GradientDescentOptimizer

---

# shapeworks::GradientDescentOptimizer





Inherits from itk::Object

## Public Types

|                | Name           |
| -------------- | -------------- |
| using float | **[TGradientNumericType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#using-tgradientnumerictype)**  |
| typedef [GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) | **[Self](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-self)**  |
| typedef itk::Object | **[Superclass](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-constweakpointer)**  |
| typedef [shapeworks::ImageDomainWithGradients](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)< TGradientNumericType > | **[DomainType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-domaintype)**  |
| typedef [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[ParticleSystemType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-particlesystemtype)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[GradientFunctionType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-gradientfunctiontype)**  |
| typedef [GradientFunctionType::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-vectortype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itktypemacro)**([ParticleGradientDescentPositionOptimizer](../Classes/classParticleGradientDescentPositionOptimizer.md) , Object ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[StartOptimization](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-startoptimization)**() |
| void | **[StartAdaptiveGaussSeidelOptimization](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-startadaptivegaussseideloptimization)**() |
| void | **[AugmentedLagrangianConstraints](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-augmentedlagrangianconstraints)**([VectorType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-vectortype) & gradient, const [PointType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-pointtype) & pt, const size_t & dom, const double & maximumUpdateAllowed, size_t index) |
| void | **[StopOptimization](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-stopoptimization)**() |
| void | **[AbortProcessing](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-abortprocessing)**() |
| | **[itkGetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetmacro)**(NumberOfIterations , unsigned int ) |
| | **[itkSetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetmacro)**(NumberOfIterations , unsigned int ) |
| void | **[SetVerbosity](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-setverbosity)**(unsigned int val) |
| unsigned int | **[GetVerbosity](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-getverbosity)**() |
| | **[itkGetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetmacro)**(TimeStep , double ) |
| | **[itkSetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetmacro)**(TimeStep , double ) |
| | **[itkGetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetmacro)**(MaximumNumberOfIterations , unsigned int ) |
| | **[itkSetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetmacro)**(MaximumNumberOfIterations , unsigned int ) |
| | **[itkGetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetmacro)**(Tolerance , double ) |
| | **[itkSetMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetmacro)**(Tolerance , double ) |
| | **[itkGetObjectMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetobjectmacro)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) , [ParticleSystemType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-particlesystemtype) ) |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetobjectmacro)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) , [ParticleSystemType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-particlesystemtype) ) |
| | **[itkGetObjectMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itkgetobjectmacro)**(GradientFunction , [GradientFunctionType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-gradientfunctiontype) ) |
| | **[itkSetObjectMacro](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-itksetobjectmacro)**(GradientFunction , [GradientFunctionType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#typedef-gradientfunctiontype) ) |
| void | **[SetInitializationMode](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-setinitializationmode)**(bool b)<br>Determines if this is an initialization (true) or an optimization (false)  |
| void | **[SetCheckIterations](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-setcheckiterations)**(size_t si)<br>Sets the number of iterations when we check for convergence.  |
| void | **[SetInitializationStartScalingFactor](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-setinitializationstartscalingfactor)**(double si)<br>Sets the scaling factor at the beginning of the initialization.  |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-gradientdescentoptimizer)**() |
| | **[GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-gradientdescentoptimizer)**(const [GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) & ) |
| const [GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) & | **[operator=](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-operator=)**(const [GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) & ) |
| void | **[PrintSelf](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#function-~gradientdescentoptimizer)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static unsigned int | **[VDimension](../Classes/classshapeworks_1_1GradientDescentOptimizer.md#variable-vdimension)**  |

## Public Types Documentation

### using TGradientNumericType

```cpp
using shapeworks::GradientDescentOptimizer::TGradientNumericType =  float;
```


### typedef Self

```cpp
typedef GradientDescentOptimizer shapeworks::GradientDescentOptimizer::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef itk::Object shapeworks::GradientDescentOptimizer::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::GradientDescentOptimizer::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::GradientDescentOptimizer::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::GradientDescentOptimizer::ConstWeakPointer;
```


### typedef DomainType

```cpp
typedef shapeworks::ImageDomainWithGradients<TGradientNumericType> shapeworks::GradientDescentOptimizer::DomainType;
```


Type of the domain. 


### typedef ParticleSystemType

```cpp
typedef ParticleSystem shapeworks::GradientDescentOptimizer::ParticleSystemType;
```


Particle System type is based on the dimensionality. 


### typedef GradientFunctionType

```cpp
typedef VectorFunction shapeworks::GradientDescentOptimizer::GradientFunctionType;
```


Type of the gradient function. 


### typedef VectorType

```cpp
typedef GradientFunctionType::VectorType shapeworks::GradientDescentOptimizer::VectorType;
```


Numerical vector type. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType shapeworks::GradientDescentOptimizer::PointType;
```


Point Type 


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleGradientDescentPositionOptimizer ,
    Object 
)
```


Run-time type information (and related methods). 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Dimensionality of the domain of the particle system. 


### function StartOptimization

```cpp
inline void StartOptimization()
```


Start the optimization. 


### function StartAdaptiveGaussSeidelOptimization

```cpp
void StartAdaptiveGaussSeidelOptimization()
```


### function AugmentedLagrangianConstraints

```cpp
void AugmentedLagrangianConstraints(
    VectorType & gradient,
    const PointType & pt,
    const size_t & dom,
    const double & maximumUpdateAllowed,
    size_t index
)
```


### function StopOptimization

```cpp
inline void StopOptimization()
```


Stop the optimization. This method sets a flag that aborts the StartOptimization method after the current iteration. 


### function AbortProcessing

```cpp
inline void AbortProcessing()
```


### function itkGetMacro

```cpp
itkGetMacro(
    NumberOfIterations ,
    unsigned int 
)
```


Get/Set the number of iterations performed by the solver. 


### function itkSetMacro

```cpp
itkSetMacro(
    NumberOfIterations ,
    unsigned int 
)
```


### function SetVerbosity

```cpp
inline void SetVerbosity(
    unsigned int val
)
```


### function GetVerbosity

```cpp
inline unsigned int GetVerbosity()
```


### function itkGetMacro

```cpp
itkGetMacro(
    TimeStep ,
    double 
)
```


Get/Set a time step parameter for the update. Each update is simply scaled by this value. 


### function itkSetMacro

```cpp
itkSetMacro(
    TimeStep ,
    double 
)
```


### function itkGetMacro

```cpp
itkGetMacro(
    MaximumNumberOfIterations ,
    unsigned int 
)
```


Get/Set the maximum iterations to allow this solver to use. 


### function itkSetMacro

```cpp
itkSetMacro(
    MaximumNumberOfIterations ,
    unsigned int 
)
```


### function itkGetMacro

```cpp
itkGetMacro(
    Tolerance ,
    double 
)
```


Get/Set the precision of the solution. 


### function itkSetMacro

```cpp
itkSetMacro(
    Tolerance ,
    double 
)
```


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    ParticleSystem ,
    ParticleSystemType 
)
```


Get/Set the [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) modified by this optimizer. 


### function itkSetObjectMacro

```cpp
itkSetObjectMacro(
    ParticleSystem ,
    ParticleSystemType 
)
```


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    GradientFunction ,
    GradientFunctionType 
)
```


Get/Set the gradient function used by this optimizer. 


### function itkSetObjectMacro

```cpp
itkSetObjectMacro(
    GradientFunction ,
    GradientFunctionType 
)
```


### function SetInitializationMode

```cpp
inline void SetInitializationMode(
    bool b
)
```

Determines if this is an initialization (true) or an optimization (false) 

### function SetCheckIterations

```cpp
inline void SetCheckIterations(
    size_t si
)
```

Sets the number of iterations when we check for convergence. 

### function SetInitializationStartScalingFactor

```cpp
inline void SetInitializationStartScalingFactor(
    double si
)
```

Sets the scaling factor at the beginning of the initialization. 

## Protected Functions Documentation

### function GradientDescentOptimizer

```cpp
GradientDescentOptimizer()
```


### function GradientDescentOptimizer

```cpp
GradientDescentOptimizer(
    const GradientDescentOptimizer & 
)
```


### function operator=

```cpp
const GradientDescentOptimizer & operator=(
    const GradientDescentOptimizer & 
)
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~GradientDescentOptimizer

```cpp
inline virtual ~GradientDescentOptimizer()
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static unsigned int VDimension = 3;
```


-------------------------------

Updated on 2023-07-17 at 17:00:04 +0000