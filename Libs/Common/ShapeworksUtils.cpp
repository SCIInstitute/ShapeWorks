#include "ShapeworksUtils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>

namespace shapeworks {

bool ShapeworksUtils::icp(vtkSmartPointer<vtkPolyData> target, vtkSmartPointer<vtkPolyData> moving, unsigned iterations)
{
  using icpTransform = vtkSmartPointer<vtkIterativeClosestPointTransform>;
  icpTransform icp = icpTransform::New();
  icp->SetSource(moving);
  icp->SetTarget(target);
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfIterations(iterations);
  icp->Modified();
  icp->Update();

  using TransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>;
  TransformFilter icpTransformFilter = TransformFilter::New();
  icpTransformFilter->SetInputData(moving);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m1 = icp->GetMatrix();
  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(m1, m);
  std::cout << "The resulting matrix is: " << *m << std::endl;
}

} // shapeworks