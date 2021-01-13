#pragma once

#include "ShapeworksUtils.h"
#include "Eigen/Core"
#include "Eigen/Dense"
#include <vtkPolyData.h>

namespace shapeworks {

/// Helper functions for meshes 
class MeshUtils
{
public:

  /// Computes a rigid transformation from source to target using vtkIterativeClosestPointTransform
  static const vtkSmartPointer<vtkMatrix4x4> createIcpTransform(const vtkSmartPointer<vtkPolyData> source, const vtkSmartPointer<vtkPolyData> target, const unsigned iterations = 20);

  /// Reads particles from a .particles file to an Eigen Matrix
  Eigen::MatrixXd pointReadFormat(std::string refPointPath, int numP);

  /// Distils the vertex and face information from VTK poly data to Eigen matrices
  void distilToEigen(const vtkSmartPointer<vtkPolyData> mesh, Eigen::MatrixXd* Vref, Eigen::MatrixXi* Fref);

  /// Compute the warp matrix using the mesh and reference points
  Eigen::MatrixXd generateWarpMatrix(Eigen::MatrixXd TV , Eigen::MatrixXi TF, Eigen::MatrixXd Vref);

  /// Compute transformation from set of points files using template mesh warp&face matrices
  // static bool warpMeshes(const std::string &movingPointspath, const std::string &outputMeshPaths, Eigen::MatrixXd W, Eigen::MatrixXd Fref, const int numP);
};

} // shapeworks
