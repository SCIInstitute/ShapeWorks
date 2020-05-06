#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>
#include <itkAffineTransform.h>

namespace shapeworks {

using Coord         = itk::Index<3>;
using Dims          = itk::Size<3>;
using Point3        = itk::Point<double, 3>;
using Vector3       = itk::Vector<double, 3>;
using Matrix44      = itk::Matrix<double, 4, 4>;
using Matrix33      = itk::Matrix<double, 3, 3>;
using IPoint3       = itk::Point<int, 3>;
using FPoint3       = itk::Point<float, 3>;
using TransformType = itk::AffineTransform<double, 3>;
using Transform     = TransformType::Pointer;
using Vector        = Vector3;
using Point         = Point3;
using Matrix        = Matrix33;

/// Enables `makeVector({1,2,3});`, construction using an initializer list.
Vector3 makeVector3(std::array<double, 3>&& arr);

/// Vector cross product
Vector3 cross(const Vector3 &a, const Vector3 &b);

/// Vector dot product
Vector3 dot(const Vector3 &a, const Vector3 &b);

template <typename P>
P operator+(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

template <typename P>
P operator-(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] - q[i];
  return ret;
}

template <typename P>
P operator*(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] * q[i];
  return ret;
}

template <typename P>
P operator/(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return ret;
}

template <typename P>
P& operator+=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

template <typename P>
P& operator/=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

} // shapeworks
