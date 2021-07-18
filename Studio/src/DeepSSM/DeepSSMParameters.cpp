
#include "DeepSSMParameters.h"

namespace shapeworks {

const std::string DeepSSMParameters::DEEPSSM_SAMPLER_GAUSSIAN_C("Gaussian");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_MIXTURE_C("Mixture");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_KDE_C("KDE");

//---------------------------------------------------------------------------
DeepSSMParameters::DeepSSMParameters(ProjectHandle project) : project_(project)
{
  this->params_ = this->project_->get_parameters(Parameters::DEEPSSM_PARAMS);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_defaults()
{
  this->params_.reset_parameters();
}

//---------------------------------------------------------------------------
void DeepSSMParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::DEEPSSM_PARAMS, this->params_);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_samples()
{
  return this->params_.get("aug_num_samples", 3);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_samples(int num_samples)
{
  this->params_.set("aug_num_samples", num_samples);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_dims()
{
  return this->params_.get("aug_num_dims", 3);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_dims(int num_dims)
{
  this->params_.set("aug_num_dims", num_dims);
}

//---------------------------------------------------------------------------
double DeepSSMParameters::get_aug_percent_variability()
{
  return this->params_.get("aug_percent_variability", 0.95);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_percent_variability(double percent_variability)
{
  this->params_.set("aug_percent_variability", percent_variability);
}

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_aug_sampler_type()
{
  return this->params_.get("aug_augmentation_sampler_type", DEEPSSM_SAMPLER_GAUSSIAN_C);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_sampler_type(std::string sampler_type)
{
  this->params_.set("aug_augmentation_sampler_type", sampler_type);
}
};
