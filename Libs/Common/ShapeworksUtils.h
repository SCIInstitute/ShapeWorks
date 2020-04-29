#pragma once

#include "Shapeworks.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkMatrix4x4.h>

namespace shapeworks {

class ShapeworksUtils
{
public:
  static Matrix icp(vtkSmartPointer<vtkPolyData> target, vtkSmartPointer<vtkPolyData> moving, unsigned iterations = 20);
  static Matrix convert(vtkSmartPointer<vtkMatrix4x4> mat);

};

} // shapeworks