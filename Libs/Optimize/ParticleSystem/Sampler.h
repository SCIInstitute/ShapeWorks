#pragma once

#include "itkParticleSystem.h"
#include "itkParticleGradientDescentPositionOptimizer.h"
#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "itkParticleContainerArrayAttribute.h"
#include "itkParticleCurvatureEntropyGradientFunction.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkParticleSurfaceNeighborhood.h"
#include "itkParticleOmegaGradientFunction.h"
#include "DomainType.h"
#include "MeshWrapper.h"
#include "MeshDomain.h"

#include "CorrespondenceMode.h"
#include "Sampler.h"
#include "itkParticleDualVectorFunction.h"
#include "itkParticleEnsembleEntropyFunction.h"
#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticleShapeMixedEffectsMatrixAttribute.h"
#include "itkParticleMeshBasedGeneralEntropyGradientFunction.h"

#include "itkParticleModifiedCotangentEntropyGradientFunction.h"
#include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h"
#include "vnl/vnl_matrix_fixed.h"

#include "TriMesh.h"

namespace shapeworks {

/** \class Sampler
 *
 *
 *
 */
class Sampler {

public:
  using PixelType = float;
  static constexpr unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;
  using PointType = ImageType::PointType;

  using MeanCurvatureCacheType = itk::ParticleMeanCurvatureAttribute<PixelType, Dimension>;
  using TransformType = vnl_matrix_fixed<double, Dimension + 1, Dimension + 1>;
  using OptimizerType = itk::ParticleGradientDescentPositionOptimizer<PixelType, Dimension>;

  /** Convenient typedef for storing cutting plane information */
  struct CuttingPlaneType {
    vnl_vector_fixed<double, Dimension> a;
    vnl_vector_fixed<double, Dimension> b;
    vnl_vector_fixed<double, Dimension> c;
  };

  /** Convenient typedef for storing sphere information */
  struct SphereType {
    vnl_vector_fixed<double, Dimension> center;
    double radius;
  };

  /** Returns the particle system used in the surface sampling. */
  itkGetObjectMacro(ParticleSystem, itk::ParticleSystem<Dimension>);

  itkGetConstObjectMacro(ParticleSystem, itk::ParticleSystem<Dimension>);

  //! Constructor
  Sampler();

  //! Destructor
  virtual ~Sampler()
  {};

  /** Returns a pointer to the gradient function used. */
  itk::ParticleEntropyGradientFunction<ImageType::PixelType, Dimension>*
  GetGradientFunction()
  { return m_GradientFunction; }

  itk::ParticleCurvatureEntropyGradientFunction<ImageType::PixelType, Dimension>*
  GetCurvatureGradientFunction()
  { return m_CurvatureGradientFunction; }

  itk::ParticleModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>*
  GetModifiedCotangentGradientFunction()
  { return m_ModifiedCotangentGradientFunction; }

  itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>*
  GetConstrainedModifiedCotangentGradientFunction()
  { return m_ConstrainedModifiedCotangentGradientFunction; }

  itk::ParticleOmegaGradientFunction<ImageType::PixelType, Dimension>*
  GetOmegaGradientFunction()
  { return m_OmegaGradientFunction; }

  /** Return a pointer to the optimizer object. */
  itkGetObjectMacro(Optimizer, OptimizerType);

  itkGetConstObjectMacro(Optimizer, OptimizerType);

  /**Optionally provide a filename for an initial point set.*/
  void SetPointsFile(unsigned int i, const std::string& s)
  {
    if (m_PointsFiles.size() < i + 1) {
      m_PointsFiles.resize(i + 1);
    }
    m_PointsFiles[i] = s;
  }

  void SetPointsFile(const std::string& s)
  { this->SetPointsFile(0, s); }

  /**Optionally provide a filename for a mesh with geodesic distances.*/
  void SetMeshFile(unsigned int i, const std::string& s)
  {
    if (m_MeshFiles.size() < i + 1) {
      m_MeshFiles.resize(i + 1);
    }
    m_MeshFiles[i] = s;
  }

  void SetMeshFile(const std::string& s)
  { this->SetMeshFile(0, s); }

  void SetMeshFiles(const std::vector<std::string>& s)
  { m_MeshFiles = s; }

