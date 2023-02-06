---
title: Libs/Optimize/Optimize.h

---

# Libs/Optimize/Optimize.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MatrixContainer](../Classes/classshapeworks_1_1MatrixContainer.md)**  |
| class | **[shapeworks::Optimize](../Classes/classshapeworks_1_1Optimize.md)**  |




## Source code

```cpp
#pragma once

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

// std
#include <random>
#include <string>
#include <vector>

// itk
#include <itkCommand.h>
#include <itkImage.h>

#include <Eigen/Eigen>

// shapeworks
#include <Project/Project.h>

#include "DomainType.h"
#include "MeshWrapper.h"
#include "OptimizationVisualizer.h"
#include "ParticleProcrustesRegistration.h"
#include "Sampler.h"
#include "itkParticleSystem.h"
#include "itkParticleVectorFunction.h"

namespace shapeworks {

class Project;
class ParticleGoodBadAssessment;

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

  bool SetUpOptimize(ProjectHandle projectFile);

  void SetProject(std::shared_ptr<Project> project);

  void SetIterationCallbackFunction(const std::function<void(void)>& f) { this->iteration_callback_ = f; }

  void AbortOptimization();

  bool GetAborted();

  virtual std::vector<std::vector<itk::Point<double>>> GetLocalPoints();

  virtual std::vector<std::vector<itk::Point<double>>> GetGlobalPoints();

  void SetCutPlanes(std::vector<std::array<itk::Point<double>, 3>> cut_planes);

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
  void SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, 3>& va, const vnl_vector_fixed<double, 3>& vb,
                       const vnl_vector_fixed<double, 3>& vc);

  void SetProcessingMode(int mode);
  void SetAdaptivityMode(int adaptivity_mode);
  void SetMeshFFCMode(int mesh_ffc_mode) {
    m_mesh_ffc_mode = mesh_ffc_mode;
    m_sampler->SetMeshFFCMode(mesh_ffc_mode);
  }
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
  void SetProcrustesScaling(bool procrustes_scaling);
  void SetProcrustesRotationTranslation(bool procrustes_rotation_translation);
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

  void SetSharedBoundaryEnabled(bool enabled);
  void SetSharedBoundaryWeight(double weight);

  const std::vector<int>& GetDomainFlags();

  void SetFileOutputEnabled(bool enabled);

  std::vector<bool> GetUseXYZ();

  std::vector<bool> GetUseNormals();

  void SetNarrowBand(double v);

  double GetNarrowBand();

  void SetUseShapeStatisticsAfter(int num_particles);

  int GetUseShapeStatisticsAfter();

  void PrintParamInfo();

  std::shared_ptr<Sampler> GetSampler() { return m_sampler; }

  MatrixContainer GetParticleSystem();

  void SetPythonFile(std::string filename);

  void SetGeodesicsEnabled(bool is_enabled);

  void SetGeodesicsCacheSizeMultiplier(size_t n);

  shapeworks::OptimizationVisualizer& GetVisualizer();
  void SetShowVisualizer(bool show);
  bool GetShowVisualizer();

  bool GetMeshFFCMode() { return m_mesh_ffc_mode; }

  vnl_vector_fixed<double, 3> TransformPoint(int domain, vnl_vector_fixed<double, 3> input);

  void UpdateProgress();

 protected:
  virtual void SetIterationCallback();

  void ComputeTotalIterations();

  void OptimizerStop();

  void ReadTransformFile();
  void ReadPrefixTransformFile(const std::string& s);

  void InitializeSampler();
  double GetMinNeighborhoodRadius();
  void AddSinglePoint();
  void Initialize();
  void AddAdaptivity();
  void RunOptimize();

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
  void WriteSplitFiles(std::string name);
  void WriteCuttingPlanePoints(int iter = -1);
  void WriteParameters(std::string output_dir = "");
  void ReportBadParticles();

  int SetParameters();
  void WriteModes();

  void PrintStartMessage(std::string str, unsigned int vlevel = 0) const;

  void PrintDoneMessage(unsigned int vlevel = 0) const;

  virtual void UpdateExportablePoints();

  virtual std::vector<std::vector<std::vector<double>>> GetProcrustesTransforms();

  void UpdateProject();

  // return a checkpoint dir for the current iteration
  std::string GetCheckpointDir();

  std::shared_ptr<Sampler> m_sampler;
  ParticleProcrustesRegistration::Pointer m_procrustes;
  std::shared_ptr<ParticleGoodBadAssessment> m_good_bad;

  unsigned int m_verbosity_level = 0;

  std::vector<std::vector<itk::Point<double>>> m_local_points, m_global_points;

  int m_checkpoint_counter = 0;
  int m_procrustes_counter = 0;
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
  int m_pairwise_potential_type = 0;  // 0 - gaussian (Cates work), 1 - modified cotangent (Meyer),

  bool m_mesh_ffc_mode = 0;

  unsigned int m_timepts_per_subject = 1;
  int m_optimization_iterations = 2000;
  int m_optimization_iterations_completed = 0;
  int m_iterations_per_split = 1000;
  double m_initialization_criterion = 1e-6;
  double m_optimization_criterion = 1e-6;
  bool m_use_shape_statistics_in_init = false;
  unsigned int m_procrustes_interval = 3;
  bool m_procrustes_scaling = true;
  bool m_procrustes_rotation_translation = true;
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
  bool m_geodesics_enabled = false;             // geodesics disabled by default
  size_t m_geodesic_cache_size_multiplier = 0;  // 0 => VtkMeshWrapper will use a heuristic to determine cache size

  // m_spacing is used to scale the random update vector for particle splitting.
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
  std::vector<std::array<itk::Point<double>, 3>> m_cut_planes;

  int m_total_iterations = 0;
  int m_iteration_count = 0;
  int m_split_number = 0;

  int current_particle_iterations_ = 0;
  int total_particle_iterations_ = 0;

  std::function<void(void)> iteration_callback_;
  bool show_visualizer_ = false;
  shapeworks::OptimizationVisualizer visualizer_;

  std::shared_ptr<Project> project_;

  std::chrono::system_clock::time_point m_start_time;
  std::chrono::system_clock::time_point m_last_update_time;
  std::chrono::system_clock::time_point m_last_remaining_update_time;
  std::string m_remaining_time_message;

};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-02-06 at 20:25:59 +0000
