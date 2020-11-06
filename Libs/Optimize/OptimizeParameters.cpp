
#include "OptimizeParameters.h"

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
  return this->params_.get("initial_relative_weighting", 1.0);
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
  return this->params_.get("use_normals", {false});
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
