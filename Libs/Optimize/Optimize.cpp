// std
#include <algorithm>
#include <cstdlib>
#include <iostream>
//#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#define mkdir _mkdir
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif  // ifdef _WIN32

// itk
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageToVTKImageFilter.h>
#include <itkMultiThreaderBase.h>

// shapeworks
#include <Libs/Particles/ParticleFile.h>
#include <Project/Project.h>

#include "Libs/Optimize/Domain/Surface.h"
#include "Libs/Optimize/Utils/ObjectReader.h"
#include "Libs/Optimize/Utils/ObjectWriter.h"
#include "Libs/Optimize/Utils/ParticleGoodBadAssessment.h"
#include "Logging.h"
#include "Optimize.h"
#include "OptimizeParameterFile.h"
#include "OptimizeParameters.h"
#include "ShapeworksUtils.h"

// pybind
#include <pybind11/embed.h>

namespace py = pybind11;

namespace shapeworks {

#ifdef _WIN32
static std::string find_in_path(std::string file) {
  std::stringstream path(getenv("PATH"));
  while (!path.eof()) {
    std::string test;
    struct stat info;
    getline(path, test, ';');
    std::string base = test;
    test.append("/");
    test.append(file);
    if (stat(test.c_str(), &info) == 0) {
      return base;
    }
  }
  return "";
}
#endif

//---------------------------------------------------------------------------
Optimize::Optimize() { m_sampler = std::make_shared<Sampler>(); }

//---------------------------------------------------------------------------
Optimize::~Optimize() {}

//---------------------------------------------------------------------------
bool Optimize::Run() {
  m_start_time = std::chrono::system_clock::now();
  m_last_update_time = m_start_time;

  ShapeWorksUtils::setup_threads();

  if (m_python_filename != "") {
#ifdef _WIN32
    // need to set PYTHONHOME to the same directory as python.exe on Windows
    std::string found_path = find_in_path("python.exe");
    if (found_path != "") {
      std::cerr << "python.exe found in: " << found_path << "\n";
      _putenv_s("PYTHONHOME", found_path.c_str());
    }
#endif

    py::initialize_interpreter();

    auto dir = m_python_filename;

    auto filename = dir.substr(dir.find_last_of("/") + 1);
    SW_LOG("Running Python File: {}", filename);
    filename = filename.substr(0, filename.length() - 3);  // remove .py
    dir = dir.substr(0, dir.find_last_of("/") + 1);

    py::module sys = py::module::import("sys");
    py::print(sys.attr("path"));
    sys.attr("path").attr("insert")(1, dir);
    py::print(sys.attr("path"));

    py::module module = py::module::import(filename.c_str());
    py::object result = module.attr("run")(this);
  }

  if (!SetParameters()) {
    return false;
  }

  current_particle_iterations_ = 0;
  m_iteration_count = 0;
  m_split_number = 0;

  ComputeTotalIterations();

  std::vector<int> final_number_of_particles = m_number_of_particles;
  if (m_use_shape_statistics_after > 0) {
    m_use_shape_statistics_in_init = false;
    for (int i = 0; i < m_number_of_particles.size(); i++) {
      // run up to only the specified starting point for multiscale
      m_number_of_particles[i] = std::min(m_use_shape_statistics_after, m_number_of_particles[i]);
    }
  }

  // Initialize
  Initialize();
  RunOptimize();

  if (this->m_use_shape_statistics_after > 0) {
    // First phase is done now run iteratively until we reach the final particle counts

    // save the particles for this split if requested
    if (m_save_init_splits == true) {
      WriteSplitFiles("pts_w_opt");
    }

    // set to use shape statistics now for the Initialize mode
    m_use_shape_statistics_in_init = true;

    // reset the number of iterations completed
    m_optimization_iterations_completed = 0;

    bool finished = false;

    while (!finished) {
      m_sampler->ReInitialize();

      // determine if we have reached the final particle counts
      finished = true;
      for (int i = 0; i < m_number_of_particles.size(); i++) {
        if (m_number_of_particles[i] < final_number_of_particles[i]) {
          m_number_of_particles[i] *= 2;
          finished = false;
        }
      }

      if (!finished) {
        Initialize();
        RunOptimize();
      }
    }
  }

  UpdateExportablePoints();

  if (m_python_filename != "") {
    iteration_callback_ = nullptr;
    // Disable finalize_interpreter as it is crashing with Open3D
    // I am not sure why yet.
    // py::finalize_interpreter();
  }

  UpdateProject();

  return true;
}

//---------------------------------------------------------------------------
int Optimize::SetParameters() {
  // sanity check
  if (m_domains_per_shape != m_number_of_particles.size()) {
    SW_ERROR("Inconsistency in parameters... m_domains_per_shape != m_number_of_particles.size()");
    return false;
  }

  // ensure that use_shape_statistics_after doesn't increase the particle count over what was specified
  for (int i = 0; i < m_number_of_particles.size(); i++) {
    m_use_shape_statistics_after = std::min(m_use_shape_statistics_after, m_number_of_particles[i]);
  }

  // Set up the optimization process
  m_sampler->SetDomainsPerShape(this->m_domains_per_shape);  // must be done first!
  m_sampler->SetTimeptsPerIndividual(this->m_timepts_per_subject);
  m_sampler->GetParticleSystem()->SetDomainsPerShape(this->m_domains_per_shape);
  m_sampler->SetVerbosity(this->m_verbosity_level);

  if (this->m_use_xyz.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      m_sampler->SetXYZ(i, this->m_use_xyz[i]);
    }
  } else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      m_sampler->SetXYZ(i, true);
    }
  }

  if (this->m_use_normals.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      m_sampler->SetNormals(i, this->m_use_normals[i]);
    }
  } else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      m_sampler->SetNormals(i, false);
    }
  }

  // Set up the procrustes registration object.
  this->m_procrustes = std::make_shared<ProcrustesRegistration>();
  this->m_procrustes->SetParticleSystem(m_sampler->GetParticleSystem());
  this->m_procrustes->SetDomainsPerShape(this->m_domains_per_shape);
  this->m_procrustes->SetScaling(this->m_procrustes_scaling);
  this->m_procrustes->SetRotationTranslation(this->m_procrustes_rotation_translation);

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
  } else {
    int numShapes = m_sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      m_sampler->GetParticleSystem()->GetDomain(i)->DeletePartialDerivativeImages();
    }
  }

  if (m_domain_flags.size() > 0) {
    for (int i = 0; i < m_domain_flags.size(); i++) {
      if (m_use_normals.size() > 0) {
        if (m_use_normals[i % m_domains_per_shape]) {
          m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->DeletePartialDerivativeImages();
        } else {
          m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->DeleteImages();
        }
      } else {
        m_sampler->GetParticleSystem()->GetDomain(m_domain_flags[i])->DeleteImages();
      }
    }
  }

  if (m_verbosity_level > 1) {
    this->PrintParamInfo();
  }

  m_good_bad = std::make_shared<ParticleGoodBadAssessment>();
  m_good_bad->set_domains_per_shape(m_domains_per_shape);
  m_good_bad->set_criterion_angle(m_normal_angle);

  m_energy_a.clear();
  m_energy_b.clear();
  m_total_energy.clear();

  // Now read the transform file if present.
  if (m_transform_file != "") {
    this->ReadTransformFile();
  }
  if (m_prefix_transform_file != "") {
    this->ReadPrefixTransformFile(m_prefix_transform_file);
  }

  return true;
}

