
#include "itkParticlePositionReader.h"
#include "itkImageRegionIterator.h"
#include "object_reader.h"
#include "itkParticleImageDomain.h"
#include "MaximumEntropySurfaceSampler.h"

namespace shapeworks
{

MaximumEntropySurfaceSampler::MaximumEntropySurfaceSampler()
{
    m_AdaptivityMode = 0;
    m_Initializing = false;

    m_PrefixTransformFile = "";
    m_TransformFile = "";

    // Allocate the particle system members.
    m_ParticleSystem = itk::ParticleSystem<Dimension>::New();

    m_GradientFunction
      = itk::ParticleEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();
    m_CurvatureGradientFunction
      = itk::ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();

    m_ModifiedCotangentGradientFunction
            = itk::ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();
    m_ConstrainedModifiedCotangentGradientFunction
            = itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();

    m_OmegaGradientFunction
            = itk::ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>::New();

    // Allocate some optimization code.
    m_Optimizer = OptimizerType::New();

    m_Initialized = false;
    m_PointsFiles.push_back("");
    m_MeshFiles.push_back("");
}

void MaximumEntropySurfaceSampler::AllocateDataCaches()
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

    m_MeanCurvatureCache = itk::ParticleMeanCurvatureAttribute<typename ImageType::PixelType, Dimension>::New();
    m_MeanCurvatureCache->SetVerbosity(m_verbosity);
    m_CurvatureGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
    m_OmegaGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
    m_ParticleSystem->RegisterAttribute(m_MeanCurvatureCache);
}

