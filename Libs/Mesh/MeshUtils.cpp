#include "MeshUtils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>

namespace shapeworks {

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createIcpTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                  const vtkSmartPointer<vtkPolyData> target,
                                                                  const unsigned iterations)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source);
  icp->SetTarget(target);
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfIterations(iterations);
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(icp->GetMatrix(), m);
  return m;
}

} // shapeworks
