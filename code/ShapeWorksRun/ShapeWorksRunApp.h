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
#include "param.h"
#include "itkParticleProcrustesRegistration.h"
#include <sstream>
#include <string>

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

  virtual void ReadInputs(param::parameterFile &pf);
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
  void SetUserParameters( param::parameterFile &);

  virtual void SplitAllParticles()
  {
   this->optimize_stop();
		m_Sampler->GetParticleSystem()->SplitAllParticles(m_spacing);
  }
  
  virtual void WriteModes()
  {
    m_Sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_points_prefix,5);
  }

  // "iter" param used if "keep_checkpoints" param is set to 1.
  virtual void WritePointFiles( int iter = -1 );  
  virtual void WriteTransformFile( int iter = -1 ) const;
  virtual void WriteParameters( int iter = -1 );  

  void ReadExplanatoryVariables(param::parameterFile &);
  void FlagDomainFct(param::parameterFile &pf);


  typename itk::MemberCommand< ShapeWorksRunApp<SamplerType> >::Pointer m_Iteratecmd;
  
protected:
  typename itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_Sampler;
  typename itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;
  
  int m_CheckpointCounter;
  int m_ProcrustesCounter;
  
  static ITK_THREAD_RETURN_TYPE optimize_callback( void *arg );
  //  static ITK_THREAD_RETURN_TYPE auto_init_callback( void *arg );  

  // Keeps track of which state the optimization is in.
  unsigned int m_mode;
  double m_spacing;
  bool m_optimizing;
  bool m_use_regression;

  // User-specified parameters
  int m_optimization_iterations;
  int m_iterations_per_split;
  int m_processing_mode;
  int m_procrustes_scaling; 
  std::string m_output_points_prefix;
  std::string m_output_transform_file;
  unsigned int m_number_of_particles;
  double m_starting_regularization;
  double m_ending_regularization;
  int m_recompute_regularization_interval;
  double m_relative_weighting;
  double m_norm_penalty_weighting;
  double m_adaptivity_strength;
  unsigned int m_attributes_per_domain;
  unsigned int m_checkpointing_interval;
  unsigned int m_domains_per_shape;
  std::string m_transform_file;
  std::string m_prefix_transform_file;
  unsigned int m_procrustes_interval;
  bool m_disable_checkpointing;
  bool m_disable_procrustes;
  int m_spheres_per_domain;
  int m_adaptivity_mode;
  int m_keep_checkpoints;
};

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/ShapeWorksRunApp.txx+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "ShapeWorksRunApp.txx"
#endif

#endif
