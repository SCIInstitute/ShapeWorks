
#include "OptimizeParameters.h"
#include "Optimize.h"
#include <Libs/Image/Image.h>
#include <Libs/Utils/StringUtils.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
OptimizeParameters::OptimizeParameters(ProjectHandle project)
{
  this->project_ = project;
  this->params_ = this->project_->get_parameters(Parameters::OPTIMIZE_PARAMS);
}

//---------------------------------------------------------------------------
void OptimizeParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::OPTIMIZE_PARAMS, this->params_);
}

//---------------------------------------------------------------------------
std::vector<int> OptimizeParameters::get_number_of_particles()
{
  return this->params_.get("number_of_particles", {128});
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_number_of_particles(std::vector<int> number_of_particles)
{
  return this->params_.set("number_of_particles", number_of_particles);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_initial_relative_weighting()
{
  return this->params_.get("initial_relative_weighting", 0.1);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_initial_relative_weighting(double value)
{
  this->params_.set("initial_relative_weighting", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_relative_weighting()
{
  return this->params_.get("relative_weighting", 1.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_relative_weighting(double value)
{
  this->params_.set("relative_weighting", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_starting_regularization()
{
  return this->params_.get("starting_regularization", 10.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_starting_regularization(double value)
{
  this->params_.set("starting_regularization", value);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_ending_regularization()
{
  return this->params_.get("ending_regularization", 1.0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_ending_regularization(double value)
{
  this->params_.set("ending_regularization", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_iterations_per_split()
{
  return this->params_.get("iterations_per_split", 1000);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_iterations_per_split(int value)
{
  this->params_.set("iterations_per_split", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_optimization_iterations()
{
  return this->params_.get("optimization_iterations", 1000);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_optimization_iterations(int value)
{
  this->params_.set("optimization_iterations", value);
}

//---------------------------------------------------------------------------
std::vector<bool> OptimizeParameters::get_use_normals()
{
  std::vector<bool> use_normals = this->params_.get("use_normals", {false});
  if (use_normals.empty()) use_normals.push_back(false);
  return use_normals;
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_normals(std::vector<bool> use_normals)
{
  this->params_.set("use_normals", use_normals);
}

//---------------------------------------------------------------------------
double OptimizeParameters::get_normals_strength()
{
  return this->params_.get("normals_strength", 10);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_normals_strength(double value)
{
  this->params_.set("normals_strength", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_procrustes()
{
  return this->params_.get("procrustes", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_procrustes(bool value)
{
  this->params_.set("procrustes", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_procrustes_scaling()
{
  return this->params_.get("procrustes_scaling", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_procrustes_scaling(bool value)
{
  this->params_.set("procrustes_scaling", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_procrustes_interval()
{
  return this->params_.get("procrustes_interval", 0);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_procrustes_interval(int value)
{
  this->params_.set("procrustes_interval", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::get_use_multiscale()
{
  return this->params_.get("multiscale", false);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_use_multiscale(bool value)
{
  this->params_.set("multiscale", value);
}

//---------------------------------------------------------------------------
int OptimizeParameters::get_multiscale_particles()
{
  return this->params_.get("multiscale_particles", 32);
}

//---------------------------------------------------------------------------
void OptimizeParameters::set_multiscale_particles(int value)
{
  this->params_.set("multiscale_particles", value);
}

//---------------------------------------------------------------------------
bool OptimizeParameters::set_up_optimize(Optimize* optimize)
{

  optimize->SetDomainsPerShape(1); /// only one domain per shape right now
  optimize->SetNumberOfParticles(this->get_number_of_particles());
  optimize->SetInitialRelativeWeighting(this->get_initial_relative_weighting());
  optimize->SetRelativeWeighting(this->get_relative_weighting());
  optimize->SetStartingRegularization(this->get_starting_regularization());
  optimize->SetEndingRegularization(this->get_ending_regularization());
  optimize->SetIterationsPerSplit(this->get_iterations_per_split());
  optimize->SetOptimizationIterations(this->get_optimization_iterations());

  std::vector<bool> use_normals;
  std::vector<bool> use_xyz;
  std::vector<double> attr_scales;

  attr_scales.push_back(1);
  attr_scales.push_back(1);
  attr_scales.push_back(1);

  if (this->get_use_normals()[0]) {
    use_normals.push_back(1);
    use_xyz.push_back(1);
    double normals_strength = this->get_normals_strength();
    attr_scales.push_back(normals_strength);
    attr_scales.push_back(normals_strength);
    attr_scales.push_back(normals_strength);
  }
  else {
    use_normals.push_back(0);
    use_xyz.push_back(0);
  }
  optimize->SetUseNormals(use_normals);
  optimize->SetUseXYZ(use_xyz);
  optimize->SetUseMeshBasedAttributes(this->get_use_normals()[0]);
  optimize->SetAttributeScales(attr_scales);

  std::vector<int> attributes_per_domain;
  optimize->SetAttributesPerDomain(attributes_per_domain);

  int procrustes_interval = 0;
  if (this->get_use_procrustes()) {
    procrustes_interval = this->get_procrustes_interval();
  }
  optimize->SetProcrustesInterval(procrustes_interval);
  optimize->SetProcrustesScaling(this->get_use_procrustes_scaling());
  optimize->SetVerbosity(0);

  int multiscale_particles = 0;
  if (this->get_use_multiscale()) {
    multiscale_particles = this->get_multiscale_particles();
  }
  optimize->SetUseShapeStatisticsAfter(multiscale_particles);


  // should add the images last
  auto subjects = this->project_->get_subjects();

  if (subjects.empty()) {
    throw std::invalid_argument("No subjects to optimize");
  }

  std::vector<std::string> filenames;
  for (auto s : subjects) {
    auto files = s->get_groomed_filenames();
    if (files.empty()) {
      throw std::invalid_argument("No groomed inputs for optimization");
    }
    auto filename = files[0];
    auto domain_type = s->get_domain_types()[0];
    filenames.push_back(filename);

    if (domain_type == DomainType::Mesh) {
      auto trimesh = std::shared_ptr<TriMesh>(TriMesh::read(filename.c_str()));
      if (trimesh) {
        optimize->AddMesh(std::make_shared<shapeworks::TriMeshWrapper>(trimesh));
      }
      else {
        throw std::invalid_argument("Error loading mesh: " + filename);
      }
    }
    else {
      Image image(filename);
      optimize->AddImage(image);
    }

    auto name = StringUtils::getFileNameWithoutExtension(filename);
    s->set_global_particle_filename(name + "_world.particles");
    s->set_local_particle_filename(name + "_local.particles");
  }

  optimize->SetOutputDir(".");
  optimize->SetFilenames(StringUtils::getFileNamesFromPaths(filenames));
  optimize->SetOutputTransformFile("transform");

  return true;
}
