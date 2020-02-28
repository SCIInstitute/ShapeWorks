#pragma once

#include "Shapeworks.h"
#include "Image.h"
#include <itkTransform.h>

namespace shapeworks {

class Transform
{
public:
  static const unsigned dims = 3;
  using TransformType = itk::Transform<double, dims>;

  Transform() { setIdentity(); }

  TransformType::Pointer get() const;

  void setIdentity();
  void setTranslate(double T[3]);
  void setRotate(double R[3]);
  void setScale(double S[3]);

  void print() const;

private:
  double mat[4][4];
};

} // shapeworks
