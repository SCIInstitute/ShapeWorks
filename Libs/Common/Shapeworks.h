#pragma once

#include <itkPoint.h>
#include <itkSize.h>

namespace Shapeworks {

using Dims = itk::Size<3>;
using IPoint3 = itk::Point<int, 3>;
using FPoint3 = itk::Point<float, 3>;
using Point3 = itk::Point<double, 3>;

} // Shapeworks
