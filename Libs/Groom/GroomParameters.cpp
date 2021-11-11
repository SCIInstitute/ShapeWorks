#include "GroomParameters.h"

#include <iostream>

namespace shapeworks {

const std::string GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C("Laplacian");
const std::string GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C("WindowedSinc");

const std::string GroomParameters::GROOM_ALIGNMENT_CENTER_C("Center");
const std::string GroomParameters::GROOM_ALIGNMENT_ICP_C("Iterative Closest Point");

namespace Keys {
constexpr const char* CROP = "crop";
constexpr const char* REFLECT = "reflect";
constexpr const char* REFLECT_COLUMN = "reflect_column";
constexpr const char* REFLECT_CHOICE = "reflect_choice";
constexpr const char* REFLECT_AXIS = "reflect_axis";
constexpr const char* RESAMPLE = "resample";
constexpr const char* ISOTROPIC = "isotropic";
constexpr const char* ISO_SPACING = "iso_spacing";
constexpr const char* SPACING = "spacing";
constexpr const char* CONVERT_MESH = "convert_to_mesh";
constexpr const char* FILL_MESH_HOLES = "fill_mesh_holes";
constexpr const char* FILL_HOLES = "fill_holes";
constexpr const char* ISOLATE = "isolate";
constexpr const char* PAD = "pad";
constexpr const char* PAD_VALUE = "pad_value";
constexpr const char* ANTIALIAS = "antialias";
constexpr const char* ANTIALIAS_AMOUNT = "antialias_amount";
constexpr const char* BLUR = "blur";
constexpr const char* BLUR_SIGMA = "blur_sigma";
constexpr const char* FASTMARCHING = "fastmarching";

constexpr const char* MESH_SMOOTH = "mesh_smooth";
constexpr const char* MESH_SMOOTHING_METHOD = "mesh_smoothing_method";
constexpr const char* MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS = "mesh_smoothing_vtk_laplacian_iterations";
constexpr const char* MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION = "mesh_smoothing_vtk_laplacian_relaxation";
constexpr const char* MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS = "mesh_smoothing_vtk_windowed_sinc_iterations";
constexpr const char* MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND = "mesh_smoothing_vtk_windowed_sinc_passband";

constexpr const char* ALIGNMENT_METHOD = "alignment_method";
constexpr const char* ALIGNMENT_ENABLED = "alignment_enabled";
constexpr const char* GROOM_OUTPUT_PREFIX = "groom_output_prefix";
const char* REMESH = "remesh";
constexpr const char* REMESH_PERCENT_MODE = "remesh_percent_mode";
constexpr const char* REMESH_PERCENT = "remesh_percent";
constexpr const char* REMESH_NUM_VERTICES = "remesh_num_vertices";
constexpr const char* REMESH_GRADATION = "remesh_gradation";

}

namespace Defaults {
const bool crop = true;
const bool reflect = false;
const char* reflect_axis = "X";
const bool resample = true;
const bool isotropic = true;
const double iso_spacing = 0.0;
const std::vector<double> spacing{0, 0, 0};
const bool convert_mesh = false;
const bool fill_holes = true;
const bool fill_holes_mesh = true;
const bool isolate = true;
const bool pad = true;
const int pad_value = 10;
const bool antialias = true;
const int antialias_amount = 10;
const bool blur = true;
const double blur_sigma = 2.0;
const bool fastmarching = true;
const char* groom_output_prefix = "groomed";
const bool mesh_smooth = false;
const std::string mesh_smoothing_method = GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C;
const int mesh_smoothing_vtk_laplacian_iterations = 10;
const double mesh_smoothing_vtk_laplacian_relaxation = 1.0;
const int mesh_smoothing_vtk_windowed_sinc_iterations = 10;
const double mesh_smoothing_vtk_windowed_sinc_passband = 0.05;
const std::string alignment_method = GroomParameters::GROOM_ALIGNMENT_ICP_C;
const bool alignment_enabled = true;
const bool remesh = true;

const bool remesh_percent_mode = true;
const double remesh_percent = 75;
const int remesh_num_vertices = -1;
const double remesh_gradation = 1.0;

}

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
  return this->params_.get(Keys::ISOLATE, Defaults::isolate);
}

