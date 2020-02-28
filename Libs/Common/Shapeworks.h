#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>

namespace shapeworks {

using Dims = itk::Size<3>;
using Point3 = itk::Point<double, 3>;
using Vector3 = itk::Vector<double, 3>;
using Matrix44 = itk::Matrix<double, 4, 4>;
using Matrix = Matrix44;

using IPoint3 = itk::Point<int, 3>;
using FPoint3 = itk::Point<float, 3>;

} // shapeworks