//---------------------------------------------------------------------------
void Optimize::SetVerbosity(int verbosity_level) { this->m_verbosity_level = verbosity_level; }

//---------------------------------------------------------------------------
void Optimize::SetDomainsPerShape(int domains_per_shape) {
  this->m_domains_per_shape = domains_per_shape;
  m_sampler->SetDomainsPerShape(this->m_domains_per_shape);
}

//---------------------------------------------------------------------------
int Optimize::GetDomainsPerShape() { return this->m_domains_per_shape; }

//---------------------------------------------------------------------------
void Optimize::SetDomainType(shapeworks::DomainType type) { this->m_domain_type = type; }

//---------------------------------------------------------------------------
shapeworks::DomainType Optimize::GetDomainType() { return this->m_domain_type; }

//---------------------------------------------------------------------------
void Optimize::SetNumberOfParticles(std::vector<int> number_of_particles) {
  this->m_number_of_particles = number_of_particles;
}

//---------------------------------------------------------------------------
std::vector<int> Optimize::GetNumberOfParticles() { return this->m_number_of_particles; }

//---------------------------------------------------------------------------
void Optimize::SetTransformFile(std::string filename) { this->m_transform_file = filename; }

//---------------------------------------------------------------------------
std::string Optimize::GetTransformFile() { return this->m_transform_file; }

//---------------------------------------------------------------------------
void Optimize::SetPrefixTransformFile(std::string prefix_transform_file) {
  this->m_prefix_transform_file = prefix_transform_file;
}

//---------------------------------------------------------------------------
std::string Optimize::GetPrefixTransformFile() { return this->m_prefix_transform_file; }

//---------------------------------------------------------------------------
void Optimize::SetOutputDir(std::string output_dir) { this->m_output_dir = output_dir; }

//---------------------------------------------------------------------------
void Optimize::SetOutputTransformFile(std::string output_transform_file) {
  this->m_output_transform_file = output_transform_file;
}

//---------------------------------------------------------------------------
void Optimize::SetOutputIndividualTransformFiles(bool value) { this->m_output_transform_files = value; }

//---------------------------------------------------------------------------
void Optimize::SetUseMeshBasedAttributes(bool use_mesh_based_attributes) {
  this->m_mesh_based_attributes = use_mesh_based_attributes;

  if (this->m_mesh_based_attributes) {
    m_sampler->RegisterGeneralShapeMatrices();
  }
}

//---------------------------------------------------------------------------
bool Optimize::GetUseMeshBasedAttributes() { return this->m_mesh_based_attributes; }

//---------------------------------------------------------------------------
void Optimize::SetUseXYZ(std::vector<bool> use_xyz) { this->m_use_xyz = use_xyz; }

//---------------------------------------------------------------------------
void Optimize::SetUseNormals(std::vector<bool> use_normals) { this->m_use_normals = use_normals; }

//---------------------------------------------------------------------------
void Optimize::SetAttributesPerDomain(std::vector<int> attributes_per_domain) {
  this->m_attributes_per_domain = attributes_per_domain;
  m_sampler->SetAttributesPerDomain(attributes_per_domain);
}

//---------------------------------------------------------------------------
std::vector<int> Optimize::GetAttributesPerDomain() { return this->m_attributes_per_domain; }

//---------------------------------------------------------------------------
void Optimize::SetDistributionDomainID(int distribution_domain_id) {
  this->m_distribution_domain_id = distribution_domain_id;
}

//---------------------------------------------------------------------------
int Optimize::GetDistributionDomainID() { return this->m_distribution_domain_id; }

//---------------------------------------------------------------------------
void Optimize::SetOutputCuttingPlaneFile(std::string output_cutting_plane_file) {
  this->m_output_cutting_plane_file = output_cutting_plane_file;
}

//---------------------------------------------------------------------------
void Optimize::SetUseCuttingPlanes(bool use_cutting_planes) { this->m_use_cutting_planes = use_cutting_planes; }

//---------------------------------------------------------------------------
void Optimize::SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, 3>& va,
                               const vnl_vector_fixed<double, 3>& vb, const vnl_vector_fixed<double, 3>& vc) {
  m_sampler->SetCuttingPlane(i, va, vb, vc);
}

//---------------------------------------------------------------------------
void Optimize::ReadTransformFile() {
  ObjectReader<ParticleSystem::TransformType> reader;
  reader.SetFileName(m_transform_file);
  reader.Update();
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
void Optimize::ReadPrefixTransformFile(const std::string& fn) {
  ObjectReader<ParticleSystem::TransformType> reader;
  reader.SetFileName(fn.c_str());
  reader.Update();
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
// Initialization and Optimization
void Optimize::InitializeSampler() {
  float nbhd_to_sigma = 3.0;  // 3.0 -> 1.0
  float flat_cutoff = 0.3;    // 0.3 -> 0.85

  m_sampler->GetCurvatureGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetDisentangledEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_sampler->GetDisentangledEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_sampler->GetDisentangledEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVariance(m_starting_regularization);
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

  m_sampler->SetAdaptivityMode();
  m_sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_sampler->GetDisentangledEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(
      m_recompute_regularization_interval);
  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetRecomputeCovarianceInterval(
      m_recompute_regularization_interval);
  m_sampler->GetEnsembleRegressionEntropyFunction()->SetRecomputeCovarianceInterval(
      m_recompute_regularization_interval);
  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetRecomputeCovarianceInterval(
      m_recompute_regularization_interval);

  // These flags must be set before Initialize, since Initialize need to know which domains are fixed ahead of time
  for (unsigned int i = 0; i < this->m_domain_flags.size(); i++) {
    this->GetSampler()->GetParticleSystem()->FlagDomain(this->m_domain_flags[i]);
  }

  m_sampler->Initialize();

  m_sampler->GetOptimizer()->SetTolerance(0.0);

  // These flags have to be set after Initialize, since Initialize will set them all to zero
  for (unsigned int i = 0; i < this->m_particle_flags.size() / 2; i++) {
    this->GetSampler()->GetParticleSystem()->SetFixedParticleFlag(this->m_particle_flags[2 * i],
                                                                  this->m_particle_flags[2 * i + 1]);
  }

  m_sampler->ApplyConstraintsToZeroCrossing();
}

//---------------------------------------------------------------------------
double Optimize::GetMinNeighborhoodRadius() {
  double rad = 0.0;
  typename itk::ImageToVTKImageFilter<ImageType>::Pointer itk2vtkConnector;
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    double area = m_sampler->GetParticleSystem()->GetDomain(i)->GetSurfaceArea();
    double sigma = std::sqrt(area / (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    if (rad < sigma) {
      rad = sigma;
    }
  }
  return rad;
}

//---------------------------------------------------------------------------
void Optimize::AddSinglePoint() {
  typedef ParticleSystem ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;

  PointType firstPointPosition = m_sampler->GetParticleSystem()->GetDomain(0)->GetZeroCrossingPoint();

  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) > 0) {
      continue;
    }
    // todo this is misleading. ParticleImageDomain::GetValidLocationNear doesn't care about the argument,
    // it always returns the zero-crossing point. this behaviour is fine for now because it works, but its worth
    // noting that MeshDomain does the "right thing" and actual finds the closest valid location.
    PointType pos = m_sampler->GetParticleSystem()->GetDomain(i)->GetValidLocationNear(firstPointPosition);
    m_sampler->GetParticleSystem()->AddPosition(pos, i);
  }
}

