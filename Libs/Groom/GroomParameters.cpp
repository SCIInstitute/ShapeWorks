#include "GroomParameters.h"

#include <Logging.h>

namespace shapeworks {

const std::string GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C("Laplacian");
const std::string GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C("WindowedSinc");

const std::string GroomParameters::GROOM_ALIGNMENT_CENTER_C("Center");
const std::string GroomParameters::GROOM_ALIGNMENT_ICP_C("Iterative Closest Point");
const std::string GroomParameters::GROOM_ALIGNMENT_LANDMARK_C("Landmark");

namespace Keys {
const std::string CROP = "crop";
const std::string REFLECT = "reflect";
const std::string REFLECT_COLUMN = "reflect_column";
const std::string REFLECT_CHOICE = "reflect_choice";
const std::string REFLECT_AXIS = "reflect_axis";
const std::string RESAMPLE = "resample";
const std::string ISOTROPIC = "isotropic";
const std::string ISO_SPACING = "iso_spacing";
const std::string SPACING = "spacing";
const std::string CONVERT_MESH = "convert_to_mesh";
const std::string FILL_MESH_HOLES = "fill_mesh_holes";
const std::string FILL_HOLES = "fill_holes";
const std::string ISOLATE = "isolate";
const std::string PAD = "pad";
const std::string PAD_VALUE = "pad_value";
const std::string ANTIALIAS = "antialias";
const std::string ANTIALIAS_AMOUNT = "antialias_amount";
const std::string BLUR = "blur";
const std::string BLUR_SIGMA = "blur_sigma";
const std::string FASTMARCHING = "fastmarching";

const std::string MESH_SMOOTH = "mesh_smooth";
const std::string MESH_SMOOTHING_METHOD = "mesh_smoothing_method";
const std::string MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS = "mesh_smoothing_vtk_laplacian_iterations";
const std::string MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION = "mesh_smoothing_vtk_laplacian_relaxation";
const std::string MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS = "mesh_smoothing_vtk_windowed_sinc_iterations";
const std::string MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND = "mesh_smoothing_vtk_windowed_sinc_passband";

const std::string ALIGNMENT_METHOD = "alignment_method";
const std::string ALIGNMENT_ENABLED = "alignment_enabled";
const std::string ALIGNMENT_REFERENCE = "alignment_reference";
const std::string ALIGNMENT_SUBSET_SIZE = "alignment_subset_size";
const std::string GROOM_OUTPUT_PREFIX = "groom_output_prefix";
const std::string REMESH = "remesh";
const std::string REMESH_PERCENT_MODE = "remesh_percent_mode";
const std::string REMESH_PERCENT = "remesh_percent";
const std::string REMESH_NUM_VERTICES = "remesh_num_vertices";
const std::string REMESH_GRADATION = "remesh_gradation";

const std::string GROOM_ALL_DOMAINS_THE_SAME = "groom_all_domains_the_same";

const std::string SKIP_GROOMING = "skip_grooming";
const std::string CENTER = "center";
const std::string ICP = "icp";

}  // namespace Keys

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
const bool fill_holes_mesh = false;
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
const int remesh_num_vertices = 3000;
const double remesh_gradation = 1.0;

const bool groom_all_domains_the_same = true;

const bool skip_grooming = false;
}  // namespace Defaults

