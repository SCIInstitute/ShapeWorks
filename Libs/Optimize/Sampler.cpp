
#include "Sampler.h"

#include <Logging.h>

#include "Libs/Optimize/Domain/ContourDomain.h"
#include "Libs/Optimize/Domain/ImageDomain.h"
#include "Libs/Optimize/Utils/ObjectReader.h"
#include "itkImageRegionIterator.h"
#include "itkParticlePositionReader.h"

namespace shapeworks {

Sampler::Sampler() {
  m_ParticleSystem = ParticleSystem::New();

  m_GradientFunction = SamplingFunction::New();
  m_CurvatureGradientFunction = CurvatureSamplingFunction::New();

  m_Optimizer = OptimizerType::New();

  m_PointsFiles.push_back("");
  m_MeshFiles.push_back("");

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

void Sampler::AllocateDomainsAndNeighborhoods() {
  // Allocate all the necessary domains and neighborhoods. This must be done
  // *after* registering the attributes to the particle system since some of
  // them respond to AddDomain.
  // Here, the Constraints actually get added to the constraints class
  int ctr = 0;
  for (unsigned int i = 0; i < this->m_DomainList.size(); i++) {
    auto domain = m_DomainList[i];

    if (domain->GetDomainType() == shapeworks::DomainType::Image) {
        // Adding cutting planes to constraint object
        if (m_CuttingPlanes.size() > i) {
          for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++) {
            domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a.as_ref(), m_CuttingPlanes[i][j].b.as_ref(), m_CuttingPlanes[i][j].c.as_ref());
            if (m_verbosity >= 1)
              std::cout << "Adding cutting plane constraint to domain " << i << " shape " << j << " with normal "
                        << domain->GetConstraints()->getPlaneConstraints()[j].getPlaneNormal().transpose() << " and point "
                        << domain->GetConstraints()->getPlaneConstraints()[j].getPlanePoint().transpose() << std::endl;
          }
        }

      auto imageDomain = static_cast<ImplicitSurfaceDomain<ImageType::PixelType>*>(domain.get());


      // Adding free-form constraints to constraint object
      // std::cout << "m_FFCs.size() " << m_FFCs.size() << std::endl;
      if (m_FFCs.size() > i) {
        SW_LOG("Initializing FFCs for domain {} / {}", i, m_DomainList.size());
        initialize_ffcs(i);
      }

      if (m_AttributesPerDomain.size() > 0 && m_AttributesPerDomain[i % m_DomainsPerShape] > 0) {
        TriMesh* themesh = TriMesh::read(m_MeshFiles[i].c_str());
        if (themesh != NULL) {
          themesh->need_faces();
          themesh->need_neighbors();
          orient(themesh);
          themesh->need_bsphere();
          if (!themesh->normals.empty()) {
            themesh->normals.clear();
          }
          themesh->need_normals();
          if (!themesh->tstrips.empty()) {
            themesh->tstrips.clear();
          }
          themesh->need_tstrips();
          if (!themesh->adjacentfaces.empty()) {
            themesh->adjacentfaces.clear();
          }
          themesh->need_adjacentfaces();
          if (!themesh->across_edge.empty()) {
            themesh->across_edge.clear();
          }
          themesh->need_across_edge();
          // themesh->need_faceedges();
          // themesh->need_oneringfaces();
          // themesh->need_abs_curvatures();
          // themesh->need_speed();
          // themesh->setSpeedType(1);

          imageDomain->SetMesh(themesh);
          imageDomain->SetFids(m_FidsFiles[i].c_str());
          int d = i % m_DomainsPerShape;
          for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++) {
            int ctr1 = ctr++;
            imageDomain->SetFeaMesh(m_FeaMeshFiles[ctr1].c_str());
            imageDomain->SetFeaGrad(m_FeaGradFiles[ctr1].c_str());
          }
        }
      }
    }
    else if(domain->GetDomainType() == shapeworks::DomainType::Mesh){

        if(m_meshFFCMode == 1){
            // Adding free-form constraints to constraint object
            //std::cout << "m_FFCs.size() " << m_FFCs.size() << std::endl;
            if (m_FFCs.size() > i) {
               initialize_ffcs(i);
            }

            // Adding cutting planes to constraint object
            if (m_CuttingPlanes.size() > i) {
              for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++) {
                domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a.as_ref(), m_CuttingPlanes[i][j].b.as_ref(), m_CuttingPlanes[i][j].c.as_ref());
                if (m_verbosity >= 1)
                  std::cout << "Adding cutting plane constraint to domain " << i << " shape " << j << " with normal "
                            << domain->GetConstraints()->getPlaneConstraints()[j].getPlaneNormal().transpose() << " and point "
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

void Sampler::ReadPointsFiles() {
  // If points file names have been specified, then read the initial points.
  for (unsigned int i = 0; i < m_PointsFiles.size(); i++) {
    if (m_PointsFiles[i] != "") {
      ParticlePositionReader::Pointer reader = ParticlePositionReader::New();
      reader->SetFileName(m_PointsFiles[i].c_str());
      reader->Update();
      this->GetParticleSystem()->AddPositionList(reader->GetOutput(), i);
    }
  }

  // Push position information out to all observers (necessary to correctly
  // fill out the shape matrix).
  this->GetParticleSystem()->SynchronizePositions();
}

void Sampler::InitializeOptimizationFunctions() {
  // Set the minimum neighborhood radius and maximum sigma based on the
  // domain of the 1st input image.
  unsigned int maxdim = 0;
  double maxradius = -1.0;
  double minimumNeighborhoodRadius = this->m_Spacing;

  for (unsigned int d = 0; d < this->GetParticleSystem()->GetNumberOfDomains(); d++) {
    if (!GetParticleSystem()->GetDomain(d)->IsDomainFixed()) {
      double radius = GetParticleSystem()->GetDomain(d)->GetMaxDiameter();
      maxradius = radius > maxradius ? radius : maxradius;
    }
  }

  m_GradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_GradientFunction->SetMaximumNeighborhoodRadius(maxradius);

  m_CurvatureGradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_CurvatureGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
  m_CurvatureGradientFunction->SetParticleSystem(this->GetParticleSystem());
  m_CurvatureGradientFunction->SetDomainNumber(0);
  if(m_IsSharedBoundaryEnabled) {
    m_CurvatureGradientFunction->SetSharedBoundaryEnabled(true);
    m_CurvatureGradientFunction->SetSharedBoundaryWeight(this->m_SharedBoundaryWeight);
  }

  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_LegacyShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();
}

void Sampler::GenerateData() {}

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
    this->GetOptimizer()->SetParticleSystem(this->GetParticleSystem());
    this->ReadTransforms();
    this->ReadPointsFiles();
    this->InitializeOptimizationFunctions();

    this->SetInitialized(true);
  }

  if (this->GetInitializing() == true) return;

  // this->GetOptimizer()->SetShapeMatrix(this->m_ShapeMatrix);
  this->GetOptimizer()->StartOptimization();
}

void Sampler::ReadTransforms() {
  if (m_TransformFile != "") {
    ObjectReader<ParticleSystem::TransformType> reader;
    reader.SetFileName(m_TransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < this->GetParticleSystem()->GetNumberOfDomains(); i++)
      this->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }

  if (m_PrefixTransformFile != "") {
    ObjectReader<ParticleSystem::TransformType> reader;
    reader.SetFileName(m_PrefixTransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < this->GetParticleSystem()->GetNumberOfDomains(); i++)
      this->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
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

void Sampler::AddMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh) {
  auto domain = std::make_shared<MeshDomain>();
  m_NeighborhoodList.push_back(ParticleSurfaceNeighborhood::New());
  if (mesh) {
    this->m_Spacing = 1;
    domain->SetMesh(mesh);
    this->m_meshes.push_back(mesh->GetPolydata());
    m_NeighborhoodList.back()->SetWeightingEnabled(!mesh->IsGeodesicsEnabled()); // disable weighting for geodesics
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

void Sampler::TransformCuttingPlanes(unsigned int i) {
  if (m_Initialized == true) {
    TransformType T1 = this->GetParticleSystem()->GetTransform(i) * this->GetParticleSystem()->GetPrefixTransform(i);
    for (unsigned int d = 0; d < this->GetParticleSystem()->GetNumberOfDomains(); d++) {
      if (this->GetParticleSystem()->GetDomainFlag(d) == false) {
        TransformType T2 = this->GetParticleSystem()->InvertTransform(this->GetParticleSystem()->GetTransform(d) *
                                                                      this->GetParticleSystem()->GetPrefixTransform(d));
        m_ParticleSystem->GetDomain(d)->GetConstraints()->transformPlanes(T2 * T1);
      }
    }
  }
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

void Sampler::AddFreeFormConstraint(int domain, const FreeFormConstraint &ffc) {
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

bool Sampler::initialize_ffcs(size_t dom) {
  auto mesh = std::make_shared<Mesh>(m_meshes[dom]);
  if (m_verbosity >= 1) std::cout << "dom " << dom << " point count " << mesh->numPoints() << " faces " << mesh->numFaces() << std::endl;

  if (m_FFCs[dom].isSet()) {
    this->m_DomainList[dom]->GetConstraints()->addFreeFormConstraint(mesh);
    m_FFCs[dom].computeGradientFields(mesh);
  }

#if defined(VIZFFC)
  MeshUtils mutil;
  mutil.visualizeVectorFieldForFFCs(mesh);
#endif

  return true;
}

}  // namespace shapeworks
