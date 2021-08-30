#include "MeshUtils.h"
#include "ParticleSystem.h"
#include "Utils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkDoubleArray.h>
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

PhysicalRegion MeshUtils::boundingBox(const std::vector<std::string>& filenames, bool center)
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

PhysicalRegion MeshUtils::boundingBox(const std::vector<std::reference_wrapper<const Mesh>>& meshes, bool center)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute a bounding box");

  PhysicalRegion bbox(meshes[0].get().boundingBox());

  for (auto mesh : meshes)
    bbox.expand(mesh.get().boundingBox());

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

void MeshUtils::generateNormals(const std::vector<std::reference_wrapper<Mesh>>& meshes, bool forceRegen)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute average normals");

  for (int i = 0; i < meshes.size(); i++)
  {
    bool hasNormals = true;
    try {
      meshes[i].get().getField<vtkDataArray>("Normals");
    }
    catch (...) {
      hasNormals = false;
    }

    if ((!hasNormals) || (hasNormals  && forceRegen))
      meshes[i].get().computeNormals();
  }
}

Field MeshUtils::computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided to compute mean normals");

  std::vector<Mesh> meshes;
  meshes.reserve(filenames.size()); // create a vector large enough for all the meshes that will be loaded
  for (auto filename : filenames)
    meshes.push_back(Mesh(filename));

  std::vector<std::reference_wrapper<Mesh>> rmeshes;
  rmeshes.reserve(meshes.size());
  for (Mesh& mesh : meshes)
    rmeshes.push_back(std::reference_wrapper<Mesh>(mesh));

  if (autoGenerateNormals)
  {
    std::cerr << "NOTE: Auto generating normals\n";
    MeshUtils::generateNormals(rmeshes);
  }

  std::vector<std::reference_wrapper<const Mesh>> cmeshes;
  for (Mesh& mesh : meshes)
    cmeshes.push_back(std::reference_wrapper<const Mesh>(mesh));

  return computeMeanNormals(cmeshes);
}

Field MeshUtils::computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh>>& meshes)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute average normals");

  auto num_normals = meshes[0].get().numPoints();
  auto num_meshes = meshes.size();

  // convert all normals from all meshes to spherical coords
  std::vector<std::vector<Point3>> sphericals(num_normals, std::vector<Point3>(num_meshes));
  for (int j = 0; j < num_meshes; j++)
  {
    if (meshes[j].get().numPoints() != num_normals)
      throw std::invalid_argument("Input meshes do not all have the same number of points");

    auto normals = meshes[j].get().getField<vtkDataArray>("Normals");

    if (num_normals != normals->GetNumberOfTuples())
      throw std::invalid_argument("Expected a normal for every point in mesh. Please call generateNormals to accomplish this");

    for (int i = 0; i < num_normals; i++)
    {
      auto n = normals->GetTuple3(i);

      // note: Utils::cartesian2spherical returns atypical (r, phi, theta)
      Utils::cartesian2spherical(n, sphericals[i][j].GetDataPointer());
    }
  }

  // prep data in 1d theta/phi arrays for averageThetaArc function
  std::vector<std::vector<double>> phis(num_normals, std::vector<double>(num_meshes));
  std::vector<std::vector<double>> thetas(num_normals, std::vector<double>(num_meshes));
  for (int i = 0; i < num_normals; i++)
  {
    for (int j = 0; j < num_meshes; j++)
    {
      phis[i][j] = sphericals[i][j][1];
      thetas[i][j] = sphericals[i][j][2];
    }
  }

  vtkSmartPointer<vtkDoubleArray> normals = vtkSmartPointer<vtkDoubleArray>::New();
  normals->SetNumberOfComponents(3);
  normals->SetNumberOfTuples(num_normals);
  normals->SetName("MeanNormals");

  // compute average value for collection of normals for all meshes
  std::vector<Vector3> mean_normals(num_normals);
  for (int i = 0; i < num_normals; i++)
  {
    Vector3 avg_spherical_normal = makeVector({1.0,
                                               Utils::averageThetaArc(phis[i]),
                                               Utils::averageThetaArc(thetas[i])});

    // note: Utils::spherical2cartesian expects atypical (r, phi, theta)
    Utils::spherical2cartesian(avg_spherical_normal.GetDataPointer(),
                               mean_normals[i].GetDataPointer());

    normals->SetTuple3(i, mean_normals[i][0], mean_normals[i][1], mean_normals[i][2]);
  }

  std::cerr << "WARNING: Added a multi-component mesh field\n";

  return normals;
}

} // shapeworks
