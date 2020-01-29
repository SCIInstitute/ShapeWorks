/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   File:      ShapeWorksRunApp.h

   Copyright (c) 2020 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#pragma once

#ifdef _WIN32
#pragma warning( disable: 4996 )
#endif

// std
#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <numeric>

// itk
#include "itkImage.h"
#include "itkMaximumEntropyCorrespondenceSampler.h"
#include "itkCommand.h"
#include "itkParticleProcrustesRegistration.h"
#include "itkParticleGoodBadAssessment.h"
#include "itkParticleVectorFunction.h"

// tinyxml
#include "tinyxml.h"

// optimize library
#include <itkParticleSystem.h>

class Optimize
{
public:
  typedef itk::Image<float, 3> ImageType;
  typedef itk::MaximumEntropyCorrespondenceSampler<itk::Image<float, 3>> SamplerType;
  SamplerType* GetSampler() { return m_Sampler.GetPointer(); }

  typedef typename itk::ParticleVectorFunction<3>::VectorType VectorType;
  typename itk::MemberCommand<Optimize>::Pointer m_Iteratecmd;

  virtual bool Run();

  virtual void RunProcrustes()
  {
    this->optimize_stop();
    m_Procrustes->RunRegistration();
  }

  virtual void SetIterationCommand()
  {
    m_Iteratecmd = itk::MemberCommand<Optimize>::New();
    m_Iteratecmd->SetCallbackFunction(this, &Optimize::IterateCallback);
    m_Sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), m_Iteratecmd);
  }

  virtual void WriteModes()
  {
    const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains() % m_domains_per_shape;
    if (n >= 5) {
      m_Sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_dir + "/pts", 5);
    }
  }

  void startMessage(std::string str, unsigned int vlevel = 0) const
  {
    if (m_verbosity_level > vlevel) {
      std::cout << str;
      std::cout.flush();
    }
  }

  void doneMessage(unsigned int vlevel = 0) const
  {
    if (m_verbosity_level > vlevel) {
      std::cout << "Done." << std::endl;
    }
  }

  // constrictor
  Optimize();

  void LoadParameters(const char* fname);

  virtual ~Optimize();

  void SetVerbosity(int verbosity_level);
  void SetDomainsPerShape(int domains_per_shape);
  int GetDomainsPerShape();
  void SetNumberOfParticles(std::vector<unsigned int> number_of_particles);
  std::vector<unsigned int> GetNumberOfParticles();

  void SetTransformFile(std::string filename);
  void SetPrefixTransformFile(std::string prefix_transform_file);
  void SetOutputDir(std::string output_dir);
  void SetOutputTransformFile(std::string output_transform_file);
  void SetUseMeshBasedAttributes(bool use_mesh_based_attributes);
  void SetUseXYZ(std::vector<bool> use_xyz);
  void SetUseNormals(std::vector<bool> use_normals);
  void SetAttributesPerDomain(std::vector<int> attributes_per_domain);
  void SetDistributionDomainID(int distribution_domain_id);
  void SetOutputCuttingPlaneFile(std::string output_cutting_plane_file);

  // optimization parameters
  void SetProcessingMode(int mode);
  void SetAdaptivityMode(int adaptivity_mode);
  void SetAdaptivityStrength(double adaptivity_strength);

  virtual void ReadOptimizationParameters(const char* fname);
  void SetDebugParameters(const char* fname);
  virtual void ReadInputs(const char* fname);
  virtual void ReadMeshInputs(const char* fname);
  virtual void ReadConstraints(const char* fname);
  virtual void ReadDistributionCuttingPlane(const char* fname);
  virtual void ReadCuttingPlanes(const char* fname);
  virtual void ReadCuttingSpheres(const char* fname);
  void ReadExplanatoryVariables(const char* fname);
  std::vector<int> FlagParticlesFct(const char* fname);
  std::vector<int> FlagDomainFct(const char* fname);
  virtual void ReadTransformFile();
  virtual void ReadPrefixTransformFile(const std::string &s);

  virtual void InitializeSampler();
  double GetMinNeighborhoodRadius();
  virtual void AddSinglePoint();
  void Initialize();
  void AddAdaptivity();
  void RunOptimize();
  virtual void optimize_start();
  virtual void optimize_stop();
  void IterateCallback(itk::Object*, const itk::EventObject &);
  virtual void ComputeEnergyAfterIteration();
  void SetCotanSigma();

  void PrintParamInfo();

  virtual void WriteTransformFile(int iter = -1) const;
  virtual void WriteTransformFile(std::string iter_prefix) const;
  virtual void WritePointFiles(int iter = -1);
  virtual void WritePointFiles(std::string iter_prefix);
  virtual void WritePointFilesWithFeatures(int iter = -1);
  virtual void WritePointFilesWithFeatures(std::string iter_prefix);
  virtual void WriteEnergyFiles();
  virtual void WriteCuttingPlanePoints(int iter = -1);
  virtual void WriteParameters(int iter = -1);
  virtual void ReportBadParticles();

