---
title: itk::ParticleDualVectorFunction

---

# itk::ParticleDualVectorFunction



 [More...](#detailed-description)


`#include <itkParticleDualVectorFunction.h>`

Inherits from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md), LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md) | **[Self](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-constpointer)**  |
| typedef [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > | **[Superclass](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-superclass)**  |
| typedef [Superclass::ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) | **[ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype)**  |
| typedef [Superclass::VectorType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-itktypemacro)**([ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md) , [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate)**(unsigned int idx, unsigned int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * system, double & maxmove) const |
| virtual double | **[EnergyA](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-energya)**(unsigned int idx, unsigned int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * system) const |
| virtual double | **[EnergyB](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-energyb)**(unsigned int idx, unsigned int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * system) const |
| virtual double | **[Energy](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-energy)**(unsigned int idx, unsigned int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * system) const |
| virtual [VectorType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-evaluate)**(unsigned int idx, unsigned int d, const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * system, double & maxmove, double & energy) const |
| virtual void | **[BeforeEvaluate](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * ) |
| virtual void | **[AfterIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-beforeiteration)**() |
| virtual void | **[SetParticleSystem](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setparticlesystem)**([ParticleSystemType](../Classes/classitk_1_1ParticleDualVectorFunction.md#typedef-particlesystemtype) * p) |
| virtual void | **[SetDomainNumber](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| void | **[SetFunctionA](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setfunctiona)**([ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > * o) |
| [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > * | **[GetFunctionA](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getfunctiona)**() |
| [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > * | **[GetFunctionB](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getfunctionb)**() |
| void | **[SetFunctionB](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setfunctionb)**([ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension > * o) |
| void | **[SetAOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setaon)**() |
| void | **[SetAOff](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setaoff)**() |
| void | **[SetAOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setaon)**(bool s) |
| bool | **[GetAOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getaon)**() const |
| void | **[SetBOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setbon)**() |
| void | **[SetBOff](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setboff)**() |
| void | **[SetBOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setbon)**(bool s) |
| bool | **[GetBOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getbon)**() const |
| void | **[SetRelativeEnergyScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setrelativeenergyscaling)**(double r) |
| double | **[GetRelativeEnergyScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getrelativeenergyscaling)**() const |
| void | **[SetRelativeGradientScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-setrelativegradientscaling)**(double r) |
| double | **[GetRelativeGradientScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getrelativegradientscaling)**() const |
| double | **[GetAverageGradMagA](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getaveragegradmaga)**() const |
| double | **[GetAverageGradMagB](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getaveragegradmagb)**() const |
| double | **[GetAverageEnergyA](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getaverageenergya)**() const |
| double | **[GetAverageEnergyB](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-getaverageenergyb)**() const |
| virtual [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[Clone](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-particledualvectorfunction)**() |
| virtual | **[~ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-~particledualvectorfunction)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-operator=)**(const [ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md) & ) |
| | **[ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md#function-particledualvectorfunction)**(const [ParticleDualVectorFunction](../Classes/classitk_1_1ParticleDualVectorFunction.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_AOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-aon)**  |
| bool | **[m_BOn](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-bon)**  |
| double | **[m_RelativeGradientScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-relativegradientscaling)**  |
| double | **[m_RelativeEnergyScaling](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-relativeenergyscaling)**  |
| double | **[m_AverageGradMagA](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-averagegradmaga)**  |
| double | **[m_AverageGradMagB](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-averagegradmagb)**  |
| double | **[m_AverageEnergyA](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-averageenergya)**  |
| double | **[m_AverageEnergyB](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-averageenergyb)**  |
| double | **[m_Counter](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-counter)**  |
| [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[m_FunctionA](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-functiona)**  |
| [ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< VDimension >::Pointer | **[m_FunctionB](../Classes/classitk_1_1ParticleDualVectorFunction.md#variable-m-functionb)**  |

## Additional inherited members

**Public Functions inherited from [itk::ParticleVectorFunction< VDimension >](../Classes/classitk_1_1ParticleVectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classitk_1_1ParticleVectorFunction.md#function-resetbuffers)**() |
| virtual [ParticleSystemType](../Classes/classitk_1_1ParticleVectorFunction.md#typedef-particlesystemtype) * | **[GetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-getparticlesystem)**() const |
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
class itk::ParticleDualVectorFunction;
```


This class combines the results of evaluating 2 ParticleVector functions and presents the interface of a single function evaluation. Optionally, only the first function can be used by calling SetLinkOff(). 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleDualVectorFunction itk::ParticleDualVectorFunction< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleDualVectorFunction< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleDualVectorFunction< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef ParticleVectorFunction<VDimension> itk::ParticleDualVectorFunction< VDimension >::Superclass;
```


### typedef ParticleSystemType

```cpp
typedef Superclass::ParticleSystemType itk::ParticleDualVectorFunction< VDimension >::ParticleSystemType;
```


Type of particle system. 


### typedef VectorType

```cpp
typedef Superclass::VectorType itk::ParticleDualVectorFunction< VDimension >::VectorType;
```


Vector type. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleDualVectorFunction ,
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
inline virtual VectorType Evaluate(
    unsigned int idx,
    unsigned int d,
    const ParticleSystemType * system,
    double & maxmove
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function EnergyA

```cpp
inline virtual double EnergyA(
    unsigned int idx,
    unsigned int d,
    const ParticleSystemType * system
) const
```


### function EnergyB

```cpp
inline virtual double EnergyB(
    unsigned int idx,
    unsigned int d,
    const ParticleSystemType * system
) const
```


### function Energy

```cpp
inline virtual double Energy(
    unsigned int idx,
    unsigned int d,
    const ParticleSystemType * system
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Energy](../Classes/classitk_1_1ParticleVectorFunction.md#function-energy)


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int idx,
    unsigned int d,
    const ParticleSystemType * system,
    double & maxmove,
    double & energy
) const
```


**Reimplements**: [itk::ParticleVectorFunction::Evaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-evaluate)


### function BeforeEvaluate

```cpp
inline virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystemType * 
)
```


**Reimplements**: [itk::ParticleVectorFunction::BeforeEvaluate](../Classes/classitk_1_1ParticleVectorFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


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


### function SetParticleSystem

```cpp
inline virtual void SetParticleSystem(
    ParticleSystemType * p
)
```


**Reimplements**: [itk::ParticleVectorFunction::SetParticleSystem](../Classes/classitk_1_1ParticleVectorFunction.md#function-setparticlesystem)


Some subclasses may require a pointer to the particle system and its domain number. These methods set/get those values. 


### function SetDomainNumber

```cpp
inline virtual void SetDomainNumber(
    unsigned int i
)
```


**Reimplements**: [itk::ParticleVectorFunction::SetDomainNumber](../Classes/classitk_1_1ParticleVectorFunction.md#function-setdomainnumber)


### function SetFunctionA

```cpp
inline void SetFunctionA(
    ParticleVectorFunction< VDimension > * o
)
```


### function GetFunctionA

```cpp
inline ParticleVectorFunction< VDimension > * GetFunctionA()
```


### function GetFunctionB

```cpp
inline ParticleVectorFunction< VDimension > * GetFunctionB()
```


### function SetFunctionB

```cpp
inline void SetFunctionB(
    ParticleVectorFunction< VDimension > * o
)
```


### function SetAOn

```cpp
inline void SetAOn()
```


Turn each term on and off. 


### function SetAOff

```cpp
inline void SetAOff()
```


### function SetAOn

```cpp
inline void SetAOn(
    bool s
)
```


### function GetAOn

```cpp
inline bool GetAOn() const
```


### function SetBOn

```cpp
inline void SetBOn()
```


### function SetBOff

```cpp
inline void SetBOff()
```


### function SetBOn

```cpp
inline void SetBOn(
    bool s
)
```


### function GetBOn

```cpp
inline bool GetBOn() const
```


### function SetRelativeEnergyScaling

```cpp
inline void SetRelativeEnergyScaling(
    double r
)
```


The relative scaling scales the gradient B relative to A. By default this value is 1.0. 


### function GetRelativeEnergyScaling

```cpp
inline double GetRelativeEnergyScaling() const
```


### function SetRelativeGradientScaling

```cpp
inline void SetRelativeGradientScaling(
    double r
)
```


### function GetRelativeGradientScaling

```cpp
inline double GetRelativeGradientScaling() const
```


### function GetAverageGradMagA

```cpp
inline double GetAverageGradMagA() const
```


### function GetAverageGradMagB

```cpp
inline double GetAverageGradMagB() const
```


### function GetAverageEnergyA

```cpp
inline double GetAverageEnergyA() const
```


### function GetAverageEnergyB

```cpp
inline double GetAverageEnergyB() const
```


### function Clone

```cpp
inline virtual ParticleVectorFunction< VDimension >::Pointer Clone()
```


**Reimplements**: [itk::ParticleVectorFunction::Clone](../Classes/classitk_1_1ParticleVectorFunction.md#function-clone)


## Protected Functions Documentation

### function ParticleDualVectorFunction

```cpp
inline ParticleDualVectorFunction()
```


### function ~ParticleDualVectorFunction

```cpp
inline virtual ~ParticleDualVectorFunction()
```


### function operator=

```cpp
void operator=(
    const ParticleDualVectorFunction & 
)
```


### function ParticleDualVectorFunction

```cpp
ParticleDualVectorFunction(
    const ParticleDualVectorFunction & 
)
```


## Protected Attributes Documentation

### variable m_AOn

```cpp
bool m_AOn;
```


### variable m_BOn

```cpp
bool m_BOn;
```


### variable m_RelativeGradientScaling

```cpp
double m_RelativeGradientScaling;
```


### variable m_RelativeEnergyScaling

```cpp
double m_RelativeEnergyScaling;
```


### variable m_AverageGradMagA

```cpp
double m_AverageGradMagA;
```


### variable m_AverageGradMagB

```cpp
double m_AverageGradMagB;
```


### variable m_AverageEnergyA

```cpp
double m_AverageEnergyA;
```


### variable m_AverageEnergyB

```cpp
double m_AverageEnergyB;
```


### variable m_Counter

```cpp
double m_Counter;
```


### variable m_FunctionA

```cpp
ParticleVectorFunction< VDimension >::Pointer m_FunctionA;
```


### variable m_FunctionB

```cpp
ParticleVectorFunction< VDimension >::Pointer m_FunctionB;
```


-------------------------------

Updated on 2022-01-14 at 02:25:59 +0000