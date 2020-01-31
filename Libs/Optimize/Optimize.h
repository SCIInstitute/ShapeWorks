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
//#include <cstdio>
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

// optimize library
#include <itkParticleSystem.h>

class Optimize
{
public:
  using ImageType = itk::Image<float, 3>;
  using SamplerType = itk::MaximumEntropyCorrespondenceSampler<itk::Image<float, 3>>;
  using VectorType = itk::ParticleVectorFunction<3>::VectorType;

  // constructor
  Optimize();

  virtual ~Optimize();

  SamplerType* GetSampler() { return m_sampler.GetPointer(); }

  void SetParameters();

  virtual bool Run();

  virtual void RunProcrustes();

  virtual void SetIterationCommand();

  virtual void WriteModes();


  void SetVerbosity(int verbosity_level);
  void SetDomainsPerShape(int domains_per_shape);
  int GetDomainsPerShape();
  void SetNumberOfParticles(std::vector<unsigned int> number_of_particles);
  std::vector<unsigned int> GetNumberOfParticles();

  void SetTransformFile(std::string filename);
  std::string GetTransformFile();
  void SetPrefixTransformFile(std::string prefix_transform_file);
  std::string GetPrefixTransformFile();
  void SetOutputDir(std::string output_dir);
  void SetOutputTransformFile(std::string output_transform_file);
  void SetUseMeshBasedAttributes(bool use_mesh_based_attributes);
  bool GetUseMeshBasedAttributes();
  void SetUseXYZ(std::vector<bool> use_xyz);
  void SetUseNormals(std::vector<bool> use_normals);
  void SetAttributesPerDomain(std::vector<int> attributes_per_domain);
  std::vector<int> GetAttributesPerDomain();
  void SetDistributionDomainID(int distribution_domain_id);
  int GetDistributionDomainID();
  void SetOutputCuttingPlaneFile(std::string output_cutting_plane_file);
  void SetUseCuttingPlanes(bool use_cutting_planes);
  void SetCuttingPlane(unsigned int i,
                       const vnl_vector_fixed<double, 3> &va,
                       const vnl_vector_fixed<double, 3> &vb,
                       const vnl_vector_fixed<double, 3> &vc);

  // optimization parameters
  void SetProcessingMode(int mode);
  void SetAdaptivityMode(int adaptivity_mode);
  void SetAdaptivityStrength(double adaptivity_strength);
  void SetPairwisePotentialType(int pairwise_potential_type);
  void SetOptimizerType(int optimizer_type);
  void SetTimePtsPerSubject(int time_pts_per_subject);
  int GetTimePtsPerSubject();
  void SetOptimizationIterations(int optimization_iterations);
  void SetOptimizationIterationsCompleted(int optimization_iterations_completed);
  void SetIterationsPerSplit(int iterations_per_split);
  void SetInitCriterion(double init_criterion);
  void SetOptCriterion(double opt_criterion);
  void SetUseShapeStatisticsInInit(bool use_shape_statistics_in_init);
  void SetProcrustesInterval(int procrustes_interval);
  void SetProcrustesScaling(int procrustes_scaling);
  void SetRelativeWeighting(double relative_weighting);
  void SetInitialRelativeWeighting(double initial_relative_weighting);
  void SetStartingRegularization(double starting_regularization);
  void SetEndingRegularization(double ending_regularization);
  void SetRecomputeRegularizationInterval(int recompute_regularization_interval);
  void SetSaveInitSplits(bool save_init_splits);
  void SetCheckpointingInterval(int checkpointing_interval);
  void SetKeepCheckpoints(int keep_checkpoints);
  void SetCotanSigmaFactor(double cotan_sigma_factor);

  void SetUseRegression(bool use_regression);
  void SetUseMixedEffects(bool use_mixed_effects);

  void SetNormalAngle(double normal_angle);
  void SetPerformGoodBad(bool perform_good_bad);
  void SetLogEnergy(bool log_energy);

  void SetImages(const std::vector<ImageType::Pointer> &images);
  std::vector<ImageType::Pointer> GetImages();

  void SetFilenames(const std::vector<std::string> &filenames);
  void SetPointFiles(const std::vector <std::string> &point_files);
  int GetNumShapes();
  void SetMeshFiles(const std::vector<std::string> &mesh_files);
  void SetAttributeScales(const std::vector<double> &scales);
  void SetFeaFiles(const std::vector<std::string> &files);
  void SetFeaGradFiles(const std::vector<std::string> &files);
  void SetFidsFiles(const std::vector<std::string> &files);

  void SetParticleFlags(std::vector<int> flags);
  void SetDomainFlags(std::vector<int> flags);

  void SetFileOutputEnabled(bool enabled);

  std::vector<bool> GetUseXYZ();

  std::vector<bool> GetUseNormals();

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
  void abort_optimization();

  virtual void IterateCallback(itk::Object*, const itk::EventObject &);
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

  virtual std::vector<std::vector<itk::Point<double>>> GetLocalPoints();
  virtual std::vector<std::vector<itk::Point<double>>> GetGlobalPoints();
  void SetCutPlanes(std::vector<std::array<itk::Point<double>, 3 >> cut_planes);
  bool GetAborted();

protected:

  void PrintStartMessage(std::string str, unsigned int vlevel = 0) const;

  void PrintDoneMessage(unsigned int vlevel = 0) const;

  void UpdateExportablePoints();

  typename itk::MaximumEntropyCorrespondenceSampler<ImageType>::Pointer m_sampler;
  typename itk::ParticleProcrustesRegistration<3>::Pointer m_procrustes;
  typename itk::ParticleGoodBadAssessment<float, 3>::Pointer m_good_bad;

  static itk::ITK_THREAD_RETURN_TYPE optimize_callback(void* arg);

  unsigned int m_verbosity_level = 0;

  std::vector<std::vector<itk::Point<double>>>  m_local_points, m_global_points;

  int m_checkpoint_counter = 0;
  int m_procrustes_counter = 0;
  int m_saturation_counter = 0;
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
  std::vector <int> m_particle_flags;
  std::vector <int> m_domain_flags;

  // Keeps track of which state the optimization is in.
  unsigned int m_mode;
  double m_spacing;

  std::vector < std::string > m_filenames;
  int m_num_shapes = 0;

  std::vector<double> m_energy_a;
  std::vector<double> m_energy_b;
  std::vector<double> m_total_energy;
  bool m_log_energy = false;
  std::string m_str_energy;

  //GoodBadAssessment
  std::vector<std::vector<int>> m_bad_ids;
  double m_normal_angle = itk::Math::pi / 2.0;
  bool m_perform_good_bad = false;

  std::vector<int> m_cutting_planes_per_input;
  std::vector<int> m_spheres_per_input;

  bool m_file_output_enabled = true;
  bool m_aborted = false;
  std::vector<ImageType::Pointer> m_images;
  std::vector<std::array<itk::Point<double>, 3 >> m_cut_planes;

  itk::MemberCommand<Optimize>::Pointer m_iterate_command;
  int m_total_iterations = 0;
  size_t m_iteration_count = 0;

};
