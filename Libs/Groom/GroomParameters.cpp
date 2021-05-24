
#include "GroomParameters.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
GroomParameters::GroomParameters(ProjectHandle project, std::string domain_name) :
  project_(project), domain_name_(domain_name)
{
  this->params_ = this->project_->get_parameters(Parameters::GROOM_PARAMS, this->domain_name_);
}

//---------------------------------------------------------------------------
void GroomParameters::restore_defaults()
{
  this->params_.reset_parameters();
}

//---------------------------------------------------------------------------
bool GroomParameters::get_center_tool()
{
  return this->params_.get("center", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_center_tool(bool value)
{
  this->params_.set("center", value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_isolate_tool()
{
  return this->params_.get("isolate", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_isolate_tool(bool value)
{
  this->params_.set("isolate", value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_fill_holes_tool()
{
  return this->params_.get("fill_holes", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fill_holes_tool(bool value)
{
  this->params_.set("fill_holes", value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_auto_pad_tool()
{
  return this->params_.get("pad", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_auto_pad_tool(bool value)
{
  this->params_.set("pad", value);
}

//---------------------------------------------------------------------------
int GroomParameters::get_padding_amount()
{
  return this->params_.get("pad_value", 10);
}

//---------------------------------------------------------------------------
void GroomParameters::set_padding_amount(int padding_amount)
{
  this->params_.set("pad_value", padding_amount);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_antialias_tool()
{
  return this->params_.get("antialias", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_tool(bool value)
{
  this->params_.set("antialias", value);
}

//---------------------------------------------------------------------------
int GroomParameters::get_antialias_iterations()
{
  return this->params_.get("antialias_amount", 10);
}

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_iterations(int iterations)
{
  this->params_.set("antialias_amount", iterations);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_blur_tool()
{
  return this->params_.get("blur", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_blur_tool(bool value)
{
  this->params_.set("blur", value);
}

//---------------------------------------------------------------------------
double GroomParameters::get_blur_amount()
{
  return this->params_.get("blur_sigma", 2.0);
}

//---------------------------------------------------------------------------
void GroomParameters::set_blur_amount(double blur_amount)
{
  this->params_.set("blur_sigma", blur_amount);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_fast_marching()
{
  return this->params_.get("fastmarching", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fast_marching(bool value)
{
  this->params_.set("fastmarching", value);
}

//---------------------------------------------------------------------------
void GroomParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::GROOM_PARAMS, this->params_, this->domain_name_);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_groom_output_prefix()
{
  return this->params_.get("groom_output_prefix", "groomed");
}

//---------------------------------------------------------------------------
void GroomParameters::set_groom_output_prefix(std::string prefix)
{
  this->params_.set("groom_output_prefix", prefix);
}
