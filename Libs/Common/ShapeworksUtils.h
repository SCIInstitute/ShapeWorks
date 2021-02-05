#pragma once

#include "Shapeworks.h"

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>

namespace shapeworks {

class ShapeworksUtils
{
public:

  /// returns true if pathname is a directory
  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string &pathname);

  /// returns true if filename exists
  // TODO: in C++17 this is a standard function
  static bool exists(const std::string& filename);

  /// converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector
  static Matrix33 getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat);
};

} // shapeworks
