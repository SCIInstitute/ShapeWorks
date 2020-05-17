/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkMaximumEntropySurfaceSampler.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkMaximumEntropySurfaceSampler_txx
#define __itkMaximumEntropySurfaceSampler_txx

#include "itkParticlePositionReader.h"
#include "itkImageRegionIterator.h"
#include "itkZeroCrossingImageFilter.h"
#include "object_reader.h"
#include "itkParticleImageDomain.h"

namespace itk
{

template <class TImage>
MaximumEntropySurfaceSampler<TImage>::MaximumEntropySurfaceSampler()
{
    m_AdaptivityMode = 0;
    m_Initializing = false;

    m_PrefixTransformFile = "";
    m_TransformFile = "";

    // Allocate the particle system members.
    m_ParticleSystem = ParticleSystem<Dimension>::New();

    m_GradientFunction
            = ParticleEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();
    m_CurvatureGradientFunction
            = ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();

    m_ModifiedCotangentGradientFunction
            = ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();
    m_ConstrainedModifiedCotangentGradientFunction
            = ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>::New();

    m_OmegaGradientFunction
            = ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>::New();

    // Allocate some optimization code.
    m_Optimizer = OptimizerType::New();

    m_Initialized = false;
    m_PointsFiles.push_back("");
    m_MeshFiles.push_back("");
}

template <class TImage>
void
MaximumEntropySurfaceSampler<TImage>::AllocateDataCaches()
{
    // Set up the various data caches that the optimization functions will use.
    m_Sigma1Cache = ParticleContainerArrayAttribute<double, Dimension>::New();
    m_ParticleSystem->RegisterAttribute(m_Sigma1Cache);
    m_GradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);
    m_CurvatureGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

    m_ModifiedCotangentGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);
    m_ConstrainedModifiedCotangentGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

    m_OmegaGradientFunction->SetSpatialSigmaCache(m_Sigma1Cache);

    m_Sigma2Cache = ParticleContainerArrayAttribute<double, Dimension>::New();
    m_ParticleSystem->RegisterAttribute(m_Sigma2Cache);

    m_MeanCurvatureCache = ParticleMeanCurvatureAttribute<typename ImageType::PixelType, Dimension>::New();
    m_MeanCurvatureCache->SetVerbosity(m_verbosity);
    m_CurvatureGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
    m_OmegaGradientFunction->SetMeanCurvatureCache(m_MeanCurvatureCache);
    m_ParticleSystem->RegisterAttribute(m_MeanCurvatureCache);
}

template <class TImage>
void
MaximumEntropySurfaceSampler<TImage>::AllocateDomainsAndNeighborhoods()
{
    // Allocate all the necessary domains and neighborhoods. This must be done
    // *after* registering the attributes to the particle system since some of
    // them respond to AddDomain.
    int ctr = 0;
    for (unsigned int i = 0; i < this->m_Images.size(); i++)
    {
        m_DomainList.push_back( ParticleImplicitSurfaceDomain<typename
                                ImageType::PixelType, Dimension>::New() );

        m_NeighborhoodList.push_back( ParticleSurfaceNeighborhood<ImageType>::New() );

        typename TImage::Pointer img_temp = this->m_Images[i];

        m_DomainList[i]->SetSigma(img_temp->GetSpacing()[0] * 2.0);


        bool minimal_load = m_ParticleSystem->GetDomainFlag(i);
        m_DomainList[i]->SetImage(img_temp, minimal_load);

        if (m_CuttingPlanes.size() > i)
        {
            for (unsigned int j = 0; j< m_CuttingPlanes[i].size(); j++)
                m_DomainList[i]->SetCuttingPlane(m_CuttingPlanes[i][j].a, m_CuttingPlanes[i][j].b, m_CuttingPlanes[i][j].c);
        }

        if (m_Spheres.size() > i)
        {
            for (unsigned int j = 0; j < m_Spheres[i].size();j++)
            {
                m_DomainList[i]->AddSphere(m_Spheres[i][j].center,m_Spheres[i][j].radius);
            }
        }


        if (m_AttributesPerDomain.size() > 0)
        {
            if (m_AttributesPerDomain[i % m_DomainsPerShape] > 0)
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

                    m_DomainList[i]->SetMesh(themesh);
                    m_DomainList[i]->SetFids(m_FidsFiles[i].c_str());
                    int d = i % m_DomainsPerShape;
                    for (unsigned int c = 0; c < m_AttributesPerDomain[d]; c++)
                    {
                        int ctr1 = ctr++;
                        m_DomainList[i]->SetFeaMesh(m_FeaMeshFiles[ctr1].c_str());
                        m_DomainList[i]->SetFeaGrad(m_FeaGradFiles[ctr1].c_str());
                    }
                }
            }
        }

        // END TEST CUTTING PLANE
        m_ParticleSystem->AddDomain(m_DomainList[i]);
        m_ParticleSystem->SetNeighborhood(i, m_NeighborhoodList[i]);
    }
}

template <class TImage>
void
MaximumEntropySurfaceSampler<TImage>::ReadPointsFiles()
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

template <class TImage>
void
MaximumEntropySurfaceSampler<TImage>::InitializeOptimizationFunctions()
{
    // Set the minimum neighborhood radius and maximum sigma based on the
    // domain of the 1st input image.
    unsigned int maxdim = 0;
    double maxradius = -1.0;
    double spacing = this->GetInput()->GetSpacing()[0];

    for (unsigned int d = 0; d < this->GetParticleSystem()->GetDomainsPerShape(); d++)
    {
        double radius = GetParticleSystem()->GetDomain(d)->GetMaxDimRadius();
        maxradius = radius > maxradius ? radius : maxradius;
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

template <class TImage>
void
MaximumEntropySurfaceSampler<TImage>::GenerateData()
{
    this->SetInPlace(false); // this is required so that we don't release our inputs
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


template <class TImage>
void

MaximumEntropySurfaceSampler<TImage>::ReadTransforms()
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


} // end namespace

#endif
