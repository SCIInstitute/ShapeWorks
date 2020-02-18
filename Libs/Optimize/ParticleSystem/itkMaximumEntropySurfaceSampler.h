/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkMaximumEntropySurfaceSampler.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkMaximumEntropySurfaceSampler_h
#define __itkMaximumEntropySurfaceSampler_h

#include "itkParticleSystem.h"
#include "itkParticleGradientDescentPositionOptimizer.h"
#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleQualifierEntropyGradientFunction.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "itkInPlaceImageFilter.h"
#include "itkParticleContainerArrayAttribute.h"
#include "itkParticleCurvatureEntropyGradientFunction.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkParticleSurfaceNeighborhood.h"
#include "itkParticleOmegaGradientFunction.h"

#include "itkParticleModifiedCotangentEntropyGradientFunction.h"
#include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h"
#include "vnl/vnl_matrix_fixed.h"


#include "TriMesh.h"

namespace itk
{

/** \class MaximumEntropySurfaceSampler
 *
 *
 *
 */
template <class TImage>
class ITK_EXPORT MaximumEntropySurfaceSampler
        : public InPlaceImageFilter<TImage,TImage>
{
public:
    /** Standard class typedefs. */
    typedef MaximumEntropySurfaceSampler  Self;
    typedef InPlaceImageFilter<TImage,TImage>  Superclass;
    typedef SmartPointer<Self>   Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(MaximumEntropySurfaceSampler, InPlaceImageFilter);

    /** Expose the image dimension. */
    itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);

    typedef ParticleMeanCurvatureAttribute<typename TImage::PixelType, Dimension>  MeanCurvatureCacheType;

    /** Convenient typedef for storing cutting plane information */
    struct CuttingPlaneType
    {
        vnl_vector_fixed<double,Dimension> a;
        vnl_vector_fixed<double,Dimension> b;
        vnl_vector_fixed<double,Dimension> c;
    };

    /** Convenient typedef for storing sphere information */
    struct SphereType
    {
        vnl_vector_fixed<double,Dimension> center;
        double radius;
    };

    /** Tranform Type*/
    typedef vnl_matrix_fixed<double, Dimension +1, Dimension +1> TransformType;

    /** Type of the input/output image. */
    typedef TImage ImageType;
    typedef ParticleGradientDescentPositionOptimizer<typename ImageType::PixelType, Dimension> OptimizerType;

    /**
   * THIS IS A HACK UNTIL I CAN FIGURE OUT HOW TO ALLOCATE THE APPROPRIATE
   * NUMBER OF OUTPUTS AND GRAFT THEM TO THE INPUTS.
   */
    void SetInput(const TImage *image)
    { this->SetInput(0, image);  }


    /**
   * Override parent classes to expand input list on new inputs.
   */
    void SetInput( unsigned int index, const TImage * image )
    {
        if (this->GetNumberOfInputs() < index+1)
        {
            this->SetNumberOfRequiredInputs(index+1);
        }

        this->ProcessObject::SetNthInput(index, const_cast< TImage *>( image ) );
    }

    /** Returns the particle system used in the surface sampling. */
    itkGetObjectMacro(ParticleSystem, ParticleSystem<Dimension>);
    itkGetConstObjectMacro(ParticleSystem, ParticleSystem<Dimension>);

    /** Returns a pointer to the gradient function used. */
    ParticleEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    *GetGradientFunction()
    {
        return m_GradientFunction;
    }

    ParticleQualifierEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    *GetQualifierGradientFunction()
    {
        return m_QualifierGradientFunction;
    }
    ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    *GetCurvatureGradientFunction()
    {
        return m_CurvatureGradientFunction;
    }

    ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    *GetModifiedCotangentGradientFunction()
    {
        return m_ModifiedCotangentGradientFunction;
    }
    ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    *GetConstrainedModifiedCotangentGradientFunction()
    {
        return m_ConstrainedModifiedCotangentGradientFunction;
    }

    ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>
    *GetOmegaGradientFunction()
    {
        return m_OmegaGradientFunction;
    }

    /** Return a pointer to the optimizer object. */
    itkGetObjectMacro(Optimizer, OptimizerType);
    itkGetConstObjectMacro(Optimizer, OptimizerType);

    /**Optionally provide a filename for an initial point set.*/
    void SetPointsFile(unsigned int i, const std::string &s)
    {
        if (m_PointsFiles.size() < i+1)
        {
            m_PointsFiles.resize(i+1);
        }
        m_PointsFiles[i] = s;
    }

    void SetPointsFile(const std::string &s)
    {
        this->SetPointsFile(0,s);
    }

