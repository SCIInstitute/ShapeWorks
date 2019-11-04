#include <iostream>
#include <cmath>

#include <vtkContourFilter.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkMassProperties.h>

#include <itkImageToVTKImageFilter.h>

#include <ShapeWorksOptimize.h>

ShapeWorksOptimize::ShapeWorksOptimize()
{
  this->m_Sampler = itk::MaximumEntropyCorrespondenceSampler<itk::Image<float, 3>>::New();
  this->m_Procrustes = itk::ParticleProcrustesRegistration<3>::New();
  this->m_GoodBad = itk::ParticleGoodBadAssessment<float, 3>::New();
}

//---------------------------------------------------------------------------
ShapeWorksOptimize::~ShapeWorksOptimize()
{}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_inputs(std::vector<ImageType::Pointer> inputs)
{
  this->images_ = inputs;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_start_reg(double start_reg)
{
  this->m_starting_regularization = start_reg;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_end_reg(double end_reg)
{
  this->m_ending_regularization = end_reg;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_iterations_per_split(unsigned int iterations_per_split)
{
  this->m_iterations_per_split = iterations_per_split;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_optimization_iterations(unsigned int iterations)
{
  this->m_optimization_iterations = iterations;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_number_of_particles(unsigned int number_of_particles)
{
  // only one domain
  this->m_number_of_particles.clear();
  this->m_number_of_particles.push_back(number_of_particles);
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_procrustes_interval(unsigned procrustes_interval)
{
  this->m_procrustes_interval = procrustes_interval;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_weighting(double weighting)
{
  this->m_relative_weighting = weighting;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_verbose(bool verbose)
{
  this->verbose_ = verbose;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::stop_optimization()
{
  this->aborted_ = true;
  this->m_Sampler->GetOptimizer()->AbortProcessing();
}

//---------------------------------------------------------------------------
bool ShapeWorksOptimize::get_aborted()
{
  return this->aborted_;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::set_cut_planes(std::vector<std::array<itk::Point<double>, 3 >> cutPlanes)
{
  this->cutPlanes_ = cutPlanes;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::run()
{

  std::cerr << "ShapeWorksOptimize::run\n";

  m_CheckpointCounter = 0;
  m_ProcrustesCounter = 0;
  m_SaturationCounter = 0;
  m_disable_procrustes = true;
  m_disable_checkpointing = true;
  m_use_cutting_planes = false;
  m_optimizing = false;
  m_use_regression = false;
  m_use_mixed_effects = false;

  int number_of_splits = static_cast<int>(
    std::log2(static_cast<double>(this->m_number_of_particles[0])) + 1);
  this->iterCount_ = 0;

  this->m_total_iterations = (number_of_splits * this->m_iterations_per_split) +
                             this->m_optimization_iterations;
  std::cerr << "Total number of iterations = " << this->m_total_iterations << "\n";

  // Read parameter file

  ///this->ReadIOParameters(fn);
  ///this->ReadOptimizationParameters(fn);
  ///this->SetDebugParameters(fn);

  //m_Sampler->SetImageFiles(this->m_filenames);
  m_Sampler->SetImages(this->getImages());


  // Set up the optimization process
  m_Sampler->SetDomainsPerShape(m_domains_per_shape); // must be done first!
  m_Sampler->SetTimeptsPerIndividual(m_timepts_per_subject);
  m_Sampler->GetParticleSystem()->SetDomainsPerShape(m_domains_per_shape);

  m_Sampler->SetVerbosity(m_verbosity_level);

  // Set up the procrustes registration object.
  m_Procrustes = itk::ParticleProcrustesRegistration<3>::New();
  m_Procrustes->SetParticleSystem(m_Sampler->GetParticleSystem());
  m_Procrustes->SetDomainsPerShape(m_domains_per_shape);

  if (m_use_xyz.size() > 0) {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetXYZ(i, m_use_xyz[i]);
    }
  }
  else {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetXYZ(i, false);
    }
  }

  if (m_use_normals.size() > 0) {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetNormals(i, m_use_normals[i]);
    }
  }
  else {
    for (int i = 0; i < m_domains_per_shape; i++) {
      m_Sampler->SetNormals(i, false);
    }
  }

  if (m_procrustes_scaling == 0) {
    m_Procrustes->ScalingOff();
  }
  else {
    m_Procrustes->ScalingOn();
  }

  ///this->ReadInputs(fn);
  for (int i = 0; i < this->images_.size(); i++) {
    this->m_Sampler->SetInput(i, this->images_[i]);
  }

  ///this->ReadMeshInputs(fn);
  ///this->ReadConstraints(fn);
  this->SetIterationCommand();
  this->InitializeSampler();
  ///this->ReadExplanatoryVariables(fn);

  ///m_p_flgs.clear();
  ///m_p_flgs = this->FlagParticlesFct(fn);

  ///m_d_flgs.clear();
  ///m_d_flgs = this->FlagDomainFct(fn);

  if (m_use_normals.size() > 0) {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      if (m_use_normals[i % m_domains_per_shape]) {
        continue;
      }

      itk::ParticleImageDomainWithHessians<float,
                                           3>* domainWithHess =
        static_cast< itk::ParticleImageDomainWithHessians<float,
                                                          3>*>(m_Sampler->GetParticleSystem()->
                                                               GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }
  else {
    int numShapes = m_Sampler->GetParticleSystem()->GetNumberOfDomains();
    for (int i = 0; i < numShapes; i++) {
      itk::ParticleImageDomainWithHessians<float,
                                           3>* domainWithHess =
        static_cast< itk::ParticleImageDomainWithHessians<float,
                                                          3>*>(m_Sampler->GetParticleSystem()->
                                                               GetDomain(i));
      domainWithHess->DeletePartialDerivativeImages();
    }
  }

  if (m_d_flgs.size() > 0) {
    for (int i = 0; i < m_d_flgs.size(); i++) {
      itk::ParticleImageDomainWithHessians<float,
                                           3>* domainWithHess =
        static_cast< itk::ParticleImageDomainWithHessians<float,
                                                          3>*>(m_Sampler->GetParticleSystem()->
                                                               GetDomain(m_d_flgs[i]));
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

  m_GoodBad = itk::ParticleGoodBadAssessment<float, 3>::New();
  m_GoodBad->SetDomainsPerShape(m_domains_per_shape);
  m_GoodBad->SetCriterionAngle(m_normalAngle);
  m_GoodBad->SetPerformAssessment(m_performGoodBad);

  m_EnergyA.clear();
  m_EnergyB.clear();
  m_TotalEnergy.clear();

  // Now read the transform file if present.
  ///if (m_transform_file != "") {this->ReadTransformFile();}
  ///if (m_prefix_transform_file != "") {this->ReadPrefixTransformFile(m_prefix_transform_file);}

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

  /// TODO: is this the minimum variance?
  //this->psmFilter_->SetRegularizationDecaySpan(this->decaySpan_);

  m_Sampler->GetOptimizer()->SetTolerance(0.0);

  ///////////////////////////////////

  /// From ::Run()
  ///
  m_disable_procrustes = true;
  m_disable_checkpointing = true;

  this->Initialize();
  this->AddAdaptivity();
  this->Optimize();

  /// this stuff

/*
   this->psmFilter_->SetMaximumNumberOfIterations(this->maxIter_);
   //this->procrustesInterval_.resize(this->numScales_, this->reportInterval_);
   //this->procrustesInterval_.resize(this->numScales_, 1);
   //this->procrustesInterval_.resize(this->numScales_, 0);
   this->psmFilter_->SetShapeEntropyWeighting(this->weighting_);
   //this->psmFilter_->AddCuttingPlane(x(3),y(3),z(3), shape);
   this->totalIters_ = 0;
   for (auto &a : maxIter_) {
    this->totalIters_ += a;
   }
   size_t i = 0;
   this->iterCount_ = 0;
   for (auto &a : this->images_) {
    this->psmFilter_->SetDomainName("item_" + std::to_string(i), i);
    this->psmFilter_->SetInput(i, a);
    if ((this->cutPlanes_.size() == 1) ||
        (this->cutPlanes_.size() == this->images_.size())) {
      vnl_vector_fixed<double, 3> x;
      vnl_vector_fixed<double, 3> y;
      vnl_vector_fixed<double, 3> z;
      size_t which = (this->cutPlanes_.size() == 1) ? 0 : i;
      for (size_t idx = 0; idx < 3; idx++) {
        x[idx] = this->cutPlanes_[which][0][idx];
        y[idx] = this->cutPlanes_[which][1][idx];
        z[idx] = this->cutPlanes_[which][2][idx];
      }
      this->psmFilter_->AddCuttingPlane(x, y, z, "item_" + std::to_string(i));
    }
    i++;
   }
   this->procrustesRegistration_->SetPSMParticleSystem(
    this->psmFilter_->GetParticleSystem());

   this->iterateCmd_ = itk::MemberCommand<ShapeWorksOptimize>::New();
   this->iterateCmd_->SetCallbackFunction(this, &ShapeWorksOptimize::iterateCallback);
   this->psmFilter_->AddObserver(itk::IterationEvent(), this->iterateCmd_);
   auto o = this->psmFilter_->GetOptimizer();
   o->SetModeToGaussSeidel();
   //o->SetModeToAdaptiveGaussSeidel();
   //std::cout << "USING ADAPTIVE TIME STEP !!!!" << std::endl << std::flush;

   if (this->verbose_) {
    std::cout << "maxIter_: ";
    for (unsigned int ii = 0; ii < this->maxIter_.size(); ii++) {
      std::cout << this->maxIter_[ii] << ", ";
    }
    std::cout << std::endl;
    std::cout << "numScales_: " << this->numScales_ << std::endl;
    std::cout << "procrustesCounter_: " << this->procrustesCounter_ << std::endl;
    std::cout << "procrustesInterval_: ";
    for (unsigned int ii = 0; ii < this->procrustesInterval_.size(); ii++) {
      std::cout << this->procrustesInterval_[ii] << ", ";
    }
    std::cout << std::endl;
    std::cout << "reportInterval_: " << this->reportInterval_ << std::endl;
    std::cout << "regularizationFinal_: ";
    for (unsigned int ii = 0; ii < this->regularizationFinal_.size(); ii++) {
      std::cout << this->regularizationFinal_[ii] << ", ";
    }
    std::cout << std::endl;
    std::cout << "regularizationInitial_: ";
    for (unsigned int ii = 0; ii < this->regularizationInitial_.size(); ii++) {
      std::cout << this->regularizationInitial_[ii] << ", ";
    }
    std::cout << std::endl;
    std::cout << "totalIters_: " << this->totalIters_ << std::endl;
    std::cout << "weighting_: " << this->weighting_ << std::endl;
   }

   this->psmFilter_->Update();

   // for each domain
   for (size_t d = 0; d < this->psmFilter_->
       GetParticleSystem()->GetNumberOfDomains(); d++) {

    // blank set of points
    this->localPoints_.push_back(std::vector<itk::Point<double>>());
    this->globalPoints_.push_back(std::vector<itk::Point<double>>());

    // for each particle
    for (size_t j = 0; j < this->psmFilter_->
         GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = this->psmFilter_->GetParticleSystem()->GetPosition(j, d);
      auto pos2 = this->psmFilter_->GetParticleSystem()->GetTransformedPosition(j, d);
      this->localPoints_[d].push_back(pos);
      this->globalPoints_[d].push_back(pos2);
    }
   }

 */

  for (size_t d = 0; d < this->m_Sampler->
       GetParticleSystem()->GetNumberOfDomains(); d++) {

    // blank set of points
    this->localPoints_.push_back(std::vector<itk::Point<double>>());
    this->globalPoints_.push_back(std::vector<itk::Point<double>>());

    // for each particle
    for (size_t j = 0; j < this->m_Sampler->
         GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = this->m_Sampler->GetParticleSystem()->GetPosition(j, d);
      auto pos2 = this->m_Sampler->GetParticleSystem()->GetTransformedPosition(j, d);
      this->localPoints_[d].push_back(pos);
      this->globalPoints_[d].push_back(pos2);
    }
  }
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> ShapeWorksOptimize::localPoints()
{
  return this->localPoints_;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> ShapeWorksOptimize::globalPoints()
{
  return this->globalPoints_;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::Initialize()
{
  if (m_verbosity_level > 0) {
    std::cout << "------------------------------\n";
    std::cout << "*** Initialize Step\n";
    std::cout << "------------------------------\n";
  }

  m_disable_checkpointing = true;
  m_disable_procrustes = true;

  m_disable_procrustes = false;
  m_Procrustes->SetComputeTransformationOn();
  if (m_procrustes_interval != 0) { // Initial registration
    for (int i = 0; i < this->m_domains_per_shape; i++) {
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) > 10) {
        m_Procrustes->RunRegistration(i);
      }
    }
    //this->WritePointFiles();
    //this->WriteTransformFile();
  }
  m_disable_procrustes = true;
  m_Procrustes->SetComputeTransformationOff();

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
  std::cerr << "number of domains = " << n << "\n";
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

  std::cerr << "flag split = " << flag_split << "\n";
  std::cerr << "m_verbosity_level = " << m_verbosity_level << "\n";
  while (flag_split) {

    if (this->aborted_)
    {
      return;
    }

    //        m_Sampler->GetEnsembleEntropyFunction()->PrintShapeMatrix();
    m_Sampler->GetOptimizer()->StopOptimization();

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

      //this->WritePointFiles(tmp_dir_name + "/");
      //this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      //this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      //this->WriteParameters(split_number);
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

      //this->WritePointFiles(tmp_dir_name + "/");
      //this->WritePointFilesWithFeatures(tmp_dir_name + "/");
      //this->WriteTransformFile(tmp_dir_name + "/" + m_output_transform_file);
      //this->WriteParameters(split_number);
    }
    //this->WritePointFiles();
    //this->WritePointFilesWithFeatures();
    //this->WriteEnergyFiles();
    //this->WriteTransformFile();

    flag_split = false;
    for (int i = 0; i < n; i++) {
      int d = i % m_domains_per_shape;
      if (m_Sampler->GetParticleSystem()->GetNumberOfParticles(i) < m_number_of_particles[d]) {
        flag_split = true;
        break;
      }
    }
  }
  //this->WritePointFiles();
  //this->WritePointFilesWithFeatures();
  //this->WriteTransformFile();
  //this->WriteCuttingPlanePoints();
  if (m_verbosity_level > 0) {
    std::cout << "Finished initialization!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::AddAdaptivity()
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

  //this->WritePointFiles();
  //this->WritePointFilesWithFeatures();
  //this->WriteTransformFile();
  //this->WriteCuttingPlanePoints();

  if (m_verbosity_level > 0) {
    std::cout << "Finished adaptivity!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::Optimize()
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
    //this->WritePointFiles();
    //this->WriteTransformFile();

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

  //this->WritePointFiles();
  //this->WritePointFilesWithFeatures();
  //this->WriteEnergyFiles();
  //this->WriteTransformFile();
  //this->WriteModes();
  //this->WriteCuttingPlanePoints();
  //this->WriteParameters();
  if (m_verbosity_level > 0) {
    std::cout << "Finished optimization!!!" << std::endl;
  }
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::AddSinglePoint()
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
void ShapeWorksOptimize::SetCotanSigma()
{
  typename itk::ImageToVTKImageFilter < ImageType > ::Pointer itk2vtkConnector;
  m_Sampler->GetModifiedCotangentGradientFunction()->ClearGlobalSigma();
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains(); i++) {
    const itk::ParticleImageDomain < float, 3 >*
      domain =
      static_cast < const itk::ParticleImageDomain <float, 3>* > (m_Sampler->GetParticleSystem()
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

//---------------------------------------------------------------------------
double ShapeWorksOptimize::GetMinNeighborhoodRadius()
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
void ShapeWorksOptimize::ComputeEnergyAfterIteration()
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

//---------------------------------------------------------------------------
void ShapeWorksOptimize::SetIterationCommand()
{
  this->iterateCmd_ = itk::MemberCommand<ShapeWorksOptimize>::New();
  this->iterateCmd_->SetCallbackFunction(this, &ShapeWorksOptimize::iterateCallback);
  m_Sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), this->iterateCmd_);
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::InitializeSampler()
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
std::vector<ImageType::Pointer> ShapeWorksOptimize::getImages()
{
  return this->images_;
}

//---------------------------------------------------------------------------
void ShapeWorksOptimize::iterateCallback(itk::Object* caller, const itk::EventObject &e)
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
    //ReportBadParticles();
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
      m_Sampler->GetOptimizer()->StopOptimization();
    }
  }

  if (m_checkpointing_interval != 0 && m_disable_checkpointing == false) {
    m_CheckpointCounter++;
    if (m_CheckpointCounter == (int)m_checkpointing_interval) {
      m_CheckpointCounter = 0;

      //this->WritePointFiles();
      //this->WriteTransformFile();
      //this->WritePointFilesWithFeatures();
      //this->WriteModes();
      //this->WriteParameters();
      //this->WriteEnergyFiles();
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

      //this->WritePointFiles();
      //this->WriteTransformFile();
      //this->WritePointFilesWithFeatures();
      //this->WriteModes();
      //this->WriteParameters();
      //this->WriteEnergyFiles();

      if (m_keep_checkpoints) {
//        this->WritePointFiles(iteration_no);
//        this->WritePointFilesWithFeatures(iteration_no);
//        this->WriteTransformFile(iteration_no);
//        this->WriteParameters(iteration_no);
      }
    }
  }

  //// ITK PSM
  /*
     itk::PSMEntropyModelFilter<ImageType>* o =
     reinterpret_cast<itk::PSMEntropyModelFilter<ImageType>*>(caller);

     // Print every 10 iterations
     if (o->GetNumberOfElapsedIterations() % this->reportInterval_ != 0) { return; }
     int interval = 0;
     if (this->verbose_) {
     std::cout << "Iteration # " << o->GetNumberOfElapsedIterations() << std::endl;
     std::cout << " Eigenmode variances: ";
     for (unsigned int i = 0; i < o->GetShapePCAVariances().size(); i++) {
      std::cout << o->GetShapePCAVariances()[i] << " ";
     }
     std::cout << std::endl;
     std::cout << " Regularization = " << o->GetRegularizationConstant() << std::endl;

     // Check if optimization is run using scales. Get Procrustes
     // interval for the current scale.
     std::cout << "Optimization Scale " << (o->GetCurrentScale() + 1) << "/"
              << o->GetNumberOfScales() << std::endl;
     }
     if (o->GetNumberOfScales() > 1) {
     if (this->verbose_) {
      interval = this->procrustesInterval_[o->GetCurrentScale()];
      std::cout << "Interval = " << interval << std::endl;
     }
     }
     else {
     // Use the default interval
     interval = this->procrustesRegistration_->GetProcrustesInterval();
     }

     // Check if the Procrustes interval is set to a value other than 0
     if ((interval > 0) && (o->GetCurrentScale() > 5)) { // only perform procrustes if we have enough particles (e.g. 64)
     this->procrustesCounter_++;

     // If the counter is greater than the interval value, run
     // Procrustes registration
     if (this->procrustesCounter_ >= interval) {
      // Reset the counter
      this->procrustesCounter_ = 0;
      this->procrustesRegistration_->RunRegistration();
      if (this->verbose_) {
        std::cout << "Ran Procrustes Registration" << std::endl;
      }
     }
     }
   */
}
