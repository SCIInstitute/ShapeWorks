---
title: shapeworks::ParticleSystem
summary: A facade class managing interactions with a particle system. 

---

# shapeworks::ParticleSystem



A facade class managing interactions with a particle system.  [More...](#detailed-description)


`#include <ParticleSystem.h>`

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[Self](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-constweakpointer)**  |
| using [shapeworks::ParticleDomain](../Classes/classshapeworks_1_1ParticleDomain.md) | **[DomainType](../Classes/classshapeworks_1_1ParticleSystem.md#using-domaintype)**  |
| typedef itk::Point< double, VDimension > | **[PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype)**  |
| typedef [ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md) | **[NeighborhoodType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-neighborhoodtype)**  |
| typedef [GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md)< [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) > | **[PointContainerType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointcontainertype)**  |
| typedef [NeighborhoodType::PointVectorType](../Classes/classshapeworks_1_1ParticleNeighborhood.md#typedef-pointvectortype) | **[PointVectorType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointvectortype)**  |
| typedef vnl_matrix_fixed< double, VDimension+1, VDimension+1 > | **[TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype)**  |
| typedef vnl_vector_fixed< double, VDimension > | **[VectorType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-vectortype)**  |
| typedef vnl_matrix< double > | **[VnlMatrixType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-vnlmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticleSystem.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticleSystem.md#function-itktypemacro)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) , itk::DataObject ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleSystem.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[RegisterObserver](../Classes/classshapeworks_1_1ParticleSystem.md#function-registerobserver)**([Observer](../Classes/classshapeworks_1_1Observer.md) * attr) |
| void | **[SynchronizePositions](../Classes/classshapeworks_1_1ParticleSystem.md#function-synchronizepositions)**() |
| unsigned long int | **[GetNumberOfParticles](../Classes/classshapeworks_1_1ParticleSystem.md#function-getnumberofparticles)**(unsigned int d =0) const |
| const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & | **[AddPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-addposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & , unsigned int d =0) |
| const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & | **[SetPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-setposition)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & , unsigned long int k, unsigned int d =0) |
| void | **[RemovePosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-removeposition)**(unsigned long int k, unsigned int d =0) |
| [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & | **[GetPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-getposition)**(unsigned long int k, unsigned int d =0) |
| const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & | **[GetPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-getposition)**(unsigned long int k, unsigned int d =0) const |
| [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[GetTransformedPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransformedposition)**(unsigned long int k, unsigned int d =0) const |
| [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[GetPrefixTransformedPosition](../Classes/classshapeworks_1_1ParticleSystem.md#function-getprefixtransformedposition)**(unsigned long int k, unsigned int d =0) const |
| void | **[SplitAllParticles](../Classes/classshapeworks_1_1ParticleSystem.md#function-splitallparticles)**(double epsilon) |
| void | **[SplitParticle](../Classes/classshapeworks_1_1ParticleSystem.md#function-splitparticle)**(double epsilon, unsigned int idx, unsigned int d =0) |
| void | **[AdvancedAllParticleSplitting](../Classes/classshapeworks_1_1ParticleSystem.md#function-advancedallparticlesplitting)**(double epsilon, unsigned int domains_per_shape, unsigned int dom_to_process) |
| void | **[PrintParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md#function-printparticlesystem)**() |
| void | **[SetNeighborhood](../Classes/classshapeworks_1_1ParticleSystem.md#function-setneighborhood)**(unsigned int , [NeighborhoodType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-neighborhoodtype) * ) |
| void | **[SetNeighborhood](../Classes/classshapeworks_1_1ParticleSystem.md#function-setneighborhood)**([NeighborhoodType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-neighborhoodtype) * n) |
| NeighborhoodType::ConstPointer | **[GetNeighborhood](../Classes/classshapeworks_1_1ParticleSystem.md#function-getneighborhood)**(unsigned int k) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & p, int idx, double r, unsigned int d =0) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & p, int idx, std::vector< double > & w, std::vector< double > & distances, double r, unsigned int d =0) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & p, int idx, std::vector< double > & w, double r, unsigned int d =0) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(unsigned int idx, double r, unsigned int d =0) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(unsigned int idx, std::vector< double > & w, std::vector< double > & distances, double r, unsigned int d =0) const |
| PointVectorType | **[FindNeighborhoodPoints](../Classes/classshapeworks_1_1ParticleSystem.md#function-findneighborhoodpoints)**(unsigned int idx, std::vector< double > & w, double r, unsigned int d =0) const |
| void | **[AddDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-adddomain)**(DomainType::Pointer input) |
| std::vector< DomainType::Pointer >::const_iterator | **[GetDomainsBegin](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomainsbegin)**() const |
| std::vector< DomainType::Pointer >::const_iterator | **[GetDomainsEnd](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomainsend)**() const |
| [DomainType](../Classes/classshapeworks_1_1ParticleSystem.md#using-domaintype) * | **[GetDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomain)**(unsigned int i) |
| [DomainType](../Classes/classshapeworks_1_1ParticleSystem.md#using-domaintype) * | **[GetDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomain)**() |
| const [DomainType](../Classes/classshapeworks_1_1ParticleSystem.md#using-domaintype) * | **[GetDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomain)**(unsigned int i) const |
| const [DomainType](../Classes/classshapeworks_1_1ParticleSystem.md#using-domaintype) * | **[GetDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomain)**() const |
| unsigned int | **[GetNumberOfDomains](../Classes/classshapeworks_1_1ParticleSystem.md#function-getnumberofdomains)**() const |
| void | **[SetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-settransform)**(unsigned int i, const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & ) |
| void | **[SetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-settransform)**(const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & p) |
| void | **[SetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-setprefixtransform)**(unsigned int i, const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & ) |
| void | **[SetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-setprefixtransform)**(const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & p) |
| std::vector< [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) >::const_iterator | **[GetTransformsBegin](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransformsbegin)**() const |
| std::vector< [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) >::const_iterator | **[GetTransformsEnd](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransformsend)**() const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransform)**(unsigned int i) const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransform)**() const |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) | **[GetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransform)**(unsigned int i) |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) | **[GetTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-gettransform)**() |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getprefixtransform)**(unsigned int i) const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getprefixtransform)**() const |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) | **[GetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getprefixtransform)**(unsigned int i) |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) | **[GetPrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getprefixtransform)**() |
| std::vector< [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) >::const_iterator | **[GetInverseTransformsBegin](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransformsbegin)**() const |
| std::vector< [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) >::const_iterator | **[GetInverseTransformsEnd](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransformsend)**() const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInverseTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransform)**(unsigned int i) const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInverseTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransform)**() const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInversePrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinverseprefixtransform)**(unsigned int i) const |
| const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInversePrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinverseprefixtransform)**() const |
| const std::vector< PointContainerType::Pointer > & | **[GetPositions](../Classes/classshapeworks_1_1ParticleSystem.md#function-getpositions)**() const |
| const PointContainerType::Pointer & | **[GetPositions](../Classes/classshapeworks_1_1ParticleSystem.md#function-getpositions)**(unsigned int d) const |
| void | **[AddPositionList](../Classes/classshapeworks_1_1ParticleSystem.md#function-addpositionlist)**(const std::vector< [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) > & , unsigned int d =0) |
| [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[TransformPoint](../Classes/classshapeworks_1_1ParticleSystem.md#function-transformpoint)**(const [PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) & , const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & ) const |
| VectorType | **[TransformVector](../Classes/classshapeworks_1_1ParticleSystem.md#function-transformvector)**(const VectorType & , const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & ) const |
| VnlMatrixType | **[TransformNormalDerivative](../Classes/classshapeworks_1_1ParticleSystem.md#function-transformnormalderivative)**(const VnlMatrixType & , const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & ) const |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) | **[InvertTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-inverttransform)**(const [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & T) const |
| void | **[FlagDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-flagdomain)**(unsigned int i) |
| void | **[UnflagDomain](../Classes/classshapeworks_1_1ParticleSystem.md#function-unflagdomain)**(unsigned int i) |
| bool | **[GetDomainFlag](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomainflag)**(unsigned int i) const |
| const std::vector< bool > & | **[GetDomainFlags](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomainflags)**() const |
| void | **[SetDomainFlags](../Classes/classshapeworks_1_1ParticleSystem.md#function-setdomainflags)**() |
| void | **[ResetDomainFlags](../Classes/classshapeworks_1_1ParticleSystem.md#function-resetdomainflags)**() |
| void | **[SetFixedParticleFlag](../Classes/classshapeworks_1_1ParticleSystem.md#function-setfixedparticleflag)**(unsigned int d, unsigned int i) |
| void | **[ResetFixedParticleFlag](../Classes/classshapeworks_1_1ParticleSystem.md#function-resetfixedparticleflag)**(unsigned int d, unsigned int i) |
| bool | **[GetFixedParticleFlag](../Classes/classshapeworks_1_1ParticleSystem.md#function-getfixedparticleflag)**(unsigned int d, unsigned int i) const |
| void | **[ResetFixedParticleFlags](../Classes/classshapeworks_1_1ParticleSystem.md#function-resetfixedparticleflags)**() |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1ParticleSystem.md#function-setdomainspershape)**(unsigned int num) |
| unsigned int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1ParticleSystem.md#function-getdomainspershape)**() const |
| void | **[SetNumberOfDomains](../Classes/classshapeworks_1_1ParticleSystem.md#function-setnumberofdomains)**(unsigned int ) |
| double | **[ComputeMaxDistNearestNeighbors](../Classes/classshapeworks_1_1ParticleSystem.md#function-computemaxdistnearestneighbors)**(size_t dom) |
| void | **[SetFieldAttributes](../Classes/classshapeworks_1_1ParticleSystem.md#function-setfieldattributes)**(const std::vector< std::string > & field_attributes) |
| const std::vector< std::string > & | **[GetFieldAttributes](../Classes/classshapeworks_1_1ParticleSystem.md#function-getfieldattributes)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md#function-particlesystem)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticleSystem.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md#function-~particlesystem)**() |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInverseTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransform)**(unsigned int i) |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInverseTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinversetransform)**() |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInversePrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinverseprefixtransform)**(unsigned int i) |
| [TransformType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-transformtype) & | **[GetInversePrefixTransform](../Classes/classshapeworks_1_1ParticleSystem.md#function-getinverseprefixtransform)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr int | **[VDimension](../Classes/classshapeworks_1_1ParticleSystem.md#variable-vdimension)**  |

## Detailed Description

```cpp
class shapeworks::ParticleSystem;
```

A facade class managing interactions with a particle system. 

A particle system consists of a set of particle locations and domains in which those locations are defined. For example, a particle system may simply be a set of points in 3D Euclidean space that are constrained to a specified bounding box. A more complex example is a set of 3D points constrained to a given surface. The itkParticleSystem class is also designed to manage multiple sets of points across multiple domains. For example, one may create a system of dozens of point sets that are each defined in different spaces. The itkParticleSystem class can also maintain a set of transformations associated with each domain that map that particular domain into another coordinate frame, for example, a common coordinate frame. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleSystem shapeworks::ParticleSystem::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::ParticleSystem::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticleSystem::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticleSystem::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticleSystem::ConstWeakPointer;
```


### using DomainType

```cpp
using shapeworks::ParticleSystem::DomainType =  shapeworks::ParticleDomain;
```


Define the base domain type. 


### typedef PointType

```cpp
typedef itk::Point<double, VDimension> shapeworks::ParticleSystem::PointType;
```


Point type used to store particle locations. 


### typedef NeighborhoodType

```cpp
typedef ParticleNeighborhood shapeworks::ParticleSystem::NeighborhoodType;
```


Class used to compute neighborhoods of points. One is associated with each domain. 


### typedef PointContainerType

```cpp
typedef GenericContainer<PointType> shapeworks::ParticleSystem::PointContainerType;
```


Point container type. One is associated with each domain. 


### typedef PointVectorType

```cpp
typedef NeighborhoodType::PointVectorType shapeworks::ParticleSystem::PointVectorType;
```


### typedef TransformType

```cpp
typedef vnl_matrix_fixed<double, VDimension + 1, VDimension + 1> shapeworks::ParticleSystem::TransformType;
```


Defines a transform class type. One is associated with each domain. Transforms are affine for simplicity. This could be extended by using the itk::Transform base type so that a variety of transform types may be used. 


### typedef VectorType

```cpp
typedef vnl_vector_fixed<double, VDimension> shapeworks::ParticleSystem::VectorType;
```


### typedef VnlMatrixType

```cpp
typedef vnl_matrix<double> shapeworks::ParticleSystem::VnlMatrixType;
```


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
    ParticleSystem ,
    itk::DataObject 
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


### function RegisterObserver

```cpp
void RegisterObserver(
    Observer * attr
)
```


Register an attribute object with this particle system. This action adds the attribute object as an observer for the particle system so that it will receive published updates. For example, a gradient attribute object may need to know when particle positions have changed so that it can update its internal state appropriately (e.g. precompute a gradient that corresponds to the new particle position). Internally, this method manages registering the appropriate itk::Command and itk::Object as an observer of this itk::Object (AddObserver). Relevant state information is passed to objects in particle events. 


### function SynchronizePositions

```cpp
inline void SynchronizePositions()
```


Invokes the set event on all particle positions, resetting them to their current value. This method may be called to synchronize positional information among various observers which may have gone out of sync. 


### function GetNumberOfParticles

```cpp
inline unsigned long int GetNumberOfParticles(
    unsigned int d =0
) const
```


Returns the number of particles in domain k. 


### function AddPosition

```cpp
const PointType & AddPosition(
    const PointType & ,
    unsigned int d =0
)
```


Add/Set/Remove a single particle position. The actual position added or set will be returned. If, for example, the domain imposes any constraints on this location (e.g. out-of-bounds, projection onto a surface), then the method returns the location after constraints are applied. Both the Add and Set methods require a domain index. If unspecified, the domain index is zero. The Set method requires a specific particle index. If the given index does not exist, an exception is thrown. Set is intended to be used for moving particles. The SetTransformedPosition sets the position using a Point transformed by the m_Transform associated with the given domain. 


### function SetPosition

```cpp
const PointType & SetPosition(
    const PointType & ,
    unsigned long int k,
    unsigned int d =0
)
```


### function RemovePosition

```cpp
void RemovePosition(
    unsigned long int k,
    unsigned int d =0
)
```


### function GetPosition

```cpp
inline PointType & GetPosition(
    unsigned long int k,
    unsigned int d =0
)
```


Return a position with index k from domain d. Note the order in which the 2 integers must be specified! The domain number is specified second and the position index within the domain is specified first. Note that the domain index may be omitted if the particle system only contains a single domain. GetTransformedPosition returns the position premultiplied by the transform matrix for the given domain. 


### function GetPosition

```cpp
inline const PointType & GetPosition(
    unsigned long int k,
    unsigned int d =0
) const
```


### function GetTransformedPosition

```cpp
inline PointType GetTransformedPosition(
    unsigned long int k,
    unsigned int d =0
) const
```


### function GetPrefixTransformedPosition

```cpp
inline PointType GetPrefixTransformedPosition(
    unsigned long int k,
    unsigned int d =0
) const
```


### function SplitAllParticles

```cpp
void SplitAllParticles(
    double epsilon
)
```


Doubles the number of particles of the system by splitting each particle into 2 particles. Each new particle position is added to the system at a random epsilon distance on the surface from the original particle. The new particles are added using the AddPosition method, so all appropriate callbacks are invoked. 


### function SplitParticle

```cpp
void SplitParticle(
    double epsilon,
    unsigned int idx,
    unsigned int d =0
)
```


### function AdvancedAllParticleSplitting

```cpp
void AdvancedAllParticleSplitting(
    double epsilon,
    unsigned int domains_per_shape,
    unsigned int dom_to_process
)
```


### function PrintParticleSystem

```cpp
void PrintParticleSystem()
```


### function SetNeighborhood

```cpp
void SetNeighborhood(
    unsigned int ,
    NeighborhoodType * 
)
```


Set/Get the neighborhood object associated with domain k. 


### function SetNeighborhood

```cpp
inline void SetNeighborhood(
    NeighborhoodType * n
)
```


### function GetNeighborhood

```cpp
inline NeighborhoodType::ConstPointer GetNeighborhood(
    unsigned int k
) const
```


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    const PointType & p,
    int idx,
    double r,
    unsigned int d =0
) const
```


Return the neighborhood of points with radius r around point p in domain k. This is just a convenience method to avoid exposing the underlying Neighborhood objects. FindTransformedNeighborhoodPoints returns the list with all points transformed by the transform associated with the given domain. 


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    const PointType & p,
    int idx,
    std::vector< double > & w,
    std::vector< double > & distances,
    double r,
    unsigned int d =0
) const
```


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    const PointType & p,
    int idx,
    std::vector< double > & w,
    double r,
    unsigned int d =0
) const
```


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    unsigned int idx,
    double r,
    unsigned int d =0
) const
```


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    unsigned int idx,
    std::vector< double > & w,
    std::vector< double > & distances,
    double r,
    unsigned int d =0
) const
```


### function FindNeighborhoodPoints

```cpp
inline PointVectorType FindNeighborhoodPoints(
    unsigned int idx,
    std::vector< double > & w,
    double r,
    unsigned int d =0
) const
```


### function AddDomain

```cpp
void AddDomain(
    DomainType::Pointer input
)
```


Add a domain to the particle system. This method also allocates space for a list of positions that are contained within the domain, and a default neighborhood calculator. The final, optional argument indicates the calling thread id. 


### function GetDomainsBegin

```cpp
inline std::vector< DomainType::Pointer >::const_iterator GetDomainsBegin() const
```


Return an iterator that points to the first element of the list of the domains. 


### function GetDomainsEnd

```cpp
inline std::vector< DomainType::Pointer >::const_iterator GetDomainsEnd() const
```


Return an iterator that points one position past the last element of the list of the domains. 


### function GetDomain

```cpp
inline DomainType * GetDomain(
    unsigned int i
)
```


Return the i'th domain object. 


### function GetDomain

```cpp
inline DomainType * GetDomain()
```


API for the single domain case. 


### function GetDomain

```cpp
inline const DomainType * GetDomain(
    unsigned int i
) const
```


Return the i'th domain object. 


### function GetDomain

```cpp
inline const DomainType * GetDomain() const
```


API for the single domain case. 


### function GetNumberOfDomains

```cpp
inline unsigned int GetNumberOfDomains() const
```


Returns the number of domains contained in the particle system. 


### function SetTransform

```cpp
void SetTransform(
    unsigned int i,
    const TransformType & 
)
```


Set the transform associated with domain i. This method will also compute and set the corresponding inverse transform if possible. If the inverse transform cannot be computed, the exception is quietly handled by this method and an identity transform is set in its place. The calling program is responsible for knowing when an inverse transform cannot be computed. 


### function SetTransform

```cpp
inline void SetTransform(
    const TransformType & p
)
```


### function SetPrefixTransform

```cpp
void SetPrefixTransform(
    unsigned int i,
    const TransformType & 
)
```


### function SetPrefixTransform

```cpp
inline void SetPrefixTransform(
    const TransformType & p
)
```


### function GetTransformsBegin

```cpp
inline std::vector< TransformType >::const_iterator GetTransformsBegin() const
```


Return an iterator that points to the first element of the list of the transforms. 


### function GetTransformsEnd

```cpp
inline std::vector< TransformType >::const_iterator GetTransformsEnd() const
```


Return an iterator that points one position past the last element of the list of the transforms. 


### function GetTransform

```cpp
inline const TransformType & GetTransform(
    unsigned int i
) const
```


Return the i'th transform object. 


### function GetTransform

```cpp
inline const TransformType & GetTransform() const
```


API for the single transform case. 


### function GetTransform

```cpp
inline TransformType GetTransform(
    unsigned int i
)
```


Return the i'th transform object. 


### function GetTransform

```cpp
inline TransformType GetTransform()
```


API for the single transform case. 


### function GetPrefixTransform

```cpp
inline const TransformType & GetPrefixTransform(
    unsigned int i
) const
```


Return the i'th transform object. 


### function GetPrefixTransform

```cpp
inline const TransformType & GetPrefixTransform() const
```


API for the single transform case. 


### function GetPrefixTransform

```cpp
inline TransformType GetPrefixTransform(
    unsigned int i
)
```


Return the i'th transform object. 


### function GetPrefixTransform

```cpp
inline TransformType GetPrefixTransform()
```


API for the single transform case. 


### function GetInverseTransformsBegin

```cpp
inline std::vector< TransformType >::const_iterator GetInverseTransformsBegin() const
```


Return an iterator that points to the first element of the list of the inverse transforms. 


### function GetInverseTransformsEnd

```cpp
inline std::vector< TransformType >::const_iterator GetInverseTransformsEnd() const
```


Return an iterator that points one position past the last element of the list of the transforms. 


### function GetInverseTransform

```cpp
inline const TransformType & GetInverseTransform(
    unsigned int i
) const
```


Return the i'th transform object. 


### function GetInverseTransform

```cpp
inline const TransformType & GetInverseTransform() const
```


API for the single transform case. 


### function GetInversePrefixTransform

```cpp
inline const TransformType & GetInversePrefixTransform(
    unsigned int i
) const
```


Return the i'th transform object. 


### function GetInversePrefixTransform

```cpp
inline const TransformType & GetInversePrefixTransform() const
```


API for the single transform case. 


### function GetPositions

```cpp
inline const std::vector< PointContainerType::Pointer > & GetPositions() const
```


Return the array of particle positions. 


### function GetPositions

```cpp
inline const PointContainerType::Pointer & GetPositions(
    unsigned int d
) const
```


### function AddPositionList

```cpp
void AddPositionList(
    const std::vector< PointType > & ,
    unsigned int d =0
)
```


Adds a list of points to the specified domain. The arguments are the std::vector of points and the domain number. 


### function TransformPoint

```cpp
PointType TransformPoint(
    const PointType & ,
    const TransformType & 
) const
```


Transforms a point using the given transform. NOTE: Scaling is not currently implemented. (This method may be converted to virtual and overridden if tranform type is generalized.) 


### function TransformVector

```cpp
VectorType TransformVector(
    const VectorType & ,
    const TransformType & 
) const
```


Transforms a vector using the given transform. Only the rotational part of the transform is applied. NOTE: Scaling is not currently supported. 


### function TransformNormalDerivative

```cpp
VnlMatrixType TransformNormalDerivative(
    const VnlMatrixType & ,
    const TransformType & 
) const
```


Transforms the derivative of normals to new space. 


### function InvertTransform

```cpp
inline TransformType InvertTransform(
    const TransformType & T
) const
```


Returns the inverse of a transformation matrix. 


### function FlagDomain

```cpp
inline void FlagDomain(
    unsigned int i
)
```


Flag/Unflag a domain. Flagging a domain has different meanings according to the application using this particle system. 


### function UnflagDomain

```cpp
inline void UnflagDomain(
    unsigned int i
)
```


### function GetDomainFlag

```cpp
inline bool GetDomainFlag(
    unsigned int i
) const
```


### function GetDomainFlags

```cpp
inline const std::vector< bool > & GetDomainFlags() const
```


### function SetDomainFlags

```cpp
inline void SetDomainFlags()
```


### function ResetDomainFlags

```cpp
inline void ResetDomainFlags()
```


### function SetFixedParticleFlag

```cpp
inline void SetFixedParticleFlag(
    unsigned int d,
    unsigned int i
)
```


The following methods provide functionality for specifying particle indices that are fixed landmarks. SetPosition() calls to these particle indices will silently fail. For simplicity, only one list of indices is maintained for all dimensions. If particle index n is flagged, for example, then particle index n in all domains is fixed. 


### function ResetFixedParticleFlag

```cpp
inline void ResetFixedParticleFlag(
    unsigned int d,
    unsigned int i
)
```


### function GetFixedParticleFlag

```cpp
inline bool GetFixedParticleFlag(
    unsigned int d,
    unsigned int i
) const
```


### function ResetFixedParticleFlags

```cpp
inline void ResetFixedParticleFlags()
```


### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    unsigned int num
)
```


### function GetDomainsPerShape

```cpp
inline unsigned int GetDomainsPerShape() const
```


### function SetNumberOfDomains

```cpp
void SetNumberOfDomains(
    unsigned int 
)
```


Set the number of domains. This method modifies the size of the m_Domains, m_Positions, and m_Transform lists. 


### function ComputeMaxDistNearestNeighbors

```cpp
double ComputeMaxDistNearestNeighbors(
    size_t dom
)
```


### function SetFieldAttributes

```cpp
inline void SetFieldAttributes(
    const std::vector< std::string > & field_attributes
)
```


### function GetFieldAttributes

```cpp
inline const std::vector< std::string > & GetFieldAttributes() const
```


## Protected Functions Documentation

### function ParticleSystem

```cpp
ParticleSystem()
```


### function PrintSelf

```cpp
void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~ParticleSystem

```cpp
inline virtual ~ParticleSystem()
```


### function GetInverseTransform

```cpp
inline TransformType & GetInverseTransform(
    unsigned int i
)
```


Return the i'th transform object. 


### function GetInverseTransform

```cpp
inline TransformType & GetInverseTransform()
```


API for the single transform case. 


### function GetInversePrefixTransform

```cpp
inline TransformType & GetInversePrefixTransform(
    unsigned int i
)
```


Return the i'th transform object. 


### function GetInversePrefixTransform

```cpp
inline TransformType & GetInversePrefixTransform()
```


API for the single transform case. 


## Public Attributes Documentation

### variable VDimension

```cpp
static constexpr int VDimension = 3;
```


-------------------------------

Updated on 2024-05-02 at 21:06:12 +0000