    /**Optionally provide a filename for a mesh with geodesic distances.*/
    void SetMeshFile(unsigned int i, const std::string &s)
    {
        if (m_MeshFiles.size() < i+1)
        {
            m_MeshFiles.resize(i+1);
        }
        m_MeshFiles[i] = s;
    }
    void SetMeshFile(const std::string &s)
    {
        this->SetMeshFile(0,s);
    }

    void SetMeshFiles(const std::vector<std::string> &s)
    {
        m_MeshFiles = s;
    }

    void SetImages(const std::vector<typename TImage::Pointer> &images)
    {
        m_Images = images;
    }

    void SetFidsFiles(const std::vector<std::string> &s)
    {
        m_FidsFiles = s;
    }
    void SetFeaFiles(const std::vector<std::string> &s)
    {
        m_FeaMeshFiles = s;
    }
    void SetFeaGradFiles(const std::vector<std::string> &s)
    {
        m_FeaGradFiles = s;
    }

    void SetDomainsPerShape(int i)
    {
        m_DomainsPerShape = i;
    }
    void SetAttributesPerDomain(const std::vector<int> &i)
    {
        m_AttributesPerDomain = i;
    }


    /** Optionally supply a cutting plane that will be set as a particle
      optimization constraint in the image domains. */
    void SetCuttingPlane(unsigned int i,
                         const vnl_vector_fixed<double,Dimension> &va,
                         const vnl_vector_fixed<double,Dimension> &vb,
                         const vnl_vector_fixed<double,Dimension> &vc)
    {
        if (m_CuttingPlanes.size() < i+1)
        {
            m_CuttingPlanes.resize(i+1);
        }

        m_CuttingPlanes[i].push_back(CuttingPlaneType());

        m_CuttingPlanes[i][m_CuttingPlanes[i].size()-1].a = va;
        m_CuttingPlanes[i][m_CuttingPlanes[i].size()-1].b = vb;
        m_CuttingPlanes[i][m_CuttingPlanes[i].size()-1].c = vc;

        if (m_Initialized == true)
        {
            m_DomainList[i]->SetCuttingPlane(va,vb,vc);
        }
    }

    /** Transform a cutting plane based on procrustes transformation */
    void  TransformCuttingPlanes(unsigned int i)
    {
        if (m_Initialized == true)
        {
            TransformType T1 = this->GetParticleSystem()->GetTransform(i) * this->GetParticleSystem()->GetPrefixTransform(i);
            vnl_vector_fixed<double, Dimension> fa = m_DomainList[i]->GetA();
            vnl_vector_fixed<double, Dimension> fb = m_DomainList[i]->GetB();
            vnl_vector_fixed<double, Dimension> fc = m_DomainList[i]->GetC();
            /* Copy vnl_vector_fixed to vnl_vector */
            vnl_vector<double> a(Dimension); vnl_vector<double> b(Dimension); vnl_vector<double> c(Dimension);
            for(unsigned int k = 0; k < Dimension; k++)
            {
                a[k] = fa[k]; b[k] = fb[k]; c[k] = fc[k];
            }
            for(unsigned int d = 0; d < this->GetParticleSystem()->GetNumberOfDomains(); d++)
            {
                if (this->GetParticleSystem()->GetDomainFlag(d) == false)
                {
                    TransformType T2 = this->GetParticleSystem()->InvertTransform( this->GetParticleSystem()->GetTransform(d)
                                                                                   * this->GetParticleSystem()->GetPrefixTransform(d));
                    m_DomainList[d]->TransformCuttingPlane( T2 * T1, a, b, c );
                }
            }
        }
    }

    /** Optionally add spheres that may be used as constraints to the domain. */

    void AddSphere(unsigned int i, vnl_vector_fixed<double,Dimension> &c, double r)
    {
        if (m_Spheres.size() < i+1)
        {
            m_Spheres.resize(i+1);
        }

        m_Spheres[i].push_back( SphereType() );
        m_Spheres[i][m_Spheres[i].size()-1].center = c;
        m_Spheres[i][m_Spheres[i].size()-1].radius = r;

        if (m_Initialized == true)
        {
            m_DomainList[i]->AddSphere(c,r);
        }
    }


    /** This method sets the optimization function for the sampling.
      mode 0 = no adaptivity
      mode 1 = isotropic adaptivity
      mode 2 = anisotropic adaptivity
  */
    virtual void SetAdaptivityMode(int mode)
    {
        if (mode == 0)
        {
            if(m_pairwise_potential_type == 0)
                m_Optimizer->SetGradientFunction(m_CurvatureGradientFunction);
            else if(m_pairwise_potential_type == 1)
                m_Optimizer->SetGradientFunction(m_ModifiedCotangentGradientFunction);
        }
        else if (mode ==1)
        {
            m_Optimizer->SetGradientFunction(m_GradientFunction);
        }
        else if (mode ==2)
        {
            m_Optimizer->SetGradientFunction(m_QualifierGradientFunction);
        }
        else if (mode ==3)
        {
            if(m_pairwise_potential_type == 0)
                m_Optimizer->SetGradientFunction(m_OmegaGradientFunction);
            else if(m_pairwise_potential_type == 1)
                m_Optimizer->SetGradientFunction(m_ConstrainedModifiedCotangentGradientFunction);
        }

        m_AdaptivityMode = mode;
        this->Modified();

    }
    int GetAdaptivityMode() const
    { return m_AdaptivityMode; }

