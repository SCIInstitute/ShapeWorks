/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   File:      Optimize.cpp

   Copyright (c) 2020 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/

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
#include <itkImageFileReader.h>
#include <itkMultiThreaderBase.h>
#include <itkZeroCrossingImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkMacro.h>

// vtk
#include <vtkContourFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkMassProperties.h>

// particle system
#include "TriMesh.h"
#include "itkImageToVTKImageFilter.h"
#include "itkParticleImageDomain.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImplicitSurfaceDomain.h"
#include "itkParticleImageDomainWithHessians.h"
#include "object_reader.h"
#include "object_writer.h"

#include <Optimize.h>

//---------------------------------------------------------------------------
Optimize::Optimize()
{
  this->m_Sampler = itk::MaximumEntropyCorrespondenceSampler<ImageType>::New();
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

  m_disable_procrustes = true;
  m_disable_checkpointing = true;
  // Initialize
  if (m_processing_mode >= 0) { this->Initialize();}
  // Introduce adaptivity
  if (m_processing_mode >= 1 || m_processing_mode == -1) { this->AddAdaptivity();}
  // Optimize
  if (m_processing_mode >= 2 || m_processing_mode == -2) { this->RunOptimize();}

  return true;
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
  this->m_Sampler->SetDomainsPerShape(this->m_domains_per_shape);   // must be done first!
  this->m_Sampler->SetTimeptsPerIndividual(this->m_timepts_per_subject);
  this->m_Sampler->GetParticleSystem()->SetDomainsPerShape(this->m_domains_per_shape);
  this->m_Sampler->SetVerbosity(this->m_verbosity_level);

  if (this->m_use_xyz.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_Sampler->SetXYZ(i, this->m_use_xyz[i]);
    }
  }
  else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_Sampler->SetXYZ(i, false);
    }
  }

  if (this->m_use_normals.size() > 0) {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_Sampler->SetNormals(i, this->m_use_normals[i]);
    }
  }
  else {
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      this->m_Sampler->SetNormals(i, false);
    }
  }

  // Set up the procrustes registration object.
  this->m_Procrustes = itk::ParticleProcrustesRegistration < 3 > ::New();
  this->m_Procrustes->SetParticleSystem(this->m_Sampler->GetParticleSystem());
  this->m_Procrustes->SetDomainsPerShape(this->m_domains_per_shape);

  if (this->m_procrustes_scaling == 0) {
    this->m_Procrustes->ScalingOff();
  }
  else {
    this->m_Procrustes->ScalingOn();
  }

  this->SetIterationCommand();
  this->startMessage("Initializing variables...");
  this->InitializeSampler();
  this->doneMessage();

  if (m_use_normals.size() > 0) {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      if (m_use_normals[i % m_domains_per_shape]) {
        continue;
      }

      itk::ParticleImageDomainWithHessians<float, 3>* domainWithHess =
        static_cast < itk::ParticleImageDomainWithHessians<float,
                                                           3>*> (m_Sampler->GetParticleSystem()
                                                                 ->GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }
  else {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      itk::ParticleImageDomainWithHessians<float, 3>* domainWithHess =
        static_cast < itk::ParticleImageDomainWithHessians < float,
                                                             3 >
                      * > (m_Sampler->GetParticleSystem()->GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }

  if (m_domain_flags.size() > 0) {
    for (int i = 0; i < m_domain_flags.size(); i++) {
      itk::ParticleImageDomainWithHessians<float, 3>* domainWithHess =
        static_cast < itk::ParticleImageDomainWithHessians < float,
                                                             3 >
                      * > (m_Sampler->GetParticleSystem()->GetDomain(m_domain_flags[i]));
      if (m_use_normals.size() > 0) {
        if (m_use_normals[i % m_domains_per_shape]) {
          domainWithHess->DeletePartialDerivativeImages();
        }
        else {
          domainWithHess->DeleteImages();
        }
      }
    }
  }

  if (m_verbosity_level > 1) {
    this->PrintParamInfo();
  }

  m_GoodBad = itk::ParticleGoodBadAssessment<float, 3>::New();
  m_GoodBad->SetDomainsPerShape(m_domains_per_shape);
  m_GoodBad->SetCriterionAngle(m_normalAngle);
  m_GoodBad->SetPerformAssessment(m_performGoodBad);

  m_EnergyA.clear();
  m_EnergyB.clear();
  m_TotalEnergy.clear();

  // Now read the transform file if present.
  if (m_transform_file != "") { this->ReadTransformFile(); }
  if (m_prefix_transform_file != "") { this->ReadPrefixTransformFile(m_prefix_transform_file); }
}

//---------------------------------------------------------------------------
Optimize::~Optimize() {}

//---------------------------------------------------------------------------
void Optimize::SetVerbosity(int verbosity_level)
{
  this->m_verbosity_level = verbosity_level;
}

//---------------------------------------------------------------------------
void Optimize::SetDomainsPerShape(int domains_per_shape)
{
  this->m_domains_per_shape = domains_per_shape;
  this->m_Sampler->SetDomainsPerShape(this->m_domains_per_shape);
}

//---------------------------------------------------------------------------
int Optimize::GetDomainsPerShape()
{
  return this->m_domains_per_shape;
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
    this->m_Sampler->RegisterGeneralShapeMatrices();
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
  this->m_Sampler->SetAttributesPerDomain(attributes_per_domain);
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
void Optimize::SetCuttingPlane(unsigned int i, const vnl_vector_fixed<double, 3> &va,
                               const vnl_vector_fixed<double, 3> &vb,
                               const vnl_vector_fixed<double, 3> &vc)
{
  this->m_Sampler->SetCuttingPlane(i, va, vb, vc);
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
  object_reader < itk::ParticleSystem < 3 > ::TransformType > reader;
  reader.SetFileName(m_transform_file);
  reader.Update();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_Sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
void Optimize::ReadPrefixTransformFile(const std::string &fn)
{
  object_reader < itk::ParticleSystem < 3 > ::TransformType > reader;
  reader.SetFileName(fn.c_str());
  reader.Update();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    m_Sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
  }
}

//---------------------------------------------------------------------------
// Initialization and Optimization
void Optimize::InitializeSampler()
{
  float nbhd_to_sigma = 3.0;   // 3.0 -> 1.0
  float flat_cutoff = 0.3;   // 0.3 -> 0.85

  m_Sampler->SetPairwisePotentialType(m_pairwise_potential_type);

  m_Sampler->GetGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetCurvatureGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);
  m_Sampler->GetCurvatureGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);
  m_Sampler->GetQualifierGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetQualifierGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetModifiedCotangentGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetModifiedCotangentGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetOmegaGradientFunction()->SetFlatCutoff(flat_cutoff);
  m_Sampler->GetOmegaGradientFunction()->SetNeighborhoodToSigmaRatio(nbhd_to_sigma);

  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVariance(
    m_starting_regularization);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVariance(m_starting_regularization);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetRecomputeCovarianceInterval(1);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetHoldMinimumVariance(false);

  m_Sampler->GetOptimizer()->SetTimeStep(1.0);

  if (m_optimizer_type == 0) {
    m_Sampler->GetOptimizer()->SetModeToJacobi();
  }
  else if (m_optimizer_type == 1) {
    m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  }
  else {
    m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  }

  m_Sampler->SetSamplingOn();

  m_Sampler->SetCorrespondenceOn();

  m_Sampler->SetAdaptivityMode(m_adaptivity_mode);
  m_Sampler->GetEnsembleEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleRegressionEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);
  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()
  ->SetRecomputeCovarianceInterval(m_recompute_regularization_interval);

  m_Sampler->Initialize();

  m_Sampler->GetOptimizer()->SetTolerance(0.0);
}

