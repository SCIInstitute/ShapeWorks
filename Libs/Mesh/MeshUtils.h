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
  static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                const vtkSmartPointer<vtkPolyData> target,
                                                                Mesh::AlignmentType align,
                                                                const unsigned iterations = 20,
                                                                bool meshTransform = false);

  /// distils the vertex and face information from VTK poly data to Eigen matrices
  static Eigen::MatrixXd distilVertexInfo(Mesh mesh);
  static Eigen::MatrixXi distilFaceInfo(Mesh mesh);

  /// compute the warp matrix using the mesh and reference points
  static Eigen::MatrixXd generateWarpMatrix(Eigen::MatrixXd TV,
                                            Eigen::MatrixXi TF,
                                            const Eigen::MatrixXd& Vref);

  /// compute individual warp
  static Mesh warpMesh(const Eigen::MatrixXd& movPts, const Eigen::MatrixXd& W,
                       const Eigen::MatrixXi& Fref);

  /// compute transformation from set of points files using template mesh warp&face matrices
  static bool warpMeshes(std::vector<std::string> movingPointPsaths,
                         std::vector<std::string> outputMeshPaths,
                         const Eigen::MatrixXd& W, const Eigen::MatrixXi& Fref, const int numP);

  /// Thread safe reading of a mesh, uses a lock
  static Mesh threadSafeReadMesh(std::string filename);

  /// Thread safe writing of a mesh, uses a lock
  static void threadSafeWriteMesh(std::string filename, Mesh mesh);

  /// calculate bounding box incrementally for meshes
  static PhysicalRegion boundingBox(std::vector<std::string> &filenames, bool center = false);

  /// calculate bounding box incrementally for shapework meshes
  static PhysicalRegion boundingBox(std::vector<Mesh> &meshes, bool center = false);
};

} // shapeworks
