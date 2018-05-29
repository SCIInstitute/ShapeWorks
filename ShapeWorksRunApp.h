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
    SamplerType *GetSampler()  { return m_Sampler.GetPointer(); }

    typedef typename itk::ParticleVectorFunction<3>::VectorType VectorType;
    typename itk::MemberCommand< ShapeWorksRunApp<SamplerType> >::Pointer m_Iteratecmd;

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

    virtual void WriteModes()
    {
        const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
        if ( n >= 5 )
        {
            m_Sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_dir,5);
        }
    }

    ShapeWorksRunApp(const char *);
    virtual ~ShapeWorksRunApp();

    void SetUserParameters(const char *fname);
    virtual void ReadIOParameters(const char *fname);
    virtual void ReadOptimizationParameters(const char *fname);
    virtual void ReadInputs(const char *fname);
    virtual void ReadMeshInputs(const char *fname);
    virtual void ReadConstraints(const char *fname);
    virtual void ReadDistributionCuttingPlane(const char *fname);
    virtual void ReadCuttingPlanes(const char *fname);
    virtual void ReadCuttingSpheres(const char *fname);
    void ReadExplanatoryVariables(const char *fname);
    std::vector<int> FlagParticlesFct(const char *fname);
    std::vector<int> FlagDomainFct(const char *fname);
    virtual void ReadTransformFile();
    virtual void ReadPrefixTransformFile(const std::string &s);

    virtual void InitializeSampler();
    double GetMinNeighborhoodRadius();
    virtual void AddSinglePoint();
    void Initialize();
    void AddAdaptivity();
    void Optimize();
    virtual void optimize_start();
    virtual void optimize_stop();
    void IterateCallback(itk::Object *, const itk::EventObject &);
    virtual void ComputeEnergyAfterIteration();
    void SetCotanSigma();

    void PrintParamInfo();

    virtual void WriteTransformFile( int iter = -1 ) const;
    virtual void WriteTransformFile( std::string iter_prefix ) const;
    virtual void WritePointFiles( int iter = -1 );
    virtual void WritePointFiles( std::string iter_prefix );
    virtual void WritePointFilesWithFeatures( int iter = -1);
    virtual void WritePointFilesWithFeatures( std::string iter_prefix );
    virtual void WriteEnergyFiles();
    virtual void WriteCuttingPlanePoints( int iter = -1);
    virtual void WriteParameters( int iter = -1 );
    virtual void ReportBadParticles();

protected:
    typename itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_Sampler;
    typename itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;
    typename itk::ParticleGoodBadAssessment<float, 3>::Pointer m_GoodBad;

    static ITK_THREAD_RETURN_TYPE optimize_callback( void *arg );
    //  static ITK_THREAD_RETURN_TYPE auto_init_callback( void *arg );

    // constructor
    int m_CheckpointCounter;
    int m_ProcrustesCounter;
    int m_SaturationCounter;
    bool m_disable_procrustes;
    bool m_disable_checkpointing;
    bool m_use_cutting_planes;
    bool m_optimizing;
    bool m_use_regression;
    bool m_use_mixed_effects;

    // ReadIOParameters
    unsigned int m_domains_per_shape;
    std::vector<unsigned int> m_number_of_particles;
    std::string m_transform_file;
    std::string m_prefix_transform_file;
    std::string m_output_dir;
    std::string m_output_transform_file;
    bool m_mesh_based_attributes;
    std::vector<bool> m_use_xyz;
    std::vector<bool> m_use_normals;
    std::vector<int> m_attributes_per_domain;
    int m_distribution_domain_id;
    std::string m_output_cutting_plane_file;

    // ReadOptimizationParameters
    int m_processing_mode;
    int m_adaptivity_mode;
    double m_adaptivity_strength;
    int m_pairwise_potential_type; // 0 - gaussian (Cates work), 1 - modified cotangent (Meyer),
    int m_optimizer_type; // 0 : jacobi, 1 : gauss seidel, 2 : adaptive gauss seidel (with bad moves)
    unsigned int m_timepts_per_subject;
    int m_optimization_iterations;
    int m_optimization_iterations_completed;
    int m_iterations_per_split;
    double m_init_criterion;
    double m_opt_criterion;
    bool m_use_shape_statistics_in_init;
    unsigned int m_procrustes_interval;
    int m_procrustes_scaling;
    double m_relative_weighting;
    double m_initial_relative_weighting;
    double m_starting_regularization;
    double m_ending_regularization;
    int m_recompute_regularization_interval;
    bool m_save_init_splits;
    unsigned int m_checkpointing_interval;
    int m_keep_checkpoints;
    double m_cotan_sigma_factor;
    std::vector <int> m_p_flgs;
    std::vector <int> m_d_flgs;

    // Keeps track of which state the optimization is in.
    unsigned int m_mode;
    double m_spacing;

    std::vector < std::string > m_filenames;

    std::vector<double> m_EnergyA;
    std::vector<double> m_EnergyB;
    std::vector<double> m_TotalEnergy;
    bool m_logEnergy;
    std::string m_strEnergy;

    std::vector<std::vector<int> > m_badIds; //GoodBadAssessment
    double m_normalAngle; //GoodBadAssessment
    bool m_performGoodBad;
    bool m_debug_projection;

    std::vector <int> m_cutting_planes_per_input;
    std::vector <int> m_spheres_per_input;
};

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/ShapeWorksRunApp.txx+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "ShapeWorksRunApp.txx"
#endif

#endif