//---------------------------------------------------------------------------
double Optimize::GetMinNeighborhoodRadius()
{
  double rad = 0.0;
  typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {

    const itk::ParticleImageDomain < float,
                                     3 >* domain = static_cast < const itk::ParticleImageDomain < float,
                                                                                                  3 >
                                                                 * > (m_Sampler->GetParticleSystem()
                                                                      ->GetDomain(i));

    itk2vtkConnector = itk::ImageToVTKImageFilter < ImageType > ::New();
    itk2vtkConnector->SetInput(domain->GetImage());
    vtkSmartPointer < vtkContourFilter > ls = vtkSmartPointer < vtkContourFilter > ::New();
    ls->SetInputData(itk2vtkConnector->GetOutput());
    ls->SetValue(0, 0.0);
    ls->Update();
    vtkSmartPointer < vtkMassProperties > mp = vtkSmartPointer < vtkMassProperties > ::New();
    mp->SetInputData(ls->GetOutput());
    mp->Update();
    double area = mp->GetSurfaceArea();
    double sigma =
      std::sqrt(area / (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    if (rad < sigma) {
      rad = sigma;
    }
  }
  return rad;
}

//---------------------------------------------------------------------------
void Optimize::AddSinglePoint()
{
  typedef itk::ParticleSystem < 3 > ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++) {
    if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) > 0) {
      continue;
    }

    bool done = false;

    ImageType::Pointer img = dynamic_cast < itk::ParticleImageDomain < float, 3 >* > (
      m_Sampler->GetParticleSystem()->GetDomain(i))->GetImage();

    itk::ZeroCrossingImageFilter < ImageType, ImageType > ::Pointer zc =
      itk::ZeroCrossingImageFilter < ImageType, ImageType > ::New();
    zc->SetInput(img);
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex < ImageType > it(zc->GetOutput(),
                                                            zc->GetOutput()->GetRequestedRegion());

    for (it.GoToReverseBegin(); !it.IsAtReverseEnd() && done == false; --it) {
      if (it.Get() == 1.0) {
        PointType pos;
        img->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        done = true;
        try
        {
          m_Sampler->GetParticleSystem()->AddPosition(pos, i);
        } catch (itk::ExceptionObject &) {
          done = false;
        }
      }
    }
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
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) > 10) {
        m_Procrustes->RunRegistration(i);
      }
    }
    this->WritePointFiles();
    this->WriteTransformFile();
  }
  m_disable_procrustes = true;

  m_Sampler->GetParticleSystem()->SynchronizePositions();

  m_Sampler->GetCurvatureGradientFunction()->SetRho(0.0);
  m_Sampler->GetOmegaGradientFunction()->SetRho(0.0);

  m_Sampler->SetCorrespondenceOn();

  if (m_use_shape_statistics_in_init) {
    if (m_attributes_per_domain.size() > 0 &&
        *std::max_element(m_attributes_per_domain.begin(), m_attributes_per_domain.end()) > 0) {
      if (m_mesh_based_attributes) {
        m_Sampler->SetCorrespondenceMode(5);
      }
      else {
        m_Sampler->SetCorrespondenceMode(2);
      }
    }
    else {
      if (m_mesh_based_attributes) {
        m_Sampler->SetCorrespondenceMode(5);
      }
      else {
        m_Sampler->SetCorrespondenceMode(1);
      }
    }

    m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                     m_ending_regularization,
                                                                     m_iterations_per_split);

    m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
      m_starting_regularization,
      m_ending_regularization,
      m_iterations_per_split);
  }
  else {
    // force to mean
    if ((m_attributes_per_domain.size() > 0 &&
         *std::max_element(m_attributes_per_domain.begin(),
                           m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
      m_Sampler->SetCorrespondenceMode(6);
    }
    else {
      m_Sampler->SetCorrespondenceMode(0);
    }
  }

  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  this->AddSinglePoint();

  m_Sampler->GetParticleSystem()->SynchronizePositions();

  int split_number = 0;

  int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  vnl_vector_fixed < double, 3 > random;
  srand(1);
  for (int i = 0; i < 3; i++) {
    random[i] = static_cast < double > (rand());
  }
  double norm = random.magnitude();
  random /= norm;

  double epsilon = this->m_spacing;
  bool flag_split = false;

  for (int i = 0; i < n; i++) {
    int d = i % m_domains_per_shape;
    if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
      flag_split = true;
      break;
    }
  }

  while (flag_split) {
    //        m_Sampler->GetEnsembleEntropyFunction()->PrintShapeMatrix();
    this->optimize_stop();
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        m_Sampler->GetParticleSystem()->SplitAllParticlesInDomain(random, epsilon, i, 0);
      }
    }

    m_Sampler->GetParticleSystem()->SynchronizePositions();

    split_number++;

    if (m_verbosity_level > 0) {
      std::cout << "split number = " << split_number << std::endl;

      std::cout << std::endl << "Particle count: ";
      for (unsigned int i = 0; i < this->m_domains_per_shape; i++) {
        std::cout << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) << "  ";
      }
      std::cout << std::endl;
    }

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << split_number;

      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_wo_opt";
      std::string out_path = m_output_dir;

      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(split_number);
    }

    m_EnergyA.clear();
    m_EnergyB.clear();
    m_TotalEnergy.clear();
    std::stringstream ss;
    ss << split_number;
    std::stringstream ssp;

    ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    m_strEnergy = "split" + ss.str();

    for (int i = 0; i < m_domains_per_shape; i++) {
      ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
      m_strEnergy += "_" + ssp.str();
      ssp.str("");
    }
    m_strEnergy += "pts_init";

    if (this->m_pairwise_potential_type == 1) {
      this->SetCotanSigma();

      double minRad = 3.0 * this->GetMinNeighborhoodRadius();

      m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
      m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
        minRad);
    }

    m_SaturationCounter = 0;
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
    m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
    m_Sampler->Modified();
    m_Sampler->Update();

    if (m_save_init_splits == true) {
      std::stringstream ss;
      ss << split_number;
      std::stringstream ssp;
      std::string dir_name = "split" + ss.str();
      for (int i = 0; i < m_domains_per_shape; i++) {
        ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles(i);
        dir_name += "_" + ssp.str();
        ssp.str("");
      }
      dir_name += "pts_w_opt";
      std::string out_path = m_output_dir;
      std::string tmp_dir_name = out_path + "/" + dir_name;

      this->WritePointFiles(tmp_dir_name + "/");
      this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      this->WriteParameters(split_number);
    }
    this->WritePointFiles();
    this->WritePointFilesWithFeatures();
    this->WriteEnergyFiles();
    this->WriteTransformFile();

    flag_split = false;
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
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

  if (m_adaptivity_strength == 0.0) { return;}
  m_disable_checkpointing = true;
  m_disable_procrustes = true;

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();
  }

  double minRad = 3.0 * this->GetMinNeighborhoodRadius();

  m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
  m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);

  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_initial_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_initial_relative_weighting);

  m_SaturationCounter = 0;
  m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_iterations_per_split);
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->Modified();
  m_Sampler->Update();

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
  m_Sampler->GetCurvatureGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetOmegaGradientFunction()->SetRho(m_adaptivity_strength);
  m_Sampler->GetLinkingFunction()->SetRelativeGradientScaling(m_relative_weighting);
  m_Sampler->GetLinkingFunction()->SetRelativeEnergyScaling(m_relative_weighting);

  if (this->m_pairwise_potential_type == 1) {
    this->SetCotanSigma();

    double minRad = 3.0 * this->GetMinNeighborhoodRadius();

    m_Sampler->GetModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(minRad);
    m_Sampler->GetConstrainedModifiedCotangentGradientFunction()->SetMinimumNeighborhoodRadius(
      minRad);
  }

  m_disable_checkpointing = false;
  m_disable_procrustes = false;

  if (m_procrustes_interval != 0) { // Initial registration
    m_Procrustes->RunRegistration();
    this->WritePointFiles();
    this->WriteTransformFile();

    if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
      // transform cutting planes
      m_Sampler->TransformCuttingPlanes(m_distribution_domain_id);
    }
  }

  if (m_optimizer_type == 0) {
    m_Sampler->GetOptimizer()->SetModeToJacobi();
  }
  else if (m_optimizer_type == 1) {
    m_Sampler->GetOptimizer()->SetModeToGaussSeidel();
  }
  else {
    m_Sampler->GetOptimizer()->SetModeToAdaptiveGaussSeidel();
  }

  // Set up the minimum variance decay
  m_Sampler->GetEnsembleEntropyFunction()->SetMinimumVarianceDecay(m_starting_regularization,
                                                                   m_ending_regularization,
                                                                   m_optimization_iterations -
                                                                   m_optimization_iterations_completed);

  m_Sampler->GetMeshBasedGeneralEntropyGradientFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);
  m_Sampler->GetEnsembleRegressionEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->SetMinimumVarianceDecay(
    m_starting_regularization,
    m_ending_regularization,
    m_optimization_iterations -
    m_optimization_iterations_completed);

  m_Sampler->SetCorrespondenceOn();

  if ((m_attributes_per_domain.size() > 0 &&
       *std::max_element(m_attributes_per_domain.begin(),
                         m_attributes_per_domain.end()) > 0) || m_mesh_based_attributes) {
    m_Sampler->SetCorrespondenceMode(5);
  }
  else if (m_use_regression == true) {
    if (m_use_mixed_effects == true) {
      m_Sampler->SetCorrespondenceMode(4);       // MixedEffects
    }
    else {
      m_Sampler->SetCorrespondenceMode(3);       // Regression
    }
  }
  else if (m_starting_regularization == m_ending_regularization) {
    m_Sampler->SetCorrespondenceMode(0);     // mean force
  }
  else {
    m_Sampler->SetCorrespondenceMode(1);     // Ensemble Entropy
  }

  if (m_optimization_iterations - m_optimization_iterations_completed > 0) {
    m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(
      m_optimization_iterations - m_optimization_iterations_completed);
  }
  else { m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(0);}

  m_EnergyA.clear();
  m_EnergyB.clear();
  m_TotalEnergy.clear();
  m_strEnergy = "opt";

  m_SaturationCounter = 0;
  m_Sampler->GetOptimizer()->SetNumberOfIterations(0);
  m_Sampler->GetOptimizer()->SetTolerance(0.0);
  m_Sampler->Modified();
  m_Sampler->Update();

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
void Optimize::optimize_start()
{
  m_Sampler->GetOptimizer()->StartOptimization();
}

