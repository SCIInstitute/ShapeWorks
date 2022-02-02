#pragma once
/**
 * @file MeshGenerator.h
 * @brief Mesh generation
 *
 * The MeshGenerator performs the actual work of reconstructing
 * a mesh from a shape (list of points).
 */

#include <QSharedPointer>

#include "vnl/vnl_vector.h"

#include <Libs/Mesh/MeshWarper.h>

#include <Data/StudioMesh.h>
#include <Data/QMeshWarper.h>
#include <Data/Preferences.h>
#include <Data/SurfaceReconstructor.h>
#include <Data/MeshWorkQueue.h>

namespace shapeworks {

class MeshReconstructors {
public:
  std::vector<std::shared_ptr<SurfaceReconstructor>> surface_reconstructors_;
  std::vector<std::shared_ptr<QMeshWarper>> mesh_warpers_;
};

class MeshGenerator {
public:

  MeshGenerator();

  ~MeshGenerator();

  MeshHandle build_mesh(const MeshWorkItem& item);

  MeshHandle build_mesh_from_points(const Eigen::VectorXd& shape, int domain);

  MeshHandle build_mesh_from_image(ImageType::Pointer image, float iso_value = 0.0001);

  MeshHandle build_mesh_from_file(std::string filename, float iso_value = 0.0001);

  void set_mesh_reconstructors(std::shared_ptr<MeshReconstructors> reconstructors);

  void set_reconstruction_method(std::string method);
  std::string get_reconstruction_method();

  static const std::string RECONSTRUCTION_LEGACY_C;
  static const std::string RECONSTRUCTION_DISTANCE_TRANSFORM_C;
  static const std::string RECONSTRUCTION_MESH_WARPER_C;

private:

  std::shared_ptr<MeshReconstructors> reconstructors_ = std::make_shared<MeshReconstructors>();

  std::string reconstruction_method_ = RECONSTRUCTION_MESH_WARPER_C;

};
}