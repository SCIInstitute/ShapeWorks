#pragma once

#include <itkMatrixOffsetTransformBase.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <string>

// studio
#include "MeshGroup.h"
#include "MeshManager.h"
#include "StudioEnums.h"
#include "StudioMesh.h"

// shapeworks
#include <Analyze/Particles.h>
#include <Libs/Optimize/Constraints/Constraints.h>
#include <Project/Subject.h>

namespace shapeworks {

class Shape;
using ShapeHandle = std::shared_ptr<Shape>;
using ShapeList = std::vector<ShapeHandle>;
class Surface;

//! Representation of a single shape/patient/subject.
class Shape {
 public:
  //! TODO: replace this wherever it is used
  class Point {
   public:
    Point(){};
    Point(double _x, double _y, double _z) : x(_x), y(_y), z(_z){};
    double x = 0, y = 0, z = 0;
  };

  Shape();

  ~Shape();

  std::string get_display_name();

  MeshGroup get_meshes(DisplayMode display_mode, bool wait = false);

  void set_annotations(std::vector<std::string> annotations, bool only_overwrite_blank = true);
  std::vector<std::string> get_annotations();

  void set_mesh_manager(std::shared_ptr<MeshManager> mesh_manager);

  void set_subject(std::shared_ptr<shapeworks::Subject> subject);

  //! Is this shape a population subject (e.g. mean/pca constructions are not)
  bool is_subject();

  //! Return the pointer to the subject object
  std::shared_ptr<shapeworks::Subject> get_subject();

  //! Helper to ask if this shape is fixed or not
  bool is_fixed();

  /// Helper to ask if this shape is excluded
  bool is_excluded();

  //! Import the original raw mesh or image file
  void import_original_file(const std::string& filename);

  //! Retrieve the original meshes
  MeshGroup get_original_meshes(bool wait = false);

  //! Retrieve the groomed meshes
  MeshGroup get_groomed_meshes(bool wait = false);

  //! Retrieve the reconstructed meshes
  MeshGroup get_reconstructed_meshes(bool wait = false);

  //! Set the reconstructed meshes
  void set_reconstructed_meshes(MeshGroup meshes);

  //! Reset the groomed mesh so that it will be re-created
  void reset_groomed_mesh();

  //! Import global correspondence point files
  bool import_global_point_files(std::vector<std::string> filenames);

  //! Import local correspondence point files
  bool import_local_point_files(std::vector<std::string> filenames);

  //! Import landmarks files
  bool import_landmarks_files(std::vector<std::string> filenames);

  //! Store landmarks
  bool store_landmarks();

  //! import constraints
  bool import_constraints(std::vector<std::string> filenames);

  //! Store constraints
  bool store_constraints();

  //! Set particles
  void set_particles(Particles particles);

  //! Get particles
  Particles get_particles();

  //! Set the particle transform (alignment)
  void set_particle_transform(vtkSmartPointer<vtkTransform> transform);

  //! Set the alignment type
  void set_alignment_type(int alignment);

  //! Get the global correspondence points
  Eigen::VectorXd get_global_correspondence_points();

  //! Get the global correspondence points for display
  std::vector<Eigen::VectorXd> get_particles_for_display();

  //! Get the local correspondence points
  Eigen::VectorXd get_local_correspondence_points();

  void clear_reconstructed_mesh();

  //! Get the id of this shape
  int get_id();

  //! Set the id of this shape
  void set_id(int id);

  //! Update the name of this shape
  void update_annotations();

  std::vector<std::string> get_original_filenames();
  std::vector<std::string> get_original_filenames_with_path();

  std::string get_original_filename();
  std::string get_original_filename_with_path();

  std::string get_groomed_filename();
  std::string get_groomed_filename_with_path(int domain);

  std::string get_global_point_filename();
  std::string get_global_point_filename_with_path();

  std::string get_local_point_filename();
  std::string get_local_point_filename_with_path();

  void set_transform(vtkSmartPointer<vtkTransform> transform);
  vtkSmartPointer<vtkTransform> get_transform(int domain = 0);
  vtkSmartPointer<vtkTransform> get_inverse_transform(int domain = 0);

  bool has_alignment();

  vtkSmartPointer<vtkTransform> get_original_transform(int domain = 0);

  void set_reconstruction_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms);
  vtkSmartPointer<vtkTransform> get_reconstruction_transform(int domain);

  vtkSmartPointer<vtkTransform> get_groomed_transform(int domain = 0);

  vtkSmartPointer<vtkTransform> get_procrustes_transform(int domain = 0);
  std::vector<vtkSmartPointer<vtkTransform>> get_procrustes_transforms();

  vtkSmartPointer<vtkTransform> get_alignment(int domain = 0);

  void load_feature(DisplayMode display_mode, std::string feature);

  std::shared_ptr<Image> get_image_volume(std::string image_volume_name);

  std::shared_ptr<Image> get_segmentation();
  std::string get_segmentation_filename() { return segmentation_filename_; }

  Eigen::VectorXd get_point_features(std::string feature);

  void set_point_features(std::string feature, Eigen::VectorXd values);

  void load_feature_from_scalar_file(std::string filename, std::string feature_name);

  void set_override_feature(std::string feature);
  std::string get_override_feature();

  Eigen::MatrixXd& landmarks();

  std::vector<Constraints>& constraints();

  Constraints& get_constraints(int domain_id);

  bool has_planes();

  std::vector<std::shared_ptr<Surface>> get_groomed_mesh_wrappers();

  void recompute_original_surface();

  //! If a segmentation doesn't exist, create a blank canvas
  void ensure_segmentation();

 private:
  void generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_list, bool save_transform,
                       bool wait = false);

  static bool import_point_file(std::string filename, Eigen::VectorXd& points);

  void apply_feature_to_points(std::string feature, ImageType::Pointer image);
  void load_feature_from_mesh(std::string feature, MeshHandle mesh);

  int id_;

  MeshGroup original_meshes_;
  MeshGroup groomed_meshes_;
  MeshGroup reconstructed_meshes_;
  std::vector<std::shared_ptr<Surface>> groomed_mesh_wrappers_;

  std::string override_feature_;

  std::vector<std::string> global_point_filenames_;
  std::vector<std::string> local_point_filenames_;

  std::map<std::string, Eigen::VectorXd> point_features_;
  Particles particles_;

  std::shared_ptr<Subject> subject_;

  vtkSmartPointer<vtkTransform> transform_ = vtkSmartPointer<vtkTransform>::New();

  std::vector<vtkSmartPointer<vtkTransform>> reconstruction_transforms_;

  std::vector<std::string> corner_annotations_;

  std::shared_ptr<MeshManager> mesh_manager_;

  Eigen::MatrixXd landmarks_;

  std::shared_ptr<Image> image_volume_;
  std::string image_volume_filename_;

  std::shared_ptr<Image> segmentation_;
  std::string segmentation_filename_;

  std::vector<Constraints> constraints_;  // one set for each domain
  int alignment_type_;
};
}  // namespace shapeworks
