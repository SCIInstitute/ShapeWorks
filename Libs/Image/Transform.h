#pragma once

#include "Shapeworks.h"
#include <itkFixedCenterOfRotationAffineTransform.h>

namespace shapeworks {

class Transform
{
public:
  using itkTransformType = itk::FixedCenterOfRotationAffineTransform<double, 3>;

  Transform() : rotangle(0.0) { reset(); }

  itkTransformType::Pointer getItkTransform() const;

  void reset();
  void translate(const Vector3 &v); 
  void rotate(const Vector3 &axis, double angle); // must specify angle in radians
  void scale(const Vector3 &s);
  void setMatrix(const Matrix mat);

private:
  Vector3 scaling;
  Vector3 translation;
  Vector3 rotaxis;
  double rotangle;
  itkTransformType::MatrixType matrix;

  friend std::ostream& operator<<(std::ostream &os, const Transform &t);
};

std::ostream& operator<<(std::ostream &os, const Transform &t);

} // shapeworks
