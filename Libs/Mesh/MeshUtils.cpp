#include "MeshUtils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>

namespace shapeworks {

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createIcpTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                  const vtkSmartPointer<vtkPolyData> target,
                                                                  const std::string type,
                                                                  const unsigned iterations)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source);
  icp->SetTarget(target);

  if (type.compare("rigid") == 0)
    icp->GetLandmarkTransform()->SetModeToRigidBody();
  else if (type.compare("similarity") == 0)
    icp->GetLandmarkTransform()->SetModeToSimilarity();
  else
    icp->GetLandmarkTransform()->SetModeToAffine();

  icp->SetMaximumNumberOfIterations(iterations);
  // ICPRigid3DMeshRegistration uses this (check results while testing)
  // icp->StartByMatchingCentroidsOn();
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  // ICPRigid3DMeshRegistration  does not invert the matrix (check results while testing)
  vtkMatrix4x4::Invert(icp->GetMatrix(), m);
  return m;
}

vtkTransform MeshUtils::createRegistrationTransform(const std::unique_ptr<Mesh> &sourceMesh, const std::unique_ptr<Mesh> &targetMesh, const std::string type, unsigned iterations)
{
  const vtkSmartPointer<vtkMatrix4x4> mat(MeshUtils::createIcpTransform(sourceMesh->getMesh(), targetMesh->getMesh(), type, iterations));
  return createvtkTransform(mat);
}

} // shapeworks
