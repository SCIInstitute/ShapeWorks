#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>
#include <itkAffineTransform.h>

namespace shapeworks {

const auto Pi = atan(1.0) * 4.0;  // const double pi() { return std::atan(1)*4; } // ...but doesn't compile on OSX

using Coord         = itk::Index<3>;
using Dims          = itk::Size<3>;
using Point3        = itk::Point<double, 3>;
using Vector3       = itk::Vector<double, 3>;
using Matrix44      = itk::Matrix<double, 4, 4>;
using Matrix33      = itk::Matrix<double, 3, 3>;
using IPoint3       = itk::Point<int, 3>;
using FPoint3       = itk::Point<float, 3>;
using TransformType = itk::AffineTransform<double, 3>;
using TransformPtr  = TransformType::Pointer;
using Vector        = Vector3;
using Point         = Point3;
using Matrix        = Matrix33;

Point toPoint(const Dims &d);
Point toPoint(const Coord &c);
Vector toVector(const Dims &d);
Vector toVector(const Point &p);
Point toPoint(const Vector &v);

/// Negation operator (ITK only has it for Vectors, but sometimes useful for Points)
template<typename P>
P negate(P &&p) { return P({-p[0], -p[1], -p[2]}); }

/// Enables `makeVector({1,2,3});`, construction using an initializer list.
Vector3 makeVector(std::array<double, 3>&& arr);

/// Inversion operator for all but Vector
template<typename P>
P invert(P &&p) { return P({1.0/p[0], 1.0/p[1], 1.0/p[2]}); }

/// Inversion operator for Vector (requires makeVector)
template<>
Vector3 invert(Vector3 &&v);

/// Vector cross product
Vector3 cross(const Vector3 &a, const Vector3 &b);

/// Vector dot product
Vector3 dot(const Vector3 &a, const Vector3 &b);

/// Ensure an axis is valid
bool axis_is_valid(const Vector3 &axis);

/// convert degrees to radians
double degToRad(const double deg);

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

template <typename P>
bool epsEqual(const P &a, const P &b, const typename P::ValueType &eps)
{
  return std::abs(a[0]-b[0]) < eps && std::abs(a[1]-b[1]) < eps && std::abs(a[2]-b[2]) < eps;
}

} // shapeworks
