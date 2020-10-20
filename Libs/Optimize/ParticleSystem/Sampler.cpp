
#include "itkParticlePositionReader.h"
#include "itkImageRegionIterator.h"
#include "object_reader.h"
#include "itkParticleImageDomain.h"
#include "Sampler.h"

namespace shapeworks {

Sampler::Sampler()
{
  // Allocate the particle system members.
  m_ParticleSystem = itk::ParticleSystem<Dimension>::New();

  m_GradientFunction = itk::ParticleEntropyGradientFunction<ImageType::PixelType, Dimension>::New();
  m_CurvatureGradientFunction
    = itk::ParticleCurvatureEntropyGradientFunction<ImageType::PixelType, Dimension>::New();

  m_ModifiedCotangentGradientFunction
    = itk::ParticleModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>::New();
  m_ConstrainedModifiedCotangentGradientFunction
    = itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<ImageType::PixelType, Dimension>::New();

  m_OmegaGradientFunction
    = itk::ParticleOmegaGradientFunction<ImageType::PixelType, Dimension>::New();

  // Allocate some optimization code.
  m_Optimizer = OptimizerType::New();

  m_PointsFiles.push_back("");
  m_MeshFiles.push_back("");

  m_LinkingFunction = itk::ParticleDualVectorFunction<Dimension>::New();
  m_EnsembleEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleRegressionEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_EnsembleMixedEffectsEntropyFunction = itk::ParticleEnsembleEntropyFunction<Dimension>::New();
  m_MeshBasedGeneralEntropyGradientFunction = itk::ParticleMeshBasedGeneralEntropyGradientFunction<Dimension>::New();

  m_ShapeMatrix = itk::ParticleShapeMatrixAttribute<double, Dimension>::New();
  m_GeneralShapeMatrix = itk::ParticleGeneralShapeMatrix<double, Dimension>::New();
  m_GeneralShapeGradMatrix = itk::ParticleGeneralShapeGradientMatrix<double, Dimension>::New();

  m_LinearRegressionShapeMatrix = itk::ParticleShapeLinearRegressionMatrixAttribute<double, Dimension>::New();
  m_MixedEffectsShapeMatrix = itk::ParticleShapeMixedEffectsMatrixAttribute<double, Dimension>::New();

  m_EnsembleEntropyFunction->SetShapeMatrix(m_ShapeMatrix);

  m_EnsembleRegressionEntropyFunction->SetShapeMatrix(m_LinearRegressionShapeMatrix);
  m_EnsembleMixedEffectsEntropyFunction->SetShapeMatrix(m_MixedEffectsShapeMatrix);

  m_MeshBasedGeneralEntropyGradientFunction->SetShapeData(m_GeneralShapeMatrix);
  m_MeshBasedGeneralEntropyGradientFunction->SetShapeGradient(m_GeneralShapeGradMatrix);

  m_ParticleSystem->RegisterAttribute(m_ShapeMatrix);
  m_ParticleSystem->RegisterAttribute(m_LinearRegressionShapeMatrix);
  m_ParticleSystem->RegisterAttribute(m_MixedEffectsShapeMatrix);

  m_CorrespondenceMode = shapeworks::CorrespondenceMode::EnsembleEntropy;
}

void Sampler::AllocateDataCaches()
{
  // Set up the various data caches that the optimization functions will use.
  m_Sigma1Cache = itk::ParticleContainerArrayAttribute<double, Dimension>::New();
  m_ParticleSystem->RegisterAttribute(m_Sigma1Cache);
  m_GradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);
  m_CurvatureGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

  m_ModifiedCotangentGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);
  m_ConstrainedModifiedCotangentGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

  m_OmegaGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

  m_Sigma2Cache = itk::ParticleContainerArrayAttribute<double, Dimension>::New();
  m_ParticleSystem->RegisterAttribute(m_Sigma2Cache);

  m_MeanCurvatureCache = itk::ParticleMeanCurvatureAttribute<ImageType::PixelType, Dimension>::New();
  m_MeanCurvatureCache->SetVerbosity(m_verbosity);
  m_CurvatureGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
  m_OmegaGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
  m_ParticleSystem->RegisterAttribute(m_MeanCurvatureCache);
}

void Sampler::AllocateDomainsAndNeighborhoods()
{
  // Allocate all the necessary domains and neighborhoods. This must be done
  // *after* registering the attributes to the particle system since some of
  // them respond to AddDomain.
  int ctr = 0;
  for (unsigned int i = 0; i < this->m_DomainList.size(); i++) {
    auto domain = m_DomainList[i];
    if (m_CuttingPlanes.size() > i) {
      for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++)
        domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a, m_CuttingPlanes[i][j].b,
                                m_CuttingPlanes[i][j].c);
    }

    if (m_Spheres.size() > i) {
      for (unsigned int j = 0; j < m_Spheres[i].size(); j++) {
        domain->GetConstraints()->addSphere(m_Spheres[i][j].center, m_Spheres[i][j].radius);
      }
    }

    if (domain->GetDomainType() == shapeworks::DomainType::Image) {
      auto imageDomain = static_cast<itk::ParticleImplicitSurfaceDomain<ImageType::PixelType>*>(domain.GetPointer());

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
          //themesh->need_faceedges();
          //themesh->need_oneringfaces();
          //themesh->need_abs_curvatures();
          //themesh->need_speed();
          //themesh->setSpeedType(1);

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

    // END TEST CUTTING PLANE
    m_ParticleSystem->AddDomain(domain);
    m_ParticleSystem->SetNeighborhood(i, m_NeighborhoodList[i]);
  }
}

