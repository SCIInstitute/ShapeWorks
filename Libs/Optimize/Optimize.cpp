// std
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif // ifdef _WIN32

// itk
#include <itkMultiThreaderBase.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageToVTKImageFilter.h>

// shapeworks
#include "TriMesh.h"
#include "ParticleSystem/itkParticleImageDomain.h"
#include "ParticleSystem/itkParticleImplicitSurfaceDomain.h"
#include "ParticleSystem/object_reader.h"
#include "ParticleSystem/object_writer.h"

#include "Optimize.h"

namespace shapeworks {

//---------------------------------------------------------------------------
Optimize::Optimize()
{
  this->m_sampler = std::make_shared<Sampler>();
}

//---------------------------------------------------------------------------
bool Optimize::Run()
{
  // sanity check
  if (this->m_domains_per_shape != this->m_number_of_particles.size()) {
    std::cerr <<
              "Inconsistency in parameters... m_domains_per_shape != m_number_of_particles.size()\n";
    return false;
  }

  this->SetParameters();

  int number_of_splits = static_cast<int>(
    std::log2(static_cast<double>(this->m_number_of_particles[0])));
  this->m_iteration_count = 0;

  this->m_total_iterations = (number_of_splits * this->m_iterations_per_split) +
                             this->m_optimization_iterations;

  if (this->m_verbosity_level > 0) {
    std::cout << "Total number of iterations = " << this->m_total_iterations << "\n";
  }

  m_disable_procrustes = true;
  m_disable_checkpointing = true;

  std::vector<unsigned int> final_number_of_particles = this->m_number_of_particles;
  int scale = 1;
  if (this->m_use_shape_statistics_after > 0) {
    this->m_use_shape_statistics_in_init = false;
    for (int i = 0; i < this->m_number_of_particles.size(); i++) {
      // run up to only the specified starting point for multiscale
      this->m_number_of_particles[i] = this->m_use_shape_statistics_after;
    }
  }

  // Initialize
  if (m_processing_mode >= 0) { this->Initialize(); }
  // Introduce adaptivity
  if (m_processing_mode >= 1 || m_processing_mode == -1) { this->AddAdaptivity(); }
  // Optimize
  if (m_processing_mode >= 2 || m_processing_mode == -2) { this->RunOptimize(); }

  if (this->m_use_shape_statistics_after > 0) {
    // First phase is done now run iteratively until we reach the final particle counts

    // save the particles for this split if requested
    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << this->m_split_number;
      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_w_opt";
      std::string out_path = m_output_dir;
      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(this->m_split_number);
    }

    // set to use shape statistics now for the Initialize mode
    this->m_use_shape_statistics_in_init = true;

    // reset the number of iterations completed
    this->m_optimization_iterations_completed = 0;

    bool finished = false;

    while (!finished) {
      this->m_sampler->ReInitialize();

      // determine if we have reached the final particle counts
      finished = true;
      for (int i = 0; i < this->m_number_of_particles.size(); i++) {
        if (this->m_number_of_particles[i] < final_number_of_particles[i]) {
          this->m_number_of_particles[i] *= 2;
          finished = false;
        }
      }

      if (!finished) {
        if (m_processing_mode >= 0) { this->Initialize(); }
        if (m_processing_mode >= 1 || m_processing_mode == -1) { this->AddAdaptivity(); }
        if (m_processing_mode >= 2 || m_processing_mode == -2) { this->RunOptimize(); }
      }
    }
  }

  this->UpdateExportablePoints();
  return true;
}

//---------------------------------------------------------------------------
void Optimize::RunProcrustes()
{
  this->OptimizerStop();
  m_procrustes->RunRegistration();
}

//---------------------------------------------------------------------------
void Optimize::SetParameters()
{
  if (this->m_verbosity_level == 0) {
    std::cout <<
              "Verbosity 0: This will be the only output on your screen, unless there are any errors. Increase the verbosity if needed."
              << std::endl;
  }

  // Set up the optimization process
  this->m_sampler->SetDomainsPerShape(this->m_domains_per_shape);   // must be done first!
  this->m_sampler->SetTimeptsPerIndividual(this->m_timepts_per_subject);
  this->m_sampler->GetParticleSystem()->SetDomainsPerShape(this->m_domains_per_shape);
  this->m_sampler->SetVerbosity(this->m_verbosity_level);

  if (this->m_use_xyz.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_sampler->SetXYZ(i, this->m_use_xyz[i]);
    }
  }
  else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_sampler->SetXYZ(i, false);
    }
  }

  if (this->m_use_normals.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_sampler->SetNormals(i, this->m_use_normals[i]);
    }
  }
  else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_sampler->SetNormals(i, false);
    }
  }

  // Set up the procrustes registration object.
  this->m_procrustes = itk::ParticleProcrustesRegistration<3>::New();
  this->m_procrustes->SetParticleSystem(this->m_sampler->GetParticleSystem());
  this->m_procrustes->SetDomainsPerShape(this->m_domains_per_shape);

  if (this->m_procrustes_scaling == 0) {
    this->m_procrustes->ScalingOff();
  }
  else {
    this->m_procrustes->ScalingOn();
  }

  this->SetIterationCallback();
  this->PrintStartMessage("Initializing variables...");
  this->InitializeSampler();
  this->PrintDoneMessage();

  if (m_use_normals.size() > 0) {
    for (int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
      if (m_use_normals[i % m_domains_per_shape]) {
        continue;
      }
      m_sampler->GetParticleSystem()->GetDomain(i)->DeletePartialDerivativeImages();
    }
  }
  else {
    int numShapes = m_sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      m_sampler->GetParticleSystem()->GetDomain(i)->DeletePartialDerivativeImages();
    }
  }

  if (m_domain_flags.size() > 0) {
    for (int i = 0; i < m_domain_flags.size(); i++) {
      if (m_use_normals.size() > 0) {
        if (m_use_normals[i % m_domains_per_shape]) {
          m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->
            DeletePartialDerivativeImages();
        }
        else {
          m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->DeleteImages();
        }
      }
      else {
        m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->DeleteImages();
      }
    }
  }

  if (m_verbosity_level > 1) {
    this->PrintParamInfo();
  }

  m_good_bad = itk::ParticleGoodBadAssessment<float, 3>::New();
  m_good_bad->SetDomainsPerShape(m_domains_per_shape);
  m_good_bad->SetCriterionAngle(m_normal_angle);
  m_good_bad->SetPerformAssessment(m_perform_good_bad);

  m_energy_a.clear();
  m_energy_b.clear();
  m_total_energy.clear();

  // Now read the transform file if present.
  if (m_transform_file != "") { this->ReadTransformFile(); }
  if (m_prefix_transform_file != "") { this->ReadPrefixTransformFile(m_prefix_transform_file); }
}

//---------------------------------------------------------------------------
Optimize::~Optimize()
{}

