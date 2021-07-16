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
#include <tbb/parallel_for.h>

namespace shapeworks {

// locking to handle non-thread-safe code
static tbb::mutex mesh_mutex;

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createICPTransform(const Mesh source,
                                                                  const Mesh target,
                                                                  Mesh::AlignmentType align,
                                                                  const unsigned iterations,
                                                                  bool meshTransform)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source.getVTKMesh());
  icp->SetTarget(target.getVTKMesh());

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
  icpTransformFilter->SetInputData(source.getVTKMesh());
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

PhysicalRegion MeshUtils::boundingBox(std::vector<std::string> &filenames, bool center)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided to compute a bounding box");

  Mesh mesh(filenames[0]);
  PhysicalRegion bbox(mesh.boundingBox());

  for (auto filename : filenames) {
    Mesh mesh(filename);
    bbox.expand(mesh.boundingBox());
  }

  return bbox;
}

PhysicalRegion MeshUtils::boundingBox(std::vector<Mesh> &meshes, bool center)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute a bounding box");

  PhysicalRegion bbox(meshes[0].boundingBox());

  for (auto mesh : meshes)
    bbox.expand(mesh.boundingBox());

  return bbox;
}

int MeshUtils::findReferenceMesh(std::vector<Mesh>& meshes)
{
  std::vector<std::pair<int, int>> pairs;

  // enumerate all pairs of meshes
  for (int i = 0; i < meshes.size(); i++) {
    for (int j = i + 1; j < meshes.size(); j++) {
      pairs.push_back(std::make_pair(i, j));
    }
  }

  // map of pair to distance value
  std::map<int, double> results;
  // mutex for access to results
  tbb::mutex mutex;

  tbb::parallel_for(
    tbb::blocked_range<size_t>{0, pairs.size()},
    [&](const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i < r.end(); ++i) {

        auto pair = pairs[i];

        vtkSmartPointer<vtkPolyData> poly_data1 = vtkSmartPointer<vtkPolyData>::New();
        poly_data1->DeepCopy(meshes[pair.first].getVTKMesh());
        vtkSmartPointer<vtkPolyData> poly_data2 = vtkSmartPointer<vtkPolyData>::New();
        poly_data2->DeepCopy(meshes[pair.second].getVTKMesh());

        // register the two meshes
        auto matrix = MeshUtils::createICPTransform(poly_data1,
                                                    poly_data2, Mesh::Rigid,
                                                    10, true);
        // transform
        auto transform = createMeshTransform(matrix);
        Mesh transformed = meshes[pair.first];
        transformed.applyTransform(transform);

        // compute distance
        double distance = transformed.distance(meshes[pair.second]).getFieldMean("distance");
        {
          // lock and store results
          tbb::mutex::scoped_lock lock(mutex);
          results[i] = distance;
        }
      }
    });

  std::vector<double> sums(meshes.size(), 0);
  std::vector<int> counts(meshes.size(), 0);
  std::vector<double> means(meshes.size(), 0);

  double count = meshes.size() - 1;
  for (int i = 0; i < pairs.size(); i++) {
    auto pair = pairs[i];
    double result = results[i];
    sums[pair.first] += result;
    sums[pair.second] += result;
    counts[pair.first]++;
    counts[pair.second]++;
    means[pair.first] += result / count;
    means[pair.second] += result / count;
  }

  auto smallest = std::min_element(means.begin(), means.end());

  return std::distance(means.begin(), smallest);
}

std::vector<Point3> computeMeanNormals(std::vector<Mesh> &meshes)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute average normals");
  auto num_normals(meshes[0].numPoints());

  // convert all normals from all meshes to spherical coords
  std::vector<std::vector<Point3>> sphericals(num_normals, std::vector<Point3>(meshes.size()));
  for (int j = 0; j < meshes.size(); j++)
  {
    if (meshes[j].numPoints() != num_normals)
      throw std::invalid_argument("Input meshes do not all have the same number of points");

    auto normals = mesh.getField<vtkDataArray>("Normals");
    if (num_normals != normals.GetSize())
      throw std::invalid_argument("Expected a normal for every point in mesh");
    
    for (int i = 0; i < num_normals; i++)
    {
      auto n = normals->GetTuple3(i);

      // note: Utils::cartesian2spherical returns atypical (r, phi, theta)
      Utils::cartesian2spherical(n, sphericals[i][j].GetDataPointer());
    }
  }

  // compute average value for the normals of each point
  // - prep data in 1d theta/phi arrays first for averageThetaArc function
  std::vector<double> phis(num_normals);
  std::vector<double> thetas(num_normals);
  for (int i = 0; i < num_normals; i++)
  {
    phis[i] = sphericals[i][1];
    thetas[i] = sphericals[i][2];
  }
  std::vector<Point3> mean_normals(num_normals);
  for (int i = 0; i < num_normals; i++)
  {
    Point3 avg_spherical_normal(1.0,
                                Utils::averageThetaArc(phis[i]),
                                Utils::averageThetaArc(thetas[i]));

    // note: Utils::spherical2cartesian expects atypical (r, phi, theta)
    Utils::spherical2cartesian(avg_spherical_normal.GetDataPointer(),
                               mean_normals[i].GetDataPointer());
  }
  
  return mean_normals;
}

} // shapeworks
