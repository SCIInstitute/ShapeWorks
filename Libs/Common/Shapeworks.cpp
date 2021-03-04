#include "Shapeworks.h"

namespace shapeworks {

Point toPoint(const Dims &d) { return Point({static_cast<double>(d[0]), static_cast<double>(d[1]), static_cast<double>(d[2])}); }
Point toPoint(const Coord &c) { return Point({static_cast<double>(c[0]), static_cast<double>(c[1]), static_cast<double>(c[2])}); }
Vector toVector(const Dims &d) { return makeVector({static_cast<double>(d[0]), static_cast<double>(d[1]), static_cast<double>(d[2])}); }
Vector toVector(const Point &p) { return makeVector({p[0], p[1], p[2]}); } 
Vector toVector(const itk::CovariantVector<double, 3> &v) { return makeVector({v[0], v[1], v[2]}); } 
Point toPoint(const Vector &v) { return Point({v[0], v[1], v[2]}); }
Coord toCoord(const Dims &d) {
  return Coord({static_cast<itk::IndexValueType>(d[0]),
                static_cast<itk::IndexValueType>(d[1]),
                static_cast<itk::IndexValueType>(d[2])}); }
Dims toDims(const Coord &c) {
  return Dims({static_cast<itk::SizeValueType>(c[0]),
               static_cast<itk::SizeValueType>(c[1]),
               static_cast<itk::SizeValueType>(c[2])}); }
Coord toCoord(const Point &p) {
  return Coord({static_cast<itk::IndexValueType>(p[0]),
                static_cast<itk::IndexValueType>(p[1]),
                static_cast<itk::IndexValueType>(p[2])}); }

/// Enables construction using an initializer list: `Vector3 f() { return makeVector({1,2,3}); }`
/// itkVector doesn't have this handy ctor like itkPoint; `Point p({a,b,c})` works, but `Vector3 v({1,2,3})` doesn't.
Vector3 makeVector(std::array<double, 3>&& arr) { return Vector3(arr.data()); }

Plane makePlane(const Vector3 &n, const Point &o)
{
  Plane plane = Plane::New();
  plane->SetNormal(n[0], n[1], n[2]);
  plane->SetOrigin(o[0], o[1], o[2]);

  return plane;
}

template<>
Vector3 negate(const Vector3 &v) { return makeVector({-v[0], -v[1], -v[2]}); }

template<>                                            
Vector3 invertValue(const Vector3 &v) { return makeVector({1.0/v[0], 1.0/v[1], 1.0/v[2]}); }
                                                    
Vector3 crossProduct(const Vector3 &a, const Vector3 &b)
{
  return makeVector({a[1]*b[2] - a[2]*b[1], -(a[0]*b[2] - a[2]*b[0]), a[0]*b[1] - a[1]*b[0]});
}

Vector3 dotProduct(const Vector3 &a, const Vector3 &b)
{
  return makeVector({a[0]*b[0], a[1]*b[1], a[2]*b[2]});
}

Point3 operator+(const Point3 &p, const Point3 &q)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

Point3& operator+=(Point3 &p, const Point3 &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

Point3 operator/(const Point3 &p, const double x)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return ret;
}

Point3& operator/=(Point3 &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

itk::Index<3> toIndex(const IPoint3 &pt) 
{ 
  return itk::Index<3>({pt[0], pt[1], pt[2]}); 
}

itk::Size<3> toSize(const IPoint3 &pt) 
{ 
  return itk::Size<3>({static_cast<itk::SizeValueType>(pt[0]), static_cast<itk::SizeValueType>(pt[1]), static_cast<itk::SizeValueType>(pt[2])}); 
}

bool axis_is_valid(const Vector3 &axis)
{
  const double eps = 1E-6;
  return axis.GetSquaredNorm() > eps;
}

bool axis_is_valid(const Axis &axis)
{
  return axis >= Axis::X && axis <= Axis::Z;
}

Axis toAxis(const std::string &str)
{
  if (str == "X" || str == "x") return Axis::X;
  if (str == "Y" || str == "y") return Axis::Y;
  if (str == "Z" || str == "z") return Axis::Z;
  return Axis::invalid;
}

double degToRad(const double deg)
{
  return deg * Pi / 180.0;
}

TransformPtr createTransform(const Matrix33 &mat, const Vector3 &translate) 
{
  AffineTransformPtr xform(AffineTransform::New());
  xform->SetMatrix(mat);
  xform->SetOffset(translate);
  return xform;
}

MeshTransform createMeshTransform(const vtkSmartPointer<vtkMatrix4x4> &mat)
{
  MeshTransform xform(MeshTransform::New());
  xform->SetMatrix(mat);
  return xform;
}

}; //shapeworks
