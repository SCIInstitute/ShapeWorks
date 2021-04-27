#include "MeshUtils.h"
#include "ParticleSystem.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <igl/biharmonic_coordinates.h>
#include <igl/cat.h>
#include <igl/cotmatrix.h>
#include <igl/matrix_to_list.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>

// tbb
#include <tbb/mutex.h>


namespace shapeworks {

// locking to handle non-thread-safe code
static tbb::mutex mesh_mutex;

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createICPTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                  const vtkSmartPointer<vtkPolyData> target,
                                                                  Mesh::AlignmentType align,
                                                                  const unsigned iterations,
                                                                  bool meshTransform)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source);
  icp->SetTarget(target);

  if (align == Mesh::Rigid)
    icp->GetLandmarkTransform()->SetModeToRigidBody();
  else if (align == Mesh::Similarity)
    icp->GetLandmarkTransform()->SetModeToSimilarity();
  else
    icp->GetLandmarkTransform()->SetModeToAffine();

  icp->SetMaximumNumberOfIterations(iterations);
  if (meshTransform)
    icp->StartByMatchingCentroidsOn();
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  if (meshTransform)
    m = icp->GetMatrix();
  else
    vtkMatrix4x4::Invert(icp->GetMatrix(), m);

  return m;
}

Mesh MeshUtils::threadSafeReadMesh(std::string filename)
{
  tbb::mutex::scoped_lock lock(mesh_mutex);
  Mesh mesh(filename);
  return mesh;
}

void MeshUtils::threadSafeWriteMesh(std::string filename, Mesh mesh)
{
  tbb::mutex::scoped_lock lock(mesh_mutex);
  mesh.write(filename);
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