protected:
  typename itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_Sampler;
  typename itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;
  typename itk::ParticleGoodBadAssessment<float, 3>::Pointer m_GoodBad;

  static itk::ITK_THREAD_RETURN_TYPE optimize_callback(void* arg);

  unsigned int m_verbosity_level = 0;

  int m_CheckpointCounter = 0;
  int m_ProcrustesCounter = 0;
  int m_SaturationCounter = 0;
  bool m_disable_procrustes = true;
  bool m_disable_checkpointing = true;
  bool m_use_cutting_planes = false;
  bool m_optimizing = false;
  bool m_use_regression = false;
  bool m_use_mixed_effects = false;

  // ReadIOParameters
  unsigned int m_domains_per_shape = 1;
  std::vector<unsigned int> m_number_of_particles;
  std::string m_transform_file;
  std::string m_prefix_transform_file;
  std::string m_output_dir;
  std::string m_output_transform_file;
  bool m_mesh_based_attributes = false;
  std::vector<bool> m_use_xyz;
  std::vector<bool> m_use_normals;
  std::vector<int> m_attributes_per_domain;
  int m_distribution_domain_id = -1;
  std::string m_output_cutting_plane_file;

  // Optimization Parameters
  int m_processing_mode = 3;
  int m_adaptivity_mode = 0;
  double m_adaptivity_strength = 0.0;
  int m_pairwise_potential_type = 0;   // 0 - gaussian (Cates work), 1 - modified cotangent (Meyer),
  int m_optimizer_type = 2;   // 0 : jacobi, 1 : gauss seidel, 2 : adaptive gauss seidel (with bad moves)
  unsigned int m_timepts_per_subject = 1;
  int m_optimization_iterations = 2000;
  int m_optimization_iterations_completed = 0;
  int m_iterations_per_split = 1000;
  double m_init_criterion = 1e-6;
  double m_opt_criterion = 1e-6;
  bool m_use_shape_statistics_in_init = false;
  unsigned int m_procrustes_interval = 3;
  int m_procrustes_scaling = 1;
  double m_relative_weighting = 1.0;
  double m_initial_relative_weighting = 0.05;
  double m_starting_regularization = 1000;
  double m_ending_regularization = 1.0;
  int m_recompute_regularization_interval = 1;
  bool m_save_init_splits = true;
  unsigned int m_checkpointing_interval = 50;
  int m_keep_checkpoints = 0;
  double m_cotan_sigma_factor = 5.0;
  std::vector <int> m_p_flgs;
  std::vector <int> m_d_flgs;

  // Keeps track of which state the optimization is in.
  unsigned int m_mode;
  double m_spacing;

  std::vector < std::string > m_filenames;
  std::vector < std::string > pointFiles;

  std::vector<double> m_EnergyA;
  std::vector<double> m_EnergyB;
  std::vector<double> m_TotalEnergy;
  bool m_logEnergy;
  std::string m_strEnergy;

  //GoodBadAssessment
  std::vector<std::vector<int>> m_badIds;
  double m_normalAngle;
  bool m_performGoodBad;

  std::vector <int> m_cutting_planes_per_input;
  std::vector <int> m_spheres_per_input;
  std::vector<double> cpVals;
  std::vector<double> spVals;
  std::vector<double> radList;
};
