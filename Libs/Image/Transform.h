#pragma once

#include "Shapeworks.h"
 
#include <itkFixedCenterOfRotationAffineTransform.h>

namespace shapeworks {

class Transform
{
public:
  using TransformType = itk::FixedCenterOfRotationAffineTransform<double, 3>;

  Transform() : rotangle(0.0) {}

  TransformType::Pointer get() const;

  void reset();
  void translate(bool centerofmass, float tx, float ty, float tz); 
  void rotate(const Vector3 &axis, double angle);
  void scale(const Vector3 &s);

  void print() const;

private:
  Vector3 scaling;
  Vector3 translation;
  Vector3 rotaxis;
  double rotangle;
};

} // shapeworks