  void AddImage(ImageType::Pointer image, double narrow_band);

  void AddMesh(shapeworks::MeshWrapper* mesh);

  void SetFidsFiles(const std::vector<std::string>& s)
  { m_FidsFiles = s; }

  void SetFeaFiles(const std::vector<std::string>& s)
  { m_FeaMeshFiles = s; }

  void SetFeaGradFiles(const std::vector<std::string>& s)
  { m_FeaGradFiles = s; }

  void SetDomainsPerShape(int n)
  {
    m_DomainsPerShape = n;
    m_LinearRegressionShapeMatrix->SetDomainsPerShape(n);
    m_MixedEffectsShapeMatrix->SetDomainsPerShape(n);
    m_ShapeMatrix->SetDomainsPerShape(n);
    m_MeshBasedGeneralEntropyGradientFunction->SetDomainsPerShape(n);
    m_GeneralShapeMatrix->SetDomainsPerShape(n);
    m_GeneralShapeGradMatrix->SetDomainsPerShape(n);
  }

  /** Optionally supply a cutting plane that will be set as a particle
  optimization constraint in the image domains. */
  void SetCuttingPlane(unsigned int i,
                       const vnl_vector_fixed<double, Dimension>& va,
                       const vnl_vector_fixed<double, Dimension>& vb,
                       const vnl_vector_fixed<double, Dimension>& vc);

  /** Transform a cutting plane based on procrustes transformation */
  void TransformCuttingPlanes(unsigned int i);

  /** Optionally add spheres that may be used as constraints to the domain. */
  void AddSphere(unsigned int i, vnl_vector_fixed<double, Dimension>& c, double r);

  /** This method sets the optimization function for the sampling.
      mode 0 = isotropic adaptivity
      mode 1 = no adaptivity
  */
  virtual void SetAdaptivityMode(int mode)
  {
    if (mode == 0) {
      if (this->m_pairwise_potential_type == 0)
        m_LinkingFunction->SetFunctionA(this->GetCurvatureGradientFunction());
      else if (this->m_pairwise_potential_type == 1)
        m_LinkingFunction->SetFunctionA(this->GetModifiedCotangentGradientFunction());
    }
    else if (mode == 1) {
      m_LinkingFunction->SetFunctionA(this->GetGradientFunction());
    }
    else if (mode == 3) {
      if (this->m_pairwise_potential_type == 0)
        m_LinkingFunction->SetFunctionA(this->GetOmegaGradientFunction());
      else if (this->m_pairwise_potential_type == 1)
        m_LinkingFunction->SetFunctionA(this->GetConstrainedModifiedCotangentGradientFunction());
    }

    this->m_AdaptivityMode = mode;
  }

  int GetAdaptivityMode() const
  { return m_AdaptivityMode; }

  void SetCorrespondenceOn()
  { m_LinkingFunction->SetBOn(); }

  void SetCorrespondenceOff()
  { m_LinkingFunction->SetBOff(); }

  void SetSamplingOn()
  { m_LinkingFunction->SetAOn(); }

  void SetSamplingOff()
  { m_LinkingFunction->SetAOff(); }

  bool GetCorrespondenceOn() const
  { return m_LinkingFunction->GetBOn(); }

  bool GetSamplingOn() const
  { return m_LinkingFunction->GetAOn(); }

