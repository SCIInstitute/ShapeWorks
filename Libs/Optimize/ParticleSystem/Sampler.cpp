
#include "itkParticlePositionReader.h"
#include "itkImageRegionIterator.h"
#include "object_reader.h"
#include "itkParticleImageDomain.h"
#include "Sampler.h"
#include "ContourDomain.h"

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
  // Here, the Constraints actually get added to the constraints class
  int ctr = 0;
  for (unsigned int i = 0; i < this->m_DomainList.size(); i++) {
    auto domain = m_DomainList[i];
    m_areas.push_back(1.);
    m_sides.push_back(1.);
    // Adding cutting planes to constraint object
    if (m_CuttingPlanes.size() > i) {
      for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++){
        domain->GetConstraints()->addPlane(m_CuttingPlanes[i][j].a, m_CuttingPlanes[i][j].b,
                                m_CuttingPlanes[i][j].c);
        if(m_verbosity >= 1) std::cout << "Adding cutting plane constraint to domain " << i << " shape " << j << " with normal " << (*domain->GetConstraints()->getPlaneConstraints())[j].GetPlaneNormal().transpose() << " and point " << (*domain->GetConstraints()->getPlaneConstraints())[j].GetPlanePoint().transpose() << std::endl;
      }
    }

    // Adding spheres to constraint object
    if (m_Spheres.size() > i) {
      for (unsigned int j = 0; j < m_Spheres[i].size(); j++) {
        domain->GetConstraints()->addSphere(m_Spheres[i][j].center, m_Spheres[i][j].radius);
         if(m_verbosity >= 1) std::cout << "Adding sphere constraint to domain " << i << " shape " << j << " with center " << m_Spheres[i][j].center << " and radius " << m_Spheres[i][j].radius << std::endl;
      }
    }

    if (domain->GetDomainType() == shapeworks::DomainType::Image) {
      auto imageDomain = static_cast<itk::ParticleImplicitSurfaceDomain<ImageType::PixelType>*>(domain.GetPointer());

      // Computing surface area for expected particle spacing determination
      std::shared_ptr<Mesh> surfaceAreaMesh = std::make_shared<Mesh>(m_meshes[i]);

      // Adding free-form constraints to constraint object and applying FFCs to the surface area mesh
      if (m_FFCs.size() > i && m_FFCs[i].size() > 0) {
        std::shared_ptr<Mesh> ffcmesh = initialize_ffcs(i);

        surfaceAreaMesh = std::make_shared<Mesh>(ffcmesh->clipByField("inout", 0.0));
        //surfaceAreaMesh->write("dev/mesh_FFC_Clipped_" + std::to_string(i) + ".vtk");
      }

      // Applying plane constraints
      if (m_CuttingPlanes.size() > i && m_CuttingPlanes[i].size() > 0) {
          std::vector<itk::PlaneConstraint> planeConsts = *(domain->GetConstraints()->getPlaneConstraints());
          for (unsigned int j = 0; j < planeConsts.size(); j++){
            Eigen::Vector3d normal = planeConsts[j].GetPlaneNormal();
            Eigen::Vector3d point = planeConsts[j].GetPlanePoint();
            Plane p = Plane::New();
            p->SetNormal(normal(0), normal(1), normal(2));
            p->SetOrigin(point(0), point(1), point(2));
            surfaceAreaMesh->clip(p);
          }
      }

      double SurfaceArea = surfaceAreaMesh->getSurfaceArea();

      std::cout << "Computing areas" << std::endl;
      m_areas[i] = SurfaceArea;

      m_sides[i] = ComputeSideLength(m_max_num_particles[i], SurfaceArea);
      std::cout << "After Computing areas" << std::endl;

      //surfaceAreaMesh->write("dev/mesh_CP_Clipped_" + std::to_string(i) + ".vtk");

      //***********************************************Add results to particle systems

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

double Sampler::ComputeSideLength(size_t particleCount, double area){
    if(particleCount < 20){
        return 0.8773826753 * sqrt(area/static_cast<double>(particleCount));
    }
    else{
        return sqrt(area/(20.6457288071 + (0.19245008973*(static_cast<double>(particleCount)-20))/2 ));
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

  m_ConstrainedModifiedCotangentGradientFunction->SetMinimumNeighborhoodRadius(
    minimumNeighborhoodRadius);
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

void Sampler::AddMesh(std::shared_ptr<shapeworks::MeshWrapper> mesh)
{
  auto domain = itk::MeshDomain::New();
  m_NeighborhoodList.push_back(itk::ParticleSurfaceNeighborhood<ImageType>::New());
  if(mesh) {
    this->m_Spacing = 1;
    domain->SetMesh(mesh);
    m_NeighborhoodList.back()->SetWeightingEnabled(!mesh->IsGeodesicsEnabled()); // disable weighting for geodesics
  }
  m_DomainList.push_back(domain);
}

void Sampler::AddContour(vtkSmartPointer<vtkPolyData> poly_data)
{
  auto domain = itk::ContourDomain::New();
  m_NeighborhoodList.push_back(itk::ParticleSurfaceNeighborhood<ImageType>::New());
  if (poly_data != nullptr) {
    this->m_Spacing = 1;
    domain->SetPolyLine(poly_data);
  }
  m_NeighborhoodList.back()->SetWeightingEnabled(false);
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
    m_ParticleSystem->GetDomain(i)->GetConstraints()->addPlane(va, vb, vc);
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

void Sampler::AddFreeFormConstraint(unsigned int i,
                                    const std::vector<std::vector<Eigen::Vector3d> > boundaries,
                                    const Eigen::Vector3d query)
{
  if (m_FFCs.size() < i + 1) {
    m_FFCs.resize(i + 1);
  }

  m_FFCs[i].push_back(FFCType());
  m_FFCs[i][m_FFCs[i].size() - 1].boundaries = boundaries;
  m_FFCs[i][m_FFCs[i].size() - 1].query = query;
}

void Sampler::AddImage(ImageType::Pointer image, double narrow_band, std::string name)
{
  const auto domain = itk::ParticleImplicitSurfaceDomain<ImageType::PixelType>::New();
  m_NeighborhoodList.push_back(itk::ParticleSurfaceNeighborhood<ImageType>::New());

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

std::shared_ptr<Mesh> Sampler::initialize_ffcs(size_t dom)
{
  std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(m_meshes[dom]);

  for (size_t i = 0; i < m_FFCs[dom].size(); i++) {
    if(m_verbosity >= 1) std::cout << "Splitting mesh FFC for domain " << dom << " shape " << i << " with query point " << m_FFCs[dom][i].query.transpose() << std::endl;
    mesh->splitMeshByFFC(m_FFCs[dom][i].boundaries, m_FFCs[dom][i].query, dom, i);
  }

  this->m_DomainList[dom]->GetConstraints()->addFreeFormConstraint(mesh);

#if defined(VIZFFC)
  MeshUtils mutil;
  mutil.visualizeVectorFieldForFFCs(mesh);
#endif

  return mesh;
}

} // end namespace
