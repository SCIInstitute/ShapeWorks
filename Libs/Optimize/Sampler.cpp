
#include "Sampler.h"

#include <Logging.h>
#include <Particles/ParticleFile.h>

#include "Libs/Optimize/Domain/ContourDomain.h"
#include "Libs/Optimize/Domain/VolumeDomain.h"
#include "Libs/Optimize/Utils/ObjectReader.h"

namespace shapeworks {

Sampler::Sampler() {
  m_ParticleSystem = ParticleSystem::New();

  m_GradientFunction = SamplingFunction::New();
  m_CurvatureGradientFunction = CurvatureSamplingFunction::New();

  m_Optimizer = OptimizerType::New();

  m_PointsFiles.push_back("");

  m_LinkingFunction = DualVectorFunction::New();
  m_EnsembleEntropyFunction = LegacyCorrespondenceFunction::New();
  m_DisentangledEnsembleEntropyFunction = DisentangledCorrespondenceFunction::New();
  m_EnsembleRegressionEntropyFunction = LegacyCorrespondenceFunction::New();
  m_EnsembleMixedEffectsEntropyFunction = LegacyCorrespondenceFunction::New();
  m_CorrespondenceFunction = CorrespondenceFunction::New();

  m_LegacyShapeMatrix = LegacyShapeMatrix::New();
  m_GeneralShapeMatrix = ShapeMatrix::New();
  m_GeneralShapeGradMatrix = ShapeGradientMatrix::New();

  m_LinearRegressionShapeMatrix = LinearRegressionShapeMatrix::New();
  m_MixedEffectsShapeMatrix = MixedEffectsShapeMatrix::New();

  m_EnsembleEntropyFunction->SetShapeMatrix(m_LegacyShapeMatrix);
  m_DisentangledEnsembleEntropyFunction->SetShapeMatrix(m_LegacyShapeMatrix);

  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_LinearRegressionShapeMatrix);
  m_EnsembleMixedEffectsEntropyFunction->SetShapeMatrix(m_MixedEffectsShapeMatrix);

  m_CorrespondenceFunction->SetShapeData(m_GeneralShapeMatrix);
  m_CorrespondenceFunction->SetShapeGradient(m_GeneralShapeGradMatrix);

  m_ParticleSystem->RegisterObserver(m_LegacyShapeMatrix);
  m_ParticleSystem->RegisterObserver(m_LinearRegressionShapeMatrix);
  m_ParticleSystem->RegisterObserver(m_MixedEffectsShapeMatrix);

  m_CorrespondenceMode = shapeworks::CorrespondenceMode::EnsembleEntropy;
}

//---------------------------------------------------------------------------
void Sampler::AllocateDataCaches() {
  // Set up the various data caches that the optimization functions will use.
  m_Sigma1Cache = GenericContainerArray<double>::New();
  m_ParticleSystem->RegisterObserver(m_Sigma1Cache);
  m_GradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);
  m_CurvatureGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

  m_Sigma2Cache = GenericContainerArray<double>::New();
  m_ParticleSystem->RegisterObserver(m_Sigma2Cache);

  m_MeanCurvatureCache = MeanCurvatureContainer<ImageType::PixelType, Dimension>::New();
  m_MeanCurvatureCache->SetVerbosity(m_verbosity);
  m_CurvatureGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
  m_ParticleSystem->RegisterObserver(m_MeanCurvatureCache);
}

