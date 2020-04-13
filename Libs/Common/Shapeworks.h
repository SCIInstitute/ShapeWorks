#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>

namespace shapeworks {

using Dims = itk::Size<3>;
using Point3 = itk::Point<double, 3>;
using Vector3 = itk::Vector<double, 3>;
using Matrix44 = itk::Matrix<double, 4, 4>;
using Matrix = Matrix44;

using IPoint3 = itk::Point<int, 3>;
using FPoint3 = itk::Point<float, 3>;

itk::Index<3> toIndex(const IPoint3 &pt);
itk::Size<3> toSize(const IPoint3 &pt);

template<typename P>
P operator+(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

template<typename P>
P& operator+=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

template<typename P>
P operator/(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return ret;
}

template<typename P>
P& operator/=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

} // shapeworks
