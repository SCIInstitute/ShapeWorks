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

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkGetObjectMacro](../Classes/classshapeworks_1_1Sampler.md#function-itkgetobjectmacro)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) , [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) ) |
| | **[itkGetConstObjectMacro](../Classes/classshapeworks_1_1Sampler.md#function-itkgetconstobjectmacro)**([ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) , [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) ) |
| | **[Sampler](../Classes/classshapeworks_1_1Sampler.md#function-sampler)**()<br>Constructor.  |
| virtual | **[~Sampler](../Classes/classshapeworks_1_1Sampler.md#function-~sampler)**()<br>Destructor.  |
| [SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md) * | **[GetGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getgradientfunction)**() |
| [CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md) * | **[GetCurvatureGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getcurvaturegradientfunction)**() |
| | **[itkGetObjectMacro](../Classes/classshapeworks_1_1Sampler.md#function-itkgetobjectmacro)**(Optimizer , [OptimizerType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) ) |
| | **[itkGetConstObjectMacro](../Classes/classshapeworks_1_1Sampler.md#function-itkgetconstobjectmacro)**(Optimizer , [OptimizerType](../Classes/classshapeworks_1_1GradientDescentOptimizer.md) ) |
| void | **[SetPointsFile](../Classes/classshapeworks_1_1Sampler.md#function-setpointsfile)**(unsigned int i, const std::string & s) |
| void | **[SetPointsFile](../Classes/classshapeworks_1_1Sampler.md#function-setpointsfile)**(const std::string & s) |
| void | **[SetMeshFile](../Classes/classshapeworks_1_1Sampler.md#function-setmeshfile)**(unsigned int i, const std::string & s) |
| void | **[SetMeshFile](../Classes/classshapeworks_1_1Sampler.md#function-setmeshfile)**(const std::string & s) |
| void | **[SetMeshFiles](../Classes/classshapeworks_1_1Sampler.md#function-setmeshfiles)**(const std::vector< std::string > & s) |
| void | **[AddImage](../Classes/classshapeworks_1_1Sampler.md#function-addimage)**(ImageType::Pointer image, double narrow_band, std::string name ="") |
| void | **[ApplyConstraintsToZeroCrossing](../Classes/classshapeworks_1_1Sampler.md#function-applyconstraintstozerocrossing)**() |
| void | **[AddMesh](../Classes/classshapeworks_1_1Sampler.md#function-addmesh)**(std::shared_ptr< [shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md) > mesh) |
| void | **[AddContour](../Classes/classshapeworks_1_1Sampler.md#function-addcontour)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[SetFidsFiles](../Classes/classshapeworks_1_1Sampler.md#function-setfidsfiles)**(const std::vector< std::string > & s) |
| void | **[SetFeaFiles](../Classes/classshapeworks_1_1Sampler.md#function-setfeafiles)**(const std::vector< std::string > & s) |
| void | **[SetFeaGradFiles](../Classes/classshapeworks_1_1Sampler.md#function-setfeagradfiles)**(const std::vector< std::string > & s) |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1Sampler.md#function-setdomainspershape)**(int n) |
| void | **[SetCuttingPlane](../Classes/classshapeworks_1_1Sampler.md#function-setcuttingplane)**(unsigned int i, const vnl_vector_fixed< double, Dimension > & va, const vnl_vector_fixed< double, Dimension > & vb, const vnl_vector_fixed< double, Dimension > & vc) |
| void | **[AddFreeFormConstraint](../Classes/classshapeworks_1_1Sampler.md#function-addfreeformconstraint)**(int domain, const [FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md) & ffc) |
| void | **[TransformCuttingPlanes](../Classes/classshapeworks_1_1Sampler.md#function-transformcuttingplanes)**(unsigned int i) |
| void | **[AddSphere](../Classes/classshapeworks_1_1Sampler.md#function-addsphere)**(unsigned int i, vnl_vector_fixed< double, Dimension > & c, double r) |
| virtual void | **[SetAdaptivityMode](../Classes/classshapeworks_1_1Sampler.md#function-setadaptivitymode)**(int mode) |
| int | **[GetAdaptivityMode](../Classes/classshapeworks_1_1Sampler.md#function-getadaptivitymode)**() const |
| void | **[SetCorrespondenceOn](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondenceon)**() |
| void | **[SetCorrespondenceOff](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondenceoff)**() |
| void | **[SetSamplingOn](../Classes/classshapeworks_1_1Sampler.md#function-setsamplingon)**() |
| void | **[SetSamplingOff](../Classes/classshapeworks_1_1Sampler.md#function-setsamplingoff)**() |
| bool | **[GetCorrespondenceOn](../Classes/classshapeworks_1_1Sampler.md#function-getcorrespondenceon)**() const |
| bool | **[GetSamplingOn](../Classes/classshapeworks_1_1Sampler.md#function-getsamplingon)**() const |
| virtual void | **[SetCorrespondenceMode](../Classes/classshapeworks_1_1Sampler.md#function-setcorrespondencemode)**(shapeworks::CorrespondenceMode mode) |
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
| [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleRegressionEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleregressionentropyfunction)**() |
| [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleMixedEffectsEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensemblemixedeffectsentropyfunction)**() |
| [CorrespondenceFunction](../Classes/classshapeworks_1_1CorrespondenceFunction.md) * | **[GetMeshBasedGeneralEntropyGradientFunction](../Classes/classshapeworks_1_1Sampler.md#function-getmeshbasedgeneralentropygradientfunction)**() |
| const [DualVectorFunction](../Classes/classshapeworks_1_1DualVectorFunction.md) * | **[GetLinkingFunction](../Classes/classshapeworks_1_1Sampler.md#function-getlinkingfunction)**() const |
| const [LegacyCorrespondenceFunction](../Classes/classshapeworks_1_1LegacyCorrespondenceFunction.md) * | **[GetEnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#function-getensembleentropyfunction)**() const |
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
| [MeanCurvatureCacheType](../Classes/classshapeworks_1_1MeanCurvatureContainer.md) * | **[GetMeanCurvatureCache](../Classes/classshapeworks_1_1Sampler.md#function-getmeancurvaturecache)**() |
| void | **[SetSharedBoundaryEnabled](../Classes/classshapeworks_1_1Sampler.md#function-setsharedboundaryenabled)**(bool enabled) |
| void | **[SetSharedBoundaryWeight](../Classes/classshapeworks_1_1Sampler.md#function-setsharedboundaryweight)**(double weight) |
| void | **[ReadTransforms](../Classes/classshapeworks_1_1Sampler.md#function-readtransforms)**() |
| void | **[ReadPointsFiles](../Classes/classshapeworks_1_1Sampler.md#function-readpointsfiles)**() |
| virtual void | **[AllocateDataCaches](../Classes/classshapeworks_1_1Sampler.md#function-allocatedatacaches)**() |
| virtual void | **[AllocateDomainsAndNeighborhoods](../Classes/classshapeworks_1_1Sampler.md#function-allocatedomainsandneighborhoods)**() |
| virtual void | **[InitializeOptimizationFunctions](../Classes/classshapeworks_1_1Sampler.md#function-initializeoptimizationfunctions)**() |
| virtual void | **[Initialize](../Classes/classshapeworks_1_1Sampler.md#function-initialize)**() |
| virtual void | **[ReInitialize](../Classes/classshapeworks_1_1Sampler.md#function-reinitialize)**() |
| virtual void | **[Execute](../Classes/classshapeworks_1_1Sampler.md#function-execute)**() |
| std::vector< std::vector< std::pair< Eigen::Vector3d, Eigen::Vector3d > > > | **[ComputeCuttingPlanes](../Classes/classshapeworks_1_1Sampler.md#function-computecuttingplanes)**() |
| Eigen::Vector3d | **[ComputePlaneNormal](../Classes/classshapeworks_1_1Sampler.md#function-computeplanenormal)**(const vnl_vector< double > & a, const vnl_vector< double > & b, const vnl_vector< double > & c) |
| std::vector< [FreeFormConstraint](../Classes/classshapeworks_1_1FreeFormConstraint.md) > | **[GetFFCs](../Classes/classshapeworks_1_1Sampler.md#function-getffcs)**() |
| void | **[SetMeshFFCMode](../Classes/classshapeworks_1_1Sampler.md#function-setmeshffcmode)**(bool mesh_ffc_mode) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[GenerateData](../Classes/classshapeworks_1_1Sampler.md#function-generatedata)**() |
| bool | **[GetInitialized](../Classes/classshapeworks_1_1Sampler.md#function-getinitialized)**() |
| void | **[SetInitialized](../Classes/classshapeworks_1_1Sampler.md#function-setinitialized)**(bool value) |
| bool | **[GetInitializing](../Classes/classshapeworks_1_1Sampler.md#function-getinitializing)**() |
| void | **[SetInitializing](../Classes/classshapeworks_1_1Sampler.md#function-setinitializing)**(bool value) |
| bool | **[initialize_ffcs](../Classes/classshapeworks_1_1Sampler.md#function-initialize-ffcs)**(size_t dom) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr unsigned int | **[Dimension](../Classes/classshapeworks_1_1Sampler.md#variable-dimension)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_Initialized](../Classes/classshapeworks_1_1Sampler.md#variable-m-initialized)**  |
| int | **[m_AdaptivityMode](../Classes/classshapeworks_1_1Sampler.md#variable-m-adaptivitymode)**  |
| bool | **[m_Initializing](../Classes/classshapeworks_1_1Sampler.md#variable-m-initializing)**  |
| OptimizerType::Pointer | **[m_Optimizer](../Classes/classshapeworks_1_1Sampler.md#variable-m-optimizer)**  |
| SamplingFunction::Pointer | **[m_GradientFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-gradientfunction)**  |
| CurvatureSamplingFunction::Pointer | **[m_CurvatureGradientFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-curvaturegradientfunction)**  |
| [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< double >::Pointer | **[m_Sigma1Cache](../Classes/classshapeworks_1_1Sampler.md#variable-m-sigma1cache)**  |
| [GenericContainerArray](../Classes/classshapeworks_1_1GenericContainerArray.md)< double >::Pointer | **[m_Sigma2Cache](../Classes/classshapeworks_1_1Sampler.md#variable-m-sigma2cache)**  |
| MeanCurvatureCacheType::Pointer | **[m_MeanCurvatureCache](../Classes/classshapeworks_1_1Sampler.md#variable-m-meancurvaturecache)**  |
| ParticleSystem::Pointer | **[m_ParticleSystem](../Classes/classshapeworks_1_1Sampler.md#variable-m-particlesystem)**  |
| std::vector< ParticleDomain::Pointer > | **[m_DomainList](../Classes/classshapeworks_1_1Sampler.md#variable-m-domainlist)**  |
| std::vector< ParticleSurfaceNeighborhood::Pointer > | **[m_NeighborhoodList](../Classes/classshapeworks_1_1Sampler.md#variable-m-neighborhoodlist)**  |
| int | **[m_pairwise_potential_type](../Classes/classshapeworks_1_1Sampler.md#variable-m-pairwise-potential-type)**  |
| shapeworks::CorrespondenceMode | **[m_CorrespondenceMode](../Classes/classshapeworks_1_1Sampler.md#variable-m-correspondencemode)**  |
| DualVectorFunction::Pointer | **[m_LinkingFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-linkingfunction)**  |
| LegacyCorrespondenceFunction::Pointer | **[m_EnsembleEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-ensembleentropyfunction)**  |
| LegacyCorrespondenceFunction::Pointer | **[m_EnsembleRegressionEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-ensembleregressionentropyfunction)**  |
| LegacyCorrespondenceFunction::Pointer | **[m_EnsembleMixedEffectsEntropyFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-ensemblemixedeffectsentropyfunction)**  |
| CorrespondenceFunction::Pointer | **[m_CorrespondenceFunction](../Classes/classshapeworks_1_1Sampler.md#variable-m-correspondencefunction)**  |
| LegacyShapeMatrix::Pointer | **[m_LegacyShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#variable-m-legacyshapematrix)**  |
| LinearRegressionShapeMatrix::Pointer | **[m_LinearRegressionShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#variable-m-linearregressionshapematrix)**  |
| MixedEffectsShapeMatrix::Pointer | **[m_MixedEffectsShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#variable-m-mixedeffectsshapematrix)**  |
| shapeworks::ShapeMatrix::Pointer | **[m_GeneralShapeMatrix](../Classes/classshapeworks_1_1Sampler.md#variable-m-generalshapematrix)**  |
| shapeworks::ShapeGradientMatrix::Pointer | **[m_GeneralShapeGradMatrix](../Classes/classshapeworks_1_1Sampler.md#variable-m-generalshapegradmatrix)**  |

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


## Public Functions Documentation

### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    ParticleSystem ,
    ParticleSystem 
)
```


Returns the particle system used in the surface sampling. 


### function itkGetConstObjectMacro

```cpp
itkGetConstObjectMacro(
    ParticleSystem ,
    ParticleSystem 
)
```


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

### function GetGradientFunction

```cpp
inline SamplingFunction * GetGradientFunction()
```


Returns a pointer to the gradient function used. 


### function GetCurvatureGradientFunction

```cpp
inline CurvatureSamplingFunction * GetCurvatureGradientFunction()
```


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    Optimizer ,
    OptimizerType 
)
```


Return a pointer to the optimizer object. 


### function itkGetConstObjectMacro

```cpp
itkGetConstObjectMacro(
    Optimizer ,
    OptimizerType 
)
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


### function SetMeshFile

```cpp
inline void SetMeshFile(
    unsigned int i,
    const std::string & s
)
```


Optionally provide a filename for a mesh with geodesic distances. 


### function SetMeshFile

```cpp
inline void SetMeshFile(
    const std::string & s
)
```


### function SetMeshFiles

```cpp
inline void SetMeshFiles(
    const std::vector< std::string > & s
)
```


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
    std::shared_ptr< shapeworks::MeshWrapper > mesh
)
```


### function AddContour

```cpp
void AddContour(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function SetFidsFiles

```cpp
inline void SetFidsFiles(
    const std::vector< std::string > & s
)
```


### function SetFeaFiles

```cpp
inline void SetFeaFiles(
    const std::vector< std::string > & s
)
```


### function SetFeaGradFiles

```cpp
inline void SetFeaGradFiles(
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
inline virtual void SetAdaptivityMode(
    int mode
)
```


This method sets the optimization function for the sampling. mode 0 = isotropic adaptivity mode 1 = no adaptivity 


### function GetAdaptivityMode

```cpp
inline int GetAdaptivityMode() const
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
inline virtual void SetCorrespondenceMode(
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
inline void SetAttributesPerDomain(
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


### function GetMeanCurvatureCache

```cpp
inline MeanCurvatureCacheType * GetMeanCurvatureCache()
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
virtual void AllocateDataCaches()
```


### function AllocateDomainsAndNeighborhoods

```cpp
virtual void AllocateDomainsAndNeighborhoods()
```


### function InitializeOptimizationFunctions

```cpp
virtual void InitializeOptimizationFunctions()
```


### function Initialize

```cpp
inline virtual void Initialize()
```


### function ReInitialize

```cpp
virtual void ReInitialize()
```


### function Execute

```cpp
virtual void Execute()
```


### function ComputeCuttingPlanes

```cpp
inline std::vector< std::vector< std::pair< Eigen::Vector3d, Eigen::Vector3d > > > ComputeCuttingPlanes()
```


### function ComputePlaneNormal

```cpp
inline Eigen::Vector3d ComputePlaneNormal(
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


## Protected Functions Documentation

### function GenerateData

```cpp
void GenerateData()
```


### function GetInitialized

```cpp
inline bool GetInitialized()
```


### function SetInitialized

```cpp
inline void SetInitialized(
    bool value
)
```


### function GetInitializing

```cpp
inline bool GetInitializing()
```


### function SetInitializing

```cpp
inline void SetInitializing(
    bool value
)
```


### function initialize_ffcs

```cpp
bool initialize_ffcs(
    size_t dom
)
```


## Public Attributes Documentation

### variable Dimension

```cpp
static constexpr unsigned int Dimension = 3;
```


## Protected Attributes Documentation

### variable m_Initialized

```cpp
bool m_Initialized {false};
```


### variable m_AdaptivityMode

```cpp
int m_AdaptivityMode {0};
```


### variable m_Initializing

```cpp
bool m_Initializing {false};
```


### variable m_Optimizer

```cpp
OptimizerType::Pointer m_Optimizer;
```


### variable m_GradientFunction

```cpp
SamplingFunction::Pointer m_GradientFunction;
```


### variable m_CurvatureGradientFunction

```cpp
CurvatureSamplingFunction::Pointer m_CurvatureGradientFunction;
```


### variable m_Sigma1Cache

```cpp
GenericContainerArray< double >::Pointer m_Sigma1Cache;
```


### variable m_Sigma2Cache

```cpp
GenericContainerArray< double >::Pointer m_Sigma2Cache;
```


### variable m_MeanCurvatureCache

```cpp
MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;
```


### variable m_ParticleSystem

```cpp
ParticleSystem::Pointer m_ParticleSystem;
```


### variable m_DomainList

```cpp
std::vector< ParticleDomain::Pointer > m_DomainList;
```


### variable m_NeighborhoodList

```cpp
std::vector< ParticleSurfaceNeighborhood::Pointer > m_NeighborhoodList;
```


### variable m_pairwise_potential_type

```cpp
int m_pairwise_potential_type;
```


### variable m_CorrespondenceMode

```cpp
shapeworks::CorrespondenceMode m_CorrespondenceMode;
```


### variable m_LinkingFunction

```cpp
DualVectorFunction::Pointer m_LinkingFunction;
```


### variable m_EnsembleEntropyFunction

```cpp
LegacyCorrespondenceFunction::Pointer m_EnsembleEntropyFunction;
```


### variable m_EnsembleRegressionEntropyFunction

```cpp
LegacyCorrespondenceFunction::Pointer m_EnsembleRegressionEntropyFunction;
```


### variable m_EnsembleMixedEffectsEntropyFunction

```cpp
LegacyCorrespondenceFunction::Pointer m_EnsembleMixedEffectsEntropyFunction;
```


### variable m_CorrespondenceFunction

```cpp
CorrespondenceFunction::Pointer m_CorrespondenceFunction;
```


### variable m_LegacyShapeMatrix

```cpp
LegacyShapeMatrix::Pointer m_LegacyShapeMatrix;
```


### variable m_LinearRegressionShapeMatrix

```cpp
LinearRegressionShapeMatrix::Pointer m_LinearRegressionShapeMatrix;
```


### variable m_MixedEffectsShapeMatrix

```cpp
MixedEffectsShapeMatrix::Pointer m_MixedEffectsShapeMatrix;
```


### variable m_GeneralShapeMatrix

```cpp
shapeworks::ShapeMatrix::Pointer m_GeneralShapeMatrix;
```


### variable m_GeneralShapeGradMatrix

```cpp
shapeworks::ShapeGradientMatrix::Pointer m_GeneralShapeGradMatrix;
```


-------------------------------

Updated on 2023-04-29 at 18:09:08 +0000