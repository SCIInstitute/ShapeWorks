---
title: Libs/Optimize/Optimize.h

---

# Libs/Optimize/Optimize.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MatrixContainer](../Classes/classshapeworks_1_1MatrixContainer.md)**  |
| class | **[shapeworks::Optimize](../Classes/classshapeworks_1_1Optimize.md)**  |




## Source code

```cpp
#pragma once

#ifdef _WIN32
#pragma warning( disable: 4996 )
#endif

// std
#include <vector>
#include <string>
#include <random>

// itk
#include <itkImage.h>
#include <itkCommand.h>

#include <Eigen/Eigen>

// shapeworks particle system
#include "ParticleSystem/itkParticleSystem.h"
#include "ParticleSystem/Sampler.h"
#include "ParticleSystem/itkParticleProcrustesRegistration.h"
#include "ParticleSystem/itkParticleGoodBadAssessment.h"
#include "ParticleSystem/itkParticleVectorFunction.h"
#include "ParticleSystem/DomainType.h"
#include "ParticleSystem/MeshWrapper.h"
#include "ParticleSystem/OptimizationVisualizer.h"



namespace shapeworks {

class MatrixContainer {
  public:
  Eigen::MatrixXd matrix_;
};

class Optimize {
public:
  using ImageType = itk::Image<float, 3>;
  using VectorType = itk::ParticleVectorFunction<3>::VectorType;
  using MatrixType = Eigen::MatrixXd;

  Optimize();

  virtual ~Optimize();

  bool Run();

  bool LoadParameterFile(std::string filename);

  void SetIterationCallbackFunction(const std::function<void(void)> &f)
  { this->m_iter_callback = f; }

  void AbortOptimization();

  bool GetAborted();

  virtual std::vector<std::vector<itk::Point<double>>> GetLocalPoints();

  virtual std::vector<std::vector<itk::Point<double>>> GetGlobalPoints();

  void SetCutPlanes(std::vector<std::array<itk::Point<double>, 3 >> cut_planes);

  void SetVerbosity(int verbosity_level);

  void SetDomainsPerShape(int domains_per_shape);
  int GetDomainsPerShape();

  void SetDomainType(shapeworks::DomainType type);
  shapeworks::DomainType GetDomainType();

  void SetNumberOfParticles(std::vector<int> number_of_particles);
  std::vector<int> GetNumberOfParticles();

  void SetTransformFile(std::string filename);
  std::string GetTransformFile();

  void SetPrefixTransformFile(std::string prefix_transform_file);
  std::string GetPrefixTransformFile();

  void SetOutputDir(std::string output_dir);

  void SetOutputTransformFile(std::string output_transform_file);

  void SetOutputIndividualTransformFiles(bool value);

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
                       const vnl_vector_fixed<double, 3>& va,
                       const vnl_vector_fixed<double, 3>& vb,
                       const vnl_vector_fixed<double, 3>& vc);

  void SetProcessingMode(int mode);
  void SetAdaptivityMode(int adaptivity_mode);
  void SetAdaptivityStrength(double adaptivity_strength);
  void SetPairwisePotentialType(int pairwise_potential_type);
  void SetTimePtsPerSubject(int time_pts_per_subject);
  int GetTimePtsPerSubject();
  void SetOptimizationIterations(int optimization_iterations);
  void SetOptimizationIterationsCompleted(int optimization_iterations_completed);
  void SetIterationsPerSplit(int iterations_per_split);
  void SetInitializationCriterion(double init_criterion);
  void SetOptimizationCriterion(double opt_criterion);
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

  void AddImage(ImageType::Pointer image, std::string name = "");
  void AddMesh(vtkSmartPointer<vtkPolyData> poly_data);
  void AddContour(vtkSmartPointer<vtkPolyData> poly_data);

  void SetFilenames(const std::vector<std::string>& filenames);
  void SetPointFiles(const std::vector<std::string>& point_files);

  int GetNumShapes();
  void SetMeshFiles(const std::vector<std::string>& mesh_files);
  void SetAttributeScales(const std::vector<double>& scales);
  void SetFeaFiles(const std::vector<std::string>& files);
  void SetFeaGradFiles(const std::vector<std::string>& files);
  void SetFidsFiles(const std::vector<std::string>& files);

  void SetParticleFlags(std::vector<int> flags);
  void SetDomainFlags(std::vector<int> flags);

  const std::vector<int>& GetDomainFlags();

  void SetFileOutputEnabled(bool enabled);

  std::vector<bool> GetUseXYZ();

  std::vector<bool> GetUseNormals();

  void SetNarrowBand(double v);

  double GetNarrowBand();

  void SetUseShapeStatisticsAfter(int num_particles);

  int GetUseShapeStatisticsAfter();

  void PrintParamInfo();

  std::shared_ptr<Sampler> GetSampler()
  { return m_sampler; }

  MatrixContainer GetParticleSystem();

  void SetPythonFile(std::string filename);

  void SetGeodesicsEnabled(bool is_enabled);

  void SetGeodesicsCacheSizeMultiplier(size_t n);

  shapeworks::OptimizationVisualizer &GetVisualizer();
  void SetShowVisualizer(bool show);
  bool GetShowVisualizer();

protected:

  virtual void SetIterationCallback();

  void RunProcrustes();

  void OptimizeStart();
  void OptimizerStop();

  void ReadTransformFile();
  void ReadPrefixTransformFile(const std::string& s);

  void InitializeSampler();
  double GetMinNeighborhoodRadius();
  void AddSinglePoint();
  void Initialize();
  void AddAdaptivity();
  void RunOptimize();

  void SetInitialCorrespondenceMode();

  virtual void IterateCallback(itk::Object*, const itk::EventObject&);

  void ComputeEnergyAfterIteration();

  void SetCotanSigma();

  void WriteTransformFile(int iter = -1) const;
  void WriteTransformFile(std::string iter_prefix) const;
  void WriteTransformFiles(int iter = -1) const;
  void WriteTransformFiles(std::string iter_prefix) const;
  void WritePointFiles(int iter = -1);
  void WritePointFiles(std::string iter_prefix);
  void WritePointFilesWithFeatures(int iter = -1);
  void WritePointFilesWithFeatures(std::string iter_prefix);
  void WriteEnergyFiles();
  void WriteCuttingPlanePoints(int iter = -1);
  void WriteParameters(std::string output_dir = "");
  void ReportBadParticles();

  void SetParameters();
  void WriteModes();

  void PrintStartMessage(std::string str, unsigned int vlevel = 0) const;

  void PrintDoneMessage(unsigned int vlevel = 0) const;

  virtual void UpdateExportablePoints();

  // return a checkpoint dir for the current iteration
  std::string GetCheckpointDir();

  std::shared_ptr<Sampler> m_sampler;
  itk::ParticleProcrustesRegistration<3>::Pointer m_procrustes;
  itk::ParticleGoodBadAssessment<float, 3>::Pointer m_good_bad;

  unsigned int m_verbosity_level = 0;

  std::vector<std::vector<itk::Point<double>>> m_local_points, m_global_points;

  int m_checkpoint_counter = 0;
  int m_procrustes_counter = 0;
  int m_saturation_counter = 0;
  bool m_disable_procrustes = true;
  bool m_use_cutting_planes = false;
  bool m_optimizing = false;
  bool m_use_regression = false;
  bool m_use_mixed_effects = false;

  // IO Parameters
  unsigned int m_domains_per_shape = 1;
  shapeworks::DomainType m_domain_type = shapeworks::DomainType::Image;
  std::vector<int> m_number_of_particles;
  std::string m_transform_file;
  std::string m_prefix_transform_file;
  std::string m_output_dir;
  std::string m_output_transform_file;
  bool m_output_transform_files = false;
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

  unsigned int m_timepts_per_subject = 1;
  int m_optimization_iterations = 2000;
  int m_optimization_iterations_completed = 0;
  int m_iterations_per_split = 1000;
  double m_initialization_criterion = 1e-6;
  double m_optimization_criterion = 1e-6;
  bool m_use_shape_statistics_in_init = false;
  unsigned int m_procrustes_interval = 3;
  int m_procrustes_scaling = 1;
  double m_relative_weighting = 1.0;
  double m_initial_relative_weighting = 0.05;
  double m_starting_regularization = 1000;
  double m_ending_regularization = 1.0;
  int m_recompute_regularization_interval = 1;
  bool m_save_init_splits = false;
  unsigned int m_checkpointing_interval = 50;
  int m_keep_checkpoints = 0;
  double m_cotan_sigma_factor = 5.0;
  std::vector<int> m_particle_flags;
  std::vector<int> m_domain_flags;
  double m_narrow_band = 0.0;
  bool m_narrow_band_set = false;
  bool m_fixed_domains_present = false;
  int m_use_shape_statistics_after = -1;
  std::string m_python_filename;
  bool m_geodesics_enabled = false; // geodesics disabled by default
  size_t m_geodesic_cache_size_multiplier = 0; // 0 => VtkMeshWrapper will use a heuristic to determine cache size

  // Keeps track of which state the optimization is in.
  unsigned int m_mode = 0;
  /* m_spacing is used to scale the random update vector for particle splitting. */
  double m_spacing = 0;

  std::vector<std::string> m_filenames;
  int m_num_shapes = 0;

  std::vector<double> m_energy_a;
  std::vector<double> m_energy_b;
  std::vector<double> m_total_energy;
  bool m_log_energy = false;
  std::string m_str_energy;

  // GoodBadAssessment
  std::vector<std::vector<int>> m_bad_ids;
  double m_normal_angle = itk::Math::pi / 2.0;
  bool m_perform_good_bad = false;

  std::vector<int> m_cutting_planes_per_input;
  std::vector<int> m_spheres_per_input;

  bool m_file_output_enabled = true;
  bool m_aborted = false;
  std::vector<std::array<itk::Point<double>, 3 >> m_cut_planes;

  //itk::MemberCommand<Optimize>::Pointer m_iterate_command;
  int m_total_iterations = 0;
  int m_iteration_count = 0;

  int m_split_number{0};

  std::mt19937 m_rand{42};

  std::function<void(void)> m_iter_callback;
  bool show_visualizer = false;
  shapeworks::OptimizationVisualizer visualizer;
};

}
```


-------------------------------

Updated on 2021-11-18 at 01:30:46 +0000
