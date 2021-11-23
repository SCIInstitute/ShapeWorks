---
title: itk::ParticleGradientDescentPositionOptimizer

---

# itk::ParticleGradientDescentPositionOptimizer



 [More...](#detailed-description)


`#include <itkParticleGradientDescentPositionOptimizer.h>`

Inherits from Object

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md) | **[Self](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-self)**  |
| typedef Object | **[Superclass](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-constweakpointer)**  |
| typedef [ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)< TGradientNumericType > | **[DomainType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-domaintype)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-particlesystemtype)**  |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > | **[GradientFunctionType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-gradientfunctiontype)**  |
| typedef [GradientFunctionType::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-vectortype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itktypemacro)**([ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md) , Object ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[StartOptimization](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-startoptimization)**() |
| void | **[StartAdaptiveGaussSeidelOptimization](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-startadaptivegaussseideloptimization)**() |
| void | **[AugmentedLagrangianConstraints](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-augmentedlagrangianconstraints)**([VectorType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-vectortype) & gradient, const [PointType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-pointtype) & pt, const size_t & dom, const double & maximumUpdateAllowed) |
| void | **[StopOptimization](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-stopoptimization)**() |
| void | **[AbortProcessing](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-abortprocessing)**() |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetmacro)**(NumberOfIterations , unsigned int ) |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetmacro)**(NumberOfIterations , unsigned int ) |
| void | **[SetVerbosity](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-setverbosity)**(unsigned int val) |
| unsigned int | **[GetVerbosity](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-getverbosity)**() |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetmacro)**(TimeStep , double ) |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetmacro)**(TimeStep , double ) |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetmacro)**(MaximumNumberOfIterations , unsigned int ) |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetmacro)**(MaximumNumberOfIterations , unsigned int ) |
| | **[itkGetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetmacro)**(Tolerance , double ) |
| | **[itkSetMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetmacro)**(Tolerance , double ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetobjectmacro)**([ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) , [ParticleSystemType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-particlesystemtype) ) |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetobjectmacro)**([ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) , [ParticleSystemType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-particlesystemtype) ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itkgetobjectmacro)**(GradientFunction , [GradientFunctionType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-gradientfunctiontype) ) |
| | **[itkSetObjectMacro](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-itksetobjectmacro)**(GradientFunction , [GradientFunctionType](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#typedef-gradientfunctiontype) ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-particlegradientdescentpositionoptimizer)**() |
| | **[ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-particlegradientdescentpositionoptimizer)**(const [ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md) & ) |
| const [ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md) & | **[operator=](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-operator=)**(const [ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md) & ) |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticleGradientDescentPositionOptimizer](../Classes/classitk_1_1ParticleGradientDescentPositionOptimizer.md#function-~particlegradientdescentpositionoptimizer)**() |

## Detailed Description

```cpp
template <class TGradientNumericType ,
unsigned int VDimension>
class itk::ParticleGradientDescentPositionOptimizer;
```


This class optimizes a list of particle system positions with respect to a specified energy function using a simple gradient descent strategy. A function which computes the gradient of the function with respect to particle position must be specified. The optimization performs Jacobi updates (each particle position is changed as soon as its new position is computed). 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleGradientDescentPositionOptimizer itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef Object itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::ConstWeakPointer;
```


### typedef DomainType

```cpp
typedef ParticleImageDomainWithGradients<TGradientNumericType> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::DomainType;
```


Type of the domain. 


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::ParticleSystemType;
```


Particle System type is based on the dimensionality. 


### typedef GradientFunctionType

```cpp
typedef ParticleVectorFunction<VDimension> itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::GradientFunctionType;
```


Type of the gradient function. 


### typedef VectorType

```cpp
typedef GradientFunctionType::VectorType itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::VectorType;
```


Numerical vector type. 


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleGradientDescentPositionOptimizer< TGradientNumericType, VDimension >::PointType;
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
    const double & maximumUpdateAllowed
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


Get/Set the [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md) modified by this optimizer. 


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


## Protected Functions Documentation

### function ParticleGradientDescentPositionOptimizer

```cpp
ParticleGradientDescentPositionOptimizer()
```


### function ParticleGradientDescentPositionOptimizer

```cpp
ParticleGradientDescentPositionOptimizer(
    const ParticleGradientDescentPositionOptimizer & 
)
```


### function operator=

```cpp
const ParticleGradientDescentPositionOptimizer & operator=(
    const ParticleGradientDescentPositionOptimizer & 
)
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticleGradientDescentPositionOptimizer

```cpp
inline virtual ~ParticleGradientDescentPositionOptimizer()
```


-------------------------------

Updated on 2021-11-23 at 22:14:02 +0000