//---------------------------------------------------------------------------
GroomParameters::GroomParameters(ProjectHandle project, std::string domain_name)
    : project_(project), domain_name_(domain_name) {
  params_ = project_->get_parameters(Parameters::GROOM_PARAMS, domain_name_);

  std::vector<std::string> all_params = {Keys::CROP,
                                         Keys::REFLECT,
                                         Keys::REFLECT_COLUMN,
                                         Keys::REFLECT_CHOICE,
                                         Keys::REFLECT_AXIS,
                                         Keys::RESAMPLE,
                                         Keys::ISOTROPIC,
                                         Keys::ISO_SPACING,
                                         Keys::SPACING,
                                         Keys::CONVERT_MESH,
                                         Keys::FILL_MESH_HOLES,
                                         Keys::FILL_HOLES,
                                         Keys::ISOLATE,
                                         Keys::PAD,
                                         Keys::PAD_VALUE,
                                         Keys::ANTIALIAS,
                                         Keys::ANTIALIAS_AMOUNT,
                                         Keys::BLUR,
                                         Keys::BLUR_SIGMA,
                                         Keys::FASTMARCHING,
                                         Keys::MESH_SMOOTH,
                                         Keys::MESH_SMOOTHING_METHOD,
                                         Keys::MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS,
                                         Keys::MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION,
                                         Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS,
                                         Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND,
                                         Keys::ALIGNMENT_METHOD,
                                         Keys::ALIGNMENT_ENABLED,
                                         Keys::ALIGNMENT_REFERENCE,
                                         Keys::ALIGNMENT_SUBSET_SIZE,
                                         Keys::GROOM_OUTPUT_PREFIX,
                                         Keys::REMESH,
                                         Keys::REMESH_PERCENT_MODE,
                                         Keys::REMESH_PERCENT,
                                         Keys::REMESH_NUM_VERTICES,
                                         Keys::REMESH_GRADATION,
                                         Keys::GROOM_ALL_DOMAINS_THE_SAME,
                                         Keys::SKIP_GROOMING,
                                         Keys::CENTER,
                                         Keys::ICP};

  std::vector<std::string> to_remove;

  // check if params_ has any unknown keys
  for (auto& param : params_.get_map()) {
    if (std::find(all_params.begin(), all_params.end(), param.first) == all_params.end()) {
      SW_WARN("Unknown Grooming parameter: " + param.first);
      to_remove.push_back(param.first);
    }
  }
  for (auto& param : to_remove) {
    params_.remove_entry(param);
  }
}

//---------------------------------------------------------------------------
void GroomParameters::set_domain_name(std::string domain_name) { domain_name_ = domain_name; }

//---------------------------------------------------------------------------
void GroomParameters::restore_defaults() { params_.reset_parameters(); }

//---------------------------------------------------------------------------
bool GroomParameters::get_isolate_tool() { return params_.get(Keys::ISOLATE, Defaults::isolate); }

//---------------------------------------------------------------------------
void GroomParameters::set_isolate_tool(bool value) { params_.set(Keys::ISOLATE, value); }

//---------------------------------------------------------------------------
bool GroomParameters::get_fill_holes_tool() { return params_.get(Keys::FILL_HOLES, Defaults::fill_holes); }

//---------------------------------------------------------------------------
void GroomParameters::set_fill_holes_tool(bool value) { params_.set(Keys::FILL_HOLES, value); }

//---------------------------------------------------------------------------
bool GroomParameters::get_fill_mesh_holes_tool() {
  return params_.get(Keys::FILL_MESH_HOLES, Defaults::fill_holes_mesh);
}

//---------------------------------------------------------------------------
void GroomParameters::set_fill_mesh_holes_tool(bool value) { params_.set(Keys::FILL_MESH_HOLES, value); }

//---------------------------------------------------------------------------
bool GroomParameters::get_auto_pad_tool() { return params_.get(Keys::PAD, Defaults::pad); }

//---------------------------------------------------------------------------
void GroomParameters::set_auto_pad_tool(bool value) { params_.set(Keys::PAD, value); }

//---------------------------------------------------------------------------
int GroomParameters::get_padding_amount() { return params_.get(Keys::PAD_VALUE, Defaults::pad_value); }

//---------------------------------------------------------------------------
void GroomParameters::set_padding_amount(int padding_amount) { params_.set(Keys::PAD_VALUE, padding_amount); }

