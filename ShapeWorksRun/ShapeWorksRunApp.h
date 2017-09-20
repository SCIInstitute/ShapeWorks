/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksRunApp.h,v $
  Date:      $Date: 2011/03/24 01:17:37 $
  Version:   $Revision: 1.6 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ShapeWorksRunApp_h
#define __ShapeWorksRunApp_h

#ifdef _WIN32
#pragma warning( disable: 4996 )
#endif

#include "itkParticleSystem.h"
#include <cstdio>
#include "itkImage.h"
#include "itkMaximumEntropyCorrespondenceSampler.h"
#include "itkCommand.h"
#include <vector>
#include "tinyxml.h"
#include "itkParticleProcrustesRegistration.h"
#include <sstream>
#include <string>
#include <numeric>
#include "itkParticleGoodBadAssessment.h"

#include "itkParticleVectorFunction.h"

template<typename T> std::string toStr(T var) {
    std::ostringstream tmp; tmp << var; return tmp.str();
}

template< typename SAMPLERTYPE
          = itk::MaximumEntropyCorrespondenceSampler<itk::Image<float, 3> >  >
class ShapeWorksRunApp 
{
public:
    typedef itk::Image<float, 3> ImageType;
    typedef SAMPLERTYPE SamplerType;

    typedef typename itk::ParticleVectorFunction<3>::VectorType VectorType;

    ShapeWorksRunApp(const char *);
    virtual ~ShapeWorksRunApp();

    void AddAdaptivity();
    virtual void AddSinglePoint();

    SamplerType *GetSampler()  { return m_Sampler.GetPointer(); }
    //  itk::MaximumEntropyCorrespondenceSampler<ImageType> *GetSampler()

    void Initialize();
    virtual void InitializeSampler();
    void IterateCallback(itk::Object *, const itk::EventObject &);

    void Optimize();
    virtual void optimize_start();
    virtual void optimize_stop();

    virtual void ReadInputs(const char *fname);

    virtual void ReadPrefixTransformFile(const std::string &s);
    virtual void ReadTransformFile();
    virtual void Run()
    {
        m_disable_procrustes = true;
        m_disable_checkpointing = true;
        // Initialize
        if ( m_processing_mode >= 0 ) this->Initialize();
        // Introduce adaptivity
        if ( m_processing_mode >= 1 || m_processing_mode == -1) this->AddAdaptivity();
        // Optimize
        if ( m_processing_mode >= 2 || m_processing_mode == -2) this->Optimize();
    }
    virtual void RunProcrustes()
    {
        this->optimize_stop();
        m_Procrustes->RunRegistration();
    }
    virtual void SetIterationCommand()
    {
        m_Iteratecmd  = itk::MemberCommand<ShapeWorksRunApp>::New();
        m_Iteratecmd->SetCallbackFunction(this, &ShapeWorksRunApp::IterateCallback);
        m_Sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), m_Iteratecmd);
    }

    void SetUserParameters(const char *fname);

    virtual void SplitAllParticles() //Praful - this function is now obsolete
    {
        this->optimize_stop();
        m_Sampler->GetParticleSystem()->SplitAllParticles(m_spacing);
    }

    virtual void WriteModes()
    {
        const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
        if ( n >= 5 )
        {
            m_Sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_points_prefix,5);
        }
    }

    // "iter" param used if "keep_checkpoints" param is set to 1.
    virtual void WritePointFiles( int iter = -1 );
    virtual void WriteTransformFile( int iter = -1 ) const;
    virtual void WriteCuttingPlanePoints( int iter = -1);
    virtual void WriteParameters( int iter = -1 );

    virtual void WriteEnergyFiles();
    virtual void ReportBadParticles();

    virtual void ComputeEnergyAfterIteration();

    // SHIREEN
    virtual void WritePointFiles( std::string iter_prefix );
    virtual void WriteTransformFile( std::string iter_prefix ) const;
    // end SHIREEN

    //Praful
    virtual void WritePointFilesWithFeatures( std::string iter_prefix );

    void ReadExplanatoryVariables(const char *fname);

    void FlagDomainFct(const char *fname);

    void SetCotanSigma();
    double GetMinNeighborhoodRadius();

    typename itk::MemberCommand< ShapeWorksRunApp<SamplerType> >::Pointer m_Iteratecmd;

protected:
    typename itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_Sampler;
    typename itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;
    typename itk::ParticleGoodBadAssessment<float, 3>::Pointer m_GoodBad;

    int m_CheckpointCounter;
    int m_ProcrustesCounter;


    static ITK_THREAD_RETURN_TYPE optimize_callback( void *arg );
    //  static ITK_THREAD_RETURN_TYPE auto_init_callback( void *arg );

    // Keeps track of which state the optimization is in.
    unsigned int m_mode;
    double m_spacing;
    bool m_optimizing;
    bool m_use_normal_penalty;
    bool m_use_initial_normal_penalty;
    bool m_use_regression;
    bool m_use_mixed_effects;


    // User-specified parameters
    int m_optimization_iterations;
    int m_optimization_iterations_completed;
    int m_iterations_per_split;
    int m_processing_mode;
    int m_procrustes_scaling;
    std::string m_output_points_prefix;
    std::string m_output_transform_file;
    std::string m_output_cutting_plane_file;

    std::vector < std::string > m_filenames;

    double m_starting_regularization;
    double m_ending_regularization;
    int m_recompute_regularization_interval;
    double m_relative_weighting;
    double m_norm_penalty_weighting;
    double m_initial_relative_weighting;
    double m_initial_norm_penalty_weighting;
    double m_adaptivity_strength;


    //  unsigned int m_number_of_particles;
    std::vector<unsigned int> m_number_of_particles;
    std::vector<int> m_attributes_per_domain;


    std::vector<double> m_EnergyA;
    std::vector<double> m_EnergyB;
    std::vector<double> m_TotalEnergy;

    unsigned int m_checkpointing_interval;
    unsigned int m_domains_per_shape;
    unsigned int m_timepts_per_subject;
    std::string m_transform_file;
    std::string m_prefix_transform_file;
    unsigned int m_procrustes_interval;
    bool m_disable_checkpointing;
    bool m_disable_procrustes;

    std::vector<std::vector<int> > m_badIds; //GoodBadAssessment
    double m_normalAngle; //GoodBadAssessment
    bool m_performGoodBad;

    int m_spheres_per_domain;
    int m_adaptivity_mode;
    int m_keep_checkpoints;
    bool m_use_cutting_planes;
    int m_distribution_domain_id;
    int m_SaturationCounter;
    std::string m_strEnergy;
    // PRATEEP -- SHIREEN UPDATE
    // optimizer settings
    int m_optimizer_type; // 0 : jacobi
    // 1 : gauss seidel
    // 2 : adaptive gauss seidel (with bad moves)

    int m_pairwise_potential_type; // 0 - gaussian (Cates work), 1 - modified cotangent (Meyer),

    // SHIREEN
    bool m_save_init_splits;
    bool m_use_shape_statistics_in_init;
    // end SHIREEN

    //Praful - shape updates in randomized order
    bool m_debug_projection;

    //Praful - mesh based attributes
    bool m_mesh_based_attributes;

    double m_init_criterion, m_opt_criterion;

    double m_cotan_sigma_factor;

    std::vector<bool> m_use_xyz;
    std::vector<bool> m_use_normals;

};

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/ShapeWorksRunApp.txx+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "ShapeWorksRunApp.txx"
#endif

#endif
