#pragma once

#include "ShapeworksUtils.h"
#include "Mesh.h"
#include "Eigen/Core"
#include "Eigen/Dense"

class vtkActor;

namespace shapeworks {

/**
 * \class MeshUtils
 * \ingroup Group-Mesh
 *
 * This class provides helper functions for meshes
 *
 */
class MeshUtils
{
public:

  /// computes a rigid transformation from source to target using vtkIterativeClosestPointTransform
  static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const Mesh source,
                                                                const Mesh target,
                                                                Mesh::AlignmentType align,
                                                                const unsigned iterations = 20,
                                                                bool meshTransform = false);

  /// Thread safe reading of a mesh, uses a lock
  static Mesh threadSafeReadMesh(std::string filename);

  /// Thread safe writing of a mesh, uses a lock
  static void threadSafeWriteMesh(std::string filename, Mesh mesh);

  /// calculate bounding box incrementally for meshes
  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, bool center = false);

  /// calculate bounding box incrementally for meshes
  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh>>& meshes, bool center = false);

  /// determine the reference mesh
  static size_t findReferenceMesh(std::vector<Mesh> &meshes);

  /// generates and adds normals for points and faces for each mesh in given set of meshes
  static void generateNormals(const std::vector<std::reference_wrapper<Mesh>>& meshes, bool forceRegen = false);

  /// computes average normals for each point in given set of meshes
  static Field computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals = true);

  /// computes average normals for each point in given set of meshes
  static Field computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh>>& meshes);

  /// This function visualizes vector and scalar fields for FFCs
  void visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh);

  /// Used as an auxiliary function for vector field visualizations
  vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);
};

} // shapeworks