void Sampler::ReadPointsFiles()
{
  // If points file names have been specified, then read the initial points.
  for (unsigned int i = 0; i < m_PointsFiles.size(); i++) {
    if (m_PointsFiles[i] != "") {
      itk::ParticlePositionReader<3>::Pointer reader
        = itk::ParticlePositionReader<3>::New();
      reader->SetFileName(m_PointsFiles[i].c_str());
      reader->Update();
      this->GetParticleSystem()->AddPositionList(reader->GetOutput(), i);
    }
  }

  // Push position information out to all observers (necessary to correctly
  // fill out the shape matrix).
  this->GetParticleSystem()->SynchronizePositions();
}

void Sampler::InitializeOptimizationFunctions()
{
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

  m_ModifiedCotangentGradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_ModifiedCotangentGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
  m_ModifiedCotangentGradientFunction->SetParticleSystem(this->GetParticleSystem());
  m_ModifiedCotangentGradientFunction->SetDomainNumber(0);

  m_ConstrainedModifiedCotangentGradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_ConstrainedModifiedCotangentGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
  m_ConstrainedModifiedCotangentGradientFunction->SetParticleSystem(this->GetParticleSystem());
  m_ConstrainedModifiedCotangentGradientFunction->SetDomainNumber(0);

  m_OmegaGradientFunction->SetMinimumNeighborhoodRadius(minimumNeighborhoodRadius);
  m_OmegaGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
  m_OmegaGradientFunction->SetParticleSystem(this->GetParticleSystem());
  m_OmegaGradientFunction->SetDomainNumber(0);

  m_LinearRegressionShapeMatrix->Initialize();
  m_MixedEffectsShapeMatrix->Initialize();
  m_ShapeMatrix->Initialize();

  m_GeneralShapeMatrix->Initialize();
  m_GeneralShapeGradMatrix->Initialize();
}

void Sampler::GenerateData()
{
}

void Sampler::Execute()
{

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

  //this->GetOptimizer()->SetShapeMatrix(this->m_ShapeMatrix);
  this->GetOptimizer()->StartOptimization();
}

void Sampler::ReadTransforms()
{
  if (m_TransformFile != "") {
    object_reader<itk::ParticleSystem<3>::TransformType> reader;
    reader.SetFileName(m_TransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < this->GetParticleSystem()->GetNumberOfDomains(); i++)
      this->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }

  if (m_PrefixTransformFile != "") {
    object_reader<itk::ParticleSystem<3>::TransformType> reader;
    reader.SetFileName(m_PrefixTransformFile.c_str());
    reader.Update();

    for (unsigned int i = 0; i < this->GetParticleSystem()->GetNumberOfDomains(); i++)
      this->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
  }

}

void Sampler::ReInitialize()
{
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

void Sampler::AddMesh(shapeworks::MeshWrapper* mesh)
{
  itk::MeshDomain* domain = new itk::MeshDomain();
  m_NeighborhoodList.push_back(itk::ParticleSurfaceNeighborhood<ImageType>::New());
  if (mesh) {
    this->m_Spacing = 1;
    domain->SetMesh(mesh);
  }
  m_DomainList.push_back(domain);
}

void Sampler::TransformCuttingPlanes(unsigned int i)
{
  if (m_Initialized == true) {
    TransformType T1 =
      this->GetParticleSystem()->GetTransform(i) * this->GetParticleSystem()->GetPrefixTransform(i);
    for (unsigned int d = 0; d < this->GetParticleSystem()->GetNumberOfDomains(); d++) {
      if (this->GetParticleSystem()->GetDomainFlag(d) == false) {
        TransformType T2 = this->GetParticleSystem()->InvertTransform(
          this->GetParticleSystem()->GetTransform(d)
          *
          this->GetParticleSystem()->GetPrefixTransform(d));
        m_ParticleSystem->GetDomain(d)->GetConstraints()->transformPlanes(T2 * T1);
      }
    }
  }
}

void Sampler::SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, Dimension>& va,
                              const vnl_vector_fixed<double, Dimension>& vb,
                              const vnl_vector_fixed<double, Dimension>& vc)
{
  if (m_CuttingPlanes.size() < i + 1) {
    m_CuttingPlanes.resize(i + 1);
  }

  m_CuttingPlanes[i].push_back(CuttingPlaneType());

  m_CuttingPlanes[i][m_CuttingPlanes[i].size() - 1].a = va;
  m_CuttingPlanes[i][m_CuttingPlanes[i].size() - 1].b = vb;
  m_CuttingPlanes[i][m_CuttingPlanes[i].size() - 1].c = vc;

  if (m_Initialized == true) {
      std::cout << "Initialized plane" << std::endl;
      m_ParticleSystem->GetDomain(i)->GetConstraints()->addPlane(va,vb,vc);
  }
}

void Sampler::AddSphere(unsigned int i, vnl_vector_fixed<double, Dimension>& c, double r)
{
  if (m_Spheres.size() < i + 1) {
    m_Spheres.resize(i + 1);
  }

  m_Spheres[i].push_back(SphereType());
  m_Spheres[i][m_Spheres[i].size() - 1].center = c;
  m_Spheres[i][m_Spheres[i].size() - 1].radius = r;

  if (m_Initialized == true) {
    std::cout << "Initialized sphere" << std::endl;
    m_ParticleSystem->GetDomain(i)->GetConstraints()->addSphere(c, r);
  }
}

void Sampler::AddImage(ImageType::Pointer image, double narrow_band)
{
  const auto domain = itk::ParticleImplicitSurfaceDomain<ImageType::PixelType>::New();
  m_NeighborhoodList.push_back(itk::ParticleSurfaceNeighborhood<ImageType>::New());

  if (image) {
    this->m_Spacing = image->GetSpacing()[0];
    domain->SetSigma(this->m_Spacing * 2.0);
    domain->SetImage(image, narrow_band);
  }

  m_DomainList.push_back(domain);
}

} // end namespace

