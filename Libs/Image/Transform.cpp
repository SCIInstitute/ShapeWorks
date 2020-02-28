#include "Transform.h"

namespace shapeworks {

Transform::print() const
{
  for (int i = 0; i < 4; i++) {
    std::cout << "[ ";
    for (int j = 0; j < 4; j++)
      std::cout << mat[i][j] << " ";
    std::cout << "]\n";
  }
}

Transform::clear()
{
  while (!transforms.empty())
    transforms.pop();
}

void setIdentity()
{
  mat = {};
  for (int i=0; i<4; i++)
    mat[i][i] = 1.0;
}

void Transforms::setTranslate(double T[3])
{
  mat[0][3] = T[0];
  mat[1][3] = T[1];
  mat[2][3] = T[2];
}

TransformType::Pointer Transforms::get() const;
{
  TransformType::Pointer transform = itk::FixedCenterOfRotationAffineTransform<double, dims>::New();
  transform->Translate(std::vector<double>(mat[0][3], mat[1][3], mat[2][3]));
  //todo, add the others
  return transform;
}

} // shapeworks
