#pragma once

#include <Logging.h>
#include <Mesh/Mesh.h>

#include "CorrespondenceMode.h"
#include "GradientDescentOptimizer.h"
#include "Libs/Optimize/Container/GenericContainerArray.h"
#include "Libs/Optimize/Container/MeanCurvatureContainer.h"
#include "Libs/Optimize/Domain/Surface.h"
#include "Libs/Optimize/Function/CorrespondenceFunction.h"
#include "Libs/Optimize/Function/SamplingFunction.h"
#include "Libs/Optimize/Function/DisentangledCorrespondenceFunction.h"
#include "Libs/Optimize/Function/DualVectorFunction.h"
#include "Libs/Optimize/Function/LegacyCorrespondenceFunction.h"
#include "Libs/Optimize/Matrix/LinearRegressionShapeMatrix.h"
#include "Libs/Optimize/Matrix/MixedEffectsShapeMatrix.h"
#include "Libs/Optimize/Neighborhood/ParticleNeighborhood.h"
#include "ParticleSystem.h"
#include "vnl/vnl_matrix_fixed.h"
#include "EarlyStoppingConfig.h"

// Uncomment to visualize FFCs with scalar and vector fields
// #define VIZFFC

#if defined(VIZFFC)
#include "MeshUtils.h"
#endif

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

  using MeanCurvatureCacheType = MeanCurvatureContainer<PixelType, Dimension>;
  using TransformType = vnl_matrix_fixed<double, Dimension + 1, Dimension + 1>;
  using OptimizerType = GradientDescentOptimizer;

  /** Convenient typedef for storing cutting plane information */
  struct CuttingPlaneType {
    vnl_vector_fixed<double, 3> a;
    vnl_vector_fixed<double, 3> b;
    vnl_vector_fixed<double, 3> c;
  };

  /** Convenient typedef for storing sphere information */
  struct SphereType {
    vnl_vector_fixed<double, Dimension> center;
    double radius;
  };

  //! Constructor
  Sampler();

  //! Destructor
  virtual ~Sampler(){};

  //! Returns the particle system
  ParticleSystem* GetParticleSystem() { return m_ParticleSystem; }
  const ParticleSystem* GetParticleSystem() const { return m_ParticleSystem.GetPointer(); }

  std::shared_ptr<SamplingFunction> GetCurvatureGradientFunction() { return m_SamplingFunction; }

  //! Return a pointer to the optimizer object
  std::shared_ptr<OptimizerType> GetOptimizer() { return m_Optimizer; }
  std::shared_ptr<const OptimizerType> GetOptimizer() const { return m_Optimizer; }

  /**Optionally provide a filename for an initial point set.*/
  void SetPointsFile(unsigned int i, const std::string& s) {
    if (m_PointsFiles.size() < i + 1) {
      m_PointsFiles.resize(i + 1);
    }
    m_PointsFiles[i] = s;
  }

  void SetPointsFile(const std::string& s) { this->SetPointsFile(0, s); }

  //! Set initial particle positions (e.g. for fixed subjects)
  void SetInitialPoints(std::vector<std::vector<itk::Point<double>>> initial_points) {
    initial_points_ = initial_points;
  }

  void AddImage(ImageType::Pointer image, double narrow_band, std::string name = "");

  void ApplyConstraintsToZeroCrossing() {
    for (size_t i = 0; i < m_DomainList.size(); i++) {
      this->m_DomainList[i]->UpdateZeroCrossingPoint();
    }
  }

  void AddMesh(std::shared_ptr<shapeworks::Surface> mesh, double geodesic_remesh_percent = 100);

  void AddContour(vtkSmartPointer<vtkPolyData> poly_data);

  void SetFieldAttributes(const std::vector<std::string>& s);

  void SetDomainsPerShape(int n) {
    m_DomainsPerShape = n;
    m_LinearRegressionShapeMatrix->SetDomainsPerShape(n);
    m_MixedEffectsShapeMatrix->SetDomainsPerShape(n);
    m_LegacyShapeMatrix->SetDomainsPerShape(n);
    m_CorrespondenceFunction->SetDomainsPerShape(n);
    m_GeneralShapeMatrix->SetDomainsPerShape(n);
    m_GeneralShapeGradMatrix->SetDomainsPerShape(n);
  }

  /** Optionally supply a cutting plane that will be set as a particle
  optimization constraint in the image domains. */
  void SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, Dimension>& va,
                       const vnl_vector_fixed<double, Dimension>& vb, const vnl_vector_fixed<double, Dimension>& vc);
  void AddFreeFormConstraint(int domain, const FreeFormConstraint& ffc);

  /** Transform a cutting plane based on procrustes transformation */
  void TransformCuttingPlanes(unsigned int i);

  /** Optionally add spheres that may be used as constraints to the domain. */
  void AddSphere(unsigned int i, vnl_vector_fixed<double, Dimension>& c, double r);

  void SetAdaptivityMode() { m_LinkingFunction->set_function_a(GetCurvatureGradientFunction()); }

  void SetCorrespondenceOn() { m_LinkingFunction->set_b_on(); }

  void SetCorrespondenceOff() { m_LinkingFunction->set_b_off(); }

  void SetSamplingOn() { m_LinkingFunction->set_a_on(); }

  void SetSamplingOff() { m_LinkingFunction->set_a_off(); }

  bool GetCorrespondenceOn() const { return m_LinkingFunction->get_b_on(); }

  bool GetSamplingOn() const { return m_LinkingFunction->get_a_on(); }

  /** This method sets the optimization function for correspondences between surfaces (domains). */
  void SetCorrespondenceMode(shapeworks::CorrespondenceMode mode);

  void RegisterGeneralShapeMatrices() {
    this->m_ParticleSystem->RegisterObserver(m_GeneralShapeMatrix);
    this->m_ParticleSystem->RegisterObserver(m_GeneralShapeGradMatrix);
  }

  void SetAttributeScales(const std::vector<double>& s) {
    m_CorrespondenceFunction->SetAttributeScales(s);
    m_GeneralShapeMatrix->SetAttributeScales(s);
    m_GeneralShapeGradMatrix->SetAttributeScales(s);
  }

  void SetXYZ(unsigned int i, bool flag) {
    m_CorrespondenceFunction->SetXYZ(i, flag);
    m_GeneralShapeMatrix->SetXYZ(i, flag);
    m_GeneralShapeGradMatrix->SetXYZ(i, flag);
  }

  void SetNormals(int i, bool flag) {
    m_CorrespondenceFunction->SetNormals(i, flag);
    m_GeneralShapeMatrix->SetNormals(i, flag);
    m_GeneralShapeGradMatrix->SetNormals(i, flag);
  }

  void SetAttributesPerDomain(const std::vector<int> s);

  LegacyShapeMatrix* GetShapeMatrix() { return m_LegacyShapeMatrix.GetPointer(); }

  ShapeMatrix* GetGeneralShapeMatrix() { return m_GeneralShapeMatrix.GetPointer(); }
  ShapeGradientMatrix* GetGeneralShapeGradientMatrix() { return m_GeneralShapeGradMatrix.GetPointer(); }

  DualVectorFunction* GetLinkingFunction() { return m_LinkingFunction.get(); }

  LegacyCorrespondenceFunction* GetEnsembleEntropyFunction() { return m_EnsembleEntropyFunction.get(); }

  DisentangledCorrespondenceFunction* GetDisentangledEnsembleEntropyFunction() {
    return m_DisentangledEnsembleEntropyFunction.get();
  }

  LegacyCorrespondenceFunction* GetEnsembleRegressionEntropyFunction() {
    return m_EnsembleRegressionEntropyFunction.get();
  }

  LegacyCorrespondenceFunction* GetEnsembleMixedEffectsEntropyFunction() {
    return m_EnsembleMixedEffectsEntropyFunction.get();
  }

  CorrespondenceFunction* GetMeshBasedGeneralEntropyGradientFunction() { return m_CorrespondenceFunction.get(); }

  const DualVectorFunction* GetLinkingFunction() const { return m_LinkingFunction.get(); }

  const LegacyCorrespondenceFunction* GetEnsembleEntropyFunction() const {
    return m_EnsembleEntropyFunction.get();
  }

  const DisentangledCorrespondenceFunction* GetDisentangledEnsembleEntropyFunction() const {
    return m_DisentangledEnsembleEntropyFunction.get();
  }

  const LegacyCorrespondenceFunction* GetEnsembleRegressionEntropyFunction() const {
    return m_EnsembleRegressionEntropyFunction.get();
  }

  const LegacyCorrespondenceFunction* GetEnsembleMixedEffectsEntropyFunction() const {
    return m_EnsembleMixedEffectsEntropyFunction.get();
  }

  const CorrespondenceFunction* GetMeshBasedGeneralEntropyGradientFunction() const {
    return m_CorrespondenceFunction.get();
  }

  void SetTimeptsPerIndividual(int n) { m_MixedEffectsShapeMatrix->SetTimeptsPerIndividual(n); }

  shapeworks::CorrespondenceMode GetCorrespondenceMode() const { return m_CorrespondenceMode; }

  void SetTransformFile(const std::string& s) { m_TransformFile = s; }

  void SetTransformFile(const char* s) { m_TransformFile = std::string(s); }

  void SetPrefixTransformFile(const std::string& s) { m_PrefixTransformFile = s; }

  void SetPrefixTransformFile(const char* s) { m_PrefixTransformFile = std::string(s); }

  void SetPairwisePotentialType(int pairwise_potential_type) { m_pairwise_potential_type = pairwise_potential_type; }

  int GetPairwisePotentialType() { return m_pairwise_potential_type; }

  void SetVerbosity(unsigned int val) {
    m_verbosity = val;
    m_Optimizer->set_verbosity(val);
  }

  unsigned int GetVerbosity() { return m_verbosity; }

  void SetSharedBoundaryEnabled(bool enabled) { m_IsSharedBoundaryEnabled = enabled; }
  void SetSharedBoundaryWeight(double weight) { m_SharedBoundaryWeight = weight; }

  void SetSamplingScale(bool enabled) { m_SamplingScale = enabled; }
  void SetSamplingAutoScale(bool auto_scale) { m_SamplingAutoScale = auto_scale; }
  void SetSamplingScaleValue(double scale_value) { m_SamplingScaleValue = scale_value; }

  void SetEarlyStoppingConfig(EarlyStoppingConfig config) { early_stopping_config_ = config; }

  void ReadTransforms();
  void ReadPointsFiles();
  void AllocateDataCaches();
  void AllocateDomainsAndNeighborhoods();
  void InitializeOptimizationFunctions();

  void initialize_initial_positions();

  /** */
  void Initialize() {
    this->m_Initializing = true;
    this->Execute();
    this->m_Initializing = false;
  }

  void ReInitialize();

  void Execute();

  using CuttingPlaneList = std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>>>;

  CuttingPlaneList ComputeCuttingPlanes();

  Eigen::Vector3d ComputePlaneNormal(const vnl_vector<double>& a, const vnl_vector<double>& b,
                                     const vnl_vector<double>& c);

  std::vector<FreeFormConstraint> GetFFCs() { return m_FFCs; }

  void SetMeshFFCMode(bool mesh_ffc_mode) { m_meshFFCMode = mesh_ffc_mode; }

 private:
  bool GetInitialized() { return this->m_Initialized; }

  void SetInitialized(bool value) { this->m_Initialized = value; }

  bool GetInitializing() { return this->m_Initializing; }

  void SetInitializing(bool value) { this->m_Initializing = value; }

  bool m_Initialized{false};
  bool m_Initializing{false};

  std::shared_ptr<OptimizerType> m_Optimizer;

  std::shared_ptr<SamplingFunction> m_SamplingFunction;

  GenericContainerArray<double>::Pointer m_Sigma1Cache;

  ParticleSystem::Pointer m_ParticleSystem;

  std::vector<ParticleDomain::Pointer> m_DomainList;

  int m_pairwise_potential_type;

  shapeworks::CorrespondenceMode m_CorrespondenceMode;

  std::shared_ptr<DualVectorFunction> m_LinkingFunction;

  std::shared_ptr<LegacyCorrespondenceFunction> m_EnsembleEntropyFunction;
  std::shared_ptr<LegacyCorrespondenceFunction> m_EnsembleRegressionEntropyFunction;
  std::shared_ptr<LegacyCorrespondenceFunction> m_EnsembleMixedEffectsEntropyFunction;
  std::shared_ptr<DisentangledCorrespondenceFunction> m_DisentangledEnsembleEntropyFunction;
  std::shared_ptr<CorrespondenceFunction> m_CorrespondenceFunction;

  LegacyShapeMatrix::Pointer m_LegacyShapeMatrix;

  LinearRegressionShapeMatrix::Pointer m_LinearRegressionShapeMatrix;
  MixedEffectsShapeMatrix::Pointer m_MixedEffectsShapeMatrix;

  shapeworks::ShapeMatrix::Pointer m_GeneralShapeMatrix;
  shapeworks::ShapeGradientMatrix::Pointer m_GeneralShapeGradMatrix;

  bool initialize_ffcs(size_t dom);

 private:
  Sampler(const Sampler&);         // purposely not implemented
  void operator=(const Sampler&);  // purposely not implemented

  std::vector<std::string> m_PointsFiles;
  std::vector<int> m_AttributesPerDomain;
  int m_DomainsPerShape;
  double m_Spacing{0};
  bool m_IsSharedBoundaryEnabled;
  double m_SharedBoundaryWeight{0.5};

  bool m_SamplingScale{true};
  bool m_SamplingAutoScale{true};
  double m_SamplingScaleValue{1.0};

  std::string m_TransformFile;
  std::string m_PrefixTransformFile;
  std::vector<std::vector<CuttingPlaneType>> m_CuttingPlanes;
  std::vector<std::vector<SphereType>> m_Spheres;
  std::vector<FreeFormConstraint> m_FFCs;
  std::vector<vtkSmartPointer<vtkPolyData>> m_meshes;
  bool m_meshFFCMode = false;

  std::vector<std::string> fieldAttributes_;

  std::vector<std::vector<itk::Point<double>>> initial_points_;

  EarlyStoppingConfig early_stopping_config_; // config for early stopping

  unsigned int m_verbosity;
};

}  // namespace shapeworks
