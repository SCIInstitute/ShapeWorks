#include "ShapeworksUtils.h"

#include <sys/stat.h>

namespace shapeworks {

bool ShapeworksUtils::is_directory(const std::string &pathname)
{
  struct stat info;
  if (stat(pathname.c_str(), &info) != 0) {
    return false;
  }
  else if (info.st_mode & S_IFDIR) {
    return true;
  }
  return false;
}

Matrix33 ShapeworksUtils::getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat)
{
  Matrix33 m;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      m[i][j] = mat->GetElement(i, j);
    }
  }

  return m;
}

Vector3 ShapeworksUtils::getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat)
{
  return makeVector({mat->GetElement(0,3), mat->GetElement(1,3), mat->GetElement(2,3)});
}

} // shapeworks