void Optimize::optimize_stop()
{
  m_Sampler->GetOptimizer()->StopOptimization();
}

void Optimize::IterateCallback(itk::Object*, const itk::EventObject &)
{
  if (m_performGoodBad == true) {
    std::vector < std::vector < int >> tmp;
    tmp = m_GoodBad->RunAssessment(m_Sampler->GetParticleSystem(),
                                   m_Sampler->GetMeanCurvatureCache());

    if (!tmp.empty()) {
      if (this->m_badIds.empty()) {
        this->m_badIds.resize(m_domains_per_shape);
      }

      for (int i = 0; i < m_domains_per_shape; i++) {
        for (int j = 0; j < tmp[i].size(); j++) {
          if (m_badIds[i].empty()) {
            this->m_badIds[i].push_back(tmp[i][j]);
          }
          else {
            if (std::count(m_badIds[i].begin(), m_badIds[i].end(), tmp[i][j]) == 0) {
              this->m_badIds[i].push_back(tmp[i][j]);
            }
          }
        }
      }
    }
    ReportBadParticles();
  }

  this->ComputeEnergyAfterIteration();

  int lnth = m_TotalEnergy.size();
  if (lnth > 1) {
    double val = std::abs(m_TotalEnergy[lnth - 1] - m_TotalEnergy[lnth - 2]) / std::abs(
      m_TotalEnergy[lnth - 2]);
    if ((m_optimizing == false && val < m_init_criterion) ||
        (m_optimizing == true && val < m_opt_criterion)) {
      m_SaturationCounter++;
    }
    else {
      m_SaturationCounter = 0;
    }
    if (m_SaturationCounter > 10) {
      if (m_verbosity_level > 2) {
        std::cout << " \n ----Early termination due to minimal energy decay---- \n";
      }
      this->optimize_stop();
    }
  }

  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {
    m_CheckpointCounter++;
    if (m_CheckpointCounter == (int)m_checkpointing_interval) {
      m_CheckpointCounter = 0;

      this->WritePointFiles();
      this->WriteTransformFile();
      this->WritePointFilesWithFeatures();
      this->WriteModes();
      this->WriteParameters();
      this->WriteEnergyFiles();
    }
  }

  if (m_optimizing == false) { return;}

  if (m_procrustes_interval != 0 && m_disable_procrustes == false) {
    m_ProcrustesCounter++;

    if (m_ProcrustesCounter >= (int)m_procrustes_interval) {
      m_ProcrustesCounter = 0;
      m_Procrustes->RunRegistration();

      if (m_use_cutting_planes == true && m_distribution_domain_id > -1) {
        // transform cutting planes
        m_Sampler->TransformCuttingPlanes(m_distribution_domain_id);
      }
    }
  }

  static unsigned int iteration_no = 0;
  // Checkpointing after procrustes (override for optimizing step)
  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {

    m_CheckpointCounter++;

    if (m_CheckpointCounter == (int)m_checkpointing_interval) {
      iteration_no += m_checkpointing_interval;
      m_CheckpointCounter = 0;

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

void Optimize::ComputeEnergyAfterIteration()
{
  int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  double corrEnergy = 0.0;

  double sampEnergy = 0.0;
  for (int i = 0; i < numShapes; i++) {
    m_Sampler->GetLinkingFunction()->SetDomainNumber(i);
    for (int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      if (m_Sampler->GetParticleSystem()->GetDomainFlag(i)) {
        sampEnergy += 0.0;
      }
      else {
        sampEnergy +=
          m_Sampler->GetLinkingFunction()->EnergyA(j, i, m_Sampler->GetParticleSystem());
      }
      if (m_Sampler->GetCorrespondenceMode() == 0) {
        corrEnergy +=
          m_Sampler->GetLinkingFunction()->EnergyB(j, i, m_Sampler->GetParticleSystem());
      }
    }
  }

  if (m_Sampler->GetCorrespondenceMode() > 0) {
    corrEnergy = m_Sampler->GetLinkingFunction()->EnergyB(0, 0, m_Sampler->GetParticleSystem());
  }

  double totalEnergy = sampEnergy + corrEnergy;
  m_EnergyA.push_back(sampEnergy);
  m_EnergyB.push_back(corrEnergy);
  m_TotalEnergy.push_back(totalEnergy);
  if (m_verbosity_level > 2) {
    std::cout << "Energy: " << totalEnergy << std::endl;
  }
}

void Optimize::SetCotanSigma()
{
  typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
  m_Sampler->GetModifiedCotangentGradientFunction()->ClearGlobalSigma();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    const itk::ParticleImageDomain < float,
                                     3 >* domain = static_cast < const itk::ParticleImageDomain < float,
                                                                                                  3 >
                                                                 * > (m_Sampler->GetParticleSystem()
                                                                      ->GetDomain(i));

    itk2vtkConnector = itk::ImageToVTKImageFilter < ImageType > ::New();
    itk2vtkConnector->SetInput(domain->GetImage());
    vtkSmartPointer < vtkContourFilter > ls = vtkSmartPointer < vtkContourFilter > ::New();
    ls->SetInputData(itk2vtkConnector->GetOutput());
    ls->SetValue(0, 0.0);
    ls->Update();
    vtkSmartPointer < vtkMassProperties > mp = vtkSmartPointer < vtkMassProperties > ::New();
    mp->SetInputData(ls->GetOutput());
    mp->Update();
    double area = mp->GetSurfaceArea();
    double sigma = m_cotan_sigma_factor *
                   std::sqrt(area /
                             (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) * M_PI));
    m_Sampler->GetModifiedCotangentGradientFunction()->SetGlobalSigma(sigma);
  }
}

// File writers and info display functions

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

  std::cout << "optimizer_type = ";
  if (m_optimizer_type == 0) {
    std::cout << "jacobi";
  }
  else if (m_optimizer_type == 1) {
    std::cout << "gauss seidel";
  }
  else if (m_optimizer_type == 2) {
    std::cout << "adaptive gauss seidel (with bad moves)";
  }
  else {
    std::cerr << "Incorrect option!!";
    throw 1;
  }
  std::cout << std::endl;

  std::cout << "m_optimization_iterations = " << m_optimization_iterations << std::endl;
  std::cout << "m_optimization_iterations_completed = " << m_optimization_iterations_completed <<
    std::endl;
  std::cout << "m_iterations_per_split = " << m_iterations_per_split << std::endl;
  std::cout << "m_init_criterion = " << m_init_criterion << std::endl;
  std::cout << "m_opt_criterion = " << m_opt_criterion << std::endl;
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

  if (m_performGoodBad) {
    std::cout <<
      "Debug: Bad particles will be reported during optimization, expect significant delays!!! " <<
      std::endl;
  }

  if (m_logEnergy) {
    std::cout << "Debug: Write energy logs, might increase runtime!!! " << std::endl;
  }
}