  /** This method sets the optimization function for correspondences between surfaces (domains). */
  virtual void SetCorrespondenceMode(shapeworks::CorrespondenceMode mode)
  {
    if (mode == shapeworks::CorrespondenceMode::MeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseMeanEnergy();
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseEntropy();
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleRegressionEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleRegressionEntropyFunction);
    }
    else if (mode == shapeworks::CorrespondenceMode::EnsembleMixedEffectsEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleMixedEffectsEntropyFunction);
    }
    else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy) {
      m_LinkingFunction->SetFunctionB(m_MeshBasedGeneralEntropyGradientFunction);
      m_MeshBasedGeneralEntropyGradientFunction->UseEntropy();
    }
    else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralMeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_MeshBasedGeneralEntropyGradientFunction);
      m_MeshBasedGeneralEntropyGradientFunction->UseMeanEnergy();
    }

    m_CorrespondenceMode = mode;
  }

  void RegisterGeneralShapeMatrices()
  {
    this->m_ParticleSystem->RegisterAttribute(m_GeneralShapeMatrix);
    this->m_ParticleSystem->RegisterAttribute(m_GeneralShapeGradMatrix);
  }

  void SetAttributeScales(const std::vector<double>& s)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetAttributeScales(s);
    m_GeneralShapeMatrix->SetAttributeScales(s);
    m_GeneralShapeGradMatrix->SetAttributeScales(s);
  }

  void SetXYZ(unsigned int i, bool flag)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetXYZ(i, flag);
    m_GeneralShapeMatrix->SetXYZ(i, flag);
    m_GeneralShapeGradMatrix->SetXYZ(i, flag);
  }

  void SetNormals(int i, bool flag)
  {
    m_MeshBasedGeneralEntropyGradientFunction->SetNormals(i, flag);
    m_GeneralShapeMatrix->SetNormals(i, flag);
    m_GeneralShapeGradMatrix->SetNormals(i, flag);
  }

  void SetAttributesPerDomain(const std::vector<int> s)
  {
    std::vector<int> s1;
    if (s.size() == 0) {
      s1.resize(m_MeshBasedGeneralEntropyGradientFunction->GetDomainsPerShape());
      for (int i = 0; i < m_MeshBasedGeneralEntropyGradientFunction->GetDomainsPerShape(); i++)
        s1[i] = 0;
    }
    else
      s1 = s;

    m_AttributesPerDomain = s1;
    m_MeshBasedGeneralEntropyGradientFunction->SetAttributesPerDomain(s1);
    m_GeneralShapeMatrix->SetAttributesPerDomain(s1);
    m_GeneralShapeGradMatrix->SetAttributesPerDomain(s1);
  }

  itk::ParticleShapeMatrixAttribute<double, Dimension>* GetShapeMatrix()
  {
    return m_ShapeMatrix.GetPointer();
  }

  itk::ParticleGeneralShapeMatrix<double, Dimension>* GetGeneralShapeMatrix()
  {
    return m_GeneralShapeMatrix.GetPointer();
  }

  itk::ParticleGeneralShapeGradientMatrix<double, Dimension>* GetGeneralShapeGradientMatrix()
  {
    return m_GeneralShapeGradMatrix.GetPointer();
  }

  itk::ParticleDualVectorFunction<Dimension>* GetLinkingFunction()
  { return m_LinkingFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleEntropyFunction()
  { return m_EnsembleEntropyFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleRegressionEntropyFunction()
  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleMixedEffectsEntropyFunction()
  { return m_EnsembleMixedEffectsEntropyFunction.GetPointer(); }

  itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>*
  GetMeshBasedGeneralEntropyGradientFunction()
  { return m_MeshBasedGeneralEntropyGradientFunction.GetPointer(); }

  const itk::ParticleDualVectorFunction<Dimension>* GetLinkingFunction() const
  { return m_LinkingFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>* GetEnsembleEntropyFunction() const
  { return m_EnsembleEntropyFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>*
  GetEnsembleRegressionEntropyFunction() const
  { return m_EnsembleRegressionEntropyFunction.GetPointer(); }

  const itk::ParticleEnsembleEntropyFunction<Dimension>*
  GetEnsembleMixedEffectsEntropyFunction() const
  { return m_EnsembleMixedEffectsEntropyFunction.GetPointer(); }

  const itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>*
  GetMeshBasedGeneralEntropyGradientFunction() const
  { return m_MeshBasedGeneralEntropyGradientFunction.GetPointer(); }

  void SetTimeptsPerIndividual(int n)
  {
    m_MixedEffectsShapeMatrix->SetTimeptsPerIndividual(n);
  }

  shapeworks::CorrespondenceMode GetCorrespondenceMode() const
  { return m_CorrespondenceMode; }

  void SetTransformFile(const std::string& s)
  { m_TransformFile = s; }

  void SetTransformFile(const char* s)
  { m_TransformFile = std::string(s); }

  void SetPrefixTransformFile(const std::string& s)
  { m_PrefixTransformFile = s; }

  void SetPrefixTransformFile(const char* s)
  { m_PrefixTransformFile = std::string(s); }

  void SetPairwisePotentialType(int pairwise_potential_type)
  { m_pairwise_potential_type = pairwise_potential_type; }

  int GetPairwisePotentialType()
  { return m_pairwise_potential_type; }

  void SetVerbosity(unsigned int val)
  {
    m_verbosity = val;
    m_Optimizer->SetVerbosity(val);
  }

  unsigned int GetVerbosity()
  { return m_verbosity; }

  MeanCurvatureCacheType* GetMeanCurvatureCache()
  { return m_MeanCurvatureCache.GetPointer(); }

  void ReadTransforms();
  void ReadPointsFiles();
  virtual void AllocateDataCaches();
  virtual void AllocateDomainsAndNeighborhoods();
  virtual void InitializeOptimizationFunctions();

/** */
  virtual void Initialize()
  {
    this->m_Initializing = true;
    this->Execute();
    this->m_Initializing = false;
  }

  virtual void ReInitialize();

  virtual void Execute();

protected:

  void GenerateData();

  bool GetInitialized()
  {
    return this->m_Initialized;
  }

  void SetInitialized(bool value)
  {
    this->m_Initialized = value;
  }

  bool GetInitializing()
  {
    return this->m_Initializing;
  }

  void SetInitializing(bool value)
  {
    this->m_Initializing = value;
  }

  bool m_Initialized{false};
  int m_AdaptivityMode{0};
  bool m_Initializing{false};

  OptimizerType::Pointer m_Optimizer;

  itk::ParticleEntropyGradientFunction<ImageType::PixelType, Dimension>::Pointer m_GradientFunction;
  itk::ParticleCurvatureEntropyGradientFunction<ImageType::PixelType, Dimension>::Pointer m_CurvatureGradientFunction;

  itk::ParticleModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>
  ::Pointer m_ModifiedCotangentGradientFunction;
  itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>
  ::Pointer m_ConstrainedModifiedCotangentGradientFunction;

  itk::ParticleOmegaGradientFunction<ImageType::PixelType, Dimension>::Pointer m_OmegaGradientFunction;

  itk::ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma1Cache;
  itk::ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma2Cache;

  MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;

  itk::ParticleSystem<Dimension>::Pointer m_ParticleSystem;

  std::vector<itk::ParticleDomain::Pointer> m_DomainList;

  std::vector<itk::ParticleSurfaceNeighborhood<ImageType>::Pointer> m_NeighborhoodList;

  int m_pairwise_potential_type;

  shapeworks::CorrespondenceMode m_CorrespondenceMode;

  itk::ParticleDualVectorFunction<Dimension>::Pointer m_LinkingFunction;

  itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleEntropyFunction;
  itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleRegressionEntropyFunction;
  itk::ParticleEnsembleEntropyFunction<Dimension>::Pointer m_EnsembleMixedEffectsEntropyFunction;

  itk::ParticleShapeMatrixAttribute<double, Dimension>::Pointer m_ShapeMatrix;

  itk::ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::Pointer m_LinearRegressionShapeMatrix;
  itk::ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::Pointer m_MixedEffectsShapeMatrix;

  itk::ParticleGeneralShapeMatrix<double, Dimension>::Pointer m_GeneralShapeMatrix;
  itk::ParticleGeneralShapeGradientMatrix<double, Dimension>::Pointer m_GeneralShapeGradMatrix;

  itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::Pointer m_MeshBasedGeneralEntropyGradientFunction;

private:
  Sampler(const Sampler&); //purposely not implemented
  void operator=(const Sampler&); //purposely not implemented

  std::vector<std::string> m_PointsFiles;
  std::vector<std::string> m_MeshFiles;
  std::vector<std::string> m_FeaMeshFiles;
  std::vector<std::string> m_FeaGradFiles;
  std::vector<std::string> m_FidsFiles;
  std::vector<int> m_AttributesPerDomain;
  int m_DomainsPerShape;
  double m_Spacing{0};

  std::string m_TransformFile;
  std::string m_PrefixTransformFile;
  std::vector<std::vector<CuttingPlaneType> > m_CuttingPlanes;
  std::vector<std::vector<SphereType> > m_Spheres;

  unsigned int m_verbosity;

};

} // end namespace





