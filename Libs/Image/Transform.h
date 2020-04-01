#pragma once

#include "Shapeworks.h"
#include <itkFixedCenterOfRotationAffineTransform.h>

namespace shapeworks {

class Transform
{
public:
  using TransformType = itk::FixedCenterOfRotationAffineTransform<double, 3>;

  Transform() : rotangle(0.0) { reset(); }

  TransformType::Pointer get() const;

  void reset();
  void translate(const Vector3 &v); 
  void rotate(const Vector3 &axis, double angle);
  void scale(const Vector3 &s);

  void print() const; //todo: operator << and operator >>

private:
  Vector3 scaling;
  Vector3 translation;
  Vector3 rotaxis;
  double rotangle;
};

} // shapeworks