//---------------------------------------------------------------------------
bool GroomParameters::get_antialias_tool() { return params_.get(Keys::ANTIALIAS, Defaults::antialias); }

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_tool(bool value) { params_.set(Keys::ANTIALIAS, value); }

//---------------------------------------------------------------------------
int GroomParameters::get_antialias_iterations() {
  return params_.get(Keys::ANTIALIAS_AMOUNT, Defaults::antialias_amount);
}

//---------------------------------------------------------------------------
void GroomParameters::set_antialias_iterations(int iterations) { params_.set(Keys::ANTIALIAS_AMOUNT, iterations); }

//---------------------------------------------------------------------------
bool GroomParameters::get_blur_tool() { return params_.get(Keys::BLUR, Defaults::blur); }

//---------------------------------------------------------------------------
void GroomParameters::set_blur_tool(bool value) { params_.set(Keys::BLUR, value); }

//---------------------------------------------------------------------------
double GroomParameters::get_blur_amount() { return params_.get(Keys::BLUR_SIGMA, Defaults::blur_sigma); }

//---------------------------------------------------------------------------
void GroomParameters::set_blur_amount(double blur_amount) { params_.set(Keys::BLUR_SIGMA, blur_amount); }

//---------------------------------------------------------------------------
bool GroomParameters::get_fast_marching() { return params_.get(Keys::FASTMARCHING, Defaults::fastmarching); }

//---------------------------------------------------------------------------
void GroomParameters::set_fast_marching(bool value) { params_.set(Keys::FASTMARCHING, value); }

//---------------------------------------------------------------------------
void GroomParameters::save_to_project() { project_->set_parameters(Parameters::GROOM_PARAMS, params_, domain_name_); }

//---------------------------------------------------------------------------
std::string GroomParameters::get_groom_output_prefix() {
  return params_.get(Keys::GROOM_OUTPUT_PREFIX, Defaults::groom_output_prefix);
}

//---------------------------------------------------------------------------
void GroomParameters::set_groom_output_prefix(std::string prefix) { params_.set(Keys::GROOM_OUTPUT_PREFIX, prefix); }

//---------------------------------------------------------------------------
bool GroomParameters::get_groom_all_domains_the_same() {
  return params_.get(Keys::GROOM_ALL_DOMAINS_THE_SAME, Defaults::groom_all_domains_the_same);
}

//---------------------------------------------------------------------------
void GroomParameters::set_groom_all_domains_the_same(bool value) {
  params_.set(Keys::GROOM_ALL_DOMAINS_THE_SAME, value);
}

//---------------------------------------------------------------------------
bool GroomParameters::get_mesh_smooth() { return params_.get(Keys::MESH_SMOOTH, Defaults::mesh_smooth); }

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smooth(bool value) { params_.set(Keys::MESH_SMOOTH, value); }