//---------------------------------------------------------------------------
void Sampler::AllocateDomainsAndNeighborhoods() {
  // Allocate all the necessary domains and neighborhoods. This must be done
  // *after* registering the attributes to the particle system since some of
  // them respond to AddDomain.
  // Here, the Constraints actually get added to the constraints class
  for (unsigned int i = 0; i < this->m_DomainList.size(); i++) {
    auto domain = m_DomainList[i];

    if (domain->GetDomainType() == shapeworks::DomainType::Image) {
      // Adding cutting planes to constraint object
      if (m_CuttingPlanes.size() > i) {
        for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++) {
          domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a.as_ref(), m_CuttingPlanes[i][j].b.as_ref(),
                                             m_CuttingPlanes[i][j].c.as_ref());
          if (m_verbosity >= 1)
            std::cout << "Adding cutting plane constraint to domain " << i << " shape " << j << " with normal "
                      << domain->GetConstraints()->getPlaneConstraints()[j].getPlaneNormal().transpose()
                      << " and point " << domain->GetConstraints()->getPlaneConstraints()[j].getPlanePoint().transpose()
                      << std::endl;
        }
      }

      // Adding free-form constraints to constraint object
      // std::cout << "m_FFCs.size() " << m_FFCs.size() << std::endl;
      if (m_FFCs.size() > i) {
        SW_LOG("Initializing FFCs for domain {} / {}", i, m_DomainList.size());
        initialize_ffcs(i);
      }

    } else if (domain->GetDomainType() == shapeworks::DomainType::Mesh) {
      if (m_meshFFCMode == 1) {
        // Adding free-form constraints to constraint object
        // std::cout << "m_FFCs.size() " << m_FFCs.size() << std::endl;
        if (m_FFCs.size() > i) {
          initialize_ffcs(i);
        }

        // Adding cutting planes to constraint object
        if (m_CuttingPlanes.size() > i) {
          for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++) {
            domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a.as_ref(), m_CuttingPlanes[i][j].b.as_ref(),
                                               m_CuttingPlanes[i][j].c.as_ref());
            if (m_verbosity >= 1)
              std::cout << "Adding cutting plane constraint to domain " << i << " shape " << j << " with normal "
                        << domain->GetConstraints()->getPlaneConstraints()[j].getPlaneNormal().transpose()
                        << " and point "
                        << domain->GetConstraints()->getPlaneConstraints()[j].getPlanePoint().transpose() << std::endl;
          }
        }
      }
    }

    // END TEST CUTTING PLANE
    m_ParticleSystem->AddDomain(domain);
    m_ParticleSystem->SetNeighborhood(i, m_NeighborhoodList[i]);
  }
}

//---------------------------------------------------------------------------
void Sampler::ReadPointsFiles() {
  // If points file names have been specified, then read the initial points.
  for (unsigned int i = 0; i < m_PointsFiles.size(); i++) {
    if (m_PointsFiles[i] != "") {
      auto points = particles::read_particles_as_vector(m_PointsFiles[i]);
      m_ParticleSystem->AddPositionList(points, i);
    }
  }

  // Push position information out to all observers (necessary to correctly
  // fill out the shape matrix).
  m_ParticleSystem->SynchronizePositions();
}

//---------------------------------------------------------------------------
void Sampler::initialize_initial_positions() {
  for (unsigned int i = 0; i < initial_points_.size(); i++) {
    m_ParticleSystem->AddPositionList(initial_points_[i], i);
  }
}

//---------------------------------------------------------------------------
void Sampler::InitializeOptimizationFunctions() {
  // Set the minimum neighborhood radius and maximum sigma based on the
  // domain of the 1st input image.
  double maxradius = -1.0;
  double minimumNeighborhoodRadius = this->m_Spacing;

  for (unsigned int d = 0; d < m_ParticleSystem->GetNumberOfDomains(); d++) {
    if (!m_ParticleSystem->GetDomain(d)->IsDomainFixed()) {
      double radius = m_ParticleSystem->GetDomain(d)->GetMaxDiameter();
      maxradius = radius > maxradius ? radius : maxradius;
    }
  }

  m_GradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_GradientFunction->SetMaximumNeighborhoodRadius(maxradius);

  m_CurvatureGradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_CurvatureGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
  m_CurvatureGradientFunction->SetParticleSystem(m_ParticleSystem);
  m_CurvatureGradientFunction->SetDomainNumber(0);
  if (m_IsSharedBoundaryEnabled) {
    m_CurvatureGradientFunction->SetSharedBoundaryEnabled(true);
    m_CurvatureGradientFunction->SetSharedBoundaryWeight(this->m_SharedBoundaryWeight);
  }

  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_LegacyShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();
}

//---------------------------------------------------------------------------
void Sampler::Execute() {
  if (this->GetInitialized() == false) {
    this->AllocateDataCaches();
    this->SetAdaptivityMode(m_AdaptivityMode);
    this->SetCorrespondenceMode(m_CorrespondenceMode);
    this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
    m_LinkingFunction->SetAOn();
    m_LinkingFunction->SetBOn();

    this->AllocateDomainsAndNeighborhoods();

    // Point the optimizer to the particle system.
    this->GetOptimizer()->SetParticleSystem(m_ParticleSystem);
    this->ReadTransforms();
    this->ReadPointsFiles();
    initialize_initial_positions();
    this->InitializeOptimizationFunctions();

    this->SetInitialized(true);
  }

  if (this->GetInitializing() == true) return;

  // this->GetOptimizer()->SetShapeMatrix(this->m_ShapeMatrix);
  this->GetOptimizer()->StartOptimization();
}

