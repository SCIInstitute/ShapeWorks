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
  const vtkSmartPointer<vtkMatrix4x4> mat(MeshUtils::createIcpTransform(sourceMesh->getVTKMesh(), targetMesh->getVTKMesh(), type, iterations));
  return createvtkTransform(mat);
}

vtkSmartPointer<vtkPlane> MeshUtils::createPlane(const Vector3 &n, const Point &o)
{
  vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetNormal(n[0], n[1], n[2]);
  plane->SetOrigin(o[0], o[1], o[2]);

  return plane;
}

Region MeshUtils::boundingBox(std::vector<std::string> &filenames, bool center)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided to compute a bounding box");
  
  Mesh mesh(filenames[0]);
  Region bbox(mesh.boundingBox());

  for (auto filename : filenames)
  {
    Mesh mesh(filename);
    bbox.grow(mesh.boundingBox());
  }

  return bbox;
}

Region MeshUtils::boundingBox(std::vector<Mesh> &meshes, bool center)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute a bounding box");

  Region bbox(meshes[0].boundingBox());

  for (auto mesh : meshes)
    bbox.grow(mesh.boundingBox());

  return bbox;
}

} // shapeworks