//---------------------------------------------------------------------------
void Optimize::Initialize() {
  if (m_verbosity_level > 0) {
    SW_LOG("------------------------------");
    SW_LOG("*** Initialize Step ***");
    SW_LOG("------------------------------");
  }

  if (m_procrustes_interval != 0) {  // Initial registration
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) > 10) {
        m_procrustes->RunRegistration(i);
      }
    }
    this->WritePointFiles();
    this->WriteTransformFile();
    this->WriteTransformFiles();
  }

  m_sampler->GetParticleSystem()->SynchronizePositions();

  m_sampler->SetCorrespondenceOn();

  if (m_use_shape_statistics_in_init) {
    if (m_mesh_based_attributes) {
      SW_DEBUG("Setting correspondence mode: mesh-based general entropy");
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy);
    } else {
      SW_DEBUG("Setting correspondence mode: ensemble entropy");
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleEntropy);
    }

    m_sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization, m_ending_regularization,
                                                                     m_iterations_per_split);

    m_sampler->GetDisentangledEnsembleEntropyFunction()->SetMinimumVarianceDecay(
        m_starting_regularization, m_ending_regularization, m_iterations_per_split);

    m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
        m_starting_regularization, m_ending_regularization, m_iterations_per_split);
  } else {
    // force to mean
    if ((m_attributes_per_domain.size() > 0 &&
         *std::max_element(m_attributes_per_domain.begin(), m_attributes_per_domain.end()) > 0) ||
        m_mesh_based_attributes) {
      SW_DEBUG("Setting correspondence mode: mesh-based mean energy");
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralMeanEnergy);
    } else {
      SW_DEBUG("Setting correspondence mode: mean energy");
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeanEnergy);
    }
  }

  m_sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  this->AddSinglePoint();

  m_sampler->GetParticleSystem()->SynchronizePositions();

  int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();

  // Adaptive Initialization params
  bool adaptive_initialization = false;
  size_t particles_before_adaptive_initialization =
      8;  // # of particles before adaptive initialization starts, starts at >= this_value
  size_t check_iterations =
      50;  // The initialization optimization will check every check_iterations iterations for sampling quality
  double initialization_start_scaling_factor = 3.;

  m_sampler->GetOptimizer()->SetInitializationStartScalingFactor(initialization_start_scaling_factor);

  /*Old vector randomization
  vnl_vector_fixed<double, 3> random;

  for (int i = 0; i < 3; i++) {
    random[i] = static_cast < double > (this->m_rand());
  }
  random = random.normalize() * this->m_spacing;
  */

  double epsilon = this->m_spacing;
  bool flag_split = false;

  for (int i = 0; i < n; i++) {
    int d = i % m_domains_per_shape;
    if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
      flag_split = true;
      break;
    }
  }

  while (flag_split) {
    /*Old vector randomization
  for (int i = 0; i < 3; i++) {
    random[i] = static_cast <double> (this->m_rand());
  }

  // divide by 5 since m_spacing was artificially multiplied by 5 elsewhere
  random = random.normalize() * this->m_spacing / 5.0;
  */

    //        m_Sampler->GetEnsembleEntropyFunction()->PrintShapeMatrix();
    this->OptimizerStop();

    /*Old vector randomization
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        m_sampler->GetParticleSystem()->SplitAllParticlesInDomain(random, i, 0);
      }
    }
    */

    // Splits particles
    // Strategy: For each domain (for all samples), we make very corresponding particle
    //      go in a random direction with magnitude epsilon/5. Then the shifted particles
    //      in each domain are tested so that no particle will violate any inequality constraints
    //      after its split.
    for (int i = 0; i < m_domains_per_shape; i++) {
      if (m_sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[i]) {
        m_sampler->GetParticleSystem()->AdvancedAllParticleSplitting(epsilon, m_domains_per_shape, i);
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
      WriteSplitFiles("pts_wo_init");
    }

    m_energy_a.clear();
    m_energy_b.clear();
    m_total_energy.clear();
    std::stringstream ss;
    ss << this->m_split_number;
    std::stringstream ssp;

    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();  // size from domain 0
    m_str_energy = "split" + ss.str();

    for (int i = 0; i < m_domains_per_shape; i++) {
      ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
      m_str_energy += "_" + ssp.str();
      ssp.str("");
    }
    m_str_energy += "pts_init";

    m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_sampler->GetOptimizer()->SetNumberOfIterations(0);
    if (adaptive_initialization &&
        m_sampler->GetParticleSystem()->GetNumberOfParticles(0) >= particles_before_adaptive_initialization) {
      m_sampler->GetOptimizer()->SetInitializationMode(true);
    }
    m_sampler->Execute();
    m_sampler->GetOptimizer()->SetInitializationMode(false);

    if (m_save_init_splits == true) {
      WriteSplitFiles("pts_w_init");
    }
    this->WritePointFiles();
    this->WritePointFilesWithFeatures();
    this->WriteEnergyFiles();
    this->WriteTransformFile();
    this->WriteTransformFiles();

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
  this->WriteTransformFiles();
  this->WriteCuttingPlanePoints();
  if (m_verbosity_level > 0) {
    SW_LOG("Finished initialization");
  }
}

//---------------------------------------------------------------------------
void Optimize::RunOptimize() {
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Optimize Step\n";
    std::cout << "------------------------------\n";
  }

  m_optimizing = true;
  m_sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_relative_weighting);
  m_sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_relative_weighting);

  if (m_procrustes_interval != 0) {  // Initial registration
    m_procrustes->RunRegistration();
    this->WritePointFiles();
    this->WriteTransformFile();
    this->WriteTransformFiles();

    if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
      // transform cutting planes
      m_sampler->TransformCuttingPlanes(m_distribution_domain_id);
    }
  }

  // Set up the minimum variance decay
  m_sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);

  m_sampler->GetDisentangledEnsembleEntropyFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);

  m_sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);
  m_sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);

  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);

  m_sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
      m_starting_regularization, m_ending_regularization,
      m_optimization_iterations - m_optimization_iterations_completed);

  m_sampler->SetCorrespondenceOn();

  if (m_use_disentangled_ssm) {
    if (m_starting_regularization == m_ending_regularization)
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::DisentangledEnsembleMeanEnergy);
    else
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::DisentagledEnsembleEntropy);
  } else if ((m_attributes_per_domain.size() > 0 &&
              *std::max_element(m_attributes_per_domain.begin(), m_attributes_per_domain.end()) > 0) ||
             m_mesh_based_attributes) {
    SW_DEBUG("Setting correspondence mode: mesh-based general entropy");
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeshBasedGeneralEntropy);
  } else if (m_use_regression == true) {
    if (m_use_mixed_effects == true) {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleMixedEffectsEntropy);
    } else {
      m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleRegressionEntropy);
    }
  } else if (m_starting_regularization == m_ending_regularization) {
    SW_DEBUG("Setting correspondence mode: mean energy");
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeanEnergy);  // mean force
  } else {
    SW_DEBUG("Setting correspondence mode: ensemble energy");
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::EnsembleEntropy);
  }

  if (m_sampler->GetParticleSystem()->GetNumberOfDomains() == 1) {
    // where there is only one sample/domain, we must use mean force since there is no correspondence
    m_sampler->SetCorrespondenceMode(shapeworks::CorrespondenceMode::MeanEnergy);  // mean force
  }

  if (m_optimization_iterations - m_optimization_iterations_completed > 0) {
    m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_optimization_iterations -
                                                            m_optimization_iterations_completed);
  } else {
    m_sampler->GetOptimizer()->SetMaximumNumberOfIterations(0);
  }

  m_energy_a.clear();
  m_energy_b.clear();
  m_total_energy.clear();
  m_str_energy = "opt";

  m_sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_sampler->GetOptimizer()->SetTolerance(0.0);
  m_sampler->Execute();

  this->WritePointFiles();
  this->WritePointFilesWithFeatures();
  this->WriteEnergyFiles();
  this->WriteTransformFile();
  this->WriteTransformFiles();
  this->WriteModes();
  this->WriteCuttingPlanePoints();
  this->WriteParameters();
  if (m_verbosity_level > 0) {
    std::cout << "Finished optimization!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::OptimizerStop() { m_sampler->GetOptimizer()->StopOptimization(); }

//---------------------------------------------------------------------------
void Optimize::AbortOptimization() {
  this->m_aborted = true;
  m_sampler->GetOptimizer()->AbortProcessing();
}

//---------------------------------------------------------------------------
void Optimize::IterateCallback(itk::Object*, const itk::EventObject&) {
  if (this->iteration_callback_) {
    this->iteration_callback_();
  }

  this->m_iteration_count++;

  for (int d = 0; d < m_domains_per_shape; d++) {
    current_particle_iterations_ += m_sampler->GetParticleSystem()->GetNumberOfParticles(d);
  }

  if (this->GetShowVisualizer()) {
    this->GetVisualizer().IterationCallback(m_sampler->GetParticleSystem());
  }

  this->ComputeEnergyAfterIteration();

  if (m_optimizing && m_procrustes_interval != 0) {
    m_procrustes_counter++;

    if (m_procrustes_counter >= (int)m_procrustes_interval) {
      m_procrustes_counter = 0;
      m_procrustes->RunRegistration();

      if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
        // transform cutting planes
        m_sampler->TransformCuttingPlanes(m_distribution_domain_id);
      }
    }
  }

  // Checkpointing after procrustes
  if (m_checkpointing_interval != 0) {
    m_checkpoint_counter++;
    if (m_checkpoint_counter == (int)m_checkpointing_interval) {
      m_checkpoint_counter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WriteTransformFiles();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();

      if (m_keep_checkpoints) {
        this->WritePointFiles(this->GetCheckpointDir());
        this->WritePointFilesWithFeatures(this->GetCheckpointDir());
        this->WriteTransformFile(this->GetCheckpointDir() + "/transform");
        this->WriteTransformFiles(this->GetCheckpointDir());
        this->WriteParameters(this->GetCheckpointDir());
      }
    }
  }
  UpdateProgress();
}

