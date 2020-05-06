#include "ShapeworksUtils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>

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

Matrix ShapeworksUtils::icp(const vtkSmartPointer<vtkPolyData> target, const vtkSmartPointer<vtkPolyData> moving, const unsigned iterations)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(moving);
  icp->SetTarget(target);
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfIterations(iterations);
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(moving);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  Matrix xmat = getMatrix(icp->GetMatrix());

  return xmat;
}

Matrix33 ShapeworksUtils::getMatrix(const vtkSmartPointer<vtkMatrix4x4> mat)
{
  Matrix m;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      m[i][j] = mat->GetElement(i, j);
    }
  }

  return m;
}

} // shapeworks