void Optimize::WriteTransformFile(int iter) const
{
  std::string output_file = m_output_dir + "/" + m_output_transform_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    std::stringstream ssp;
    ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();     // size from domain 0
    output_file = m_output_dir + "/iter" + ss.str() + "_p" + ssp.str() + "/" +
                  m_output_transform_file;
  }
  this->WriteTransformFile(output_file);
}

void Optimize::WriteTransformFile(std::string iter_prefix) const
{
  std::string output_file = iter_prefix;

  std::vector < itk::ParticleSystem < 3 > ::TransformType > tlist;

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++) {
    tlist.push_back(m_Sampler->GetParticleSystem()->GetTransform(i));
  }

  std::string str = "writing " + output_file + " ...";
  startMessage(str);
  object_writer < itk::ParticleSystem < 3 > ::TransformType > writer;
  writer.SetFileName(output_file);
  writer.SetInput(tlist);
  writer.Update();
  doneMessage();
}

void Optimize::WritePointFiles(int iter)
{
  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFiles(tmp_dir_name);
}

void Optimize::WritePointFiles(std::string iter_prefix)
{
  this->startMessage("Writing point files...\n");
#ifdef _WIN32
  mkdir(iter_prefix.c_str());
#else
  mkdir(iter_prefix.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

  int counter;
  for (int i = 0; i < n; i++) {
    counter = 0;

    std::string local_file = iter_prefix + "/" + m_filenames[i] + "_local.particles";
    std::string world_file = iter_prefix + "/" + m_filenames[i] + "_world.particles";

    std::ofstream out(local_file.c_str());
    std::ofstream outw(world_file.c_str());

    std::string str = "Writing " + world_file + " and " + local_file + " files...";
    this->startMessage(str, 1);
    if (!out) {
      std::cerr << "Error opening output file: " << local_file << std::endl;
      throw 1;
    }
    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    for (unsigned int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);

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
    this->startMessage(str, 1);
    this->doneMessage(1);
  }   // end for files
  this->doneMessage();
}

void Optimize::WritePointFilesWithFeatures(int iter)
{
  std::stringstream ss;
  ss << iter + m_optimization_iterations_completed;

  std::stringstream ssp;
  ssp << m_Sampler->GetParticleSystem()->GetNumberOfParticles();   // size from domain 0

  std::string out_path = m_output_dir;
  std::string tmp_dir_name = iter >=
                             0 ? out_path + "/iter" + ss.str() + "_p" + ssp.str() : out_path;

  this->WritePointFilesWithFeatures(tmp_dir_name);
}

void Optimize::WritePointFilesWithFeatures(std::string iter_prefix)
{
  if (!m_mesh_based_attributes) {
    return;
  }

  this->startMessage("Writing point with attributes files...\n");
  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

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
    this->startMessage(str, 1);

    if (!outw) {
      std::cerr << "Error opening output file: " << world_file << std::endl;
      throw 1;
    }

    const itk::ParticleImplicitSurfaceDomain < float, 3 >* domain
      = static_cast < const itk::ParticleImplicitSurfaceDomain < float,
                                                                 3 >* > (m_Sampler->
                                                                         GetParticleSystem()->
                                                                         GetDomain(i));

    const itk::ParticleImageDomainWithGradients < float, 3 >* domainWithGrad
      = static_cast < const itk::ParticleImageDomainWithGradients < float,
                                                                    3 >* > (m_Sampler->
                                                                            GetParticleSystem()->
                                                                            GetDomain(i));

    TriMesh* ptr;
    std::vector < float > fVals;
    if (m_mesh_based_attributes && m_attributes_per_domain.size() > 0) {
      if (m_attributes_per_domain[i % m_domains_per_shape] > 0) {
        ptr = domain->GetMesh();
      }
    }

    for (unsigned int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++) {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);

      for (unsigned int k = 0; k < 3; k++) {
        outw << wpos[k] << " ";
      }

      if (m_use_normals[i % m_domains_per_shape]) {
//                if (m_Sampler->GetParticleSystem()->GetDomainFlag(i))
//                {
//                    outw << 0.0 << " " << 0.0 << " " << 0.0 << " ";
//                }
//                else
//                {
        typename itk::ParticleImageDomainWithGradients < float,
                                                         3 > ::VnlVectorType pG =
          domainWithGrad->SampleNormalVnl(pos);
        VectorType pN;
        pN[0] = pG[0]; pN[1] = pG[1]; pN[2] = pG[2];
        pN = m_Sampler->GetParticleSystem()->TransformVector(pN,
                                                             m_Sampler->GetParticleSystem()->GetTransform(
                                                               i) * m_Sampler->GetParticleSystem()->GetPrefixTransform(
                                                               i));
        outw << pN[0] << " " << pN[1] << " " << pN[2] << " ";
//                }
      }

      if (m_attributes_per_domain.size() > 0) {
        if (m_attributes_per_domain[i % m_domains_per_shape] > 0) {
//                    if (m_Sampler->GetParticleSystem()->GetDomainFlag(i))
//                    {
//                        for (unsigned int k = 0; k < m_attributes_per_domain[i % m_domains_per_shape]; k++)
//                            outw << 0.0 << " ";
//                    }
//                    else
//                    {
          point pt;
          pt.clear();
          pt[0] = pos[0];
          pt[1] = pos[1];
          pt[2] = pos[2];
          fVals.clear();
          ptr->GetFeatureValues(pt, fVals);
          for (unsigned int k = 0; k < m_attributes_per_domain[i % m_domains_per_shape]; k++) {
            outw << fVals[k] << " ";
          }
//                    }
        }
      }

      outw << std::endl;

      counter++;
    }      // end for points

    outw.close();
    this->doneMessage(1);
  }   // end for files
  this->doneMessage();
}

