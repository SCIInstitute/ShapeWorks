#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class GroomParameters {

  enum class MeshSmoothingOption {
    laplacian,
    sinc
  };

  enum class AlignmentOption {
    none,
    center,
    icp
  };

public:

  explicit GroomParameters(ProjectHandle project, std::string domain_name = "");
  void save_to_project();

  std::string get_groom_output_prefix();
  void set_groom_output_prefix(std::string prefix);

  bool get_alignment_enabled();
  void set_alignment_enabled(bool value);

  std::string get_alignment_method();
  void set_alignment_method(std::string method);

  bool get_isolate_tool();
  void set_isolate_tool(bool value);

  bool get_fill_holes_tool();
  void set_fill_holes_tool(bool value);

  bool get_auto_pad_tool();
  void set_auto_pad_tool(bool value);

  int get_padding_amount();
  void set_padding_amount(int padding_amount);

  bool get_antialias_tool();
  void set_antialias_tool(bool value);

  int get_antialias_iterations();
  void set_antialias_iterations(int iterations);

  bool get_blur_tool();
  void set_blur_tool(bool value);

  double get_blur_amount();
  void set_blur_amount(double blur_amount);

  bool get_fast_marching();
  void set_fast_marching(bool value);

  bool get_mesh_smooth();
  void set_mesh_smooth(bool value);

  std::string get_mesh_smoothing_method();
  void set_mesh_smoothing_method(std::string method);

  int get_mesh_vtk_laplacian_iterations();
  void set_mesh_vtk_laplacian_iterations(int iterations);

  double get_mesh_vtk_laplacian_relaxation();
  void set_mesh_vtk_laplacian_relaxation(double relaxation);

  int get_mesh_vtk_windowed_sinc_iterations();
  void set_mesh_vtk_windowed_sinc_iterations(int iterations);

  double get_mesh_vtk_windowed_sinc_passband();
  void set_mesh_vtk_windowed_sinc_passband(double passband);

  bool get_use_icp();
  bool get_use_center();

  bool get_crop();
  void set_crop(bool crop);

  // reflection
  bool get_reflect();
  void set_reflect(bool reflect);
  std::string get_reflect_column();
  void set_reflect_column(std::string column);
  std::string get_reflect_choice();
  void set_reflect_choice(std::string choice);

  // resampling
  bool get_resample();
  void set_resample(bool resample);
  bool get_isotropic();
  void set_isotropic(bool isotropic);
  double get_iso_spacing();
  void set_iso_spacing(double spacing);
  std::vector<double> get_spacing();
  void set_spacing(std::vector<double> spacing);

  void restore_defaults();

  // constants
  const static std::string GROOM_SMOOTH_VTK_LAPLACIAN_C;
  const static std::string GROOM_SMOOTH_VTK_WINDOWED_SINC_C;

  const static std::string GROOM_ALIGNMENT_CENTER_C;
  const static std::string GROOM_ALIGNMENT_ICP_C;

private:

  Parameters params_;
  ProjectHandle project_;
  std::string domain_name_;
};
}