void MaximumEntropySurfaceSampler::AllocateDomainsAndNeighborhoods()
{
    // Allocate all the necessary domains and neighborhoods. This must be done
    // *after* registering the attributes to the particle system since some of
    // them respond to AddDomain.
    int ctr = 0;
    for (unsigned int i = 0; i < this->m_DomainList.size(); i++)
    {
      auto domain = m_DomainList[i];
      if (m_CuttingPlanes.size() > i) {
        for (unsigned int j = 0; j < m_CuttingPlanes[i].size(); j++)
          domain->SetCuttingPlane(m_CuttingPlanes[i][j].a, m_CuttingPlanes[i][j].b, m_CuttingPlanes[i][j].c);
      }

      if (m_Spheres.size() > i) {
        for (unsigned int j = 0; j < m_Spheres[i].size(); j++) {
          domain->AddSphere(m_Spheres[i][j].center, m_Spheres[i][j].radius);
        }
      }

      if(domain->GetDomainType() == shapeworks::DomainType::Image) {
        auto imageDomain = static_cast<itk::ParticleImplicitSurfaceDomain<typename ImageType::PixelType> *>(domain.GetPointer());

        if (m_AttributesPerDomain.size() > 0 && m_AttributesPerDomain[i % m_DomainsPerShape] > 0)
        {
          TriMesh *themesh = TriMesh::read(m_MeshFiles[i].c_str());
          if(themesh != NULL)
          {
              themesh->need_faces();
              themesh->need_neighbors();
              orient(themesh);
              themesh->need_bsphere();
              if (!themesh->normals.empty())
                  themesh->normals.clear();
              themesh->need_normals();
              if (!themesh->tstrips.empty())
                  themesh->tstrips.clear();
              themesh->need_tstrips();
              if (!themesh->adjacentfaces.empty())
                  themesh->adjacentfaces.clear();
              themesh->need_adjacentfaces();
              if (!themesh->across_edge.empty())
                  themesh->across_edge.clear();
              themesh->need_across_edge();
              themesh->need_faceedges();
              themesh->need_oneringfaces();
              themesh->need_abs_curvatures();
              themesh->need_speed();
              themesh->setSpeedType(1);

              imageDomain->SetMesh(themesh);
              imageDomain->SetFids(m_FidsFiles[i].c_str());
              int d = i % m_DomainsPerShape;
              for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
              {
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

void MaximumEntropySurfaceSampler::ReadPointsFiles()
{
    // If points file names have been specified, then read the initial points.
    for (unsigned int i = 0; i < m_PointsFiles.size(); i++)
    {
        if (m_PointsFiles[i] != "")
        {
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

void MaximumEntropySurfaceSampler::InitializeOptimizationFunctions()
{
    // Set the minimum neighborhood radius and maximum sigma based on the
    // domain of the 1st input image.
    unsigned int maxdim = 0;
    double maxradius = -1.0;
    double spacing = this->m_Spacing;

    for (unsigned int d = 0; d < this->GetParticleSystem()->GetNumberOfDomains(); d++)
    {
      if (!GetParticleSystem()->GetDomain(d)->IsDomainFixed()) {
        double radius = GetParticleSystem()->GetDomain(d)->GetMaxDimRadius();
        maxradius = radius > maxradius ? radius : maxradius;
      }
    }

    m_GradientFunction->SetMinimumNeighborhoodRadius(spacing * 5.0);
    m_GradientFunction->SetMaximumNeighborhoodRadius(maxradius);

    m_CurvatureGradientFunction->SetMinimumNeighborhoodRadius(spacing * 5.0);
    m_CurvatureGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
    m_CurvatureGradientFunction->SetParticleSystem(this->GetParticleSystem());
    m_CurvatureGradientFunction->SetDomainNumber(0);

    m_ModifiedCotangentGradientFunction->SetMinimumNeighborhoodRadius(spacing * 5.0);
    m_ModifiedCotangentGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
    m_ModifiedCotangentGradientFunction->SetParticleSystem(this->GetParticleSystem());
    m_ModifiedCotangentGradientFunction->SetDomainNumber(0);

    m_ConstrainedModifiedCotangentGradientFunction->SetMinimumNeighborhoodRadius(spacing * 5.0);
    m_ConstrainedModifiedCotangentGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
    m_ConstrainedModifiedCotangentGradientFunction->SetParticleSystem(this->GetParticleSystem());
    m_ConstrainedModifiedCotangentGradientFunction->SetDomainNumber(0);

    m_OmegaGradientFunction->SetMinimumNeighborhoodRadius(spacing * 5.0);
    m_OmegaGradientFunction->SetMaximumNeighborhoodRadius(maxradius);
    m_OmegaGradientFunction->SetParticleSystem(this->GetParticleSystem());
    m_OmegaGradientFunction->SetDomainNumber(0);
}


void MaximumEntropySurfaceSampler::GenerateData()
{
}


void MaximumEntropySurfaceSampler::Execute()
{
    if (m_Initialized == false)
    {
        this->AllocateDataCaches();
        this->SetAdaptivityMode(m_AdaptivityMode);
        this->AllocateDomainsAndNeighborhoods();

        // Point the optimizer to the particle system.
        m_Optimizer->SetParticleSystem(m_ParticleSystem);
        this->ReadTransforms();
        this->ReadPointsFiles();
        this->InitializeOptimizationFunctions();
        m_Initialized = true;
    }

    if (m_Initializing == true) return;
    m_Optimizer->StartOptimization();
}


void MaximumEntropySurfaceSampler::ReadTransforms()
{
    if (m_TransformFile != "")
    {
        object_reader< itk::ParticleSystem<3>::TransformType > reader;
        reader.SetFileName(m_TransformFile.c_str());
        reader.Update();

        for (unsigned int i = 0; i <this->GetParticleSystem()->GetNumberOfDomains(); i++)
            this->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
    }

    if (m_PrefixTransformFile != "")
    {
        object_reader< itk::ParticleSystem<3>::TransformType > reader;
        reader.SetFileName(m_PrefixTransformFile.c_str());
        reader.Update();

        for (unsigned int i = 0; i < this->GetParticleSystem()->GetNumberOfDomains(); i++)
            this->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
    }

}

void MaximumEntropySurfaceSampler::ReInitialize() {
}


} // end namespace

