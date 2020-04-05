#include "Shapeworks.h"
#include "Image.h"
#include "Transform.h"

namespace shapeworks {

void Transform::reset()
{
  double translate[3] = {0.0, 0.0, 0.0};
  double scale[3] = {1.0, 1.0, 1.0};
  double rotate[3] = {0.0, 0.0, 1.0};
  scaling = Vector3(scale);
  translation = Vector3(translate);
  rotaxis = Vector3(rotate);
  rotangle = 0.0;
}

void Transform::translate(const Vector3 &v)
{
  std::cout << v;
  translation += v;
#if DEBUG_CONSOLIDATION
  std::cout << v << std::endl;
  std::cout << "Translate succeeded!\n";
#endif
}

void Transform::rotate(const Vector3 &axis, double angle)
{
  //note: this should be accumulative like scale and translate, but it's tricker for rotation
  rotaxis = axis;
  rotangle = angle;
}

void Transform::scale(const Vector3 &s)
{
  scaling = scaling * s;
}

Transform::itkTransformType::Pointer Transform::getItkTransform() const
{
  itkTransformType::Pointer transform = itkTransformType::New();
  transform->Translate(-translation);       // -translation b/c itk transformations are based on output image space
  // transform->Scale(scaling);             // TODO
  // transform->Rotate3D(rotaxis, rotangle);//TODO

#if DEBUG_CONSOLIDATION
  std::cout << "rawTransform: " << *this << std::endl;
  std::cout << "itkTransform: " << *transform << std::endl;
#endif
  return transform;
}

std::ostream& operator<<(std::ostream &os, const Transform &t)
{
  return os << "shapeworks::Transform {\n\ttranslate: "
            << t.translation << ",\n\tscale: " << t.scaling
            << ",\n\trotation: " << t.rotangle << ",\n\trot_axis: "
            << t.rotaxis << "\n}";
}

} // shapeworks
