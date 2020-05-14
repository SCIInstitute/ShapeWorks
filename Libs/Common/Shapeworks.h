#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>
#include <itkAffineTransform.h>

namespace shapeworks {

// Simple names for common types used in the framework
using Coord         = itk::Index<3>;
using Dims          = itk::Size<3>;
using Point3        = itk::Point<double, 3>;
using Vector3       = itk::Vector<double, 3>;
using Matrix44      = itk::Matrix<double, 4, 4>;
using Matrix33      = itk::Matrix<double, 3, 3>;
using IPoint3       = itk::Point<int, 3>;
using FPoint3       = itk::Point<float, 3>;
using Vector        = Vector3;
using Point         = Point3;
using Matrix        = Matrix33;

/// All transforms can be accessed using a generic transform pointer
using GenericTransform   = itk::Transform<double, 3>;
using TransformPtr       = GenericTransform::Pointer;

/// Affine transforms are used for many Image manipulation commands
using AffineTransform    = itk::AffineTransform<double, 3>;
using AffineTransformPtr = AffineTransform::Pointer;

/// For deliberate conversions between types
Point toPoint(const Dims &d);
Point toPoint(const Coord &c);
Vector toVector(const Dims &d);

/// Enables `makeVector({1,2,3});`, construction using an initializer list (likely an accidental omission in current ITK version)
Vector3 makeVector(std::array<double, 3>&& arr);

/// Vector dot and cross products
Vector3 dot(const Vector3 &a, const Vector3 &b);
Vector3 cross(const Vector3 &a, const Vector3 &b);

/// Handy functions to perform operations on Points and Vectors with which ITK is too restrictive
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
