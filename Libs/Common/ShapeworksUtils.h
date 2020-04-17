#pragma once

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

namespace shapeworks {

class ShapeworksUtils
{
public:
     static bool icp(vtkSmartPointer<vtkPolyData> target, vtkSmartPointer<vtkPolyData> moving, unsigned iterations);
};

} // shapeworks