//---------------------------------------------------------------------------
void Optimize::SetVerbosity(int verbosity_level)
{
  this->m_verbosity_level = verbosity_level;
}

//---------------------------------------------------------------------------
void Optimize::SetDomainsPerShape(int domains_per_shape)
{
  this->m_domains_per_shape = domains_per_shape;
  this->m_sampler->SetDomainsPerShape(this->m_domains_per_shape);
}

//---------------------------------------------------------------------------
int Optimize::GetDomainsPerShape()
{
  return this->m_domains_per_shape;
}

//---------------------------------------------------------------------------
void Optimize::SetDomainType(shapeworks::DomainType type)
{
  this->m_domain_type = type;
}

//---------------------------------------------------------------------------
shapeworks::DomainType Optimize::GetDomainType()
{
  return this->m_domain_type;
}

//---------------------------------------------------------------------------
void Optimize::SetNumberOfParticles(std::vector<unsigned int> number_of_particles)
{
  this->m_number_of_particles = number_of_particles;
}

//---------------------------------------------------------------------------
std::vector<unsigned int> Optimize::GetNumberOfParticles()
{
  return this->m_number_of_particles;
}

//---------------------------------------------------------------------------
void Optimize::SetTransformFile(std::string filename)
{
  this->m_transform_file = filename;
}

//---------------------------------------------------------------------------
std::string Optimize::GetTransformFile()
{
  return this->m_transform_file;
}

//---------------------------------------------------------------------------
void Optimize::SetPrefixTransformFile(std::string prefix_transform_file)
{
  this->m_prefix_transform_file = prefix_transform_file;
}

//---------------------------------------------------------------------------
std::string Optimize::GetPrefixTransformFile()
{
  return this->m_prefix_transform_file;
}

//---------------------------------------------------------------------------
void Optimize::SetOutputDir(std::string output_dir)
{
  this->m_output_dir = output_dir;
}

//---------------------------------------------------------------------------
void Optimize::SetOutputTransformFile(std::string output_transform_file)
{
  this->m_output_transform_file = output_transform_file;
}

//---------------------------------------------------------------------------
void Optimize::SetUseMeshBasedAttributes(bool use_mesh_based_attributes)
{
  this->m_mesh_based_attributes = use_mesh_based_attributes;

  if (this->m_mesh_based_attributes) {
    this->m_sampler->RegisterGeneralShapeMatrices();
  }
}

//---------------------------------------------------------------------------
bool Optimize::GetUseMeshBasedAttributes()
{
  return this->m_mesh_based_attributes;
}

//---------------------------------------------------------------------------
void Optimize::SetUseXYZ(std::vector<bool> use_xyz)
{
  this->m_use_xyz = use_xyz;
}

//---------------------------------------------------------------------------
void Optimize::SetUseNormals(std::vector<bool> use_normals)
{
  this->m_use_normals = use_normals;
}

//---------------------------------------------------------------------------
void Optimize::SetAttributesPerDomain(std::vector<int> attributes_per_domain)
{
  this->m_attributes_per_domain = attributes_per_domain;
  this->m_sampler->SetAttributesPerDomain(attributes_per_domain);
}

//---------------------------------------------------------------------------
std::vector<int> Optimize::GetAttributesPerDomain()
{
  return this->m_attributes_per_domain;
}

//---------------------------------------------------------------------------
void Optimize::SetDistributionDomainID(int distribution_domain_id)
{
  this->m_distribution_domain_id = distribution_domain_id;
}

//---------------------------------------------------------------------------
int Optimize::GetDistributionDomainID()
{
  return this->m_distribution_domain_id;
}

//---------------------------------------------------------------------------
void Optimize::SetOutputCuttingPlaneFile(std::string output_cutting_plane_file)
{
  this->m_output_cutting_plane_file = output_cutting_plane_file;
}

//---------------------------------------------------------------------------
void Optimize::SetUseCuttingPlanes(bool use_cutting_planes)
{
  this->m_use_cutting_planes = use_cutting_planes;
}

//---------------------------------------------------------------------------
void Optimize::SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, 3>& va,
                               const vnl_vector_fixed<double, 3>& vb,
                               const vnl_vector_fixed<double, 3>& vc)
{
  this->m_sampler->SetCuttingPlane(i, va, vb, vc);
}

//---------------------------------------------------------------------------
void Optimize::SetProcessingMode(int mode)
{
  this->m_processing_mode = mode;
}

//---------------------------------------------------------------------------
void Optimize::SetAdaptivityMode(int adaptivity_mode)
{
  this->m_adaptivity_mode = adaptivity_mode;
}

//---------------------------------------------------------------------------
void Optimize::SetAdaptivityStrength(double adaptivity_strength)
{
  this->m_adaptivity_strength = adaptivity_strength;
}

//---------------------------------------------------------------------------
void Optimize::ReadTransformFile()
{
  object_reader<itk::ParticleSystem<3>::TransformType> reader;
  reader.SetFileName(m_transform_file);
  reader.Update();
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
void Optimize::ReadPrefixTransformFile(const std::string& fn)
{
  object_reader<itk::ParticleSystem<3>::TransformType> reader;
  reader.SetFileName(fn.c_str());
  reader.Update();
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
// Initialization and Optimization
void Optimize::InitializeSampler()
{
  float nbhd_to_sigma = 3.0;   // 3.0 -> 1.0
  float flat_cutoff = 0.3;   // 0.3 -> 0.85


  m_sampler->SetPairwisePotentialType(m_pairwise_potential_type);

  m_sampler->GetGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_sampler->GetCurvatureGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_sampler->GetGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);
  m_sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_sampler->GetModifiedCotangentGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_sampler->GetModifiedCotangentGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_sampler->GetOmegaGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_sampler->GetOmegaGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVariance(
    m_starting_regularization);
  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_sampler->GetEnsembleRegressionEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetOptimizer()->SetTimeStep(1.0);

  m_sampler->SetSamplingOn();

  m_sampler->SetCorrespondenceOn();

  m_sampler->SetAdaptivityMode(m_adaptivity_mode);
  m_sampler->GetEnsembleEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_sampler->GetEnsembleRegressionEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_sampler->GetEnsembleMixedEffectsEntropyFunction()
    ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);


  // These flags must be set before Initialize, since Initialize need to know which domains are fixed ahead of time
  for (unsigned int i = 0; i < this->m_domain_flags.size(); i++) {
    this->GetSampler()->GetParticleSystem()->FlagDomain(this->m_domain_flags[i]);
  }

  m_sampler->Initialize();

  m_sampler->GetOptimizer()->SetTolerance(0.0);

  // These flags have to be set after Initialize, since Initialize will set them all to zero
  for (unsigned int i = 0; i < this->m_particle_flags.size() / 2; i++) {
    this->GetSampler()->GetParticleSystem()
      ->SetFixedParticleFlag(this->m_particle_flags[2 * i], this->m_particle_flags[2 * i + 1]);
  }
}

