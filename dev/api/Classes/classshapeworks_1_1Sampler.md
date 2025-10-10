---
title: shapeworks::Sampler

---

# shapeworks::Sampler





## Public Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[CuttingPlaneType](../Classes/structshapeworks_1_1Sampler_1_1CuttingPlaneType.md)**  |
| struct | **[SphereType](../Classes/structshapeworks_1_1Sampler_1_1SphereType.md)**  |

## Public Types

|                | Name           |
| -------------- | -------------- |
| using float | **[PixelType](../Classes/classshapeworks_1_1Sampler.md#using-pixeltype)**  |
| using itk::Image< PixelType, Dimension > | **[ImageType](../Classes/classshapeworks_1_1Sampler.md#using-imagetype)**  |
| using ImageType::PointType | **[PointType](../Classes/classshapeworks_1_1Sampler.md#using-pointtype)**  |
| using [MeanCurvatureContainer](../Classes/classshapeworks_1_1MeanCurvatureContainer.md)< PixelType, Dimension > | **[MeanCurvatureCacheType](../Classes/classshapeworks_1_1Sampler.md#using-meancurvaturecachetype)**  |
| using vnl_matrix_fixed< double, Dimension+1, Dimension+1 > | **[TransformType](../Classes/classshapeworks_1_1Sampler.md#using-transformtype)**  |
| using [GradientDescentOptimizer](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) | **[OptimizerType](../Classes/classshapeworks_1_1Sampler.md#using-optimizertype)**  |
| using std::vector< std::vector< std::pair< Eigen::Vector3d, Eigen::Vector3d > > > | **[CuttingPlaneList](../Classes/classshapeworks_1_1Sampler.md#using-cuttingplanelist)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Sampler](../Classes/classshapeworks_1_1Sampler.md#function-sampler)**()<br>Constructor.  |
| virtual | **[~Sampler](../Classes/classshapeworks_1_1Sampler.md#function-~sampler)**()<br>Destructor.  |
| [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1Sampler.md#function-getparticlesystem)**()<br>Returns the particle system.  |
| const [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) * | **[GetParticleSystem](../Classes/classshapeworks_1_1Sampler.md#function-getparticlesystem)**() const |
| [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) * | **[GetCurvatureGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getcurvaturegradientfunction)**() |
| [OptimizerType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) * | **[GetOptimizer](../Classes/classshapeworks_1_1Sampler.md#function-getoptimizer)**()<br>Return a pointer to the optimizer object.  |
| const [OptimizerType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) * | **[GetOptimizer](../Classes/classshapeworks_1_1Sampler.md#function-getoptimizer)**() const |
| void | **[SetPointsFile](../Classes/classshapeworks_1_1Sampler.md#function-setpointsfile)**(unsigned int i, const std::string & s) |
| void | **[SetPointsFile](../Classes/classshapeworks_1_1Sampler.md#function-setpointsfile)**(const std::string & s) |
| void | **[SetInitialPoints](../Classes/classshapeworks_1_1Sampler.md#function-setinitialpoints)**(std::vector< std::vector< itk::Point< double > > > initial_points)<br>Set initial particle positions (e.g. for fixed subjects)  |
| void | **[AddImage](../Classes/classshapeworks_1_1Sampler.md#function-addimage)**(ImageType::Pointer image, double narrow_band, std::string name ="") |
| void | **[ApplyConstraintsToZeroCrossing](../Classes/classshapeworks_1_1Sampler.md#function-applyconstraintstozerocrossing)**() |
| void | **[AddMesh](../Classes/classshapeworks_1_1Sampler.md#function-addmesh)**(std::shared_ptr< [shapeworks::Surface](../Classes/classshapeworks_1_1Surface.md) > mesh, double geodesic_remesh_percent =100) |
| void | **[AddContour](../Classes/classshapeworks_1_1Sampler.md#function-addcontour)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[SetFieldAttributes](../Classes/classshapeworks_1_1Sampler.md#function-setfieldattributes)**(const std::vector< std::string > & s) |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1Sampler.md#function-setdomainspershape)**(int n) |
| void | **[SetCuttingPlane](../Classes/classshapeworks_1_1Sampler.md#function-setcuttingplane)**(unsigned int i, const vnl_vector_fixed< double, Dimension > & va, const vnl_vector_fixed< double, Dimension > & vb, const vnl_vector_fixed< double, Dimension > & vc) |
| void | **[AddFreeFormConstraint](../Classes/classshapeworks_1_1Sampler.md#function-addfreeformconstraint)**(int domain, const [FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md) & ffc) |
| void | **[TransformCuttingPlanes](../Classes/classshapeworks_1_1Sampler.md#function-transformcuttingplanes)**(unsigned int i) |
| void | **[AddSphere](../Classes/classshapeworks_1_1Sampler.md#function-addsphere)**(unsigned int i, vnl_vector_fixed< double, Dimension > & c, double r) |
| void | **[SetAdaptivityMode](../Classes/classshapeworks_1_1Sampler.md#function-setadaptivitymode)**() |
| void | **[SetCorrespondenceOn](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondenceon)**() |
| void | **[SetCorrespondenceOff](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondenceoff)**() |
| void | **[SetSamplingOn](../Classes/classshapeworks_1_1Sampler.md#function-setsamplingon)**() |
| void | **[SetSamplingOff](../Classes/classshapeworks_1_1Sampler.md#function-setsamplingoff)**() |
| bool | **[GetCorrespondenceOn](../Classes/classshapeworks_1_1Sampler.md#function-getcorrespondenceon)**() const |
| bool | **[GetSamplingOn](../Classes/classshapeworks_1_1Sampler.md#function-getsamplingon)**() const |
| void | **[SetCorrespondenceMode](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondencemode)**(shapeworks::CorrespondenceMode mode) |
| void | **[RegisterGeneralShapeMatrices](../Classes/classshapeworks_1_1Sampler.md#function-registergeneralshapematrices)**() |
| void | **[SetAttributeScales](../Classes/classshapeworks_1_1Sampler.md#function-setattributescales)**(const std::vector< double > & s) |
| void | **[SetXYZ](../Classes/classshapeworks_1_1Sampler.md#function-setxyz)**(unsigned int i, bool flag) |
| void | **[SetNormals](../Classes/classshapeworks_1_1Sampler.md#function-setnormals)**(int i, bool flag) |
| void | **[SetAttributesPerDomain](../Classes/classshapeworks_1_1Sampler.md#function-setattributesperdomain)**(const std::vector< int > s) |
| [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#function-getshapematrix)**() |
| [ShapeMatrix](../Classes/classshapeworks_1_1ShapeMatrix.md) * | **[GetGeneralShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#function-getgeneralshapematrix)**() |
| [ShapeGradientMatrix](../Classes/classshapeworks_1_1ShapeGradientMatrix.md) * | **[GetGeneralShapeGradientMatrix](../Classes/classshapeworks_1_1Sampler.md#function-getgeneralshapegradientmatrix)**() |
| [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) * | **[GetLinkingFunction](../Classes/classshapeworks_1_1Sampler.md#function-getlinkingfunction)**() |
| [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleentropyfunction)**() |
| [DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) * | **[GetDisentangledEnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getdisentangledensembleentropyfunction)**() |
| [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleRegressionEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleregressionentropyfunction)**() |
| [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleMixedEffectsEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensemblemixedeffectsentropyfunction)**() |
| [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) * | **[GetMeshBasedGeneralEntropyGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getmeshbasedgeneralentropygradientfunction)**() |
| const [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) * | **[GetLinkingFunction](../Classes/classshapeworks_1_1Sampler.md#function-getlinkingfunction)**() const |
| const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleentropyfunction)**() const |
| const [DisentangledCorrespondenceFunction](../Classes/classshapeworks_1_1DisentangledCorrespondenceFunction.md) * | **[GetDisentangledEnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getdisentangledensembleentropyfunction)**() const |
| const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleRegressionEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleregressionentropyfunction)**() const |
| const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleMixedEffectsEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensemblemixedeffectsentropyfunction)**() const |
| const [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) * | **[GetMeshBasedGeneralEntropyGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getmeshbasedgeneralentropygradientfunction)**() const |
| void | **[SetTimeptsPerIndividual](../Classes/classshapeworks_1_1Sampler.md#function-settimeptsperindividual)**(int n) |
| shapeworks::CorrespondenceMode | **[GetCorrespondenceMode](../Classes/classshapeworks_1_1Sampler.md#function-getcorrespondencemode)**() const |
| void | **[SetTransformFile](../Classes/classshapeworks_1_1Sampler.md#function-settransformfile)**(const std::string & s) |
| void | **[SetTransformFile](../Classes/classshapeworks_1_1Sampler.md#function-settransformfile)**(const char * s) |
| void | **[SetPrefixTransformFile](../Classes/classshapeworks_1_1Sampler.md#function-setprefixtransformfile)**(const std::string & s) |
| void | **[SetPrefixTransformFile](../Classes/classshapeworks_1_1Sampler.md#function-setprefixtransformfile)**(const char * s) |
| void | **[SetPairwisePotentialType](../Classes/classshapeworks_1_1Sampler.md#function-setpairwisepotentialtype)**(int pairwise_potential_type) |
| int | **[GetPairwisePotentialType](../Classes/classshapeworks_1_1Sampler.md#function-getpairwisepotentialtype)**() |
| void | **[SetVerbosity](../Classes/classshapeworks_1_1Sampler.md#function-setverbosity)**(unsigned int val) |
| unsigned int | **[GetVerbosity](../Classes/classshapeworks_1_1Sampler.md#function-getverbosity)**() |
| void | **[SetSharedBoundaryEnabled](../Classes/classshapeworks_1_1Sampler.md#function-setsharedboundaryenabled)**(bool enabled) |
| void | **[SetSharedBoundaryWeight](../Classes/classshapeworks_1_1Sampler.md#function-setsharedboundaryweight)**(double weight) |
| void | **[SetEarlyStoppingConfig](../Classes/classshapeworks_1_1Sampler.md#function-setearlystoppingconfig)**([EarlyStoppingConfig](../Classes/structshapeworks_1_1EarlyStoppingConfig.md) config) |
| void | **[ReadTransforms](../Classes/classshapeworks_1_1Sampler.md#function-readtransforms)**() |
| void | **[ReadPointsFiles](../Classes/classshapeworks_1_1Sampler.md#function-readpointsfiles)**() |
| void | **[AllocateDataCaches](../Classes/classshapeworks_1_1Sampler.md#function-allocatedatacaches)**() |
| void | **[AllocateDomainsAndNeighborhoods](../Classes/classshapeworks_1_1Sampler.md#function-allocatedomainsandneighborhoods)**() |
| void | **[InitializeOptimizationFunctions](../Classes/classshapeworks_1_1Sampler.md#function-initializeoptimizationfunctions)**() |
| void | **[initialize_initial_positions](../Classes/classshapeworks_1_1Sampler.md#function-initialize-initial-positions)**() |
| void | **[Initialize](../Classes/classshapeworks_1_1Sampler.md#function-initialize)**() |
| void | **[ReInitialize](../Classes/classshapeworks_1_1Sampler.md#function-reinitialize)**() |
| void | **[Execute](../Classes/classshapeworks_1_1Sampler.md#function-execute)**() |
| CuttingPlaneList | **[ComputeCuttingPlanes](../Classes/classshapeworks_1_1Sampler.md#function-computecuttingplanes)**() |
| Eigen::Vector3d | **[ComputePlaneNormal](../Classes/classshapeworks_1_1Sampler.md#function-computeplanenormal)**(const vnl_vector< double > & a, const vnl_vector< double > & b, const vnl_vector< double > & c) |
| std::vector< [FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md) > | **[GetFFCs](../Classes/classshapeworks_1_1Sampler.md#function-getffcs)**() |
| void | **[SetMeshFFCMode](../Classes/classshapeworks_1_1Sampler.md#function-setmeshffcmode)**(bool mesh_ffc_mode) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr unsigned int | **[Dimension](../Classes/classshapeworks_1_1Sampler.md#variable-dimension)**  |

## Public Types Documentation

### using PixelType

```cpp
using shapeworks::Sampler::PixelType =  float;
```


### using ImageType

```cpp
using shapeworks::Sampler::ImageType =  itk::Image<PixelType, Dimension>;
```


### using PointType

```cpp
using shapeworks::Sampler::PointType =  ImageType::PointType;
```


### using MeanCurvatureCacheType

```cpp
using shapeworks::Sampler::MeanCurvatureCacheType =  MeanCurvatureContainer<PixelType, Dimension>;
```


### using TransformType

```cpp
using shapeworks::Sampler::TransformType =  vnl_matrix_fixed<double, Dimension + 1, Dimension + 1>;
```


### using OptimizerType

```cpp
using shapeworks::Sampler::OptimizerType =  GradientDescentOptimizer;
```


### using CuttingPlaneList

```cpp
using shapeworks::Sampler::CuttingPlaneList =  std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d> >>;
```


## Public Functions Documentation

### function Sampler

```cpp
Sampler()
```

Constructor. 

### function ~Sampler

```cpp
inline virtual ~Sampler()
```

Destructor. 

### function GetParticleSystem

```cpp
inline ParticleSystem * GetParticleSystem()
```

Returns the particle system. 

### function GetParticleSystem

```cpp
inline const ParticleSystem * GetParticleSystem() const
```


### function GetCurvatureGradientFunction

```cpp
inline SamplingFunction * GetCurvatureGradientFunction()
```


### function GetOptimizer

```cpp
inline OptimizerType * GetOptimizer()
```

Return a pointer to the optimizer object. 

### function GetOptimizer

```cpp
inline const OptimizerType * GetOptimizer() const
```


### function SetPointsFile

```cpp
inline void SetPointsFile(
    unsigned int i,
    const std::string & s
)
```


Optionally provide a filename for an initial point set. 


### function SetPointsFile

```cpp
inline void SetPointsFile(
    const std::string & s
)
```


### function SetInitialPoints

```cpp
inline void SetInitialPoints(
    std::vector< std::vector< itk::Point< double > > > initial_points
)
```

Set initial particle positions (e.g. for fixed subjects) 

### function AddImage

```cpp
void AddImage(
    ImageType::Pointer image,
    double narrow_band,
    std::string name =""
)
```


### function ApplyConstraintsToZeroCrossing

```cpp
inline void ApplyConstraintsToZeroCrossing()
```


### function AddMesh

```cpp
void AddMesh(
    std::shared_ptr< shapeworks::Surface > mesh,
    double geodesic_remesh_percent =100
)
```


### function AddContour

```cpp
void AddContour(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function SetFieldAttributes

```cpp
void SetFieldAttributes(
    const std::vector< std::string > & s
)
```


### function SetDomainsPerShape

```cpp
inline void SetDomainsPerShape(
    int n
)
```


### function SetCuttingPlane

```cpp
void SetCuttingPlane(
    unsigned int i,
    const vnl_vector_fixed< double, Dimension > & va,
    const vnl_vector_fixed< double, Dimension > & vb,
    const vnl_vector_fixed< double, Dimension > & vc
)
```


Optionally supply a cutting plane that will be set as a particle optimization constraint in the image domains. 


### function AddFreeFormConstraint

```cpp
void AddFreeFormConstraint(
    int domain,
    const FreeFormConstraint & ffc
)
```


### function TransformCuttingPlanes

```cpp
void TransformCuttingPlanes(
    unsigned int i
)
```


Transform a cutting plane based on procrustes transformation 


### function AddSphere

```cpp
void AddSphere(
    unsigned int i,
    vnl_vector_fixed< double, Dimension > & c,
    double r
)
```


Optionally add spheres that may be used as constraints to the domain. 


### function SetAdaptivityMode

```cpp
inline void SetAdaptivityMode()
```


### function SetCorrespondenceOn

```cpp
inline void SetCorrespondenceOn()
```


### function SetCorrespondenceOff

```cpp
inline void SetCorrespondenceOff()
```


### function SetSamplingOn

```cpp
inline void SetSamplingOn()
```


### function SetSamplingOff

```cpp
inline void SetSamplingOff()
```


### function GetCorrespondenceOn

```cpp
inline bool GetCorrespondenceOn() const
```


### function GetSamplingOn

```cpp
inline bool GetSamplingOn() const
```


### function SetCorrespondenceMode

```cpp
void SetCorrespondenceMode(
    shapeworks::CorrespondenceMode mode
)
```


This method sets the optimization function for correspondences between surfaces (domains). 


### function RegisterGeneralShapeMatrices

```cpp
inline void RegisterGeneralShapeMatrices()
```


### function SetAttributeScales

```cpp
inline void SetAttributeScales(
    const std::vector< double > & s
)
```


### function SetXYZ

```cpp
inline void SetXYZ(
    unsigned int i,
    bool flag
)
```


### function SetNormals

```cpp
inline void SetNormals(
    int i,
    bool flag
)
```


### function SetAttributesPerDomain

```cpp
void SetAttributesPerDomain(
    const std::vector< int > s
)
```


### function GetShapeMatrix

```cpp
inline LegacyShapeMatrix * GetShapeMatrix()
```


### function GetGeneralShapeMatrix

```cpp
inline ShapeMatrix * GetGeneralShapeMatrix()
```


### function GetGeneralShapeGradientMatrix

```cpp
inline ShapeGradientMatrix * GetGeneralShapeGradientMatrix()
```


### function GetLinkingFunction

```cpp
inline DualVectorFunction * GetLinkingFunction()
```


### function GetEnsembleEntropyFunction

```cpp
inline LegacyCorrespondenceFunction * GetEnsembleEntropyFunction()
```


### function GetDisentangledEnsembleEntropyFunction

```cpp
inline DisentangledCorrespondenceFunction * GetDisentangledEnsembleEntropyFunction()
```


### function GetEnsembleRegressionEntropyFunction

```cpp
inline LegacyCorrespondenceFunction * GetEnsembleRegressionEntropyFunction()
```


### function GetEnsembleMixedEffectsEntropyFunction

```cpp
inline LegacyCorrespondenceFunction * GetEnsembleMixedEffectsEntropyFunction()
```


### function GetMeshBasedGeneralEntropyGradientFunction

```cpp
inline CorrespondenceFunction * GetMeshBasedGeneralEntropyGradientFunction()
```


### function GetLinkingFunction

```cpp
inline const DualVectorFunction * GetLinkingFunction() const
```


### function GetEnsembleEntropyFunction

```cpp
inline const LegacyCorrespondenceFunction * GetEnsembleEntropyFunction() const
```


### function GetDisentangledEnsembleEntropyFunction

```cpp
inline const DisentangledCorrespondenceFunction * GetDisentangledEnsembleEntropyFunction() const
```


### function GetEnsembleRegressionEntropyFunction

```cpp
inline const LegacyCorrespondenceFunction * GetEnsembleRegressionEntropyFunction() const
```


### function GetEnsembleMixedEffectsEntropyFunction

```cpp
inline const LegacyCorrespondenceFunction * GetEnsembleMixedEffectsEntropyFunction() const
```


### function GetMeshBasedGeneralEntropyGradientFunction

```cpp
inline const CorrespondenceFunction * GetMeshBasedGeneralEntropyGradientFunction() const
```


### function SetTimeptsPerIndividual

```cpp
inline void SetTimeptsPerIndividual(
    int n
)
```


### function GetCorrespondenceMode

```cpp
inline shapeworks::CorrespondenceMode GetCorrespondenceMode() const
```


### function SetTransformFile

```cpp
inline void SetTransformFile(
    const std::string & s
)
```


### function SetTransformFile

```cpp
inline void SetTransformFile(
    const char * s
)
```


### function SetPrefixTransformFile

```cpp
inline void SetPrefixTransformFile(
    const std::string & s
)
```


### function SetPrefixTransformFile

```cpp
inline void SetPrefixTransformFile(
    const char * s
)
```


### function SetPairwisePotentialType

```cpp
inline void SetPairwisePotentialType(
    int pairwise_potential_type
)
```


### function GetPairwisePotentialType

```cpp
inline int GetPairwisePotentialType()
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


### function SetSharedBoundaryEnabled

```cpp
inline void SetSharedBoundaryEnabled(
    bool enabled
)
```


### function SetSharedBoundaryWeight

```cpp
inline void SetSharedBoundaryWeight(
    double weight
)
```


### function SetEarlyStoppingConfig

```cpp
inline void SetEarlyStoppingConfig(
    EarlyStoppingConfig config
)
```


### function ReadTransforms

```cpp
void ReadTransforms()
```


### function ReadPointsFiles

```cpp
void ReadPointsFiles()
```


### function AllocateDataCaches

```cpp
void AllocateDataCaches()
```


### function AllocateDomainsAndNeighborhoods

```cpp
void AllocateDomainsAndNeighborhoods()
```


### function InitializeOptimizationFunctions

```cpp
void InitializeOptimizationFunctions()
```


### function initialize_initial_positions

```cpp
void initialize_initial_positions()
```


### function Initialize

```cpp
inline void Initialize()
```


### function ReInitialize

```cpp
void ReInitialize()
```


### function Execute

```cpp
void Execute()
```


### function ComputeCuttingPlanes

```cpp
CuttingPlaneList ComputeCuttingPlanes()
```


### function ComputePlaneNormal

```cpp
Eigen::Vector3d ComputePlaneNormal(
    const vnl_vector< double > & a,
    const vnl_vector< double > & b,
    const vnl_vector< double > & c
)
```


### function GetFFCs

```cpp
inline std::vector< FreeFormConstraint > GetFFCs()
```


### function SetMeshFFCMode

```cpp
inline void SetMeshFFCMode(
    bool mesh_ffc_mode
)
```


## Public Attributes Documentation

### variable Dimension

```cpp
static constexpr unsigned int Dimension = 3;
```


-------------------------------

Updated on 2025-10-10 at 18:16:00 +0000