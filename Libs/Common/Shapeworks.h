#pragma once

#include <itkPoint.h>
#include <itkVector.h>
#include <itkCovariantVector.h>
#include <itkMatrix.h>
#include <itkSize.h>
#include <itkIndex.h>
#include <itkAffineTransform.h>
#include <itkIdentityTransform.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkPlane.h>

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
using Plane         = vtkSmartPointer<vtkPlane>;
using Array         = vtkSmartPointer<vtkDataArray>;
using Field         = Array;

/// Enables `makeVector({1,2,3});`, construction using an initializer list (likely an accidental omission in current ITK version)
Vector3 makeVector(std::array<double, 3>&& arr);

/// Type of transform used for Images or Meshes
typedef enum XFormType { CenterOfMass, IterativeClosestPoint } XFormType;

/// All transforms can be accessed using a generic transform pointer
using GenericTransform   = itk::Transform<double, 3>;
using IdentityTransform  = itk::IdentityTransform<double, 3>;
using TransformPtr       = GenericTransform::Pointer;
TransformPtr createTransform(const Matrix33 &mat, const Vector3 &translate = makeVector({0,0,0}));

/// Make a plane
Plane makePlane(const Point &p, const Vector3 &n);
Plane makePlane(const Point &p0, const Point &p1, const Point &p2);

/// Get origin and normal of plane
Point getOrigin(const Plane plane);
Vector3 getNormal(const Plane plane);

/// Affine transforms are used for many Image manipulation commands
using AffineTransform    = itk::AffineTransform<double, 3>;
using AffineTransformPtr = AffineTransform::Pointer;

/// Mesh transforms
using MeshTransform = vtkSmartPointer<vtkTransform>;
MeshTransform createMeshTransform(const vtkSmartPointer<vtkMatrix4x4> &mat);

/// For deliberate conversions between types
Point toPoint(const Dims &d);
Point toPoint(const Coord &c);
Vector toVector(const Dims &d);
Vector toVector(const Point &p);
Vector toVector(const itk::CovariantVector<double, 3> &v);
Point toPoint(const Vector &v);
Coord toCoord(const Dims &d);
Dims toDims(const Coord &c);
Dims toDims(const Point &p);
Coord toCoord(const Point &p);

/// Negation operator (ITK only has it for Vectors, but sometimes useful for Points)
template<typename P>
P negate(const P &p) { return P({-p[0], -p[1], -p[2]}); }

/// Negate function for Vector (requires makeVector)
template<>
Vector3 negate(const Vector3 &v);

/// Inversion function for all but Vector
template<typename P>
P invertValue(const P &p) { return P({1.0/p[0], 1.0/p[1], 1.0/p[2]}); }

/// Inversion function for Vector (requires makeVector)
template<>
Vector3 invertValue(const Vector3 &v);

/// Vector dot and cross products
Vector3 dotProduct(const Vector3 &a, const Vector3 &b);
Vector3 crossProduct(const Vector3 &a, const Vector3 &b);
double length(const Vector3 &v);

/// handy way to specify an axis
enum Axis { invalid = -1, X, Y, Z };
Axis toAxis(const std::string &str);

/// Ensure an axis is valid
bool axis_is_valid(const Vector3 &axis);
bool axis_is_valid(const Axis &axis);

/// convert degrees to radians
double degToRad(const double deg);

/// Handy functions to perform operations on Points and Vectors with which ITK is more restrictive
class Image;

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator+(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator-(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] - q[i];
  return ret;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Vector, P>::value || // use operator*(v0, v1); (or call dotProduct)
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator*(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] * q[i];
  return ret;
}


template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Vector, P>::value || // use operator/(v0, v1); 
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator/(const P &p, const P &q)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / q[i];
  return ret;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P& operator+=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P& operator-=(P &p, const P &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] -= q[i];
  return p;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator*(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] * x;
  return std::move(ret);
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P operator/(const P &p, const double x)
{
  P ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return std::move(ret);
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P& operator*=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] *= x;
  return p;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
P& operator/=(P &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

template<typename T>
bool epsEqual(T a, T b, T epsilon)
{
  return std::abs(a-b) < epsilon;
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Vector, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
bool epsEqual(const P &a, const P &b, const typename P::ValueType &eps)
{
  return std::abs(a[0]-b[0]) < eps && std::abs(a[1]-b[1]) < eps && std::abs(a[2]-b[2]) < eps;
}

// https://stackoverflow.com/a/17382806/207044
template<typename P>
bool equalNSigDigits(P a, P b, int n = 4)
{
  return std::abs(a - b) <= pow(0.1, n) * std::max(std::abs(a), std::abs(b));
}

template<typename P, typename = std::enable_if_t<std::is_same<Image, P>::value ||
                                                 std::is_same<Coord, P>::value ||
                                                 std::is_same<Dims, P>::value ||
                                                 std::is_same<Vector, P>::value ||
                                                 std::is_same<Point, P>::value ||
                                                 std::is_same<IPoint3, P>::value ||
                                                 std::is_same<FPoint3, P>::value> >
bool epsEqualN(const P &a, const P &b, int n = 4)
{
  return equalNSigDigits(a[0], b[0], n) && equalNSigDigits(a[1], b[1], n) && equalNSigDigits(a[2], b[2], n);
}

} // shapeworks
