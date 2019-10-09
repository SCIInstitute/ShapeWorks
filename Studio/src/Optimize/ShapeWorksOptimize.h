#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <array>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkCommand.h>

#include <Groom/ShapeWorksGroom.h>

/*
 #include <ParticleShapeworks/include/itkPSMProcrustesRegistration.h>
 #include <ParticleShapeworks/include/itkPSMEntropyModelFilter.h>
 #include <ParticleShapeworks/include/itkPSMProject.h>
 #include <ParticleShapeworks/include/itkPSMProjectReader.h>
 #include <ParticleShapeworks/include/itkPSMParticleSystem.h>
 */

//#include <itkParticleSystem.h>
//#include <itkMaximumEntropyCorrespondenceSampler.h>

#include <itkParticleSystem.h>
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

class ShapeWorksOptimize {
public:
  ShapeWorksOptimize();

  void set_inputs(std::vector<ImageType::Pointer> inputs);
  void set_cut_planes(std::vector<std::array<itk::Point<double>, 3 >> cutPlanes);
  void set_start_reg(double start_reg);
  void set_end_reg(double end_reg);
  void set_iterations_per_split(unsigned int iterations_per_split);
  void set_number_of_particles(unsigned int number_of_particles);
  void set_decay_span(double decay_span);
  void set_procrustes_interval(unsigned procrustes_interval);
  void set_weighting(double weighting);
  void set_verbose(bool verbose);

  void run();
  std::vector<ImageType::Pointer> getImages();
  std::vector<std::vector<itk::Point<double>>> localPoints();
  std::vector<std::vector<itk::Point<double>>> globalPoints();

protected:

  void Initialize();
  void AddAdaptivity();
  void Optimize();

  void AddSinglePoint();
  void SetCotanSigma();
  double GetMinNeighborhoodRadius();
  void ComputeEnergyAfterIteration();

  virtual void iterateCallback(itk::Object* caller, const itk::EventObject &);

protected:
  std::vector<ImageType::Pointer> images_;
  bool verbose_;
  double weighting_;
  double decaySpan_;
  std::vector<std::vector<itk::Point<double>>>  localPoints_, globalPoints_;
  itk::MemberCommand<ShapeWorksOptimize>::Pointer iterateCmd_;
  size_t reportInterval_, procrustesCounter_, totalIters_, iterCount_;
  unsigned int procrustesInterval_;
  std::vector<std::array<itk::Point<double>, 3 >> cutPlanes_;

  // PSM
  itk::MaximumEntropyCorrespondenceSampler<itk::Image<float, 3>>::Pointer m_Sampler;
  itk::ParticleProcrustesRegistration<3>::Pointer m_Procrustes;
  itk::ParticleGoodBadAssessment<float, 3>::Pointer m_GoodBad;

  unsigned int m_verbosity_level;


  unsigned int iterations_per_split_;
  unsigned int number_of_particles_;


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
  std::vector < std::string > pointFiles;

  std::vector<double> m_EnergyA;
  std::vector<double> m_EnergyB;
  std::vector<double> m_TotalEnergy;
  bool m_logEnergy;
  std::string m_strEnergy;

  std::vector<std::vector<int>> m_badIds;  //GoodBadAssessment
  double m_normalAngle; //GoodBadAssessment
  bool m_performGoodBad;

  std::vector <int> m_cutting_planes_per_input;
  std::vector <int> m_spheres_per_input;
  std::vector<double> cpVals;
  std::vector<double> spVals;
  std::vector<double> radList;
};