    void SetTransformFile(const std::string& s)
    { m_TransformFile = s; }
    void SetTransformFile(const char *s)
    { m_TransformFile = std::string(s); }

    void SetPrefixTransformFile(const std::string& s)
    { m_PrefixTransformFile = s; }
    void SetPrefixTransformFile(const char *s)
    { m_PrefixTransformFile = std::string(s); }

    void SetPairwisePotentialType(int pairwise_potential_type)
    { m_pairwise_potential_type = pairwise_potential_type; }

    int GetPairwisePotentialType()
    {return m_pairwise_potential_type;}

    void SetVerbosity(unsigned int val)
    {
        m_verbosity = val;
        m_Optimizer->SetVerbosity(val);
    }

    unsigned int GetVerbosity()
    { return m_verbosity; }

    MeanCurvatureCacheType *GetMeanCurvatureCache()
    {   return  m_MeanCurvatureCache.GetPointer();  }

    void ReadTransforms();
    void ReadPointsFiles();
    virtual void AllocateDataCaches();
    virtual void AllocateDomainsAndNeighborhoods();
    virtual void InitializeOptimizationFunctions();
    virtual void DeleteImages() {
        for(int i=0; i<m_Images.size(); i++) {
            m_DomainList[i]->DeleteImages();
            m_Images[i] = 0;
        }
    }

    /** */
    virtual void Initialize()
    {
        this->m_Initializing = true;
        this->Update();
        this->m_Initializing = false;
    }

protected:
    MaximumEntropySurfaceSampler();
    virtual ~MaximumEntropySurfaceSampler() {};

    void PrintSelf(std::ostream& os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
    }

    void GenerateData();

    itkSetMacro(Initialized, bool);
    itkGetMacro(Initialized, bool);
    itkSetMacro(Initializing, bool);
    itkGetMacro(Initializing, bool);

    bool m_Initialized;
    int m_AdaptivityMode;
    bool m_Initializing;

    typename OptimizerType::Pointer m_Optimizer;

    typename ParticleEntropyGradientFunction<typename ImageType::PixelType,  Dimension>
    ::Pointer m_GradientFunction;
    typename ParticleQualifierEntropyGradientFunction<typename ImageType::PixelType,  Dimension>
    ::Pointer m_QualifierGradientFunction;
    typename ParticleCurvatureEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    ::Pointer m_CurvatureGradientFunction;

    typename ParticleModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    ::Pointer m_ModifiedCotangentGradientFunction;
    typename ParticleConstrainedModifiedCotangentEntropyGradientFunction<typename ImageType::PixelType, Dimension>
    ::Pointer m_ConstrainedModifiedCotangentGradientFunction;


    typename ParticleOmegaGradientFunction<typename ImageType::PixelType, Dimension>
    ::Pointer m_OmegaGradientFunction;

    typename ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma1Cache;
    typename ParticleContainerArrayAttribute<double, Dimension>::Pointer m_Sigma2Cache;

    typename MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;

    typename ParticleSystem<Dimension>::Pointer m_ParticleSystem;

    std::vector<typename ParticleImplicitSurfaceDomain<typename
    ImageType::PixelType, Dimension>::Pointer> m_DomainList;

    std::vector<typename ParticleSurfaceNeighborhood<ImageType>::Pointer> m_NeighborhoodList;

    int m_pairwise_potential_type;

private:
    MaximumEntropySurfaceSampler(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    std::vector<std::string> m_PointsFiles;
    std::vector<std::string> m_MeshFiles;
    std::vector<std::string> m_FeaMeshFiles;
    std::vector<std::string> m_FeaGradFiles;
    std::vector<std::string> m_FidsFiles;
    std::vector<int> m_AttributesPerDomain;
    int m_DomainsPerShape;

    std::vector<typename TImage::Pointer> m_Images;
    std::string m_TransformFile;
    std::string m_PrefixTransformFile;
    std::vector< std::vector< CuttingPlaneType> > m_CuttingPlanes;
    std::vector< std::vector<SphereType> > m_Spheres;

    unsigned int m_verbosity;

};

} // end namespace itk

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkMaximumEntropySurfaceSampler+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkMaximumEntropySurfaceSampler.txx"
#endif

#include "itkMaximumEntropySurfaceSampler.txx"

#endif