void Optimize::WriteEnergyFiles()
{
  if (!this->m_logEnergy) {
    return;
  }
  this->startMessage("Writing energy files...\n");
  std::string strA = m_output_dir + "/" + this->m_strEnergy + "_samplingEnergy.txt";
  std::string strB = m_output_dir + "/" + this->m_strEnergy + "_correspondenceEnergy.txt";
  std::string strTotal = m_output_dir + "/" + this->m_strEnergy + "_totalEnergy.txt";
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

  int n = m_EnergyA.size() - 1;
  n = n < 0 ? 0 : n;

  std::string str = "Appending to " + strA + " ...";
  this->startMessage(str, 1);
  outA << m_EnergyA[n] << std::endl;
  outA.close();
  this->doneMessage(1);

  str = "Appending to " + strB + " ...";
  this->startMessage(str, 1);
  outB << m_EnergyB[n] << std::endl;
  outB.close();
  this->doneMessage(1);

  str = "Appending to " + strTotal + " ...";
  this->startMessage(str, 1);
  outTotal << m_TotalEnergy[n] << std::endl;
  outTotal.close();
  this->doneMessage(1);

  this->doneMessage();
}

void Optimize::WriteCuttingPlanePoints(int iter)
{
  this->startMessage("Writing cutting plane points...\n");
  std::string output_file = m_output_cutting_plane_file;

  if (iter >= 0) {
    std::stringstream ss;
    ss << iter + m_optimization_iterations_completed;
    output_file = "./iter" + ss.str() + "/" + output_file;
  }

  std::ofstream out(output_file.c_str());

  std::string str = "Writing " + output_file + "...";
  this->startMessage(str, 1);

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    const itk::ParticleImplicitSurfaceDomain < float, 3 >* dom
      = static_cast < const itk::ParticleImplicitSurfaceDomain < float
                                                                 , 3 >* > (m_Sampler->
                                                                           GetParticleSystem()->
                                                                           GetDomain(i));

    for (unsigned int j = 0; j < dom->GetNumberOfPlanes(); j++) {
      vnl_vector_fixed < double, 3 > a = dom->GetA(j);
      vnl_vector_fixed < double, 3 > b = dom->GetB(j);
      vnl_vector_fixed < double, 3 > c = dom->GetC(j);

      for (int d = 0; d < 3; d++) {
        out << a[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << b[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << c[d] << " ";
      }
      out << std::endl;
    }
  }
  out.close();
  this->doneMessage(1);
  this->doneMessage();
}

void Optimize::WriteParameters(int iter)
{
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

  std::vector < double > slope;
  std::vector < double > intercept;

  if (m_use_mixed_effects == true) {
    vnl_vector < double > slopevec = dynamic_cast < itk::ParticleShapeMixedEffectsMatrixAttribute <
                                                      double, 3 >* >
                                     (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->
                                      GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    vnl_vector < double > interceptvec = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute < double,
                                                      3 >* >
                                         (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->
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

    vnl_matrix < double > sloperand_mat = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute < double,
                                                      3 >* >
                                          (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->
                                           GetShapeMatrix())->GetSlopeRandom();

    out.open(slopename.c_str());
    for (unsigned int i = 0; i < sloperand_mat.rows(); i++) {
      for (unsigned int j = 0; j < sloperand_mat.cols(); j++) {
        out << sloperand_mat.get(i, j) << " ";
      }
      out << "\n";
    }
    out.close();

    vnl_matrix < double > interceptrand_mat = dynamic_cast <
      itk::ParticleShapeMixedEffectsMatrixAttribute <
        double, 3 >* >
                                              (m_Sampler->GetEnsembleMixedEffectsEntropyFunction()->
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
    vnl_vector < double > slopevec = dynamic_cast <
      itk::ParticleShapeLinearRegressionMatrixAttribute < double,
                                                          3 >* >
                                     (m_Sampler->GetEnsembleRegressionEntropyFunction()->
                                      GetShapeMatrix())->GetSlope();

    for (unsigned int i = 0; i < slopevec.size(); i++) {
      slope.push_back(slopevec[i]);
    }

    std::ofstream out(slopename.c_str());
    for (unsigned int i = 0; i < slope.size(); i++) {
      out << slope[i] << "\n";
    }
    out.close();

    std::vector < double > intercept;
    vnl_vector < double > interceptvec = dynamic_cast <
      itk::ParticleShapeLinearRegressionMatrixAttribute < double,
                                                          3 >* >
                                         (m_Sampler->GetEnsembleRegressionEntropyFunction()->
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

void Optimize::ReportBadParticles()
{
  this->startMessage("Reporting bad particles...", 2);
  typedef  itk::MaximumEntropyCorrespondenceSampler < ImageType > ::PointType PointType;
  const int totalDomains = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
  const int numShapes = totalDomains / m_domains_per_shape;
  std::string outDomDir;
  std::string outPtDir;
  if (this->m_badIds.empty()) {
    return;
  }
  for (int i = 0; i < this->m_domains_per_shape; i++) {
    if (this->m_badIds[i].empty()) {
      continue;
    }

    std::stringstream ss;
    ss << i;
    outDomDir = m_output_dir + "/" + this->m_strEnergy + "_BadParticles_domain" + ss.str();
#ifdef _WIN32
    mkdir(outDomDir.c_str());
#else
    mkdir(outDomDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    for (int j = 0; j < this->m_badIds[i].size(); j++) {
      std::stringstream ssj;
      ssj << m_badIds[i][j];
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
        PointType pos = m_Sampler->GetParticleSystem()->GetPosition(m_badIds[i][j], dom);
        for (unsigned int d = 0; d < 3; d++) {
          outl << pos[d] << " ";
        }
        outl << std::endl;
        outl.close();
      }
    }
  }
  this->doneMessage(2);
}

//---------------------------------------------------------------------------
void Optimize::SetPairwisePotentialType(int pairwise_potential_type)
{ this->m_pairwise_potential_type = pairwise_potential_type;}

//---------------------------------------------------------------------------
void Optimize::SetOptimizerType(int optimizer_type)
{ this->m_optimizer_type = optimizer_type;}

//---------------------------------------------------------------------------
void Optimize::SetTimePtsPerSubject(int time_pts_per_subject)
{ this->m_timepts_per_subject = time_pts_per_subject;}

//---------------------------------------------------------------------------
int Optimize::GetTimePtsPerSubject()
{ return this->m_timepts_per_subject; }

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterations(int optimization_iterations)
{ this->m_optimization_iterations = optimization_iterations;}

//---------------------------------------------------------------------------
void Optimize::SetOptimizationIterationsCompleted(int optimization_iterations_completed)
{ this->m_optimization_iterations_completed = optimization_iterations_completed;}

//---------------------------------------------------------------------------
void Optimize::SetIterationsPerSplit(int iterations_per_split)
{ this->m_iterations_per_split = iterations_per_split;}

//---------------------------------------------------------------------------
void Optimize::SetInitCriterion(double init_criterion)
{ this->m_init_criterion = init_criterion;}

//---------------------------------------------------------------------------
void Optimize::SetOptCriterion(double opt_criterion)
{ this->m_opt_criterion = opt_criterion;}

//---------------------------------------------------------------------------
void Optimize::SetUseShapeStatisticsInInit(bool use_shape_statistics_in_init)
{ this->m_use_shape_statistics_in_init = use_shape_statistics_in_init;}

//---------------------------------------------------------------------------
void Optimize::SetProcrustesInterval(int procrustes_interval)
{ this->m_procrustes_interval = procrustes_interval;}

//---------------------------------------------------------------------------
void Optimize::SetProcrustesScaling(int procrustes_scaling)
{ this->m_procrustes_scaling = procrustes_scaling;}

//---------------------------------------------------------------------------
void Optimize::SetRelativeWeighting(double relative_weighting)
{ this->m_relative_weighting = relative_weighting;}

//---------------------------------------------------------------------------
void Optimize::SetInitialRelativeWeighting(double initial_relative_weighting)
{ this->m_initial_relative_weighting = initial_relative_weighting;}

//---------------------------------------------------------------------------
void Optimize::SetStartingRegularization(double starting_regularization)
{ this->m_starting_regularization = starting_regularization;}

//---------------------------------------------------------------------------
void Optimize::SetEndingRegularization(double ending_regularization)
{ this->m_ending_regularization = ending_regularization;}

//---------------------------------------------------------------------------
void Optimize::SetRecomputeRegularizationInterval(int recompute_regularization_interval)
{ this->m_recompute_regularization_interval = recompute_regularization_interval;}

//---------------------------------------------------------------------------
void Optimize::SetSaveInitSplits(bool save_init_splits)
{ this->m_save_init_splits = save_init_splits;}

//---------------------------------------------------------------------------
void Optimize::SetCheckpointingInterval(int checkpointing_interval)
{ this->m_checkpointing_interval = checkpointing_interval;}

//---------------------------------------------------------------------------
void Optimize::SetKeepCheckpoints(int keep_checkpoints)
{ this->m_keep_checkpoints = keep_checkpoints;}

//---------------------------------------------------------------------------
void Optimize::SetCotanSigmaFactor(double cotan_sigma_factor)
{ this->m_cotan_sigma_factor = cotan_sigma_factor;}

//---------------------------------------------------------------------------
void Optimize::SetUseRegression(bool use_regression)
{ this->m_use_regression = use_regression; }

//---------------------------------------------------------------------------
void Optimize::SetUseMixedEffects(bool use_mixed_effects)
{ this->m_use_mixed_effects = use_mixed_effects; }

//---------------------------------------------------------------------------
void Optimize::SetNormalAngle(double normal_angle)
{ this->m_normalAngle = normal_angle;}

//---------------------------------------------------------------------------
void Optimize::SetPerformGoodBad(bool perform_good_bad)
{ this->m_performGoodBad = perform_good_bad;}

//---------------------------------------------------------------------------
void Optimize::SetLogEnergy(bool log_energy)
{ this->m_logEnergy = log_energy;}

//---------------------------------------------------------------------------
void Optimize::SetImages(const std::vector<ImageType::Pointer> &images)
{
  this->m_Sampler->SetImages(images);
  ImageType::Pointer first_image = images[0];
  this->m_Sampler->SetInput(0, first_image);            // set the 0th input
  this->m_spacing = first_image->GetSpacing()[0];
  this->m_num_shapes = images.size();
}

//---------------------------------------------------------------------------
void Optimize::SetFilenames(const std::vector<std::string> &filenames)
{ this->m_filenames = filenames; }

//---------------------------------------------------------------------------
void Optimize::SetPointFiles(const std::vector<std::string> &point_files)
{
  for (int shapeCount = 0; shapeCount < point_files.size(); shapeCount++) {
    this->m_Sampler->SetPointsFile(shapeCount, point_files[shapeCount]);
  }
}

//---------------------------------------------------------------------------
int Optimize::GetNumShapes()
{
  return this->m_num_shapes;
}

//---------------------------------------------------------------------------
void Optimize::SetMeshFiles(const std::vector<std::string> &mesh_files)
{
  m_Sampler->SetMeshFiles(mesh_files);
}

//---------------------------------------------------------------------------
void Optimize::SetAttributeScales(const std::vector<double> &scales)
{
  this->m_Sampler->SetAttributeScales(scales);
}

//---------------------------------------------------------------------------
void Optimize::SetFeaFiles(const std::vector<std::string> &files)
{
  this->m_Sampler->SetFeaFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetFeaGradFiles(const std::vector<std::string> &files)
{
  this->m_Sampler->SetFeaGradFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetFidsFiles(const std::vector<std::string> &files)
{
  this->m_Sampler->SetFidsFiles(files);
}

//---------------------------------------------------------------------------
void Optimize::SetParticleFlags(std::vector<int> flags)
{
  this->m_particle_flags = flags;

  for (unsigned int i = 0; i < flags.size() / 2; i++) {
    this->GetSampler()->GetParticleSystem()->SetFixedParticleFlag(flags[2 * i], flags[2 * i + 1]);
  }
}

//---------------------------------------------------------------------------
void Optimize::SetDomainFlags(std::vector<int> flags)
{
  this->m_domain_flags = flags;

  for (unsigned int i = 0; i < flags.size(); i++) {
    this->GetSampler()->GetParticleSystem()->FlagDomain(flags[i]);
  }
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
