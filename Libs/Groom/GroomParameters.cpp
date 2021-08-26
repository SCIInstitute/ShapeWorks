
#include "GroomParameters.h"

namespace shapeworks {

const std::string GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C("Laplacian");
const std::string GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C("WindowedSinc");

const std::string GroomParameters::GROOM_ALIGNMENT_CENTER_C("Center");
const std::string GroomParameters::GROOM_ALIGNMENT_ICP_C("Iterative Closest Point");

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

//---------------------------------------------------------------------------
bool GroomParameters::get_mesh_smooth()
{
  return this->params_.get("mesh_smooth", false);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smooth(bool value)
{
  this->params_.set("mesh_smooth", value);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_mesh_smoothing_method()
{
  return this->params_.get("mesh_smoothing_method", GROOM_SMOOTH_VTK_LAPLACIAN_C);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smoothing_method(std::string method)
{
  this->params_.set("mesh_smoothing_method", method);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_laplacian_iterations()
{
  return this->params_.get("mesh_smoothing_vtk_laplacian_iterations", 10);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_iterations(int iterations)
{
  this->params_.set("mesh_smoothing_vtk_laplacian_iterations", iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_laplacian_relaxation()
{
  return this->params_.get("mesh_smoothing_vtk_laplacian_relaxation", 1);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_relaxation(double relaxation)
{
  this->params_.set("mesh_smoothing_vtk_laplacian_relaxation", relaxation);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_windowed_sinc_iterations()
{
  return this->params_.get("mesh_smoothing_vtk_windowed_sinc_iterations", 10);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_iterations(int iterations)
{
  this->params_.set("mesh_smoothing_vtk_windowed_sinc_iterations", iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_windowed_sinc_passband()
{
  return this->params_.get("mesh_smoothing_vtk_windowed_sinc_passband", 0.05);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_passband(double passband)
{
  this->params_.set("mesh_smoothing_vtk_windowed_sinc_passband", passband);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_alignment_method()
{
  if (!this->params_.key_exists("alignment")) {
    // if alignment doesn't exist, perhaps the old keys "center" or "icp" do, if so use them
    if (this->params_.key_exists("icp") && this->params_.get("icp", false)) {
      return GROOM_ALIGNMENT_ICP_C;
    }
    if (this->params_.key_exists("center")) {
      if (this->params_.get("center", false)) {
        return GROOM_ALIGNMENT_CENTER_C;
      }
    }
  }
  return this->params_.get("alignment_method", GROOM_ALIGNMENT_CENTER_C);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_method(std::string method)
{
  this->params_.set("alignment_method", method);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_global_alignment()
{
  return this->params_.get("global_alignment", false);
}

//---------------------------------------------------------------------------
void GroomParameters::set_global_alignment(bool value)
{
  this->params_.set("global_alignment", value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_alignment_enabled()
{
  return this->params_.get("alignment_enabled", true);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_enabled(bool value)
{
  this->params_.set("alignment_enabled", value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_use_icp()
{
  return this->get_alignment_enabled() && this->get_alignment_method() == GROOM_ALIGNMENT_ICP_C;
}

//---------------------------------------------------------------------------
bool GroomParameters::get_use_center()
{
  return this->get_alignment_enabled() && this->get_alignment_method() == GROOM_ALIGNMENT_CENTER_C;
}
};
