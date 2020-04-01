#include "Shapeworks.h"

shapeworks::Point3 shapeworks::operator+(const shapeworks::Point3 &p, const shapeworks::Point3 &q)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

shapeworks::Point3& shapeworks::operator+=(shapeworks::Point3 &p, const shapeworks::Point3 &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

shapeworks::Point3 shapeworks::operator/(const shapeworks::Point3 &p, const double x)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return ret;
}

shapeworks::Point3& shapeworks::operator/=(shapeworks::Point3 &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}
