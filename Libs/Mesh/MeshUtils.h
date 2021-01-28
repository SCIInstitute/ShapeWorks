#pragma once

#include "ShapeworksUtils.h"
#include "Mesh.h"
#include "Eigen/Core"
#include "Eigen/Dense"
#include <vtkPolyData.h>

namespace shapeworks {

/// Helper functions for meshes 
class MeshUtils
{
public:

  /// Computes a rigid transformation from source to target using vtkIterativeClosestPointTransform
  static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                const vtkSmartPointer<vtkPolyData> target,
                                                                Mesh::AlignmentType align,
                                                                const unsigned iterations = 20,
                                                                bool meshTransform = false);

  /// Distils the vertex and face information from VTK poly data to Eigen matrices
  static Eigen::MatrixXd distilVertexInfo(Mesh mesh);
  static Eigen::MatrixXi distilFaceInfo(Mesh mesh);

  /// Compute the warp matrix using the mesh and reference points
  static Eigen::MatrixXd generateWarpMatrix(Eigen::MatrixXd TV , Eigen::MatrixXi TF, Eigen::MatrixXd Vref);

  /// Compute individual warp
  static Mesh warpMesh(Eigen::MatrixXd movPts, Eigen::MatrixXd W, Eigen::MatrixXi Fref);

  /// Compute transformation from set of points files using template mesh warp&face matrices
  static bool warpMeshes(std::vector< std::string> movingPointpaths, std::vector< std::string> outputMeshPaths, Eigen::MatrixXd W, Eigen::MatrixXi Fref, const int numP);

  /// Create plane
  static vtkSmartPointer<vtkPlane> createPlane(const Vector3 &n, const Point &o);

  /// calculate bounding box incrementally for meshes
  static Region boundingBox(std::vector<std::string> &filenames, bool center = false);

  /// calculate bounding box incrementally for shapework meshes
  static Region boundingBox(std::vector<Mesh> &meshes, bool center = false);
};

} // shapeworks
