#pragma once

#include <QSharedPointer>
#include <QString>

#include <Data/StudioMesh.h>
#include <Data/StudioParticles.h>
#include <Libs/Project/Subject.h>
#include <Data/MeshManager.h>
#include <Data/MeshGroup.h>

#include <itkMatrixOffsetTransformBase.h>

namespace shapeworks {

class Shape;
using ShapeHandle = QSharedPointer<Shape>;
using ShapeList = QVector<ShapeHandle>;

//! TODO: replace this
using TransformType = itk::MatrixOffsetTransformBase<double, 3, 3>::ParametersType;

//! Representation of a single shape/patient/subject.
class Shape {

public:

  //! TODO: replace this wherever it is used
  class Point {
  public:
    double x, y, z;
  };

  Shape();

  ~Shape();

  MeshGroup get_meshes(const string& display_mode);

  void set_annotations(QStringList annotations, bool only_overwrite_blank = true);
  QStringList get_annotations();

  void set_mesh_manager(QSharedPointer<MeshManager> mesh_manager);

  void set_subject(std::shared_ptr<shapeworks::Subject> subject);

  std::shared_ptr<shapeworks::Subject> get_subject();

  /// Import the original raw image file
  void import_original_image(const std::string& filename);

  /// Retrieve the original meshes
  MeshGroup get_original_meshes(bool wait = false);

  /// Retrieve the groomed meshes
  MeshGroup get_groomed_meshes(bool wait = false);

  /// Retrieve the reconstructed meshes
  MeshGroup get_reconstructed_meshes(bool wait = false);

  /// Reset the groomed mesh so that it will be re-created
  void reset_groomed_mesh();

  /// Import global correspondence point file
  bool import_global_point_files(std::vector<std::string> filenames);

  /// Import local correspondence point file
  bool import_local_point_files(std::vector<std::string> filenames);

  void set_particles(StudioParticles particles);
  StudioParticles get_particles();

  /// Get the global correspondence points
  vnl_vector<double> get_global_correspondence_points();

  /// Get the local correspondence points
  vnl_vector<double> get_local_correspondence_points();

  void set_reconstructed_mesh(MeshHandle mesh);

  void clear_reconstructed_mesh();

  void set_global_particles(const vnl_vector<double>& points);

  /// Get the id of this shape
  int get_id();

  /// Set the id of this shape
  void set_id(int id);

  std::vector<QString> get_original_filenames();
  std::vector<QString> get_original_filenames_with_path();

  QString get_original_filename();
  QString get_original_filename_with_path();

  QString get_groomed_filename();
  QString get_groomed_filename_with_path(int domain);

  QString get_global_point_filename();
  QString get_global_point_filename_with_path();

  QString get_local_point_filename();
  QString get_local_point_filename_with_path();

  QList<Point> get_exclusion_sphere_centers();
  void set_exclusion_sphere_centers(QList<Point> centers);

  QList<double> get_exclusion_sphere_radii();
  void set_exclusion_sphere_radii(QList<double> radii);

  int get_group_id();
  void set_group_id(int id);

  std::vector<Point> get_vectors();
  void set_vectors(std::vector<Point> vectors);

  void set_transform(const vnl_vector<double>& transform);
  vnl_vector<double> get_transform(int domain = 0);

  void set_reconstruction_transform(int domain, const vnl_vector<double>& transform);
  vnl_vector<double> get_reconstruction_transform(int domain);

  TransformType get_groomed_transform(int domain = 0);

  void load_feature(std::string display_mode, std::string feature);

  Eigen::VectorXf get_point_features(std::string feature);

  void set_point_features(std::string feature, Eigen::VectorXf values);

private:

  //void generate_original_meshes();

  void generate_meshes(std::vector<std::string> filenames, MeshHandle& mesh,
                       bool save_transform, bool wait = false);

  void generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_list,
                       bool save_transform, bool wait = false);

  static bool import_point_file(QString filename, vnl_vector<double>& points);

  void apply_feature_to_points(std::string feature, ImageType::Pointer image);
  void apply_feature_to_points(std::string feature, MeshHandle mesh);

  int id_;

  MeshGroup original_meshes_;
  MeshGroup groomed_meshes_;
  MeshGroup reconstructed_meshes_;

  MeshHandle original_mesh_;
  MeshHandle groomed_mesh_;
  MeshHandle reconstructed_mesh_;
  ImageType::Pointer original_image_, groomed_image_;
  int group_id_ = 1;

  QString original_mesh_filename_;
  QString groomed_filename_;
  std::vector<std::string> global_point_filenames_;
  std::vector<std::string> local_point_filenames_;

  //vnl_vector<double> global_correspondence_points_;
  //vnl_vector<double> local_correspondence_points_;
  std::map<std::string, Eigen::VectorXf> point_features_;
  StudioParticles particles_;

  QList<Point> exclusion_sphere_centers_;
  QList<double> exclusion_sphere_radii_;

  std::shared_ptr<shapeworks::Subject> subject_;

  std::vector<Point> vectors_;
  vnl_vector<double> transform_;

  std::vector<vnl_vector<double>> reconstruction_transforms_;

  TransformType groomed_transform_;

  QStringList corner_annotations_;

  QSharedPointer<MeshManager> mesh_manager_;
};

}