//---------------------------------------------------------------------------
double Optimize::GetMinNeighborhoodRadius()
{
  double rad = 0.0;
  typename itk::ImageToVTKImageFilter<ImageType>::Pointer itk2vtkConnector;
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    double area = m_sampler->GetParticleSystem()->GetDomain(i)->GetSurfaceArea();
    double sigma =
      std::sqrt(area / (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    if (rad < sigma) {
      rad = sigma;
    }
  }
  return rad;
}

//---------------------------------------------------------------------------
void Optimize::AddSinglePoint()
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;

  PointType firstPointPosition;
  firstPointPosition = m_sampler->GetParticleSystem()->GetDomain(0)->GetValidLocationNear(firstPointPosition);

  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) > 0) {
      continue;
    }
    PointType pos = m_sampler->GetParticleSystem()->GetDomain(0)->GetValidLocationNear(firstPointPosition);
    m_sampler->GetParticleSystem()->AddPosition(pos, i);
  }
}

//---------------------------------------------------------------------------
void Optimize::Initialize()
{
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Initialize Step\n";
    std::cout << "------------------------------\n";
  }

  m_disable_checkpointing = true;
  m_disable_procrustes = false;

  if (m_procrustes_interval != 0) { // Initial registration
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) > 10) {
        m_procrustes->RunRegistration(i);
      }
    }
    this->WritePointFiles();
    this->WriteTransformFile();
  }
  m_disable_procrustes = true;

  m_sampler->GetParticleSystem()->SynchronizePositions();

  m_sampler->GetCurvatureGradientFunction()->SetRho(0.0);
  m_sampler->GetOmegaGradientFunction()->SetRho(0.0);

  m_sampler->SetCorrespondenceOn();

  if (m_use_shape_statistics_in_init) {
    if (m_mesh_based_attributes) {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy);
    }
    else {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleEntropy);
    }

    m_sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                     m_ending_regularization,
                                                                     m_iterations_per_split);

    m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
      m_starting_regularization,
      m_ending_regularization,
      m_iterations_per_split);
  }
  else {
    // force to mean
    if ((m_attributes_per_domain.size() > 0 &&
         *std::max_element(m_attributes_per_domain.begin(),
                           m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralMeanEnergy);
    }
    else {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeanEnergy);
    }
  }

  m_sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  this->AddSinglePoint();

  m_sampler->GetParticleSystem()->SynchronizePositions();

  this->m_split_number = 0;

  int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();
  vnl_vector_fixed<double, 3> random;

  for (int i = 0; i < 3; i++) {
    random[i] = static_cast < double > (this->m_rand());
  }
  random = random.normalize() * this->m_spacing;

  bool flag_split = false;

  for (int i = 0; i < n; i++) {
    int d = i % m_domains_per_shape;
    if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
      flag_split = true;
      break;
    }
  }

  while (flag_split) {
    for (int i = 0; i < 3; i++) {
      random[i] = static_cast <double> (this->m_rand());
    }
    random = random.normalize() * this->m_spacing;
    //        m_Sampler->GetEnsembleEntropyFunction()->PrintShapeMatrix();
    this->OptimizerStop();
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        m_sampler->GetParticleSystem()->SplitAllParticlesInDomain(random, i, 0);
      }
    }

    m_sampler->GetParticleSystem()->SynchronizePositions();

    this->m_split_number++;

    if (m_verbosity_level > 0) {
      std::cout << "split number = " << this->m_split_number << std::endl;

      std::cout << std::endl << "Particle count: ";
      for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
        std::cout << m_sampler->GetParticleSystem()->GetNumberOfParticles(i) << "  ";
      }
      std::cout << std::endl;
    }

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << this->m_split_number;

      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_wo_init";
      std::string out_path = m_output_dir;

      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name);
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(this->m_split_number);
    }

    m_energy_a.clear();
    m_energy_b.clear();
    m_total_energy.clear();
    std::stringstream ss;
    ss << this->m_split_number;
    std::stringstream ssp;

    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    m_str_energy = "split" + ss.str();

    for (int i = 0; i < m_domains_per_shape; i++) {
      ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
      m_str_energy += "_" + ssp.str();
      ssp.str("");
    }
    m_str_energy += "pts_init";

    if (this->m_pairwise_potential_type == 1) {
      this->SetCotanSigma();

      double minRad = 3.0 * this->GetMinNeighborhoodRadius();

      m_sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
      m_sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
        minRad);
    }

    m_saturation_counter = 0;
    m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_sampler->GetOptimizer()->SetNumberOfIterations(0);
    m_sampler->Execute();

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << this->m_split_number;
      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_w_init";
      std::string out_path = m_output_dir;
      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(this->m_split_number);
    }
    this->WritePointFiles();
    this->WritePointFilesWithFeatures();
    this->WriteEnergyFiles();
    this->WriteTransformFile();

    flag_split = false;
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        flag_split = true;
        break;
      }
    }
  }
  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteTransformFile();
  this->WriteCuttingPlanePoints();
  if (m_verbosity_level > 0) {
    std::cout << "Finished initialization!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::AddAdaptivity()
{
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** AddAdaptivity Step\n";
    std::cout << "------------------------------\n";
  }

  if (m_adaptivity_strength == 0.0) { return; }
  m_disable_checkpointing = true;
  m_disable_procrustes = true;

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();
  }

  double minRad = 3.0 * this->GetMinNeighborhoodRadius();

  m_sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
  m_sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);

  m_sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  m_saturation_counter = 0;
  m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
  m_sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_sampler->Execute();

  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteTransformFile();
  this->WriteCuttingPlanePoints();

  if (m_verbosity_level > 0) {
    std::cout << "Finished adaptivity!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::RunOptimize()
{
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Optimize Step\n";
    std::cout << "------------------------------\n";
  }

  m_optimizing = true;
  m_sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_relative_weighting);
  m_sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_relative_weighting);

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();

    double minRad = 3.0 * this->GetMinNeighborhoodRadius();

    m_sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
    m_sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
      minRad);
  }

  m_disable_checkpointing = false;
  m_disable_procrustes = false;

  if (m_procrustes_interval != 0) { // Initial registration
    m_procrustes->RunRegistration();
    this->WritePointFiles();
    this->WriteTransformFile();

    if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
      // transform cutting planes
      m_sampler->TransformCuttingPlanes(m_distribution_domain_id);
    }
  }

  // Set up the minimum variance decay
  m_sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                   m_ending_regularization,
                                                                   m_optimization_iterations -
                                                                   m_optimization_iterations_completed);

  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);
  m_sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_sampler->SetCorrespondenceOn();

  if ((m_attributes_per_domain.size() > 0 &&
       *std::max_element(m_attributes_per_domain.begin(),
                         m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy);
  }
  else if (m_use_regression == true) {
    if (m_use_mixed_effects == true) {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleMixedEffectsEntropy);
    }
    else {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleRegressionEntropy);
    }
  }
  else if (m_starting_regularization == m_ending_regularization) {
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeanEnergy);     // mean force
  }
  else {
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleEntropy);
  }

  if (m_optimization_iterations - m_optimization_iterations_completed > 0) {
    m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(
      m_optimization_iterations - m_optimization_iterations_completed);
  }
  else { m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(0); }

  m_energy_a.clear();
  m_energy_b.clear();
  m_total_energy.clear();
  m_str_energy = "opt";

  m_saturation_counter = 0;
  m_sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_sampler->GetOptimizer()->SetTolerance(0.0);
  m_sampler->Execute();

  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteEnergyFiles();
  this->WriteTransformFile();
  this->WriteModes();
  this->WriteCuttingPlanePoints();
  this->WriteParameters();
  if (m_verbosity_level > 0) {
    std::cout << "Finished optimization!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::OptimizeStart()
{
  m_sampler->GetOptimizer()->StartOptimization();
}

//---------------------------------------------------------------------------
void Optimize::OptimizerStop()
{
  m_sampler->GetOptimizer()->StopOptimization();
}

//---------------------------------------------------------------------------
void Optimize::AbortOptimization()
{
  this->m_aborted = true;
  this->m_sampler->GetOptimizer()->AbortProcessing();
}


//---------------------------------------------------------------------------
void Optimize::IterateCallback(itk::Object*, const itk::EventObject&)
{
  if (this->GetShowVisualizer()) {
    this->GetVisualizer().IterationCallback(m_sampler->GetParticleSystem());
  }

  if (m_perform_good_bad == true) {
    std::vector<std::vector<int >> tmp;
    tmp = m_good_bad->RunAssessment(m_sampler->GetParticleSystem(),
                                    m_sampler->GetMeanCurvatureCache());

    if (!tmp.empty()) {
      if (this->m_bad_ids.empty()) {
        this->m_bad_ids.resize(m_domains_per_shape);
      }

      for (int i = 0; i < m_domains_per_shape; i++) {
        for (int j = 0; j < tmp[i].size(); j++) {
          if (m_bad_ids[i].empty()) {
            this->m_bad_ids[i].push_back(tmp[i][j]);
          }
          else {
            if (std::count(m_bad_ids[i].begin(), m_bad_ids[i].end(), tmp[i][j]) == 0) {
              this->m_bad_ids[i].push_back(tmp[i][j]);
            }
          }
        }
      }
    }
    ReportBadParticles();
  }

  this->ComputeEnergyAfterIteration();

  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {
    m_checkpoint_counter++;
    if (m_checkpoint_counter == (int) m_checkpointing_interval) {
      m_checkpoint_counter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();
    }
  }

  if (m_optimizing == false) { return; }

  if (m_procrustes_interval != 0 && m_disable_procrustes == false) {
    m_procrustes_counter++;

    if (m_procrustes_counter >= (int) m_procrustes_interval) {
      m_procrustes_counter = 0;
      m_procrustes->RunRegistration();

      if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
        // transform cutting planes
        m_sampler->TransformCuttingPlanes(m_distribution_domain_id);
      }
    }
  }

  static unsigned int iteration_no = 0;
  // Checkpointing after procrustes (override for optimizing step)
  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {

    m_checkpoint_counter++;

    if (m_checkpoint_counter == (int) m_checkpointing_interval) {
      iteration_no += m_checkpointing_interval;
      m_checkpoint_counter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();

      if (m_keep_checkpoints) {
        this->WritePointFiles(iteration_no);
        this->WritePointFilesWithFeatures(iteration_no);
        this->WriteTransformFile(iteration_no);
        this->WriteParameters(iteration_no);
      }
    }
  }
}

