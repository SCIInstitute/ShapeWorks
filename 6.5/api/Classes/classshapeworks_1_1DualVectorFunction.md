---
title: shapeworks::DualVectorFunction

---

# shapeworks::DualVectorFunction



 [More...](#detailed-description)


`#include <DualVectorFunction.h>`

Inherits from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md), itk::LightObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) | **[Self](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-constpointer)**  |
| typedef [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) | **[Superclass](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-superclass)**  |
| typedef [Superclass::VectorType](../Classes/classshapeworks_1_1VectorFunction.md#typedef-vectortype) | **[VectorType](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-vectortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1DualVectorFunction.md#function-itktypemacro)**([DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) , [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1DualVectorFunction.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1DualVectorFunction.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| virtual [VectorType](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-evaluate)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system, double & maxmove) const |
| virtual double | **[EnergyA](../Classes/classshapeworks_1_1DualVectorFunction.md#function-energya)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system) const |
| virtual double | **[EnergyB](../Classes/classshapeworks_1_1DualVectorFunction.md#function-energyb)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system) const |
| virtual double | **[Energy](../Classes/classshapeworks_1_1DualVectorFunction.md#function-energy)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system) const |
| virtual [VectorType](../Classes/classshapeworks_1_1DualVectorFunction.md#typedef-vectortype) | **[Evaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-evaluate)**(unsigned int idx, unsigned int d, const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * system, double & maxmove, double & energy) const |
| virtual void | **[BeforeEvaluate](../Classes/classshapeworks_1_1DualVectorFunction.md#function-beforeevaluate)**(unsigned int , unsigned int , const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * ) |
| virtual void | **[AfterIteration](../Classes/classshapeworks_1_1DualVectorFunction.md#function-afteriteration)**() |
| virtual void | **[BeforeIteration](../Classes/classshapeworks_1_1DualVectorFunction.md#function-beforeiteration)**() |
| virtual void | **[SetParticleSystem](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setparticlesystem)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * p) |
| virtual void | **[SetDomainNumber](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setdomainnumber)**(unsigned int i) |
| void | **[SetFunctionA](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setfunctiona)**([VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) * o) |
| [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) * | **[GetFunctionA](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getfunctiona)**() |
| [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) * | **[GetFunctionB](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getfunctionb)**() |
| void | **[SetFunctionB](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setfunctionb)**([VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) * o) |
| void | **[SetAOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setaon)**() |
| void | **[SetAOff](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setaoff)**() |
| void | **[SetAOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setaon)**(bool s) |
| bool | **[GetAOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getaon)**() const |
| void | **[SetBOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setbon)**() |
| void | **[SetBOff](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setboff)**() |
| void | **[SetBOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setbon)**(bool s) |
| bool | **[GetBOn](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getbon)**() const |
| virtual void | **[SetRelativeEnergyScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setrelativeenergyscaling)**(double r) override |
| virtual double | **[GetRelativeEnergyScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getrelativeenergyscaling)**() const override |
| void | **[SetRelativeGradientScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-setrelativegradientscaling)**(double r) |
| double | **[GetRelativeGradientScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getrelativegradientscaling)**() const |
| double | **[GetAverageGradMagA](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getaveragegradmaga)**() const |
| double | **[GetAverageGradMagB](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getaveragegradmagb)**() const |
| double | **[GetAverageEnergyA](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getaverageenergya)**() const |
| double | **[GetAverageEnergyB](../Classes/classshapeworks_1_1DualVectorFunction.md#function-getaverageenergyb)**() const |
| virtual VectorFunction::Pointer | **[Clone](../Classes/classshapeworks_1_1DualVectorFunction.md#function-clone)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md#function-dualvectorfunction)**() |
| virtual | **[~DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md#function-~dualvectorfunction)**() |
| void | **[operator=](../Classes/classshapeworks_1_1DualVectorFunction.md#function-operator=)**(const [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) & ) |
| | **[DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md#function-dualvectorfunction)**(const [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) & ) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr static int | **[VDimension](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-vdimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_AOn](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-aon)**  |
| bool | **[m_BOn](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-bon)**  |
| double | **[m_RelativeGradientScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-relativegradientscaling)**  |
| double | **[m_RelativeEnergyScaling](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-relativeenergyscaling)**  |
| double | **[m_AverageGradMagA](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-averagegradmaga)**  |
| double | **[m_AverageGradMagB](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-averagegradmagb)**  |
| double | **[m_AverageEnergyA](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-averageenergya)**  |
| double | **[m_AverageEnergyB](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-averageenergyb)**  |
| double | **[m_Counter](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-counter)**  |
| VectorFunction::Pointer | **[m_FunctionA](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-functiona)**  |
| VectorFunction::Pointer | **[m_FunctionB](../Classes/classshapeworks_1_1DualVectorFunction.md#variable-m-functionb)**  |

## Additional inherited members

**Public Functions inherited from [shapeworks::VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md)**

|                | Name           |
| -------------- | -------------- |
| virtual void | **[ResetBuffers](../Classes/classshapeworks_1_1VectorFunction.md#function-resetbuffers)**() |
| virtual [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-getparticlesystem)**() const |
| virtual int | **[GetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-getdomainnumber)**() const |

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
class shapeworks::DualVectorFunction;
```


This class combines the results of evaluating 2 [VectorFunction](../Classes/classshapeworks_1_1VectorFunction.md) and presents the interface of a single function evaluation. Optionally, only the first function can be used by calling SetLinkOff(). 

## Public Types Documentation

### typedef Self

```cpp
typedef DualVectorFunction shapeworks::DualVectorFunction::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::DualVectorFunction::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::DualVectorFunction::ConstPointer;
```


### typedef Superclass

```cpp
typedef VectorFunction shapeworks::DualVectorFunction::Superclass;
```


### typedef VectorType

```cpp
typedef Superclass::VectorType shapeworks::DualVectorFunction::VectorType;
```


Vector type. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    DualVectorFunction ,
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
inline virtual VectorType Evaluate(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system,
    double & maxmove
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


The first argument is a pointer to the particle system. The second argument is the index of the domain within that particle system. The third argument is the index of the particle location within the given domain. 


### function EnergyA

```cpp
inline virtual double EnergyA(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system
) const
```


### function EnergyB

```cpp
inline virtual double EnergyB(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system
) const
```


### function Energy

```cpp
inline virtual double Energy(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system
) const
```


**Reimplements**: [shapeworks::VectorFunction::Energy](../Classes/classshapeworks_1_1VectorFunction.md#function-energy)


### function Evaluate

```cpp
inline virtual VectorType Evaluate(
    unsigned int idx,
    unsigned int d,
    const ParticleSystem * system,
    double & maxmove,
    double & energy
) const
```


**Reimplements**: [shapeworks::VectorFunction::Evaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-evaluate)


### function BeforeEvaluate

```cpp
inline virtual void BeforeEvaluate(
    unsigned int ,
    unsigned int ,
    const ParticleSystem * 
)
```


**Reimplements**: [shapeworks::VectorFunction::BeforeEvaluate](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeevaluate)


This method may be called to set up the state of the function object before a call to Evaluate. It is necessary in order to initialize certain constants and variables that may be used for calculating the Energy as well as the Gradients. Typically this is only necessary for the adaptive gradient descent algorithm. 


### function AfterIteration

```cpp
inline virtual void AfterIteration()
```


**Reimplements**: [shapeworks::VectorFunction::AfterIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-afteriteration)


This method is called by a solver after each iteration. Subclasses may or may not implement this method. 


### function BeforeIteration

```cpp
inline virtual void BeforeIteration()
```


**Reimplements**: [shapeworks::VectorFunction::BeforeIteration](../Classes/classshapeworks_1_1VectorFunction.md#function-beforeiteration)


This method is called by a solver before each iteration. Subclasses may or may not implement this method. 


### function SetParticleSystem

```cpp
inline virtual void SetParticleSystem(
    ParticleSystem * p
)
```


**Reimplements**: [shapeworks::VectorFunction::SetParticleSystem](../Classes/classshapeworks_1_1VectorFunction.md#function-setparticlesystem)


Some subclasses may require a pointer to the particle system and its domain number. These methods set/get those values. 


### function SetDomainNumber

```cpp
inline virtual void SetDomainNumber(
    unsigned int i
)
```


**Reimplements**: [shapeworks::VectorFunction::SetDomainNumber](../Classes/classshapeworks_1_1VectorFunction.md#function-setdomainnumber)


### function SetFunctionA

```cpp
inline void SetFunctionA(
    VectorFunction * o
)
```


### function GetFunctionA

```cpp
inline VectorFunction * GetFunctionA()
```


### function GetFunctionB

```cpp
inline VectorFunction * GetFunctionB()
```


### function SetFunctionB

```cpp
inline void SetFunctionB(
    VectorFunction * o
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
inline virtual void SetRelativeEnergyScaling(
    double r
) override
```


**Reimplements**: [shapeworks::VectorFunction::SetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-setrelativeenergyscaling)


The relative scaling scales the gradient B relative to A. By default this value is 1.0. 


### function GetRelativeEnergyScaling

```cpp
inline virtual double GetRelativeEnergyScaling() const override
```


**Reimplements**: [shapeworks::VectorFunction::GetRelativeEnergyScaling](../Classes/classshapeworks_1_1VectorFunction.md#function-getrelativeenergyscaling)


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
inline virtual VectorFunction::Pointer Clone()
```


**Reimplements**: [shapeworks::VectorFunction::Clone](../Classes/classshapeworks_1_1VectorFunction.md#function-clone)


## Protected Functions Documentation

### function DualVectorFunction

```cpp
inline DualVectorFunction()
```


### function ~DualVectorFunction

```cpp
inline virtual ~DualVectorFunction()
```


### function operator=

```cpp
void operator=(
    const DualVectorFunction & 
)
```


### function DualVectorFunction

```cpp
DualVectorFunction(
    const DualVectorFunction & 
)
```


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr static int VDimension = 3;
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
VectorFunction::Pointer m_FunctionA;
```


### variable m_FunctionB

```cpp
VectorFunction::Pointer m_FunctionB;
```


-------------------------------

Updated on 2024-03-13 at 22:27:53 +0000