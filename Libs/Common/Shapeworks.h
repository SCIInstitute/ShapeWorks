#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>
#include <itkAffineTransform.h>
#include <itkIdentityTransform.h>

namespace shapeworks {

/// pi that doesn't depend on deprecated or non-std lib defines
const auto Pi = std::atan(1.0) * 4.0;

/// Simple names for common types used in the framework
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

/// Enables `makeVector({1,2,3});`, construction using an initializer list (likely an accidental omission in current ITK version)
Vector3 makeVector(std::array<double, 3>&& arr);

/// All transforms can be accessed using a generic transform pointer
using GenericTransform   = itk::Transform<double, 3>;
using IdentityTransform  = itk::IdentityTransform<double, 3>;
using TransformPtr       = GenericTransform::Pointer;

/// Affine transforms are used for many Image manipulation commands
using AffineTransform    = itk::AffineTransform<double, 3>;
using AffineTransformPtr = AffineTransform::Pointer;
AffineTransformPtr createAffineTransform(const Matrix33 &mat, const Vector3 &translate = makeVector({0,0,0}));

/// For deliberate conversions between types
Point toPoint(const Dims &d);
Point toPoint(const Coord &c);
Vector toVector(const Dims &d);
Vector toVector(const Point &p);
Point toPoint(const Vector &v);

/// Negation operator (ITK only has it for Vectors, but sometimes useful for Points)
template<typename P>
P negate(P &&p) { return P({-p[0], -p[1], -p[2]}); }

/// Inversion function for all but Vector
template<typename P>
P invert(P &&p) { return P({1.0/p[0], 1.0/p[1], 1.0/p[2]}); }

/// Inversion function for Vector (requires makeVector)
template<>
Vector3 invert(Vector3 &&v);

/// Vector dot and cross products
Vector3 dot(const Vector3 &a, const Vector3 &b);
Vector3 cross(const Vector3 &a, const Vector3 &b);

/// handy way to specify an axis
enum Axis { invalid = -1, X, Y, Z };
Axis toAxis(const std::string &str);

/// Ensure an axis is valid
bool axis_is_valid(const Vector3 &axis);
bool axis_is_valid(const Axis &axis);

/// convert degrees to radians
double degToRad(const double deg);

/// Handy functions to perform operations on Points and Vectors with which ITK is more restrictive
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
P& operator+=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

template <typename P>
P& operator-=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] -= q[i];
  return p;
}

template <typename P>
P operator*(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] * x;
  return std::move(ret);
}

template <typename P>
P operator/(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return std::move(ret);
}

template <typename P>
P& operator*=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] *= x;
  return p;
}

template <typename P>
P& operator/=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

template <typename P>
bool epsEqual(const P &a, const P &b, const typename P::ValueType &eps)
{
  return std::abs(a[0]-b[0]) < eps && std::abs(a[1]-b[1]) < eps && std::abs(a[2]-b[2]) < eps;
}

} // shapeworks