//---------------------------------------------------------------------------
std::string GroomParameters::get_mesh_smoothing_method() {
  return params_.get(Keys::MESH_SMOOTHING_METHOD, Defaults::mesh_smoothing_method);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_smoothing_method(std::string method) {
  params_.set(Keys::MESH_SMOOTHING_METHOD, method);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_laplacian_iterations() {
  return params_.get(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS, Defaults::mesh_smoothing_vtk_laplacian_iterations);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_iterations(int iterations) {
  params_.set(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_ITERATIONS, iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_laplacian_relaxation() {
  return params_.get(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION, Defaults::mesh_smoothing_vtk_laplacian_relaxation);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_laplacian_relaxation(double relaxation) {
  params_.set(Keys::MESH_SMOOTHING_VTK_LAPLACIAN_RELAXATION, relaxation);
}

//---------------------------------------------------------------------------
int GroomParameters::get_mesh_vtk_windowed_sinc_iterations() {
  return params_.get(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS,
                     Defaults::mesh_smoothing_vtk_windowed_sinc_iterations);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_iterations(int iterations) {
  params_.set(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_ITERATIONS, iterations);
}

//---------------------------------------------------------------------------
double GroomParameters::get_mesh_vtk_windowed_sinc_passband() {
  return params_.get(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND,
                     Defaults::mesh_smoothing_vtk_windowed_sinc_passband);
}

//---------------------------------------------------------------------------
void GroomParameters::set_mesh_vtk_windowed_sinc_passband(double passband) {
  params_.set(Keys::MESH_SMOOTHING_VTK_WINDOWED_SINC_PASSBAND, passband);
}

//---------------------------------------------------------------------------
std::string GroomParameters::get_alignment_method() {
  if (!params_.key_exists(Keys::ALIGNMENT_METHOD)) {
    // if alignment doesn't exist, perhaps the old keys "center" or "icp" do, if so use them
    if (params_.key_exists(Keys::ICP) && params_.get(Keys::ICP, false)) {
      return GROOM_ALIGNMENT_ICP_C;
    }
    if (params_.key_exists(Keys::CENTER)) {
      if (params_.get(Keys::CENTER, false)) {
        return GROOM_ALIGNMENT_CENTER_C;
      }
    }
  }
  return params_.get(Keys::ALIGNMENT_METHOD, GROOM_ALIGNMENT_CENTER_C);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_method(std::string method) { params_.set(Keys::ALIGNMENT_METHOD, method); }

//---------------------------------------------------------------------------
int GroomParameters::get_alignment_reference() { return params_.get(Keys::ALIGNMENT_REFERENCE, -1); }

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_reference(int reference) { params_.set(Keys::ALIGNMENT_REFERENCE, reference); }

//---------------------------------------------------------------------------
int GroomParameters::get_alignment_subset_size() { return params_.get(Keys::ALIGNMENT_SUBSET_SIZE, -1); }

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_subset_size(int size) { params_.set(Keys::ALIGNMENT_SUBSET_SIZE, size); }

//---------------------------------------------------------------------------
bool GroomParameters::get_alignment_enabled() {
  return params_.get(Keys::ALIGNMENT_ENABLED, Defaults::alignment_enabled);
}

//---------------------------------------------------------------------------
void GroomParameters::set_alignment_enabled(bool value) { params_.set(Keys::ALIGNMENT_ENABLED, value); }

//---------------------------------------------------------------------------
bool GroomParameters::get_use_icp() {
  return get_alignment_enabled() && get_alignment_method() == GROOM_ALIGNMENT_ICP_C;
}

//---------------------------------------------------------------------------
bool GroomParameters::get_use_center() {
  return get_alignment_enabled() && get_alignment_method() == GROOM_ALIGNMENT_CENTER_C;
}

//---------------------------------------------------------------------------
bool GroomParameters::get_use_landmarks() {
  return get_alignment_enabled() && get_alignment_method() == GROOM_ALIGNMENT_LANDMARK_C;
}

//---------------------------------------------------------------------------
bool GroomParameters::get_crop() { return params_.get(Keys::CROP, Defaults::crop); }

//---------------------------------------------------------------------------
void GroomParameters::set_crop(bool crop) { params_.set(Keys::CROP, crop); }

//---------------------------------------------------------------------------
bool GroomParameters::get_convert_to_mesh() { return params_.get(Keys::CONVERT_MESH, Defaults::convert_mesh); }

//---------------------------------------------------------------------------
void GroomParameters::set_convert_to_mesh(bool value) { params_.set(Keys::CONVERT_MESH, value); }

//---------------------------------------------------------------------------
bool GroomParameters::get_reflect() { return params_.get(Keys::REFLECT, Defaults::reflect); }

//---------------------------------------------------------------------------
void GroomParameters::set_reflect(bool reflect) { params_.set(Keys::REFLECT, reflect); }

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_column() { return params_.get(Keys::REFLECT_COLUMN, ""); }

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_column(std::string column) { params_.set(Keys::REFLECT_COLUMN, column); }

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_choice() { return params_.get(Keys::REFLECT_CHOICE, ""); }

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_choice(std::string choice) { params_.set(Keys::REFLECT_CHOICE, choice); }

//---------------------------------------------------------------------------
std::string GroomParameters::get_reflect_axis() { return params_.get(Keys::REFLECT_AXIS, Defaults::reflect_axis); }

//---------------------------------------------------------------------------
void GroomParameters::set_reflect_axis(std::string axis) { return params_.set(Keys::REFLECT_AXIS, axis); }

//---------------------------------------------------------------------------
bool GroomParameters::get_resample() { return params_.get(Keys::RESAMPLE, Defaults::resample); }

//---------------------------------------------------------------------------
void GroomParameters::set_resample(bool resample) { params_.set(Keys::RESAMPLE, resample); }

//---------------------------------------------------------------------------
bool GroomParameters::get_isotropic() { return params_.get(Keys::ISOTROPIC, Defaults::isotropic); }

//---------------------------------------------------------------------------
void GroomParameters::set_isotropic(bool isotropic) { params_.set(Keys::ISOTROPIC, isotropic); }

//---------------------------------------------------------------------------
double GroomParameters::get_iso_spacing() { return params_.get(Keys::ISO_SPACING, Defaults::iso_spacing); }

//---------------------------------------------------------------------------
void GroomParameters::set_iso_spacing(double spacing) { params_.set(Keys::ISO_SPACING, spacing); }

//---------------------------------------------------------------------------
std::vector<double> GroomParameters::get_spacing() {
  std::vector<double> spacing = params_.get(Keys::SPACING, Defaults::spacing);
  if (spacing.size() != 3) {
    spacing = Defaults::spacing;
  }
  return spacing;
}

//---------------------------------------------------------------------------
void GroomParameters::set_spacing(std::vector<double> spacing) { params_.set(Keys::SPACING, spacing); }

//---------------------------------------------------------------------------
bool GroomParameters::get_remesh() { return params_.get(Keys::REMESH, Defaults::remesh); }

//---------------------------------------------------------------------------
void GroomParameters::set_remesh(bool remesh) { params_.set(Keys::REMESH, remesh); }

//---------------------------------------------------------------------------
bool GroomParameters::get_remesh_percent_mode() {
  return params_.get(Keys::REMESH_PERCENT_MODE, Defaults::remesh_percent_mode);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_percent_mode(bool mode) { params_.set(Keys::REMESH_PERCENT_MODE, mode); }

//---------------------------------------------------------------------------
double GroomParameters::get_remesh_percent() { return params_.get(Keys::REMESH_PERCENT, Defaults::remesh_percent); }

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_percent(double percent) { params_.set(Keys::REMESH_PERCENT, percent); }

//---------------------------------------------------------------------------
int GroomParameters::get_remesh_num_vertices() {
  return params_.get(Keys::REMESH_NUM_VERTICES, Defaults::remesh_num_vertices);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_num_vertices(int num_vertices) {
  params_.set(Keys::REMESH_NUM_VERTICES, num_vertices);
}

//---------------------------------------------------------------------------
double GroomParameters::get_remesh_gradation() {
  return params_.get(Keys::REMESH_GRADATION, Defaults::remesh_gradation);
}

//---------------------------------------------------------------------------
void GroomParameters::set_remesh_gradation(double gradation) { params_.set(Keys::REMESH_GRADATION, gradation); }

//---------------------------------------------------------------------------
bool GroomParameters::get_skip_grooming() { return params_.get(Keys::SKIP_GROOMING, Defaults::skip_grooming); }

//---------------------------------------------------------------------------
void GroomParameters::set_skip_grooming(bool skip) { params_.set(Keys::SKIP_GROOMING, skip); }

//---------------------------------------------------------------------------
}  // namespace shapeworks
//---------------------------------------------------------------------------