//---------------------------------------------------------------------------
Sampler::CuttingPlaneList Sampler::ComputeCuttingPlanes() {
  CuttingPlaneList planes;
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

//---------------------------------------------------------------------------
Eigen::Vector3d Sampler::ComputePlaneNormal(const vnl_vector<double>& a, const vnl_vector<double>& b,
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
    std::cerr << "Error in Sampler::ComputePlaneNormal" << std::endl;
    std::cerr << "There was an issue with a cutting plane that was defined. It has yielded a 0,0,0 vector. Please "
                 "check the inputs."
              << std::endl;
    throw std::runtime_error("Error computing plane normal");
  }
}

void Sampler::ReadTransforms() {
  if (m_TransformFile != "") {
    ObjectReader<ParticleSystem::TransformType> reader;
    reader.SetFileName(m_TransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++)
      m_ParticleSystem->SetTransform(i, reader.GetOutput()[i]);
  }

  if (m_PrefixTransformFile != "") {
    ObjectReader<ParticleSystem::TransformType> reader;
    reader.SetFileName(m_PrefixTransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++)
      m_ParticleSystem->SetPrefixTransform(i, reader.GetOutput()[i]);
  }
}

void Sampler::ReInitialize() {
  this->SetAdaptivityMode(m_AdaptivityMode);
  this->SetCorrespondenceMode(m_CorrespondenceMode);
  this->GetOptimizer()->SetGradientFunction(m_LinkingFunction);
  this->m_LinkingFunction->SetAOn();
  this->m_LinkingFunction->SetBOn();
  this->InitializeOptimizationFunctions();
  this->m_Sigma1Cache->ZeroAllValues();
  this->m_Sigma2Cache->ZeroAllValues();
  this->m_MeanCurvatureCache->ZeroAllValues();
}

void Sampler::AddMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh, double geodesic_remesh_percent) {
  auto domain = std::make_shared<MeshDomain>();
  m_NeighborhoodList.push_back(ParticleSurfaceNeighborhood::New());
  if (mesh) {
    this->m_Spacing = 1;
    domain->SetMesh(mesh, geodesic_remesh_percent);
    this->m_meshes.push_back(mesh->GetPolydata());
    m_NeighborhoodList.back()->SetWeightingEnabled(!mesh->IsGeodesicsEnabled());  // disable weighting for geodesics
  }
  m_DomainList.push_back(domain);
}

void Sampler::AddContour(vtkSmartPointer<vtkPolyData> poly_data) {
  auto domain = std::make_shared<ContourDomain>();
  m_NeighborhoodList.push_back(ParticleSurfaceNeighborhood::New());
  if (poly_data != nullptr) {
    this->m_Spacing = 1;
    domain->SetPolyLine(poly_data);
  }
  m_NeighborhoodList.back()->SetWeightingEnabled(false);
  m_DomainList.push_back(domain);
}

void Sampler::SetFieldAttributes(const std::vector<std::string>& s) {
  fieldAttributes_ = s;
  if (m_ParticleSystem) {
    m_ParticleSystem->SetFieldAttributes(s);
  }
}

void Sampler::TransformCuttingPlanes(unsigned int i) {
  if (m_Initialized == true) {
    TransformType T1 = m_ParticleSystem->GetTransform(i) * m_ParticleSystem->GetPrefixTransform(i);
    for (unsigned int d = 0; d < m_ParticleSystem->GetNumberOfDomains(); d++) {
      if (m_ParticleSystem->GetDomainFlag(d) == false) {
        TransformType T2 = m_ParticleSystem->InvertTransform(m_ParticleSystem->GetTransform(d) *
                                                             m_ParticleSystem->GetPrefixTransform(d));
        m_ParticleSystem->GetDomain(d)->GetConstraints()->transformPlanes(T2 * T1);
      }
    }
  }
}

