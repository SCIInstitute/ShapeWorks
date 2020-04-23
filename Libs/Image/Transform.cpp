#include "Shapeworks.h"
#include "Image.h"
#include "Transform.h"

namespace shapeworks {

/// reset
///
/// resets values of translate, scale and rotate
void Transform::reset()
{
  double translate[3] = {0.0, 0.0, 0.0};
  double scale[3] = {1.0, 1.0, 1.0};
  double rotate[3] = {0.0, 0.0, 1.0};
  Matrix matrix;
  scaling = Vector3(scale);
  translation = Vector3(translate);
  rotaxis = Vector3(rotate);
  rotangle = 0.0;
  matrix.Fill(0);
}

/// translate
///
/// updates translate
///
/// \param v      translation
void Transform::translate(const Vector3 &v)
{
  translation += v;
}

/// rotate
///
/// updates rotate
///
/// \param axis      axis of rotation
/// \param angle     angle of rotation
void Transform::rotate(const Vector3 &axis, double angle)
{
  //note: this should be accumulative like scale and translate, but it's tricker for rotation
  rotaxis = axis;
  rotangle = angle;
}

/// scale
///
/// updates translate
///
/// \param s      scaling factor
void Transform::scale(const Vector3 &s)
{
  scaling = scaling * s;
}

void Transform::setMatrix(const Matrix mat)
{
  matrix = mat;
}

/// getItkTransform
///
/// performs translation, scaling and rotation
Transform::itkTransformType::Pointer Transform::getItkTransform() const
{
  itkTransformType::Pointer transform = itkTransformType::New();
  transform->Translate(-translation);       // -translation b/c itk transformations are based on output image space
  transform->Scale(scaling);
  transform->Rotate3D(rotaxis, rotangle);
  transform->SetMatrix(matrix);

  return transform;
}

/// operator<<
///
/// Stream insertion operator
/// Prints translation, scaling, axis of rotation and angle of rotation
std::ostream& operator<<(std::ostream &os, const Transform &t)
{
  return os << "shapeworks::Transform {\n\ttranslate: "
            << t.translation << ",\n\tscale: " << t.scaling
            << ",\n\trotation: " << t.rotangle << ",\n\trot_axis: "
            << t.rotaxis << "\nMatrix: " << t.matrix << "}";
}

} // shapeworks
