#pragma once

#include <QSharedPointer>
#include <QString>
#include <Groom/ShapeWorksGroom.h>
#include <Data/Mesh.h>
#include <Libs/Project/Subject.h>
#include <Data/MeshManager.h>

class Shape;
using ShapeHandle = QSharedPointer<Shape>;
using ShapeList = QVector<ShapeHandle>;

class Point
{
public:
  double x, y, z;
};

//! Representation of a single shape/patient/subject.
class Shape
{

public:

  Shape();

  ~Shape();

  QSharedPointer<Mesh> get_mesh(string display_mode);

  void set_annotations(QStringList annotations);
  QStringList get_annotations();

  void set_mesh_manager(QSharedPointer<MeshManager> mesh_manager);

  void set_subject(std::shared_ptr<shapeworks::Subject> subject);

  std::shared_ptr<shapeworks::Subject> get_subject();

  /// Import the original raw image file
  void import_original_image(std::string filename, float iso_value);

  /// Retrieve the original mesh
  QSharedPointer<Mesh> get_original_mesh();

  ImageType::Pointer get_original_image();
  ImageType::Pointer get_groomed_image();

  /// Import the groomed raw image file
  void import_groomed_image(ImageType::Pointer img, double iso, transform_type transform);

  /// Retrieve the groomed mesh
  QSharedPointer<Mesh> get_groomed_mesh();

  /// Import global correspondence point file
  bool import_global_point_file(QString filename);

  /// Import local correspondence point file
  bool import_local_point_file(QString filename);

  /// Import local correspondence point data
  bool import_points(std::vector<itk::Point<double>> points, bool local);

  /// Retrieve the reconstructed mesh
  QSharedPointer<Mesh> get_reconstructed_mesh();

  /// Get the global correspondence points
  vnl_vector<double> get_global_correspondence_points();

  /// Get the local correspondence points
  vnl_vector<double> get_local_correspondence_points();

  void set_reconstructed_mesh(MeshHandle mesh);

  void clear_reconstructed_mesh();

  void set_global_particles(const vnl_vector<double> &points);


  /// Get the id of this shape
  int get_id();

  /// Set the id of this shape
  void set_id(int id);

  QString get_original_filename();
  QString get_original_filename_with_path();

  QString get_groomed_filename();
  QString get_groomed_filename_with_path();

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
  vnl_vector<double> get_transform();


  void load_feature(std::string display_mode, std::string feature);

  Eigen::VectorXf get_point_features(std::string feature);

  void set_point_features(std::string feature, Eigen::VectorXf values);

private:

  void generate_original_meshes();

  void generate_meshes(std::vector<std::string> filenames, QSharedPointer<Mesh> &mesh);

  static bool import_point_file(QString filename, vnl_vector<double> &points);

  void apply_feature_to_points(std::string feature, ImageType::Pointer image);

  int id_;

  QSharedPointer<Mesh> original_mesh_;
  QSharedPointer<Mesh> groomed_mesh_;
  QSharedPointer<Mesh> reconstructed_mesh_;
  ImageType::Pointer original_image_, groomed_image_;
  int group_id_ = 1;

  QString original_mesh_filename_;
  QString groomed_filename_;
  QString global_point_filename_;
  QString local_point_filename_;

  vnl_vector<double> global_correspondence_points_;
  vnl_vector<double> local_correspondence_points_;
  std::map<std::string, Eigen::VectorXf> point_features_;

  QList<Point> exclusion_sphere_centers_;
  QList<double> exclusion_sphere_radii_;

  std::shared_ptr<shapeworks::Subject> subject_;

  std::vector<Point> vectors_;
  vnl_vector<double> transform_;

  transform_type groomed_transform_;

  QStringList corner_annotations_;

  QSharedPointer<MeshManager> mesh_manager_;
};