void Sampler::SetCorrespondenceMode(CorrespondenceMode mode) {
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

void Sampler::SetAttributesPerDomain(const std::vector<int> s) {
  std::vector<int> s1;
  if (s.size() == 0) {
    s1.resize(m_CorrespondenceFunction->GetDomainsPerShape());
    for (int i = 0; i < m_CorrespondenceFunction->GetDomainsPerShape(); i++) s1[i] = 0;
  } else {
    s1 = s;
  }

  m_AttributesPerDomain = s1;
  m_CorrespondenceFunction->SetAttributesPerDomain(s1);
  m_GeneralShapeMatrix->SetAttributesPerDomain(s1);
  m_GeneralShapeGradMatrix->SetAttributesPerDomain(s1);
}

void Sampler::SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, Dimension>& va,
                              const vnl_vector_fixed<double, Dimension>& vb,
                              const vnl_vector_fixed<double, Dimension>& vc) {
  if (m_CuttingPlanes.size() < i + 1) {
    m_CuttingPlanes.resize(i + 1);
  }

  m_CuttingPlanes[i].push_back(CuttingPlaneType());
  int last = m_CuttingPlanes[i].size() - 1;

  m_CuttingPlanes[i][last].a = va;
  m_CuttingPlanes[i][last].b = vb;
  m_CuttingPlanes[i][last].c = vc;

  if (m_Initialized == true) {
    std::cout << "Initialized plane" << std::endl;
    m_ParticleSystem->GetDomain(i)->GetConstraints()->addPlane(va.as_ref(), vb.as_ref(), vc.as_ref());
  }
}

void Sampler::AddFreeFormConstraint(int domain, const FreeFormConstraint& ffc) {
  if (m_FFCs.size() < domain + 1) {
    m_FFCs.resize(domain + 1);
  }
  m_FFCs[domain] = ffc;
}

void Sampler::AddImage(ImageType::Pointer image, double narrow_band, std::string name) {
  auto domain = std::make_shared<ImplicitSurfaceDomain<ImageType::PixelType>>();

  m_NeighborhoodList.push_back(ParticleSurfaceNeighborhood::New());

  if (image) {
    this->m_Spacing = image->GetSpacing()[0];
    // convert narrow band (index space) to world space
    // (e.g. narrow band of 4 means 4 voxels (largest side)
    double narrow_band_world = image->GetSpacing().GetVnlVector().max_value() * narrow_band;
    domain->SetImage(image, narrow_band_world);

    // Adding meshes for FFCs
    vtkSmartPointer<vtkPolyData> mesh = Image(image).toMesh(0.0).getVTKMesh();
    this->m_meshes.push_back(mesh);
  }

  domain->SetDomainID(m_DomainList.size());
  domain->SetDomainName(name);
  m_DomainList.push_back(domain);
}


void Sampler::AddVolume(ImageType::Pointer image, ImageType::Pointer ct_image, double narrow_band, std::string name) {
  auto domain = std::make_shared<VolumeDomain>();

  m_NeighborhoodList.push_back(ParticleSurfaceNeighborhood::New());

  if (image) {
    this->m_Spacing = image->GetSpacing()[0];
    // convert narrow band (index space) to world space
    // (e.g. narrow band of 4 means 4 voxels (largest side)
    double narrow_band_world = image->GetSpacing().GetVnlVector().max_value() * narrow_band;
    domain->SetImage(image, narrow_band_world);
    domain->SetImageCT(ct_image, narrow_band_world);

    vtkSmartPointer<vtkPolyData> mesh = Image(image).toMesh(0.0).getVTKMesh();
    this->m_meshes.push_back(mesh);
  }

  domain->SetDomainID(m_DomainList.size());
  domain->SetDomainName(name);
  m_DomainList.push_back(domain);
}

bool Sampler::initialize_ffcs(size_t dom) {
  auto mesh = std::make_shared<Mesh>(m_meshes[dom]);
  if (m_verbosity >= 1)
    std::cout << "dom " << dom << " point count " << mesh->numPoints() << " faces " << mesh->numFaces() << std::endl;

  if (m_FFCs[dom].isSet()) {
    m_DomainList[dom]->GetConstraints()->addFreeFormConstraint(mesh);
    m_FFCs[dom].computeGradientFields(mesh);
  }

#if defined(VIZFFC)
  MeshUtils mutil;
  mutil.visualizeVectorFieldForFFCs(mesh);
#endif

  return true;
}

}  // namespace shapeworks