//---------------------------------------------------------------------------
void GroomParameters::set_isolate_tool(bool value)
{
  this->params_.set(Keys::ISOLATE, value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_fill_holes_tool()
{
  return this->params_.get(Keys::FILL_HOLES, Defaults::fill_holes);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fill_holes_tool(bool value)
{
  this->params_.set(Keys::FILL_HOLES, value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_fill_mesh_holes_tool()
{
  return this->params_.get(Keys::FILL_MESH_HOLES, Defaults::fill_holes_mesh);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fill_mesh_holes_tool(bool value)
{
  this->params_.set(Keys::FILL_MESH_HOLES, value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_auto_pad_tool()
{
  return this->params_.get(Keys::PAD, Defaults::pad);
}

//---------------------------------------------------------------------------
void GroomParameters::set_auto_pad_tool(bool value)
{
  this->params_.set(Keys::PAD, value);
}

//---------------------------------------------------------------------------
int GroomParameters::get_padding_amount()
{
  return this->params_.get(Keys::PAD_VALUE, Defaults::pad_value);
}

//---------------------------------------------------------------------------
void GroomParameters::set_padding_amount(int padding_amount)
{
  this->params_.set(Keys::PAD_VALUE, padding_amount);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_antialias_tool()
{
  return this->params_.get(Keys::ANTIALIAS, Defaults::antialias);
}

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_tool(bool value)
{
  this->params_.set(Keys::ANTIALIAS, value);
}

//---------------------------------------------------------------------------
int GroomParameters::get_antialias_iterations()
{
  return this->params_.get(Keys::ANTIALIAS_AMOUNT, Defaults::antialias_amount);
}

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_iterations(int iterations)
{
  this->params_.set(Keys::ANTIALIAS_AMOUNT, iterations);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_blur_tool()
{
  return this->params_.get(Keys::BLUR, Defaults::blur);
}

//---------------------------------------------------------------------------
void GroomParameters::set_blur_tool(bool value)
{
  this->params_.set(Keys::BLUR, value);
}

//---------------------------------------------------------------------------
double GroomParameters::get_blur_amount()
{
  return this->params_.get(Keys::BLUR_SIGMA, Defaults::blur_sigma);
}

//---------------------------------------------------------------------------
void GroomParameters::set_blur_amount(double blur_amount)
{
  this->params_.set(Keys::BLUR_SIGMA, blur_amount);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_fast_marching()
{
  return this->params_.get(Keys::FASTMARCHING, Defaults::fastmarching);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fast_marching(bool value)
{
  this->params_.set(Keys::FASTMARCHING, value);
}

//---------------------------------------------------------------------------
void GroomParameters::save_to_project()
{
  this->project_->set_parameters(Parameters::GROOM_PARAMS, this->params_, this->domain_name_);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_groom_output_prefix()
{
  return this->params_.get(Keys::GROOM_OUTPUT_PREFIX, Defaults::groom_output_prefix);
}

//---------------------------------------------------------------------------
void GroomParameters::set_groom_output_prefix(std::string prefix)
{
  this->params_.set(Keys::GROOM_OUTPUT_PREFIX, prefix);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_mesh_smooth()
{
  return this->params_.get(Keys::MESH_SMOOTH, Defaults::mesh_smooth);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smooth(bool value)
{
  this->params_.set(Keys::MESH_SMOOTH, value);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_mesh_smoothing_method()
{
  return this->params_.get(Keys::MESH_SMOOTHING_METHOD, Defaults::mesh_smoothing_method);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smoothing_method(std::string method)
{
  this->params_.set(Keys::MESH_SMOOTHING_METHOD, method);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_laplacian_iterations()
{
  return this->params_.get(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS,
                           Defaults::mesh_smoothing_vtk_laplacian_iterations);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_iterations(int iterations)
{
  this->params_.set(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS, iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_laplacian_relaxation()
{
  return this->params_.get(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION,
                           Defaults::mesh_smoothing_vtk_laplacian_relaxation);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_relaxation(double relaxation)
{
  this->params_.set(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION, relaxation);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_windowed_sinc_iterations()
{
  return this->params_.get(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS,
                           Defaults::mesh_smoothing_vtk_windowed_sinc_iterations);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_iterations(int iterations)
{
  this->params_.set(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS, iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_windowed_sinc_passband()
{
  return this->params_.get(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND,
                           Defaults::mesh_smoothing_vtk_windowed_sinc_passband);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_passband(double passband)
{
  this->params_.set(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND, passband);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_alignment_method()
{
  if (!this->params_.key_exists(Keys::ALIGNMENT_METHOD)) {
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
  return this->params_.get(Keys::ALIGNMENT_METHOD, GROOM_ALIGNMENT_CENTER_C);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_method(std::string method)
{
  this->params_.set(Keys::ALIGNMENT_METHOD, method);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_alignment_enabled()
{
  return this->params_.get(Keys::ALIGNMENT_ENABLED, Defaults::alignment_enabled);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_enabled(bool value)
{
  this->params_.set(Keys::ALIGNMENT_ENABLED, value);
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

//---------------------------------------------------------------------------
bool GroomParameters::get_crop()
{
  return this->params_.get(Keys::CROP, Defaults::crop);
}

//---------------------------------------------------------------------------
void GroomParameters::set_crop(bool crop)
{
  this->params_.set(Keys::CROP, crop);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_convert_to_mesh()
{
  return this->params_.get(Keys::CONVERT_MESH, Defaults::convert_mesh);
}

//---------------------------------------------------------------------------
void GroomParameters::set_convert_to_mesh(bool value)
{
  this->params_.set(Keys::CONVERT_MESH, value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_reflect()
{
  return this->params_.get(Keys::REFLECT, Defaults::reflect);
}

//---------------------------------------------------------------------------
void GroomParameters::set_reflect(bool reflect)
{
  this->params_.set(Keys::REFLECT, reflect);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_column()
{
  return this->params_.get(Keys::REFLECT_COLUMN, "");
}

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_column(std::string column)
{
  this->params_.set(Keys::REFLECT_COLUMN, column);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_choice()
{
  return this->params_.get(Keys::REFLECT_CHOICE, "");
}

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_choice(std::string choice)
{
  this->params_.set(Keys::REFLECT_CHOICE, choice);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_axis()
{
  return this->params_.get(Keys::REFLECT_AXIS, Defaults::reflect_axis);
}

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_axis(std::string axis)
{
  return this->params_.set(Keys::REFLECT_AXIS, axis);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_resample()
{
  return this->params_.get(Keys::RESAMPLE, Defaults::resample);
}

//---------------------------------------------------------------------------
void GroomParameters::set_resample(bool resample)
{
  this->params_.set(Keys::RESAMPLE, resample);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_isotropic()
{
  return this->params_.get(Keys::ISOTROPIC, Defaults::isotropic);
}

//---------------------------------------------------------------------------
void GroomParameters::set_isotropic(bool isotropic)
{
  this->params_.set(Keys::ISOTROPIC, isotropic);
}

//---------------------------------------------------------------------------
double GroomParameters::get_iso_spacing()
{
  return this->params_.get(Keys::ISO_SPACING, Defaults::iso_spacing);
}

//---------------------------------------------------------------------------
void GroomParameters::set_iso_spacing(double spacing)
{
  this->params_.set(Keys::ISO_SPACING, spacing);
}

//---------------------------------------------------------------------------
std::vector<double> GroomParameters::get_spacing()
{
  std::vector<double> spacing = this->params_.get(Keys::SPACING, Defaults::spacing);
  if (spacing.size() != 3) {
    spacing = Defaults::spacing;
  }
  return spacing;
}

//---------------------------------------------------------------------------
void GroomParameters::set_spacing(std::vector<double> spacing)
{
  this->params_.set(Keys::SPACING, spacing);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_remesh()
{
  std::cerr << "get_remesh returning: " << (bool)this->params_.get(Keys::REMESH, Defaults::remesh) << "\n";
  return this->params_.get(Keys::REMESH, Defaults::remesh);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh(bool remesh)
{
  this->params_.set(Keys::REMESH, remesh);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_remesh_percent_mode()
{
  return this->params_.get(Keys::REMESH_PERCENT_MODE, Defaults::remesh_percent_mode);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_percent_mode(bool mode)
{
  this->params_.set(Keys::REMESH_PERCENT_MODE, mode);
}

//---------------------------------------------------------------------------
double GroomParameters::get_remesh_percent()
{
  return this->params_.get(Keys::REMESH_PERCENT, Defaults::remesh_percent);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_percent(double percent)
{
  this->params_.set(Keys::REMESH_PERCENT, percent);
}

//---------------------------------------------------------------------------
int GroomParameters::get_remesh_num_vertices()
{
  return this->params_.get(Keys::REMESH_NUM_VERTICES, Defaults::remesh_num_vertices);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_num_vertices(int num_vertices)
{
  this->params_.set(Keys::REMESH_NUM_VERTICES, num_vertices);
}

//---------------------------------------------------------------------------
double GroomParameters::get_remesh_gradation()
{
  return this->params_.get(Keys::REMESH_GRADATION, Defaults::remesh_gradation);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_gradation(double gradation)
{
  this->params_.set(Keys::REMESH_GRADATION, gradation);

}
}
//---------------------------------------------------------------------------
