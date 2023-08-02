---
title: Libs/Optimize/Sampler.h

---

# Libs/Optimize/Sampler.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Sampler](../Classes/classshapeworks_1_1Sampler.md)**  |
| struct | **[shapeworks::Sampler::CuttingPlaneType](../Classes/structshapeworks_1_1Sampler_1_1CuttingPlaneType.md)**  |
| struct | **[shapeworks::Sampler::SphereType](../Classes/structshapeworks_1_1Sampler_1_1SphereType.md)**  |




## Source code

```cpp
#pragma once

#include <Logging.h>
#include <Mesh/Mesh.h>

#include "CorrespondenceMode.h"
#include "GradientDescentOptimizer.h"
#include "Libs/Optimize/Container/GenericContainerArray.h"
#include "Libs/Optimize/Container/MeanCurvatureContainer.h"
#include "Libs/Optimize/Domain/DomainType.h"
#include "Libs/Optimize/Domain/ImplicitSurfaceDomain.h"
#include "Libs/Optimize/Domain/MeshDomain.h"
#include "Libs/Optimize/Domain/MeshWrapper.h"
#include "Libs/Optimize/Function/CorrespondenceFunction.h"
#include "Libs/Optimize/Function/DisentangledCorrespondenceFunction.h"
#include "Libs/Optimize/Function/CurvatureSamplingFunction.h"
#include "Libs/Optimize/Function/DualVectorFunction.h"
#include "Libs/Optimize/Function/LegacyCorrespondenceFunction.h"
#include "Libs/Optimize/Function/SamplingFunction.h"
#include "Libs/Optimize/Matrix/LinearRegressionShapeMatrix.h"
#include "Libs/Optimize/Matrix/MixedEffectsShapeMatrix.h"
#include "Libs/Optimize/Neighborhood/ParticleSurfaceNeighborhood.h"
#include "ParticleSystem.h"
#include "TriMesh.h"
#include "vnl/vnl_matrix_fixed.h"

// Uncomment to visualize FFCs with scalar and vector fields
// #define VIZFFC

#if defined(VIZFFC)
#include "MeshUtils.h"
#endif

namespace shapeworks {

class Sampler {
 public:
  using PixelType = float;
  static constexpr unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;
  using PointType = ImageType::PointType;

  using MeanCurvatureCacheType = MeanCurvatureContainer<PixelType, Dimension>;
  using TransformType = vnl_matrix_fixed<double, Dimension + 1, Dimension + 1>;
  using OptimizerType = GradientDescentOptimizer;

  struct CuttingPlaneType {
    vnl_vector_fixed<double, 3> a;
    vnl_vector_fixed<double, 3> b;
    vnl_vector_fixed<double, 3> c;
  };

  struct SphereType {
    vnl_vector_fixed<double, Dimension> center;
    double radius;
  };

  itkGetObjectMacro(ParticleSystem, ParticleSystem);

  itkGetConstObjectMacro(ParticleSystem, ParticleSystem);

  Sampler();

  virtual ~Sampler(){};

  SamplingFunction* GetGradientFunction() {
    return m_GradientFunction;
  }

  CurvatureSamplingFunction* GetCurvatureGradientFunction() {
    return m_CurvatureGradientFunction;
  }

  itkGetObjectMacro(Optimizer, OptimizerType);

  itkGetConstObjectMacro(Optimizer, OptimizerType);

  void SetPointsFile(unsigned int i, const std::string& s) {
    if (m_PointsFiles.size() < i + 1) {
      m_PointsFiles.resize(i + 1);
    }
    m_PointsFiles[i] = s;
  }

  void SetPointsFile(const std::string& s) { this->SetPointsFile(0, s); }

  void SetMeshFile(unsigned int i, const std::string& s) {
    if (m_MeshFiles.size() < i + 1) {
      m_MeshFiles.resize(i + 1);
    }
    m_MeshFiles[i] = s;
  }

  void SetMeshFile(const std::string& s) { this->SetMeshFile(0, s); }

  void SetMeshFiles(const std::vector<std::string>& s) { m_MeshFiles = s; }

  void AddImage(ImageType::Pointer image, double narrow_band, std::string name = "");

  void ApplyConstraintsToZeroCrossing() {
    for (size_t i = 0; i < m_DomainList.size(); i++) {
      this->m_DomainList[i]->UpdateZeroCrossingPoint();
    }
  }

  void AddMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh);

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

  void SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, Dimension>& va,
                       const vnl_vector_fixed<double, Dimension>& vb, const vnl_vector_fixed<double, Dimension>& vc);
  void AddFreeFormConstraint(int domain, const FreeFormConstraint& ffc);

  void TransformCuttingPlanes(unsigned int i);

  void AddSphere(unsigned int i, vnl_vector_fixed<double, Dimension>& c, double r);

  virtual void SetAdaptivityMode(int mode) {
    //SW_LOG("SetAdaptivityMode: {}, pairwise_potential_type: {}", mode, m_pairwise_potential_type);
    if (mode == 0) {
      m_LinkingFunction->SetFunctionA(this->GetCurvatureGradientFunction());
    } else if (mode == 1) {
      m_LinkingFunction->SetFunctionA(this->GetGradientFunction());
    }

    this->m_AdaptivityMode = mode;
  }

  int GetAdaptivityMode() const { return m_AdaptivityMode; }

  void SetCorrespondenceOn() { m_LinkingFunction->SetBOn(); }

  void SetCorrespondenceOff() { m_LinkingFunction->SetBOff(); }

  void SetSamplingOn() { m_LinkingFunction->SetAOn(); }

  void SetSamplingOff() { m_LinkingFunction->SetAOff(); }

  bool GetCorrespondenceOn() const { return m_LinkingFunction->GetBOn(); }

  bool GetSamplingOn() const { return m_LinkingFunction->GetAOn(); }

  virtual void SetCorrespondenceMode(shapeworks::CorrespondenceMode mode) {
    if (mode == shapeworks::CorrespondenceMode::MeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseMeanEnergy();
    } else if (mode == shapeworks::CorrespondenceMode::EnsembleEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleEntropyFunction);
      m_EnsembleEntropyFunction->UseEntropy();
    } else if (mode == shapeworks::CorrespondenceMode::EnsembleRegressionEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleRegressionEntropyFunction);
    } else if (mode == shapeworks::CorrespondenceMode::EnsembleMixedEffectsEntropy) {
      m_LinkingFunction->SetFunctionB(m_EnsembleMixedEffectsEntropyFunction);
    } else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy) {
      m_LinkingFunction->SetFunctionB(m_CorrespondenceFunction);
      m_CorrespondenceFunction->UseEntropy();
    } else if (mode == shapeworks::CorrespondenceMode::MeshBasedGeneralMeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_CorrespondenceFunction);
      m_CorrespondenceFunction->UseMeanEnergy();
    } else if (mode == shapeworks::CorrespondenceMode::DisentagledEnsembleEntropy) {
      m_LinkingFunction->SetFunctionB(m_DisentangledEnsembleEntropyFunction);
      m_DisentangledEnsembleEntropyFunction->UseEntropy();
    } else if (mode == shapeworks::CorrespondenceMode::DisentangledEnsembleMeanEnergy) {
      m_LinkingFunction->SetFunctionB(m_DisentangledEnsembleEntropyFunction);
      m_DisentangledEnsembleEntropyFunction->UseMeanEnergy();
    }

    m_CorrespondenceMode = mode;
  }

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

  void SetAttributesPerDomain(const std::vector<int> s) {
    std::vector<int> s1;
    if (s.size() == 0) {
      s1.resize(m_CorrespondenceFunction->GetDomainsPerShape());
      for (int i = 0; i < m_CorrespondenceFunction->GetDomainsPerShape(); i++) s1[i] = 0;
    } else
      s1 = s;

    m_AttributesPerDomain = s1;
    m_CorrespondenceFunction->SetAttributesPerDomain(s1);
    m_GeneralShapeMatrix->SetAttributesPerDomain(s1);
    m_GeneralShapeGradMatrix->SetAttributesPerDomain(s1);
  }

  LegacyShapeMatrix* GetShapeMatrix() { return m_LegacyShapeMatrix.GetPointer(); }

  ShapeMatrix* GetGeneralShapeMatrix() { return m_GeneralShapeMatrix.GetPointer(); }
  ShapeGradientMatrix* GetGeneralShapeGradientMatrix() { return m_GeneralShapeGradMatrix.GetPointer(); }

  DualVectorFunction* GetLinkingFunction() { return m_LinkingFunction.GetPointer(); }

  LegacyCorrespondenceFunction* GetEnsembleEntropyFunction() { return m_EnsembleEntropyFunction.GetPointer(); }

  DisentangledCorrespondenceFunction* GetDisentangledEnsembleEntropyFunction() { return m_DisentangledEnsembleEntropyFunction.GetPointer(); }

  LegacyCorrespondenceFunction* GetEnsembleRegressionEntropyFunction() {
    return m_EnsembleRegressionEntropyFunction.GetPointer();
  }

  LegacyCorrespondenceFunction* GetEnsembleMixedEffectsEntropyFunction() {
    return m_EnsembleMixedEffectsEntropyFunction.GetPointer();
  }

  CorrespondenceFunction* GetMeshBasedGeneralEntropyGradientFunction() {
    return m_CorrespondenceFunction.GetPointer();
  }

  const DualVectorFunction* GetLinkingFunction() const { return m_LinkingFunction.GetPointer(); }

  const LegacyCorrespondenceFunction* GetEnsembleEntropyFunction() const {
    return m_EnsembleEntropyFunction.GetPointer();
  }

  const DisentangledCorrespondenceFunction* GetDisentangledEnsembleEntropyFunction() const {
    return m_DisentangledEnsembleEntropyFunction.GetPointer();
  }

  const LegacyCorrespondenceFunction* GetEnsembleRegressionEntropyFunction() const {
    return m_EnsembleRegressionEntropyFunction.GetPointer();
  }

  const LegacyCorrespondenceFunction* GetEnsembleMixedEffectsEntropyFunction() const {
    return m_EnsembleMixedEffectsEntropyFunction.GetPointer();
  }

  const CorrespondenceFunction* GetMeshBasedGeneralEntropyGradientFunction() const {
    return m_CorrespondenceFunction.GetPointer();
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
    m_Optimizer->SetVerbosity(val);
  }

  unsigned int GetVerbosity() { return m_verbosity; }

  MeanCurvatureCacheType* GetMeanCurvatureCache() { return m_MeanCurvatureCache.GetPointer(); }

  void SetSharedBoundaryEnabled(bool enabled) { m_IsSharedBoundaryEnabled = enabled; }
  void SetSharedBoundaryWeight(double weight) { m_SharedBoundaryWeight = weight; }

  void ReadTransforms();
  void ReadPointsFiles();
  virtual void AllocateDataCaches();
  virtual void AllocateDomainsAndNeighborhoods();
  virtual void InitializeOptimizationFunctions();

  virtual void Initialize() {
    this->m_Initializing = true;
    this->Execute();
    this->m_Initializing = false;
  }

  virtual void ReInitialize();

  virtual void Execute();

  std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>>> ComputeCuttingPlanes() {
    std::vector<std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>>> planes;
    for (size_t i = 0; i < m_CuttingPlanes.size(); i++) {
      std::vector<std::pair<Eigen::Vector3d, Eigen::Vector3d>> domain_i_cps;
      for (size_t j = 0; j < m_CuttingPlanes[i].size(); j++) {
        std::pair<Eigen::Vector3d, Eigen::Vector3d> cut_plane;
        cut_plane.first = ComputePlaneNormal(m_CuttingPlanes[i][j].a.as_ref(), m_CuttingPlanes[i][j].b.as_ref(),
                                             m_CuttingPlanes[i][j].c.as_ref());
        cut_plane.second =
            Eigen::Vector3d(m_CuttingPlanes[i][j].a[0], m_CuttingPlanes[i][j].a[1], m_CuttingPlanes[i][j].a[2]);
        domain_i_cps.push_back(cut_plane);
      }
      planes.push_back(domain_i_cps);
    }
    return planes;
  }

  Eigen::Vector3d ComputePlaneNormal(const vnl_vector<double>& a, const vnl_vector<double>& b,
                                     const vnl_vector<double>& c) {
    // See http://mathworld.wolfram.com/Plane.html, for example
    vnl_vector<double> q;
    q = vnl_cross_3d((b - a), (c - a));

    if (q.magnitude() > 0.0) {
      Eigen::Vector3d qp;
      q = q / q.magnitude();
      qp(0) = q[0];
      qp(1) = q[1];
      qp(2) = q[2];
      return qp;
    } else {
      std::cerr << "Error in Libs/Optimize/ParticleSystem/Sampler.h::ComputePlaneNormal" << std::endl;
      std::cerr << "There was an issue with a cutting plane that was defined. It has yielded a 0,0,0 vector. Please "
                   "check the inputs."
                << std::endl;
      throw std::runtime_error("Error computing plane normal");
    }
  }

  std::vector<FreeFormConstraint> GetFFCs() { return m_FFCs; }

  void SetMeshFFCMode(bool mesh_ffc_mode) { m_meshFFCMode = mesh_ffc_mode; }

 protected:
  void GenerateData();

  bool GetInitialized() { return this->m_Initialized; }

  void SetInitialized(bool value) { this->m_Initialized = value; }

  bool GetInitializing() { return this->m_Initializing; }

  void SetInitializing(bool value) { this->m_Initializing = value; }

  bool m_Initialized{false};
  int m_AdaptivityMode{0};
  bool m_Initializing{false};

  OptimizerType::Pointer m_Optimizer;

  SamplingFunction::Pointer m_GradientFunction;
  CurvatureSamplingFunction::Pointer m_CurvatureGradientFunction;

  GenericContainerArray<double>::Pointer m_Sigma1Cache;
  GenericContainerArray<double>::Pointer m_Sigma2Cache;

  MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;

  ParticleSystem::Pointer m_ParticleSystem;

  std::vector<ParticleDomain::Pointer> m_DomainList;

  std::vector<ParticleSurfaceNeighborhood::Pointer> m_NeighborhoodList;

  int m_pairwise_potential_type;

  shapeworks::CorrespondenceMode m_CorrespondenceMode;

  DualVectorFunction::Pointer m_LinkingFunction;

  LegacyCorrespondenceFunction::Pointer m_EnsembleEntropyFunction;
  LegacyCorrespondenceFunction::Pointer m_EnsembleRegressionEntropyFunction;
  LegacyCorrespondenceFunction::Pointer m_EnsembleMixedEffectsEntropyFunction;
  DisentangledCorrespondenceFunction::Pointer m_DisentangledEnsembleEntropyFunction;
  CorrespondenceFunction::Pointer m_CorrespondenceFunction;

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
  std::vector<std::string> m_MeshFiles;
  std::vector<std::string> m_FeaMeshFiles;
  std::vector<std::string> m_FeaGradFiles;
  std::vector<std::string> m_FidsFiles;
  std::vector<int> m_AttributesPerDomain;
  int m_DomainsPerShape;
  double m_Spacing{0};
  bool m_IsSharedBoundaryEnabled;
  double m_SharedBoundaryWeight{0.5};

  std::string m_TransformFile;
  std::string m_PrefixTransformFile;
  std::vector<std::vector<CuttingPlaneType>> m_CuttingPlanes;
  std::vector<std::vector<SphereType>> m_Spheres;
  std::vector<FreeFormConstraint> m_FFCs;
  std::vector<vtkSmartPointer<vtkPolyData>> m_meshes;
  bool m_meshFFCMode = false;

  std::vector<std::string> fieldAttributes_;

  unsigned int m_verbosity;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-08-02 at 17:10:01 +0000
