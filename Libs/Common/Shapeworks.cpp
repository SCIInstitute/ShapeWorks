#include "Shapeworks.h"

namespace shapeworks {

Vector3 makeVector3(std::array<double, 3>&& arr) { return Vector3(arr.data()); }

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

}; //shapeworks
