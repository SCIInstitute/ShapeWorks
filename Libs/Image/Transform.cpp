#include "Transform.h"

namespace shapeworks {

void Transform::print() const
{
  TransformType::Pointer xform = this->get();
  std::cout << xform << std::endl;
}

void Transform::reset()
{
  scaling = Point3();
  translation = Vector3();
  rotaxis = Vector3();
  rotangle = 0.0;
}

void Transforms::translate(const Vector3 &v)
{
  translation += v;
}

void rotate(const Vector3 &axis, double angle)
{
  //note: this should be accumulative like scale and translate, but it's tricker for rotation
  rotaxis = axis;
  rotangle = angle;
}

void scale(const Point3 &s)
{
  scale *= s;
}

TransformType::Pointer Transforms::get() const;
{
  TransformType::Pointer transform = TransformType::New();
  transform->Translate(std::vector<double>(mat[0][3], mat[1][3], mat[2][3]));
  transform->Scale(std::vector<double>(mat[0][0], mat[1][1], mat[2][2]));
  transform->Rotate3D(rotaxis, angle);
  return transform;
}

} // shapeworks