//---------------------------------------------------------------------------
void Optimize::ComputeEnergyAfterIteration()
{
  // The energy computed here is only used for writing to file
  if (!this->m_file_output_enabled) {
    return;
  }
  if (!this->m_log_energy) {
    return;
  }
  int numShapes = m_sampler->GetParticleSystem()->GetNumberOfDomains();
  double corrEnergy = 0.0;

  double sampEnergy = 0.0;
  for (int i = 0; i < numShapes; i++) {
    m_sampler->GetLinkingFunction()->SetDomainNumber(i);
    for (int j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      if (m_sampler->GetParticleSystem()->GetDomainFlag(i)) {
        sampEnergy += 0.0;
      }
      else {
        sampEnergy +=
          m_sampler->GetLinkingFunction()->EnergyA(j, i, m_sampler->GetParticleSystem());
      }
      if (m_sampler->GetCorrespondenceMode() == shapeworks::CorrespondenceMode::MeanEnergy) {
        corrEnergy +=
          m_sampler->GetLinkingFunction()->EnergyB(j, i, m_sampler->GetParticleSystem());
      }
    }
  }

  if (m_sampler->GetCorrespondenceMode() != shapeworks::CorrespondenceMode::MeanEnergy) {
    corrEnergy = m_sampler->GetLinkingFunction()->EnergyB(0, 0, m_sampler->GetParticleSystem());
  }

  double totalEnergy = sampEnergy + corrEnergy;
  m_energy_a.push_back(sampEnergy);
  m_energy_b.push_back(corrEnergy);
  m_total_energy.push_back(totalEnergy);
  if (m_verbosity_level > 2) {
    std::cout << "Energy: " << totalEnergy << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::SetCotanSigma()
{
  itk::ImageToVTKImageFilter<ImageType>::Pointer itk2vtkConnector;
  m_sampler->GetModifiedCotangentGradientFunction()->ClearGlobalSigma();
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    double area = m_sampler->GetParticleSystem()->GetDomain(i)->GetSurfaceArea();
    double sigma = m_cotan_sigma_factor *
                   std::sqrt(area /
                             (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    m_sampler->GetModifiedCotangentGradientFunction()->SetGlobalSigma(sigma);
  }
}

// File writers and info display functions

//---------------------------------------------------------------------------
void Optimize::PrintParamInfo()
{

  if (m_verbosity_level < 2) {
    return;
  }

#ifdef SW_USE_OPENMP
  std::cout << "OpenMP is enabled ... \n" << std::flush;
#else
  std::cout << "OpenMP is disabled ... \n" << std::flush;
#endif
  // Write out the parameters
  std::cout << "---------------------" << std::endl;
  std::cout << "   I/O parameters" << std::endl;
  std::cout << "---------------------" << std::endl << std::endl;

  std::cout << "Domains per shape = " << m_domains_per_shape << std::endl;
  std::cout << m_filenames.size() << " image files provided!!!" << std::endl;

  if (m_domain_flags.size() > 0) {
    std::cout << "Following " << m_domain_flags.size() << " domains have been declared fixed: " <<
              std::endl;

    for (int i = 0; i < m_domain_flags.size(); i++) {
      std::cout << m_domain_flags[i] << "\t" << m_filenames[m_domain_flags[i]] << std::endl;
    }

    std::cout << std::endl;
  }

  if (m_adaptivity_mode == 3) {
    std::cout << std::endl << std::endl << "*****Using constraints on shapes*****" << std::endl;
  }

  std::cout << "Target number of particles = ";
  if (m_domains_per_shape == 1) {
    std::cout << m_number_of_particles[0];
  }
  else {
    for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
      std::cout << "domain " << i << " : " << m_number_of_particles[i] << ", ";
    }
  }
  std::cout << std::endl;

  if (m_particle_flags.size() > 0) {
    std::cout << "Total " << m_particle_flags.size() / 2 <<
              " particles have been declared fixed." <<
              std::endl;
  }

  if (m_mesh_based_attributes) {
    std::cout << std::endl << std::endl << "*****Using attributes*****" << std::endl;

    std::cout << "Domain(s) using XYZ: ";
    for (int i = 0; i < m_domains_per_shape; i++) {
      if (m_use_xyz[i]) {
        std::cout << i << " ";
      }
    }
    std::cout << std::endl;

    std::cout << "Domain(s) using Normals: ";
    for (int i = 0; i < m_domains_per_shape; i++) {
      if (m_use_normals[i]) {
        std::cout << i << " ";
      }
    }
    std::cout << std::endl;

    if (this->m_attributes_per_domain.size() > 0) {
      std::cout << "Other attributes per domain:" << std::endl;
      for (int i = 0; i < m_domains_per_shape; i++) {
        std::cout << i << ":" << m_attributes_per_domain[i] << "  ";
      }
      std::cout << std::endl;
    }
  }

  if (m_transform_file.length() > 0) {
    std::cout << "m_transform_file = " << m_transform_file << std::endl;
  }
  if (m_prefix_transform_file.length() > 0) {
    std::cout << "m_prefix_transform_file = " << m_prefix_transform_file << std::endl;
  }

  std::cout << "Output path = " << m_output_dir << std::endl;
  std::cout << "Output transform filename = " << m_output_transform_file << std::endl;

  std::cout << std::endl;

  std::cout << "------------------------------" << std::endl;
  std::cout << "   Optimization parameters" << std::endl;
  std::cout << "------------------------------" << std::endl;

  std::cout << "Processing modes = ";
  if (m_processing_mode >= 0) {
    std::cout << "Initialization";
  }
  if ((m_processing_mode >= 1 || m_processing_mode == -1) && m_adaptivity_strength > 0.0) {
    std::cout << ", Adaptivity";
  }
  if (m_processing_mode >= 2 || m_processing_mode == -2) {
    std::cout << ", Optimization";
  }
  std::cout << std::endl;

  if (m_adaptivity_strength > 0.0) {
    std::cout << "adaptivity_strength = " << m_adaptivity_strength << std::endl;
  }

  std::cout << "pairwise_potential_type = ";
  if (m_pairwise_potential_type == 0) {
    std::cout << "gaussian" << std::endl;
  }
  else {
    std::cout << "cotan" << std::endl;
  }

  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_optimization_iterations_completed = " << m_optimization_iterations_completed <<
            std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_init_criterion = " << m_initialization_criterion << std::endl;
  std::cout << "m_opt_criterion = " << m_optimization_criterion << std::endl;
  std::cout << "m_use_shape_statistics_in_init = " << m_use_shape_statistics_in_init << std::endl;
  std::cout << "m_procrustes_interval = " << m_procrustes_interval << std::endl;
  std::cout << "m_procrustes_scaling = " << m_procrustes_scaling << std::endl;
  std::cout << "m_relative_weighting = " << m_relative_weighting << std::endl;
  std::cout << "m_initial_relative_weighting = " << m_initial_relative_weighting << std::endl;
  std::cout << "m_starting_regularization = " << m_starting_regularization << std::endl;
  std::cout << "m_ending_regularization = " << m_ending_regularization << std::endl;
  std::cout << "m_recompute_regularization_interval = " << m_recompute_regularization_interval <<
            std::endl;
  std::cout << "m_save_init_splits = " << m_save_init_splits << std::endl;
  std::cout << "m_checkpointing_interval = " << m_checkpointing_interval << std::endl;
  std::cout << "m_keep_checkpoints = " << m_keep_checkpoints << std::endl;

  std::cout << std::endl;

  if (m_perform_good_bad) {
    std::cout <<
              "Debug: Bad particles will be reported during optimization, expect significant delays!!! " <<
              std::endl;
  }

  if (m_log_energy) {
    std::cout << "Debug: Write energy logs, might increase runtime!!! " << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFile(int iter) const
{
  if (!this->m_file_output_enabled) {
    return;
  }

  std::string output_file = m_output_dir + "/" + m_output_transform_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    std::stringstream ssp;
    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    output_file = m_output_dir + "/iter" + ss.str() + "_p" + ssp.str() + "/" +
                  m_output_transform_file;
  }
  this->WriteTransformFile(output_file);
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFile(std::string iter_prefix) const
{
  if (!this->m_file_output_enabled) {
    return;
  }

  std::string output_file = iter_prefix;

  std::vector<itk::ParticleSystem<3>::TransformType> tlist;

  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains();
       i++) {
    tlist.push_back(m_sampler->GetParticleSystem()->GetTransform(i));
  }

  std::string str = "writing " + output_file + " ...";
  PrintStartMessage(str);
  object_writer<itk::ParticleSystem<3>::TransformType> writer;
  writer.SetFileName(output_file);
  writer.SetInput(tlist);
  writer.Update();
  PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WritePointFiles(int iter)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFiles(tmp_dir_name);
}

//---------------------------------------------------------------------------
void Optimize::WritePointFiles(std::string iter_prefix)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  this->PrintStartMessage("Writing point files...\n");
#ifdef _WIN32
  mkdir(iter_prefix.c_str());
#else
  mkdir(iter_prefix.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

  typedef Sampler::PointType PointType;
  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string local_file = iter_prefix + "/" + m_filenames[i] + "_local.particles";
    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_world.particles";

    std::ofstream out(local_file.c_str());
    std::ofstream outw(world_file.c_str());

    std::string str = "Writing " + world_file + " and " + local_file + " files...";
    this->PrintStartMessage(str, 1);
    if (!out) {
      std::cerr << "Error opening output file: " << local_file << std::endl;
      throw 1;
    }
    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    for (unsigned int j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_sampler->GetParticleSystem()->GetTransformedPosition(j, i);

      for (unsigned int k = 0; k < 3; k++) {
        out << pos[k] << " ";
      }
      out << std::endl;

      for (unsigned int k = 0; k < 3; k++) {
        outw << wpos[k] << " ";
      }
      outw << std::endl;

      counter++;
    }      // end for points

    out.close();
    outw.close();

    std::stringstream st;
    st << counter;
    str = "with " + st.str() + "points...";
    this->PrintStartMessage(str, 1);
    this->PrintDoneMessage(1);
  }   // end for files
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WritePointFilesWithFeatures(int iter)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFilesWithFeatures(tmp_dir_name);
}

//---------------------------------------------------------------------------
void Optimize::WritePointFilesWithFeatures(std::string iter_prefix)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  if (!m_mesh_based_attributes) {
    return;
  }

  this->PrintStartMessage("Writing point with attributes files...\n");
  typedef Sampler::PointType PointType;
  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_wptsFeatures.particles";

    std::ofstream outw(world_file.c_str());

    std::string str = "Writing " + world_file + "...";
    int attrNum = 3 * int(m_use_xyz[i % m_domains_per_shape]) + 3 *
                                                                int(m_use_normals[i % m_domains_per_shape]);
    if (m_attributes_per_domain.size() > 0) {
      attrNum += m_attributes_per_domain[i % m_domains_per_shape];
    }
    std::stringstream st;
    st << attrNum;
    str += "with " + st.str() + " attributes per point...";
    this->PrintStartMessage(str, 1);

    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    // Only run the following code if we are dealing with ImplicitSurfaceDomains
    const itk::ParticleImplicitSurfaceDomain<float>* domain
      = dynamic_cast <const itk::ParticleImplicitSurfaceDomain<float>*> (m_sampler->GetParticleSystem()
        ->GetDomain(i));
    if (domain) {
      std::vector<float> fVals;

      for (unsigned int j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
        PointType pos = m_sampler->GetParticleSystem()->GetPosition(j, i);
        PointType wpos = m_sampler->GetParticleSystem()->GetTransformedPosition(j, i);

        for (unsigned int k = 0; k < 3; k++) {
          outw << wpos[k] << " ";
        }

        if (m_use_normals[i % m_domains_per_shape]) {
          vnl_vector_fixed<float, DIMENSION> pG = domain->SampleNormalAtPoint(pos);
          VectorType pN;
          pN[0] = pG[0];
          pN[1] = pG[1];
          pN[2] = pG[2];
          pN = m_sampler->GetParticleSystem()->TransformVector(pN,
                                                               m_sampler->GetParticleSystem()->GetTransform(
                                                                 i) *
                                                               m_sampler->GetParticleSystem()->GetPrefixTransform(
                                                                 i));
          outw << pN[0] << " " << pN[1] << " " << pN[2] << " ";
        }

        if (m_attributes_per_domain.size() > 0) {
          if (m_attributes_per_domain[i % m_domains_per_shape] > 0) {
            point pt;
            pt.clear();
            pt[0] = pos[0];
            pt[1] = pos[1];
            pt[2] = pos[2];
            fVals.clear();
            if (m_mesh_based_attributes) {
              domain->GetMesh()->GetFeatureValues(pt, fVals);
            }
            for (unsigned int k = 0; k < m_attributes_per_domain[i % m_domains_per_shape]; k++) {
              outw << fVals[k] << " ";
            }
          }
        }

        outw << std::endl;

        counter++;
      }      // end for points
    }

    outw.close();
    this->PrintDoneMessage(1);
  }   // end for files
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteEnergyFiles()
{
  if (!this->m_file_output_enabled) {
    return;
  }

  if (!this->m_log_energy) {
    return;
  }
  this->PrintStartMessage("Writing energy files...\n");
  std::string strA = m_output_dir + "/" + this->m_str_energy + "_samplingEnergy.txt";
  std::string strB = m_output_dir + "/" + this->m_str_energy + "_correspondenceEnergy.txt";
  std::string strTotal = m_output_dir + "/" + this->m_str_energy + "_totalEnergy.txt";
  std::ofstream outA(strA.c_str(), std::ofstream::app);
  std::ofstream outB(strB.c_str(), std::ofstream::app);
  std::ofstream outTotal(strTotal.c_str(), std::ofstream::app);

  if (!outA) {
    std::cerr << "Error opening output energy file: " << strA << std::endl;
    throw 1;
  }
  if (!outB) {
    std::cerr << "Error opening output energy file: " << strB << std::endl;
    throw 1;
  }
  if (!outTotal) {
    std::cerr << "Error opening output energy file: " << strTotal << std::endl;
    throw 1;
  }

  int n = m_energy_a.size() - 1;
  n = n < 0 ? 0 : n;

  std::string str = "Appending to " + strA + " ...";
  this->PrintStartMessage(str, 1);
  outA << m_energy_a[n] << std::endl;
  outA.close();
  this->PrintDoneMessage(1);

  str = "Appending to " + strB + " ...";
  this->PrintStartMessage(str, 1);
  outB << m_energy_b[n] << std::endl;
  outB.close();
  this->PrintDoneMessage(1);

  str = "Appending to " + strTotal + " ...";
  this->PrintStartMessage(str, 1);
  outTotal << m_total_energy[n] << std::endl;
  outTotal.close();
  this->PrintDoneMessage(1);

  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteCuttingPlanePoints(int iter)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  this->PrintStartMessage("Writing cutting plane points...\n");
  std::string output_file = m_output_cutting_plane_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    output_file = "./iter" + ss.str() + "/" + output_file;
  }

  std::ofstream out(output_file.c_str());

  std::string str = "Writing " + output_file + "...";
  this->PrintStartMessage(str, 1);

  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->GetDomain(i)->PrintCuttingPlaneConstraints(out);
  }
  out.close();
  this->PrintDoneMessage(1);
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteParameters(int iter)
{
  if (!this->m_file_output_enabled) {
    return;
  }

  if (!m_use_regression) {
    return;
  }
  std::string slopename, interceptname;

  slopename = std::string(m_output_dir) + std::string("slope");
  interceptname = std::string(m_output_dir) + std::string("intercept");

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;

    slopename = "./.iter" + ss.str() + "/" + slopename;
    interceptname = "./.iter" + ss.str() + "/" + interceptname;
  }

  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector<double> slope;
  std::vector<double> intercept;

  if (m_use_mixed_effects == true) {
    vnl_vector<double> slopevec = dynamic_cast < itk::ParticleShapeMixedEffectsMatrixAttribute<
      double, 3>* >
    (m_sampler->GetEnsembleMixedEffectsEntropyFunction()->
        GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    vnl_vector<double> interceptvec = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute<double,
        3>* >
    (m_sampler->GetEnsembleMixedEffectsEntropyFunction()->
        GetShapeMatrix())->GetIntercept();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << intercept[i] << "\n";
    }
    out.close();

    slopename = std::string(m_output_dir) + std::string("sloperand");
    interceptname = std::string(m_output_dir) + std::string("interceptrand");

    if (iter >= 0) {
      std::stringstream ss;
      ss << iter + m_optimization_iterations_completed;

      slopename = "./.iter" + ss.str() + "/" + slopename;
      interceptname = "./.iter" + ss.str() + "/" + interceptname;
    }

    std::cout << "writing " << slopename << std::endl;
    std::cout << "writing " << interceptname << std::endl;

    vnl_matrix<double> sloperand_mat = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute<double,
        3>* >
    (m_sampler->GetEnsembleMixedEffectsEntropyFunction()->
        GetShapeMatrix())->GetSlopeRandom();

    out.open(slopename.c_str());
    for (unsigned int i = 0; i < sloperand_mat.rows(); i++) {
      for (unsigned int j = 0; j < sloperand_mat.cols(); j++) {
        out << sloperand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();

    vnl_matrix<double> interceptrand_mat = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute<
        double, 3>* >
    (m_sampler->GetEnsembleMixedEffectsEntropyFunction()->
        GetShapeMatrix())->GetInterceptRandom();

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < interceptrand_mat.rows(); i++) {
      for (unsigned int j = 0; j < interceptrand_mat.cols(); j++) {
        out << interceptrand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();
  }
  else {
    vnl_vector<double> slopevec = dynamic_cast <
      itk::ParticleShapeLinearRegressionMatrixAttribute<double,
        3>* >
    (m_sampler->GetEnsembleRegressionEntropyFunction()->
        GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    std::vector<double> intercept;
    vnl_vector<double> interceptvec = dynamic_cast <
      itk::ParticleShapeLinearRegressionMatrixAttribute<double,
        3>* >
    (m_sampler->GetEnsembleRegressionEntropyFunction()->
        GetShapeMatrix())->GetIntercept();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << intercept[i] << "\n";
    }
    out.close();
  }
}

//---------------------------------------------------------------------------
void Optimize::ReportBadParticles()
{
  if (!this->m_file_output_enabled) {
    return;
  }

  this->PrintStartMessage("Reporting bad particles...", 2);
  typedef Sampler::PointType PointType;
  const int totalDomains = m_sampler->GetParticleSystem()->GetNumberOfDomains();
  const int numShapes = totalDomains / m_domains_per_shape;
  std::string outDomDir;
  std::string outPtDir;
  if (this->m_bad_ids.empty()) {
    return;
  }
  for (int i = 0; i < this->m_domains_per_shape; i++) {
    if (this->m_bad_ids[i].empty()) {
      continue;
    }

    std::stringstream ss;
    ss << i;
    outDomDir = m_output_dir + "/" + this->m_str_energy + "_BadParticles_domain" + ss.str();
#ifdef _WIN32
    mkdir(outDomDir.c_str());
#else
    mkdir(outDomDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    for (int j = 0; j < this->m_bad_ids[i].size(); j++) {
      std::stringstream ssj;
      ssj << m_bad_ids[i][j];
      outPtDir = outDomDir + "/particle" + ssj.str();
#ifdef _WIN32
      mkdir(outPtDir.c_str());
#else
      mkdir(outPtDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
      for (int k = 0; k < numShapes; k++) {
        int dom = k * m_domains_per_shape + i;
        std::string localPtFile = outPtDir + "/" + m_filenames[dom] + ".particles";
        std::ofstream outl(localPtFile.c_str(), std::ofstream::app);
        PointType pos = m_sampler->GetParticleSystem()->GetPosition(m_bad_ids[i][j], dom);
        for (unsigned int d = 0; d < 3; d++) {
          outl << pos[d] << " ";
        }
        outl << std::endl;
        outl.close();
      }
    }
  }
  this->PrintDoneMessage(2);
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> Optimize::GetLocalPoints()
{
  return this->m_local_points;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> Optimize::GetGlobalPoints()
{
  return this->m_global_points;
}

//---------------------------------------------------------------------------
void Optimize::SetCutPlanes(std::vector<std::array<itk::Point<double>, 3>> cut_planes)
{
  this->m_cut_planes = cut_planes;
}

//---------------------------------------------------------------------------
bool Optimize::GetAborted()
{
  return this->m_aborted;
}

//---------------------------------------------------------------------------
void Optimize::UpdateExportablePoints()
{
  this->m_local_points.clear();
  this->m_global_points.clear();
  for (size_t d = 0; d < this->m_sampler->
    GetParticleSystem()->GetNumberOfDomains(); d++) {

    // blank set of points
    this->m_local_points.push_back(std::vector<itk::Point<double>>());
    this->m_global_points.push_back(std::vector<itk::Point<double>>());

    // for each particle
    for (size_t j = 0; j < this->m_sampler->
      GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = this->m_sampler->GetParticleSystem()->GetPosition(j, d);
      auto pos2 = this->m_sampler->GetParticleSystem()->GetTransformedPosition(j, d);
      this->m_local_points[d].push_back(pos);
      this->m_global_points[d].push_back(pos2);
    }
  }
}

//---------------------------------------------------------------------------
void Optimize::SetPairwisePotentialType(int pairwise_potential_type)
{ this->m_pairwise_potential_type = pairwise_potential_type; }

//---------------------------------------------------------------------------
void Optimize::SetTimePtsPerSubject(int time_pts_per_subject)
{ this->m_timepts_per_subject = time_pts_per_subject; }

//---------------------------------------------------------------------------
int Optimize::GetTimePtsPerSubject()
{ return this->m_timepts_per_subject; }

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterations(int optimization_iterations)
{ this->m_optimization_iterations = optimization_iterations; }

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterationsCompleted(int optimization_iterations_completed)
{ this->m_optimization_iterations_completed = optimization_iterations_completed; }

//---------------------------------------------------------------------------
void Optimize::SetIterationsPerSplit(int iterations_per_split)
{ this->m_iterations_per_split = iterations_per_split; }

//---------------------------------------------------------------------------
void Optimize::SetInitializationCriterion(double init_criterion)
{ this->m_initialization_criterion = init_criterion; }

//---------------------------------------------------------------------------
void Optimize::SetOptimizationCriterion(double opt_criterion)
{ this->m_optimization_criterion = opt_criterion; }

//---------------------------------------------------------------------------
void Optimize::SetUseShapeStatisticsInInit(bool use_shape_statistics_in_init)
{ this->m_use_shape_statistics_in_init = use_shape_statistics_in_init; }

//---------------------------------------------------------------------------
void Optimize::SetProcrustesInterval(int procrustes_interval)
{ this->m_procrustes_interval = procrustes_interval; }

//---------------------------------------------------------------------------
void Optimize::SetProcrustesScaling(int procrustes_scaling)
{ this->m_procrustes_scaling = procrustes_scaling; }

//---------------------------------------------------------------------------
void Optimize::SetRelativeWeighting(double relative_weighting)
{ this->m_relative_weighting = relative_weighting; }

//---------------------------------------------------------------------------
void Optimize::SetInitialRelativeWeighting(double initial_relative_weighting)
{ this->m_initial_relative_weighting = initial_relative_weighting; }

//---------------------------------------------------------------------------
void Optimize::SetStartingRegularization(double starting_regularization)
{ this->m_starting_regularization = starting_regularization; }

//---------------------------------------------------------------------------
void Optimize::SetEndingRegularization(double ending_regularization)
{ this->m_ending_regularization = ending_regularization; }

//---------------------------------------------------------------------------
void Optimize::SetRecomputeRegularizationInterval(int recompute_regularization_interval)
{ this->m_recompute_regularization_interval = recompute_regularization_interval; }

//---------------------------------------------------------------------------
void Optimize::SetSaveInitSplits(bool save_init_splits)
{ this->m_save_init_splits = save_init_splits; }

//---------------------------------------------------------------------------
void Optimize::SetCheckpointingInterval(int checkpointing_interval)
{ this->m_checkpointing_interval = checkpointing_interval; }

//---------------------------------------------------------------------------
void Optimize::SetKeepCheckpoints(int keep_checkpoints)
{ this->m_keep_checkpoints = keep_checkpoints; }

//---------------------------------------------------------------------------
void Optimize::SetCotanSigmaFactor(double cotan_sigma_factor)
{ this->m_cotan_sigma_factor = cotan_sigma_factor; }

//---------------------------------------------------------------------------
void Optimize::SetUseRegression(bool use_regression)
{ this->m_use_regression = use_regression; }

//---------------------------------------------------------------------------
void Optimize::SetUseMixedEffects(bool use_mixed_effects)
{ this->m_use_mixed_effects = use_mixed_effects; }

//---------------------------------------------------------------------------
void Optimize::SetNormalAngle(double normal_angle)
{ this->m_normal_angle = normal_angle; }

//---------------------------------------------------------------------------
void Optimize::SetPerformGoodBad(bool perform_good_bad)
{ this->m_perform_good_bad = perform_good_bad; }

//---------------------------------------------------------------------------
void Optimize::SetLogEnergy(bool log_energy)
{ this->m_log_energy = log_energy; }

//---------------------------------------------------------------------------
void Optimize::AddImage(ImageType::Pointer image)
{
  this->m_sampler->AddImage(image, this->GetNarrowBand());
  this->m_num_shapes++;
  if (image) {
    this->m_spacing = image->GetSpacing()[0] * 5;
  }
}

//---------------------------------------------------------------------------
void Optimize::AddMesh(shapeworks::MeshWrapper* mesh)
{
  this->m_sampler->AddMesh(mesh);
  this->m_num_shapes++;
  this->m_spacing = 0.5;
}

//---------------------------------------------------------------------------
void Optimize::SetFilenames(const std::vector<std::string>& filenames)
{ this->m_filenames = filenames; }

//---------------------------------------------------------------------------
void Optimize::SetPointFiles(const std::vector<std::string>& point_files)
{
  for (int shapeCount = 0; shapeCount < point_files.size(); shapeCount++) {
    this->m_sampler->SetPointsFile(shapeCount, point_files[shapeCount]);
  }
}

//---------------------------------------------------------------------------
int Optimize::GetNumShapes()
{
  return this->m_num_shapes;
}

shapeworks::OptimizationVisualizer& Optimize::GetVisualizer() {
  return visualizer;
}

void Optimize::SetShowVisualizer(bool show) {
  if (show && this->m_verbosity_level > 0) {
    std::cout << "WARNING Using the visualizer will increase run time!\n";
  }
  this->show_visualizer = show;
}

bool Optimize::GetShowVisualizer() {
  return this->show_visualizer;
}

//---------------------------------------------------------------------------
void Optimize::SetMeshFiles(const std::vector<std::string>& mesh_files)
{
  m_sampler->SetMeshFiles(mesh_files);
}

//---------------------------------------------------------------------------
void Optimize::SetAttributeScales(const std::vector<double>& scales)
{
  this->m_sampler->SetAttributeScales(scales);
}

//---------------------------------------------------------------------------
void Optimize::SetFeaFiles(const std::vector<std::string>& files)
{
  this->m_sampler->SetFeaFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetFeaGradFiles(const std::vector<std::string>& files)
{
  this->m_sampler->SetFeaGradFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetFidsFiles(const std::vector<std::string>& files)
{
  this->m_sampler->SetFidsFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetParticleFlags(std::vector<int> flags)
{
  this->m_particle_flags = flags;
}

//---------------------------------------------------------------------------
void Optimize::SetDomainFlags(std::vector<int> flags)
{
  if (flags.size() > 0) {
    // Fixed domains are in use.
    this->m_fixed_domains_present = true;
  }
  this->m_domain_flags = flags;
}

//---------------------------------------------------------------------------
const std::vector<int>& Optimize::GetDomainFlags()
{
  return this->m_domain_flags;
}

//---------------------------------------------------------------------------
void Optimize::SetFileOutputEnabled(bool enabled)
{
  this->m_file_output_enabled = enabled;
}

//---------------------------------------------------------------------------
std::vector<bool> Optimize::GetUseXYZ()
{
  return this->m_use_xyz;
}

//---------------------------------------------------------------------------
std::vector<bool> Optimize::GetUseNormals()
{
  return this->m_use_normals;
}

//---------------------------------------------------------------------------
void Optimize::SetNarrowBand(double v)
{
  this->m_narrow_band_set = true;
  this->m_narrow_band = v;
}

//---------------------------------------------------------------------------
double Optimize::GetNarrowBand()
{
  if (this->m_narrow_band_set) {
    return this->m_narrow_band;
  }

  if (this->m_fixed_domains_present) {
    return 1e10;
  }
  else {
    return 10.0;
  }
}

//---------------------------------------------------------------------------
void Optimize::SetUseShapeStatisticsAfter(int num_particles)
{
  this->m_use_shape_statistics_after = num_particles;
}

//---------------------------------------------------------------------------
int Optimize::GetUseShapeStatisticsAfter()
{
  return this->m_use_shape_statistics_after;
}

//---------------------------------------------------------------------------
void Optimize::SetIterationCallback()
{
  itk::MemberCommand<Optimize>::Pointer m_iterate_command = itk::MemberCommand<Optimize>::New();
  m_iterate_command->SetCallbackFunction(this, &Optimize::IterateCallback);
  m_sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), m_iterate_command);
}

//---------------------------------------------------------------------------
void Optimize::WriteModes()
{
  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains() % m_domains_per_shape;
  if (n >= 5) {
    m_sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_dir + "/pts", 5);
  }
}

//---------------------------------------------------------------------------
void Optimize::PrintStartMessage(std::string str, unsigned int vlevel) const
{
  if (this->m_verbosity_level > vlevel) {
    std::cout << str;
    std::cout.flush();
  }
}

//---------------------------------------------------------------------------
void Optimize::PrintDoneMessage(unsigned int vlevel) const
{
  if (m_verbosity_level > vlevel) {
    std::cout << "Done." << std::endl;
  }
}

}
