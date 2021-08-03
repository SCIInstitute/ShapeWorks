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


//boundary loop extractor libraries
#include <igl/boundary_loop.h>
#include <vtkXMLPolyDataWriter.h>
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPolyData.h>

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



/*
 * boundary_loop_extractor
 * boundary_loop_extractor <in_file.ply> <out_file.vtp>
 *
 * Given a .ply mesh, extract the boundary loop and export the boundary loop as a VTK .vtp file
 */

bool is_clockwise(const Eigen::MatrixXd& V,
                  const Eigen::MatrixXi& F,
                  const std::vector<int>& loop) {
  Eigen::RowVector3d centroid{0.0, 0.0, 0.0};
  for(const auto& i : loop) {
    centroid += V.row(i);
  }
  centroid /= loop.size();

  // todo this is arbitrary and works for the peanut data and initial tests on LA+Septum data
  // it enforces a consistent ordering in the boundary loop
  const auto v0 = V.row(loop[0]) - centroid;
  const auto v1 = V.row(loop[1]) - centroid;
  const double angle0 = atan2(v0.z(), v0.y());
  const double angle1 = atan2(v1.z(), v1.y());
  return angle0 > angle1;
}

int MeshUtils::boundaryLoopExtractor(std::string filename, Mesh mesh) 
{

  // if(argc != 3) {
  //   std::cerr << "Usage: " << argv[0] << "<in_file.ply> <out_file.vtp>";
  //   exit(1);
  // }

  // const std::string in_fname = argv[1];
  // const std::string out_fname = argv[2];

  Eigen::MatrixXd V = mesh.points();
  Eigen::MatrixXi F = mesh.faces();
  
  // igl::readPLY(in_fname, V, F);

  std::vector<std::vector<int>> loops;
  igl::boundary_loop(F, loops);
  assert(loops.size() == 1);

  const auto& loop = loops[0];
  const auto is_cw = is_clockwise(V, F, loop);

  auto pts = vtkSmartPointer<vtkPoints>::New();
  for(const auto& i : loop) {
    pts->InsertNextPoint(V(i, 0), V(i, 1), V(i, 2));
  }

  auto lines = vtkSmartPointer<vtkCellArray>::New();
  for(size_t i=0; i<loop.size(); i++) {
    auto line = vtkSmartPointer<vtkLine>::New();
    if(is_cw) {
      line->GetPointIds()->SetId(0, i);
      line->GetPointIds()->SetId(1, (i+1)%loop.size());
    } else {
      line->GetPointIds()->SetId(1, i);
      line->GetPointIds()->SetId(0, (i+1)%loop.size());
    }

    lines->InsertNextCell(line);
  }

  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(pts);
  polydata->SetLines(lines);

  auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(polydata);
  writer->Write();
}






} // shapeworks
