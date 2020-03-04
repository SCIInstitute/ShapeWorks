#include "Shapeworks.h"
#include "Image.h"
#include "Transform.h"

namespace shapeworks {

void Transform::print() const
{
  TransformType::Pointer xform = this->get();
  std::cout << xform << std::endl;
}

void Transform::reset()
{
  scaling = Vector3();
  translation = Vector3();
  rotaxis = Vector3();
  rotangle = 0.0;
}

void Transform::translate(bool centerofmass, float tx, float ty, float tz)
{
  // translation += v;
}

void Transform::rotate(const Vector3 &axis, double angle)
{
  //note: this should be accumulative like scale and translate, but it's tricker for rotation
  rotaxis = axis;
  rotangle = angle;
}

void Transform::scale(const Vector3 &s)
{
  //todo
//   scaling = scaling * s;
}

Transform::TransformType::Pointer Transform::get() const
{
  TransformType::Pointer transform = TransformType::New();
  transform->Translate(translation);
  transform->Scale(scaling);
  transform->Rotate3D(rotaxis, rotangle);
  
  return transform;
}

} // shapeworks
