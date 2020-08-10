#pragma once

#include "ShapeworksUtils.h"
#include "Mesh.h"

#include <vtkPolyData.h>

namespace shapeworks {

/// Helper functions for meshes 
class MeshUtils
{
public:

  /// Computes a rigid transformation from source to target using vtkIterativeClosestPointTransform
  static const vtkSmartPointer<vtkMatrix4x4> createIcpTransform(const vtkSmartPointer<vtkPolyData> source, const vtkSmartPointer<vtkPolyData> target, const std::string type, const unsigned iterations = 20);

  /// Creates transform from source mesh to target using ICP registration
  static vtkTransform createRegistrationTransform(const std::unique_ptr<Mesh> &sourceMesh, const std::unique_ptr<Mesh> &targetMesh, const std::string type, unsigned iterations = 20);

};

} // shapeworks