//---------------------------------------------------------------------------
void Optimize::ComputeEnergyAfterIteration() {
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
      } else {
        sampEnergy += m_sampler->GetLinkingFunction()->EnergyA(j, i, m_sampler->GetParticleSystem());
      }
      if (m_sampler->GetCorrespondenceMode() == shapeworks::CorrespondenceMode::MeanEnergy) {
        corrEnergy += m_sampler->GetLinkingFunction()->EnergyB(j, i, m_sampler->GetParticleSystem());
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

// File writers and info display functions

//---------------------------------------------------------------------------
void Optimize::PrintParamInfo() {
  if (m_verbosity_level < 2) {
    return;
  }

  // Write out the parameters
  std::cout << "---------------------" << std::endl;
  std::cout << "   I/O parameters" << std::endl;
  std::cout << "---------------------" << std::endl << std::endl;

  std::cout << "Domains per shape = " << m_domains_per_shape << std::endl;
  std::cout << m_filenames.size() << " image files provided!!!" << std::endl;

  if (m_domain_flags.size() > 0) {
    std::cout << "Following " << m_domain_flags.size() << " domains have been declared fixed: " << std::endl;

    for (int i = 0; i < m_domain_flags.size(); i++) {
      std::cout << m_domain_flags[i] << "\t" << m_filenames[m_domain_flags[i]] << std::endl;
    }

    std::cout << std::endl;
  }

  std::cout << "Target number of particles = ";
  if (m_domains_per_shape == 1) {
    std::cout << m_number_of_particles[0];
  } else {
    for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
      std::cout << "domain " << i << " : " << m_number_of_particles[i] << ", ";
    }
  }
  std::cout << std::endl;

  if (m_particle_flags.size() > 0) {
    std::cout << "Total " << m_particle_flags.size() / 2 << " particles have been declared fixed." << std::endl;
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
  std::cout << "Initialization";
  std::cout << ", Optimization";
  std::cout << std::endl;

  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_optimization_iterations_completed = " << m_optimization_iterations_completed << std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_use_shape_statistics_in_init = " << m_use_shape_statistics_in_init << std::endl;
  std::cout << "m_procrustes_interval = " << m_procrustes_interval << std::endl;
  std::cout << "m_procrustes_scaling = " << m_procrustes_scaling << std::endl;
  std::cout << "m_relative_weighting = " << m_relative_weighting << std::endl;
  std::cout << "m_initial_relative_weighting = " << m_initial_relative_weighting << std::endl;
  std::cout << "m_starting_regularization = " << m_starting_regularization << std::endl;
  std::cout << "m_ending_regularization = " << m_ending_regularization << std::endl;
  std::cout << "m_recompute_regularization_interval = " << m_recompute_regularization_interval << std::endl;
  std::cout << "m_save_init_splits = " << m_save_init_splits << std::endl;
  std::cout << "m_checkpointing_interval = " << m_checkpointing_interval << std::endl;
  std::cout << "m_keep_checkpoints = " << m_keep_checkpoints << std::endl;

  std::cout << std::endl;

  if (m_perform_good_bad) {
    std::cout << "Debug: Bad particles will be reported during optimization, expect significant delays!!! "
              << std::endl;
  }

  if (m_log_energy) {
    std::cout << "Debug: Write energy logs, might increase runtime!!! " << std::endl;
  }
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFile(int iter) const {
  if (!this->m_file_output_enabled) {
    return;
  }

  std::string output_file = m_output_dir + "/" + m_output_transform_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    std::stringstream ssp;
    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();  // size from domain 0
    output_file = m_output_dir + "/iter" + ss.str() + "_p" + ssp.str() + "/" + m_output_transform_file;
  }
  this->WriteTransformFile(output_file);
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFile(std::string iter_prefix) const {
  if (!this->m_file_output_enabled) {
    return;
  }

  std::string output_file = iter_prefix;

  std::vector<ParticleSystem::TransformType> tlist;

  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    tlist.push_back(m_sampler->GetParticleSystem()->GetTransform(i));
  }

  std::string str = "writing " + output_file + " ...";
  PrintStartMessage(str);
  ObjectWriter<ParticleSystem::TransformType> writer;
  writer.SetFileName(output_file);
  writer.SetInput(tlist);
  writer.Update();
  PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFiles(int iter) const {
  if (!this->m_file_output_enabled || !this->m_output_transform_files) {
    return;
  }
  std::string output_file = m_output_dir + "/";

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    std::stringstream ssp;
    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();  // size from domain 0
    output_file = m_output_dir + "/iter" + ss.str() + "_p" + ssp.str() + "/";
  }
  this->WriteTransformFiles(output_file);
}

//---------------------------------------------------------------------------
void Optimize::WriteTransformFiles(std::string iter_prefix) const {
  if (!this->m_file_output_enabled || !this->m_output_transform_files) {
    return;
  }

  this->PrintStartMessage("Writing transform files...\n");

  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string transform_file = iter_prefix + "/" + m_filenames[i] + ".transform";

    std::ofstream out(transform_file.c_str());

    std::string str = "Writing " + transform_file + "...";
    this->PrintStartMessage(str, 1);
    if (!out) {
      std::cerr << "Error opening output file: " << transform_file << std::endl;
      throw 1;
    }

    auto transform = m_sampler->GetParticleSystem()->GetTransform(i);

    for (int i = 0; i < transform.cols(); i++) {
      for (int j = 0; j < transform.rows(); j++) {
        out << transform(i, j) << " ";
      }
    }
    out << std::endl;

    out.close();
  }
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WritePointFiles(int iter) {
  if (!this->m_file_output_enabled) {
    return;
  }

  int num_digits = std::to_string(abs(this->m_total_iterations)).length();
  std::stringstream ss;
  ss << std::setw(num_digits) << std::setfill('0') << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();  // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >= 0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFiles(tmp_dir_name);
}

//---------------------------------------------------------------------------
void Optimize::WritePointFiles(std::string iter_prefix) {
  if (!this->m_file_output_enabled) {
    return;
  }

  this->PrintStartMessage("Writing point files...\n");
#ifdef _WIN32
  mkdir(iter_prefix.c_str());
#else
  mkdir(iter_prefix.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string local_file = iter_prefix + "/" + m_filenames[i] + "_local." + particle_format_;
    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_world." + particle_format_;

    std::string str = "Writing " + world_file + " and " + local_file + " files...";
    this->PrintStartMessage(str, 1);

    auto ps = m_sampler->GetParticleSystem();

    Eigen::VectorXd local_particles;
    Eigen::VectorXd world_particles;
    int idx = 0;
    local_particles.resize(ps->GetNumberOfParticles(i) * 3);
    world_particles.resize(ps->GetNumberOfParticles(i) * 3);

    for (unsigned int j = 0; j < ps->GetNumberOfParticles(i); j++) {
      auto pos = ps->GetPosition(j, i);
      for (unsigned int k = 0; k < 3; k++) {
        local_particles(idx) = pos[k];
        world_particles(idx) = ps->GetTransformedPosition(j, i)[k];
        idx++;
      }
    }

    particles::write_particles(local_file, local_particles);
    particles::write_particles(world_file, world_particles);

    std::stringstream st;
    st << counter;
    str = "with " + st.str() + "points...";
    this->PrintStartMessage(str, 1);
    this->PrintDoneMessage(1);

  }  // end for files
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WritePointFilesWithFeatures(int iter) {
  if (!this->m_file_output_enabled) {
    return;
  }

  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles();  // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >= 0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFilesWithFeatures(tmp_dir_name);
}

//---------------------------------------------------------------------------
void Optimize::WritePointFilesWithFeatures(std::string iter_prefix) {
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
    int attrNum = 3 * int(m_use_xyz[i % m_domains_per_shape]) + 3 * int(m_use_normals[i % m_domains_per_shape]);
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

    std::vector<float> fVals;

    for (unsigned int j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_sampler->GetParticleSystem()->GetTransformedPosition(j, i);

      for (unsigned int k = 0; k < 3; k++) {
        outw << wpos[k] << " ";
      }

      if (m_use_normals[i % m_domains_per_shape]) {
        vnl_vector_fixed<float, DIMENSION> pG =
            m_sampler->GetParticleSystem()->GetDomain(i)->SampleNormalAtPoint(pos, j);
        VectorType pN;
        pN[0] = pG[0];
        pN[1] = pG[1];
        pN[2] = pG[2];
        pN = m_sampler->GetParticleSystem()->TransformVector(
            pN,
            m_sampler->GetParticleSystem()->GetTransform(i) * m_sampler->GetParticleSystem()->GetPrefixTransform(i));
        outw << pN[0] << " " << pN[1] << " " << pN[2] << " ";
      }

      // TODO: Needs rewrite:
      /*
      // Only run the following code if we are dealing with ImplicitSurfaceDomains
      const ImplicitSurfaceDomain<float>* domain =
          dynamic_cast<const ImplicitSurfaceDomain<float>*>(m_sampler->GetParticleSystem()->GetDomain(i));
      if (domain && m_attributes_per_domain.size() > 0) {
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
      */

      outw << std::endl;

      counter++;
    }  // end for points

    outw.close();
    this->PrintDoneMessage(1);
  }  // end for files
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteEnergyFiles() {
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
void Optimize::WriteSplitFiles(std::string name) {
  std::stringstream ss;
  ss << this->m_split_number;
  std::stringstream ssp;
  std::string dir_name = "split" + ss.str();
  for (int i = 0; i < m_domains_per_shape; i++) {
    ssp << m_sampler->GetParticleSystem()->GetNumberOfParticles(i);
    dir_name += "_" + ssp.str();
    ssp.str("");
  }
  dir_name += name;
  std::string out_path = m_output_dir;
  std::string tmp_dir_name = out_path + "/" + dir_name;

  WritePointFiles(tmp_dir_name);
  WritePointFilesWithFeatures(tmp_dir_name + "/");
  WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
  WriteTransformFiles(tmp_dir_name + "/");
  WriteParameters(tmp_dir_name);
}

//---------------------------------------------------------------------------
void Optimize::WriteCuttingPlanePoints(int iter) {
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

  /*
  for (unsigned int i = 0; i < m_sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_sampler->GetParticleSystem()->GetDomain(i)->GetConstraints()->PrintAll();
  }
  */
  out.close();
  this->PrintDoneMessage(1);
  this->PrintDoneMessage();
}

//---------------------------------------------------------------------------
void Optimize::WriteParameters(std::string output_dir) {
  if (!this->m_file_output_enabled) {
    return;
  }

  if (!m_use_regression) {
    return;
  }

  if (output_dir.empty()) {
    output_dir = m_output_dir;
  }

  std::string slopename = output_dir + std::string("slope");
  std::string interceptname = output_dir + std::string("intercept");

  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector<double> slope;
  std::vector<double> intercept;

  if (m_use_mixed_effects == true) {
    vnl_vector<double> slopevec =
        dynamic_cast<MixedEffectsShapeMatrix*>(m_sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())
            ->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    vnl_vector<double> interceptvec =
        dynamic_cast<MixedEffectsShapeMatrix*>(m_sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())
            ->GetIntercept();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      intercept.push_back(interceptvec[i]);
    }

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << intercept[i] << "\n";
    }
    out.close();

    slopename = output_dir + std::string("sloperand");
    interceptname = output_dir + std::string("interceptrand");

    std::cout << "writing " << slopename << std::endl;
    std::cout << "writing " << interceptname << std::endl;

    vnl_matrix<double> sloperand_mat =
        dynamic_cast<MixedEffectsShapeMatrix*>(m_sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())
            ->GetSlopeRandom();

    out.open(slopename.c_str());
    for (unsigned int i = 0; i < sloperand_mat.rows(); i++) {
      for (unsigned int j = 0; j < sloperand_mat.cols(); j++) {
        out << sloperand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();

    vnl_matrix<double> interceptrand_mat =
        dynamic_cast<MixedEffectsShapeMatrix*>(m_sampler->GetEnsembleMixedEffectsEntropyFunction()->GetShapeMatrix())
            ->GetInterceptRandom();

    out.open(interceptname.c_str());
    for (unsigned int i = 0; i < interceptrand_mat.rows(); i++) {
      for (unsigned int j = 0; j < interceptrand_mat.cols(); j++) {
        out << interceptrand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();
  } else {
    vnl_vector<double> slopevec =
        dynamic_cast<LinearRegressionShapeMatrix*>(m_sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
            ->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    std::vector<double> intercept;
    vnl_vector<double> interceptvec =
        dynamic_cast<LinearRegressionShapeMatrix*>(m_sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
            ->GetIntercept();

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
void Optimize::ReportBadParticles() {
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
std::vector<std::vector<itk::Point<double>>> Optimize::GetLocalPoints() { return this->m_local_points; }

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> Optimize::GetGlobalPoints() { return this->m_global_points; }

//---------------------------------------------------------------------------
void Optimize::SetCutPlanes(std::vector<std::array<itk::Point<double>, 3>> cut_planes) {
  this->m_cut_planes = cut_planes;
}

//---------------------------------------------------------------------------
bool Optimize::GetAborted() { return this->m_aborted; }

//---------------------------------------------------------------------------
void Optimize::UpdateExportablePoints() {
  this->m_local_points.clear();
  this->m_global_points.clear();
  for (size_t d = 0; d < m_sampler->GetParticleSystem()->GetNumberOfDomains(); d++) {
    // blank set of points
    this->m_local_points.push_back(std::vector<itk::Point<double>>());
    this->m_global_points.push_back(std::vector<itk::Point<double>>());

    // for each particle
    for (size_t j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = m_sampler->GetParticleSystem()->GetPosition(j, d);
      auto pos2 = m_sampler->GetParticleSystem()->GetTransformedPosition(j, d);
      this->m_local_points[d].push_back(pos);
      this->m_global_points[d].push_back(pos2);
    }
  }
}

//---------------------------------------------------------------------------
std::vector<std::vector<std::vector<double>>> Optimize::GetProcrustesTransforms() {
  std::vector<std::vector<std::vector<double>>> transforms;

  int num_domains_per_subject = this->GetDomainsPerShape();
  int num_subjects = this->GetNumShapes() / num_domains_per_subject;
  transforms.resize(num_subjects);

  auto ps = this->GetSampler()->GetParticleSystem();

  int subject = 0;
  int domain = 0;
  std::vector<std::vector<double>> subject_transform;
  for (int i = 0; i < this->m_local_points.size(); i++) {  // iterate over all domains

    auto procrustes = ps->GetTransform(i);
    std::vector<double> transform;
    for (int i = 0; i < procrustes.cols(); i++) {
      for (int j = 0; j < procrustes.rows(); j++) {
        transform.push_back(procrustes(i, j));
      }
    }
    subject_transform.push_back(transform);

    domain++;
    if (domain == num_domains_per_subject) {
      transforms[subject] = subject_transform;
      subject++;
      domain = 0;
      subject_transform = std::vector<std::vector<double>>();
    }
  }

  return transforms;
}
//---------------------------------------------------------------------------
void Optimize::UpdateProject() {
  if (!project_) {
    return;
  }

  auto transforms = GetProcrustesTransforms();
  auto& subjects = project_->get_subjects();

  int transform_index = 0;
  for (size_t i = 0; i < subjects.size(); i++) {
    if (!subjects[i]->is_excluded() && transform_index < transforms.size()) {
      subjects[i]->set_procrustes_transforms(transforms[transform_index++]);
    }
  }
}

//---------------------------------------------------------------------------
void Optimize::SetTimePtsPerSubject(int time_pts_per_subject) { this->m_timepts_per_subject = time_pts_per_subject; }

//---------------------------------------------------------------------------
int Optimize::GetTimePtsPerSubject() { return this->m_timepts_per_subject; }

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterations(int optimization_iterations) {
  this->m_optimization_iterations = optimization_iterations;
}

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterationsCompleted(int optimization_iterations_completed) {
  this->m_optimization_iterations_completed = optimization_iterations_completed;
}

//---------------------------------------------------------------------------
void Optimize::SetIterationsPerSplit(int iterations_per_split) { this->m_iterations_per_split = iterations_per_split; }

//---------------------------------------------------------------------------
void Optimize::SetUseShapeStatisticsInInit(bool use_shape_statistics_in_init) {
  this->m_use_shape_statistics_in_init = use_shape_statistics_in_init;
}

//---------------------------------------------------------------------------
void Optimize::SetProcrustesInterval(int procrustes_interval) { this->m_procrustes_interval = procrustes_interval; }

//---------------------------------------------------------------------------
void Optimize::SetProcrustesScaling(bool procrustes_scaling) { this->m_procrustes_scaling = procrustes_scaling; }

//---------------------------------------------------------------------------
void Optimize::SetProcrustesRotationTranslation(bool procrustes_rotation_translation) {
  this->m_procrustes_rotation_translation = procrustes_rotation_translation;
}

//---------------------------------------------------------------------------
void Optimize::SetRelativeWeighting(double relative_weighting) { this->m_relative_weighting = relative_weighting; }

//---------------------------------------------------------------------------
void Optimize::SetInitialRelativeWeighting(double initial_relative_weighting) {
  this->m_initial_relative_weighting = initial_relative_weighting;
}

//---------------------------------------------------------------------------
void Optimize::SetStartingRegularization(double starting_regularization) {
  this->m_starting_regularization = starting_regularization;
}

//---------------------------------------------------------------------------
void Optimize::SetEndingRegularization(double ending_regularization) {
  this->m_ending_regularization = ending_regularization;
}

//---------------------------------------------------------------------------
void Optimize::SetRecomputeRegularizationInterval(int recompute_regularization_interval) {
  this->m_recompute_regularization_interval = recompute_regularization_interval;
}

//---------------------------------------------------------------------------
void Optimize::SetSaveInitSplits(bool save_init_splits) { this->m_save_init_splits = save_init_splits; }

//---------------------------------------------------------------------------
void Optimize::SetCheckpointingInterval(int checkpointing_interval) {
  this->m_checkpointing_interval = checkpointing_interval;
}

//---------------------------------------------------------------------------
void Optimize::SetKeepCheckpoints(int keep_checkpoints) { this->m_keep_checkpoints = keep_checkpoints; }

//---------------------------------------------------------------------------
void Optimize::SetUseRegression(bool use_regression) { this->m_use_regression = use_regression; }

//---------------------------------------------------------------------------
void Optimize::SetUseMixedEffects(bool use_mixed_effects) { this->m_use_mixed_effects = use_mixed_effects; }

//---------------------------------------------------------------------------
void Optimize::SetUseDisentangledSpatiotemporalSSM(bool use_disentangled_ssm) {
  this->m_use_disentangled_ssm = use_disentangled_ssm;
}

//---------------------------------------------------------------------------
bool Optimize::GetUseDisentangledSpatiotemporalSSM() { return this->m_use_disentangled_ssm; }

//---------------------------------------------------------------------------
void Optimize::SetNormalAngle(double normal_angle) { this->m_normal_angle = normal_angle; }

//---------------------------------------------------------------------------
void Optimize::SetPerformGoodBad(bool perform_good_bad) { this->m_perform_good_bad = perform_good_bad; }

//---------------------------------------------------------------------------
void Optimize::SetLogEnergy(bool log_energy) { this->m_log_energy = log_energy; }

//---------------------------------------------------------------------------
void Optimize::AddImage(ImageType::Pointer image, std::string name) {
  m_sampler->AddImage(image, this->GetNarrowBand(), name);
  this->m_num_shapes++;
  if (image) {
    this->m_spacing = image->GetSpacing()[0] * 5;
  }
}

//---------------------------------------------------------------------------
void Optimize::AddMesh(vtkSmartPointer<vtkPolyData> poly_data) {
  if (poly_data == nullptr) {
    // fixed domain
    m_sampler->AddMesh(nullptr);
  } else {
    double geodesic_remesh_percent = m_geodesics_enabled ? m_geodesic_remesh_percent : 100;

    const auto mesh =
        std::make_shared<shapeworks::Surface>(poly_data, m_geodesics_enabled, m_geodesic_cache_size_multiplier);
    m_sampler->AddMesh(mesh, geodesic_remesh_percent);
  }
  this->m_num_shapes++;
  this->m_spacing = 0.5;
}

//---------------------------------------------------------------------------
void Optimize::AddContour(vtkSmartPointer<vtkPolyData> poly_data) {
  m_sampler->AddContour(poly_data);
  m_num_shapes++;
  m_spacing = 0.5;
}

//---------------------------------------------------------------------------
void Optimize::SetFilenames(const std::vector<std::string>& filenames) { this->m_filenames = filenames; }

//---------------------------------------------------------------------------
void Optimize::SetPointFiles(const std::vector<std::string>& point_files) {
  for (int shapeCount = 0; shapeCount < point_files.size(); shapeCount++) {
    m_sampler->SetPointsFile(shapeCount, point_files[shapeCount]);
  }
}

//---------------------------------------------------------------------------
void Optimize::SetInitialPoints(std::vector<std::vector<itk::Point<double>>> initial_points) {
  m_sampler->SetInitialPoints(initial_points);
}

//---------------------------------------------------------------------------
int Optimize::GetNumShapes() { return this->m_num_shapes; }

//---------------------------------------------------------------------------
shapeworks::OptimizationVisualizer& Optimize::GetVisualizer() { return visualizer_; }

//---------------------------------------------------------------------------
void Optimize::SetShowVisualizer(bool show) {
  if (show && this->m_verbosity_level > 0) {
    std::cout << "WARNING Using the visualizer will increase run time!\n";
  }
  this->show_visualizer_ = show;
}

//---------------------------------------------------------------------------
bool Optimize::GetShowVisualizer() { return this->show_visualizer_; }

//---------------------------------------------------------------------------
void Optimize::SetAttributeScales(const std::vector<double>& scales) { m_sampler->SetAttributeScales(scales); }

//---------------------------------------------------------------------------
void Optimize::SetFieldAttributes(const std::vector<std::string>& field_attributes) {
  m_sampler->SetFieldAttributes(field_attributes);
}

//---------------------------------------------------------------------------
void Optimize::SetParticleFlags(std::vector<int> flags) { this->m_particle_flags = flags; }

//---------------------------------------------------------------------------
void Optimize::SetFixedDomains(std::vector<int> flags) {
  if (flags.size() > 0) {
    // Fixed domains are in use.
    this->m_fixed_domains_present = true;
  }
  this->m_domain_flags = flags;
}

//---------------------------------------------------------------------------
const std::vector<int>& Optimize::GetDomainFlags() { return this->m_domain_flags; }

//---------------------------------------------------------------------------
void Optimize::SetFileOutputEnabled(bool enabled) { this->m_file_output_enabled = enabled; }

//---------------------------------------------------------------------------
std::vector<bool> Optimize::GetUseXYZ() { return this->m_use_xyz; }

//---------------------------------------------------------------------------
std::vector<bool> Optimize::GetUseNormals() { return this->m_use_normals; }

//---------------------------------------------------------------------------
void Optimize::SetNarrowBand(double v) {
  this->m_narrow_band_set = true;
  this->m_narrow_band = v;
}

//---------------------------------------------------------------------------
double Optimize::GetNarrowBand() {
  if (this->m_fixed_domains_present) {
    return 1e10;
  }

  if (this->m_narrow_band_set) {
    return this->m_narrow_band;
  }

  return 4.0;
}

//---------------------------------------------------------------------------
void Optimize::SetUseShapeStatisticsAfter(int num_particles) { this->m_use_shape_statistics_after = num_particles; }

//---------------------------------------------------------------------------
int Optimize::GetUseShapeStatisticsAfter() { return this->m_use_shape_statistics_after; }

//---------------------------------------------------------------------------
void Optimize::SetIterationCallback() {
  itk::MemberCommand<Optimize>::Pointer m_iterate_command = itk::MemberCommand<Optimize>::New();
  m_iterate_command->SetCallbackFunction(this, &Optimize::IterateCallback);
  m_sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), m_iterate_command);
}

//---------------------------------------------------------------------------
void Optimize::WriteModes() {
  const int n = m_sampler->GetParticleSystem()->GetNumberOfDomains() % m_domains_per_shape;
  if (n >= 5) {
    m_sampler->GetEnsembleEntropyFunction()->WriteModes(m_output_dir + "/pts", 5);
  }
}

//---------------------------------------------------------------------------
void Optimize::PrintStartMessage(std::string str, unsigned int vlevel) const {
  if (this->m_verbosity_level > vlevel) {
    std::cout << str;
    std::cout.flush();
  }
}

//---------------------------------------------------------------------------
void Optimize::PrintDoneMessage(unsigned int vlevel) const {
  if (m_verbosity_level > vlevel) {
    std::cout << "Done." << std::endl;
  }
}

//---------------------------------------------------------------------------
bool Optimize::LoadParameterFile(std::string filename) {
  OptimizeParameterFile param;
  if (!param.load_parameter_file(filename, this)) {
    std::cerr << "Error reading parameter file\n";
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
bool Optimize::SetUpOptimize(ProjectHandle project) {
  OptimizeParameters param(project);
  param.set_up_optimize(this);
  SetProject(project);
  return true;
}
//---------------------------------------------------------------------------
void Optimize::SetProject(std::shared_ptr<Project> project) { project_ = project; }

//---------------------------------------------------------------------------
MatrixContainer Optimize::GetParticleSystem() {
  auto shape_matrix = m_sampler->GetGeneralShapeMatrix();

  MatrixType matrix;
  matrix.resize(shape_matrix->rows(), shape_matrix->cols());

  for (int i = 0; i < shape_matrix->rows(); i++) {
    for (int j = 0; j < shape_matrix->cols(); j++) {
      matrix(i, j) = shape_matrix->get(i, j);
    }
  }

  MatrixContainer container;
  container.matrix_ = matrix;
  return container;
}

//---------------------------------------------------------------------------
std::string Optimize::GetCheckpointDir() {
  int num_digits = std::to_string(abs(m_total_iterations)).length();
  std::stringstream ss;
  ss << std::setw(num_digits) << std::setfill('0')  // set leading zeros
     << m_iteration_count + m_optimization_iterations_completed;

  int num_particles = this->m_number_of_particles[0];  // size from domain 0
  num_digits = std::to_string(num_particles).length();
  std::stringstream ssp;
  ssp << std::setw(num_digits) << std::setfill('0')  // set leading zeros
      << m_sampler->GetParticleSystem()->GetNumberOfParticles();

  std::string suffix = "_init";
  if (this->m_optimizing) {
    suffix = "_opt";
  }

  std::string out_path = m_output_dir + "/checkpoints";

#ifdef _WIN32
  mkdir(out_path.c_str());
#else
  mkdir(out_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

  out_path = out_path + "/p" + ssp.str() + suffix + "_iter" + ss.str();

  return out_path;
}

//---------------------------------------------------------------------------
void Optimize::SetPythonFile(std::string filename) { this->m_python_filename = filename; }

//---------------------------------------------------------------------------
void Optimize::SetGeodesicsEnabled(bool is_enabled) { this->m_geodesics_enabled = is_enabled; }

//---------------------------------------------------------------------------
void Optimize::SetGeodesicsCacheSizeMultiplier(size_t n) { this->m_geodesic_cache_size_multiplier = n; }

//---------------------------------------------------------------------------
void Optimize::SetGeodesicsRemeshPercent(double percent) { m_geodesic_remesh_percent = percent; }

//---------------------------------------------------------------------------
vnl_vector_fixed<double, 3> Optimize::TransformPoint(int domain, vnl_vector_fixed<double, 3> input) {
  // If initial transform provided, transform cutting plane points
  if (GetPrefixTransformFile() == "" || GetTransformFile() == "") {
    return input;
  }

  ParticleSystem::PointType pa;

  pa[0] = input[0];
  pa[1] = input[1];
  pa[2] = input[2];

  ParticleSystem::TransformType T = GetSampler()->GetParticleSystem()->GetTransform(domain);
  ParticleSystem::TransformType prefT = GetSampler()->GetParticleSystem()->GetPrefixTransform(domain);
  pa = GetSampler()->GetParticleSystem()->TransformPoint(pa, T * prefT);

  vnl_vector_fixed<double, 3> output;

  output[0] = pa[0];
  output[1] = pa[1];
  output[2] = pa[2];
  return output;
}

//---------------------------------------------------------------------------
void Optimize::SetSharedBoundaryEnabled(bool enabled) { m_sampler->SetSharedBoundaryEnabled(enabled); }

//---------------------------------------------------------------------------
void Optimize::SetSharedBoundaryWeight(double weight) { m_sampler->SetSharedBoundaryWeight(weight); }

//---------------------------------------------------------------------------
void Optimize::ComputeTotalIterations() {
  total_particle_iterations_ = 0;

  int highest_particle_count = *std::max_element(m_number_of_particles.begin(), m_number_of_particles.end());

  int number_of_splits = static_cast<int>(std::log2(static_cast<double>(highest_particle_count)));

  int num_particles = 2;
  for (int i = 0; i < number_of_splits; i++) {
    for (int d = 0; d < m_number_of_particles.size(); d++) {
      double add = m_iterations_per_split * std::min(num_particles, m_number_of_particles[d]);
      total_particle_iterations_ += add;
    }

    if (m_use_shape_statistics_after > 0 && num_particles >= m_use_shape_statistics_after) {
      for (int d = 0; d < m_number_of_particles.size(); d++) {
        total_particle_iterations_ += m_optimization_iterations * std::min(num_particles, m_number_of_particles[d]);
      }
    }

    num_particles *= 2;
  }

  if (m_use_shape_statistics_after <= 0) {
    for (int d = 0; d < m_number_of_particles.size(); d++) {
      double add = m_optimization_iterations * std::min(num_particles, m_number_of_particles[d]);
      total_particle_iterations_ += add;
    }
  }

  m_total_iterations = (number_of_splits * m_iterations_per_split) + m_optimization_iterations;

  if (this->m_verbosity_level > 0) {
    SW_LOG("Total number of iterations: {}", m_total_iterations);
  }
}

//---------------------------------------------------------------------------
void Optimize::UpdateProgress() {
  auto now = std::chrono::system_clock::now();

  bool final = current_particle_iterations_ == total_particle_iterations_;

  if (final || (now - m_last_update_time) > std::chrono::milliseconds(100)) {
    m_last_update_time = now;
    std::chrono::duration<double> elapsed_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);
    double seconds_per_iteration = elapsed_seconds.count() / current_particle_iterations_;
    double seconds_remaining = seconds_per_iteration * (total_particle_iterations_ - current_particle_iterations_);
    int hours = static_cast<int>(seconds_remaining / 3600);
    int minutes = static_cast<int>((seconds_remaining - (hours * 3600)) / 60);
    int seconds = static_cast<int>(seconds_remaining - (hours * 3600) - (minutes * 60));

    std::string message;
    if (m_optimizing) {
      message = "Optimizing";
    } else {
      message = "Initializing";
    }

    int stage_num_iterations = m_sampler->GetOptimizer()->GetNumberOfIterations();
    int stage_total_iterations = m_sampler->GetOptimizer()->GetMaximumNumberOfIterations();
    int num_particles = m_sampler->GetParticleSystem()->GetNumberOfParticles(0);

    message = fmt::format("{} : Particles: {}, Iteration: {} / {}", message, num_particles, stage_num_iterations,
                          stage_total_iterations);

    if ((now - m_last_remaining_update_time) > std::chrono::seconds(1)) {
      m_remaining_time_message = fmt::format("({:02d}:{:02d}:{:02d} remaining)", hours, minutes, seconds);
      m_last_remaining_update_time = now;
    }

    // only show the time remaining if it's been more than 3 seconds
    if (elapsed_seconds > std::chrono::seconds(3)) {
      message = fmt::format("{} {}", message, m_remaining_time_message);
    }

    double progress =
        static_cast<double>(current_particle_iterations_) * 100 / static_cast<double>(total_particle_iterations_);
    SW_PROGRESS(progress, message);
  }
}
}  // namespace shapeworks
