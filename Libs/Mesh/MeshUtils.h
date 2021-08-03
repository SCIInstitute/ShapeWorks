#pragma once

#include "ShapeworksUtils.h"
#include "Mesh.h"
#include "Eigen/Core"
#include "Eigen/Dense"

namespace shapeworks {

/// Helper functions for meshes
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

  /// calculate bounding box incrementally for shapework meshes
  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh>>& meshes, bool center = false);

  /// determine the reference mesh
  static int findReferenceMesh(std::vector<Mesh> &meshes);

  /// returns array of average normals for each point in given set of meshes
  static Array computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh>>& meshes);
};

} // shapeworks
