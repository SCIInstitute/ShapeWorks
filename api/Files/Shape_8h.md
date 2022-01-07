---
title: Studio/src/Data/Shape.h

---

# Studio/src/Data/Shape.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Shape](../Classes/classshapeworks_1_1Shape.md)** <br>Representation of a single shape/patient/subject.  |
| class | **[shapeworks::Shape::Point](../Classes/classshapeworks_1_1Shape_1_1Point.md)** <br>TODO: replace this wherever it is used.  |




## Source code

```cpp
#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

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

class Shape {

public:

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

  void import_original_image(const std::string& filename);

  MeshGroup get_original_meshes(bool wait = false);

  MeshGroup get_groomed_meshes(bool wait = false);

  MeshGroup get_reconstructed_meshes(bool wait = false);

  void reset_groomed_mesh();

  bool import_global_point_files(QStringList filenames);

  bool import_local_point_files(QStringList filenames);

  void set_particles(StudioParticles particles);
  StudioParticles get_particles();

  void set_particle_transform(vtkSmartPointer<vtkTransform> transform);

  vnl_vector<double> get_global_correspondence_points();

  vnl_vector<double> get_global_correspondence_points_for_display();

  vnl_vector<double> get_local_correspondence_points();

  void clear_reconstructed_mesh();

  int get_id();

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

  void set_transform(vtkSmartPointer<vtkTransform> transform);
  vtkSmartPointer<vtkTransform> get_transform(int domain = 0);
  bool has_alignment();

  vtkSmartPointer<vtkTransform> get_original_transform(int domain = 0);

  void set_reconstruction_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms);
  vtkSmartPointer<vtkTransform> get_reconstruction_transform(int domain);

  vtkSmartPointer<vtkTransform> get_groomed_transform(int domain = 0);

  vtkSmartPointer<vtkTransform> get_procrustest_transform(int domain = 0);
  std::vector<vtkSmartPointer<vtkTransform>> get_procrustest_transforms();

  vtkSmartPointer<vtkTransform> get_alignment(int domain = 0);

  void load_feature(std::string display_mode, std::string feature);

  Eigen::VectorXf get_point_features(std::string feature);

  void set_point_features(std::string feature, Eigen::VectorXf values);

  void load_feature_from_scalar_file(std::string filename, std::string feature_name);

  void set_override_feature(std::string feature);
  std::string get_override_feature();

private:

  void generate_meshes(std::vector<std::string> filenames, MeshGroup& mesh_list,
                       bool save_transform, bool wait = false);

  static bool import_point_file(QString filename, vnl_vector<double>& points);

  void apply_feature_to_points(std::string feature, ImageType::Pointer image);
  void load_feature_from_mesh(std::string feature, MeshHandle mesh);

  int id_;

  MeshGroup original_meshes_;
  MeshGroup groomed_meshes_;
  MeshGroup reconstructed_meshes_;

  int group_id_ = 1;

  std::string override_feature_;

  std::vector<std::string> global_point_filenames_;
  std::vector<std::string> local_point_filenames_;

  std::map<std::string, Eigen::VectorXf> point_features_;
  StudioParticles particles_;

  QList<Point> exclusion_sphere_centers_;
  QList<double> exclusion_sphere_radii_;

  std::shared_ptr<shapeworks::Subject> subject_;

  std::vector<Point> vectors_;
  vtkSmartPointer<vtkTransform> transform_ = vtkSmartPointer<vtkTransform>::New();

  std::vector<vtkSmartPointer<vtkTransform>> reconstruction_transforms_;

  QStringList corner_annotations_;

  QSharedPointer<MeshManager> mesh_manager_;
};
}
```


-------------------------------

Updated on 2022-01-07 at 00:54:40 +0000