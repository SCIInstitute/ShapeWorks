#pragma once

#include <Project/Project.h>

namespace shapeworks {

/**
 * \class GroomParameters
 * \ingroup Group-Groom
 *
 * This class encapsulated processing of Groom parameters
 */
class GroomParameters {
  enum class MeshSmoothingOption { laplacian, sinc };

  enum class AlignmentOption { none, center, icp };

 public:
  explicit GroomParameters(ProjectHandle project, std::string domain_name = "");
  void set_domain_name(std::string domain_name);
  void save_to_project();

  std::string get_groom_output_prefix();
  void set_groom_output_prefix(std::string prefix);

  bool get_groom_all_domains_the_same();
  void set_groom_all_domains_the_same(bool value);

  bool get_alignment_enabled();
  void set_alignment_enabled(bool value);

  std::string get_alignment_method();
  void set_alignment_method(std::string method);

  int get_alignment_reference();
  void set_alignment_reference(int reference);

  int get_alignment_reference_chosen();
  void set_alignment_reference_chosen(int reference);

  int get_alignment_subset_size();
  void set_alignment_subset_size(int size);

  bool get_isolate_tool();
  void set_isolate_tool(bool value);

  bool get_fill_holes_tool();
  void set_fill_holes_tool(bool value);

  bool get_fill_mesh_holes_tool();
  void set_fill_mesh_holes_tool(bool value);

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
  bool get_use_landmarks();

  bool get_crop();
  void set_crop(bool crop);

  bool get_convert_to_mesh();
  void set_convert_to_mesh(bool value);

  // reflection
  bool get_reflect();
  void set_reflect(bool reflect);
  std::string get_reflect_column();
  void set_reflect_column(std::string column);
  std::string get_reflect_choice();
  void set_reflect_choice(std::string choice);
  std::string get_reflect_axis();
  void set_reflect_axis(std::string axies);

  // resampling
  bool get_resample();
  void set_resample(bool resample);
  bool get_isotropic();
  void set_isotropic(bool isotropic);
  double get_iso_spacing();
  void set_iso_spacing(double spacing);
  std::vector<double> get_spacing();
  void set_spacing(std::vector<double> spacing);

  bool get_remesh();
  void set_remesh(bool remesh);

  bool get_remesh_percent_mode();
  void set_remesh_percent_mode(bool mode);

  double get_remesh_percent();
  void set_remesh_percent(double percent);

  int get_remesh_num_vertices();
  void set_remesh_num_vertices(int num_vertices);

  double get_remesh_gradation();
  void set_remesh_gradation(double gradation);

  bool get_skip_grooming();
  void set_skip_grooming(bool skip);

  bool get_shared_boundary();
  void set_shared_boundary(bool shared_boundary);

  std::string get_shared_boundary_first_domain();
  void set_shared_boundary_first_domain(const std::string& domain_name);

  std::string get_shared_boundary_second_domain();
  void set_shared_boundary_second_domain(const std::string& domain_name);

  double get_shared_boundary_tolerance();
  void set_shared_boundary_tolerance(double tolerance);

  void restore_defaults();

  Parameters get_parameters() const { return params_; }

  // constants
  const static std::string GROOM_SMOOTH_VTK_LAPLACIAN_C;
  const static std::string GROOM_SMOOTH_VTK_WINDOWED_SINC_C;

  const static std::string GROOM_ALIGNMENT_CENTER_C;
  const static std::string GROOM_ALIGNMENT_ICP_C;
  const static std::string GROOM_ALIGNMENT_LANDMARK_C;

 private:
  Parameters params_;
  ProjectHandle project_;
  std::string domain_name_;
};
}  // namespace shapeworks
