#include "MeshUtils.h"

#include <Logging.h>
#include <igl/matrix_to_list.h>
#include <vtkArrowSource.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransformPolyDataFilter.h>

#include "StringUtils.h"
#include "Utils.h"

// boundary loop extractor libraries
#include <igl/boundary_loop.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>
// shared boundary extractor libraries
#include <igl/AABB.h>
#include <igl/remove_unreferenced.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// tbb
#include <tbb/parallel_for.h>

namespace shapeworks {
// locking to handle non-thread-safe code
static std::mutex mesh_mutex;

//! Return a random subset of size num from 0 to max-1
static std::vector<int> get_random_subset(int num, int max) {
  // Seed the random number generator
  std::mt19937 rng(42);

  // Generate indices from 0 to max-1
  std::vector<int> all_indices(max);
  std::iota(all_indices.begin(), all_indices.end(), 0);

  // Shuffle the indices
  std::shuffle(all_indices.begin(), all_indices.end(), rng);

  // Take the first n elements as the random subset
  std::vector<int> random_subset(all_indices.begin(), all_indices.begin() + num);

  return random_subset;
}

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createICPTransform(const Mesh source, const Mesh target,
                                                                  Mesh::AlignmentType align, const unsigned iterations,
                                                                  bool meshTransform) {
  if (source.numPoints() == 0 || target.numPoints() == 0) {
    throw std::invalid_argument("empty mesh passed to MeshUtils::createICPTransform");
  }

  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source.getVTKMesh());
  icp->SetTarget(target.getVTKMesh());

  if (align == Mesh::Rigid) {
    icp->GetLandmarkTransform()->SetModeToRigidBody();
  } else if (align == Mesh::Similarity) {
    icp->GetLandmarkTransform()->SetModeToSimilarity();
  } else {
    icp->GetLandmarkTransform()->SetModeToAffine();
  }

  icp->SetMaximumNumberOfIterations(iterations);
  if (meshTransform) icp->StartByMatchingCentroidsOn();
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source.getVTKMesh());
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  if (meshTransform) {
    m = icp->GetMatrix();
  } else {
    vtkMatrix4x4::Invert(icp->GetMatrix(),
                         m);  // It's inversed because when an image is transformed,
    // a new image is created in the target space and samples through the transform back to the original space
  }

  return m;
}

Mesh MeshUtils::create_mesh_from_file(std::string filename, double iso_value) {
  bool is_mesh = false;
  for (auto& type : shapeworks::Mesh::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      is_mesh = true;
    }
  }

  bool is_image = false;
  for (auto& type : shapeworks::Image::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      is_image = true;
    }
  }

  if (is_mesh) {
    return Mesh(filename);
  } else if (is_image) {
    return Image(filename).toMesh(iso_value);
  } else {
    throw std::invalid_argument("Unsupported file type");
  }
}

Mesh MeshUtils::threadSafeReadMesh(std::string filename) {
  std::scoped_lock lock(mesh_mutex);
  Mesh mesh(filename);
  return mesh;
}

void MeshUtils::threadSafeWriteMesh(std::string filename, Mesh mesh) {
  std::scoped_lock lock(mesh_mutex);
  mesh.write(filename);
}

PhysicalRegion MeshUtils::boundingBox(const std::vector<std::string>& filenames, bool center) {
  if (filenames.empty()) throw std::invalid_argument("No filenames provided to compute a bounding box");

  Mesh mesh(filenames[0]);
  PhysicalRegion bbox(mesh.boundingBox());

  for (auto& filename : filenames) {
    Mesh mesh(filename);
    bbox.expand(mesh.boundingBox());
  }

  return bbox;
}

PhysicalRegion MeshUtils::boundingBox(const std::vector<std::reference_wrapper<const Mesh> >& meshes, bool center) {
  if (meshes.empty()) throw std::invalid_argument("No meshes provided to compute a bounding box");

  PhysicalRegion bbox(meshes[0].get().boundingBox());

  for (auto mesh : meshes) {
    bbox.expand(mesh.get().boundingBox());
  }

  return bbox;
}

PhysicalRegion MeshUtils::boundingBox(const std::vector<Mesh>& meshes, bool center) {
  if (meshes.empty()) throw std::invalid_argument("No meshes provided to compute a bounding box");

  PhysicalRegion bbox(meshes[0].boundingBox());

  for (auto& mesh : meshes) {
    bbox.expand(mesh.boundingBox());
  }

  return bbox;
}

int MeshUtils::findReferenceMesh(std::vector<Mesh>& meshes, int random_subset_size) {
  // auto (-1) defaults to a subset of 30 to avoid O(n^2) pairwise ICP on large datasets
  if (random_subset_size < 0) {
    random_subset_size = 30;
  }
  bool use_random_subset = random_subset_size > 0 && random_subset_size < meshes.size();
  int num_meshes = use_random_subset ? random_subset_size : meshes.size();

  std::vector<int> subset;
  if (use_random_subset) {
    subset = get_random_subset(random_subset_size, meshes.size());
  }

  std::vector<std::pair<int, int> > pairs;

  // enumerate all pairs of meshes
  for (size_t i = 0; i < num_meshes; i++) {
    for (size_t j = i + 1; j < num_meshes; j++) {
      if (use_random_subset) {
        pairs.push_back(std::make_pair(subset[i], subset[j]));
      } else {
        pairs.push_back(std::make_pair(i, j));
      }
    }
  }

  // map of pair to distance value
  std::map<size_t, double> results;
  // mutex for access to results
  std::mutex mutex;

  tbb::parallel_for(tbb::blocked_range<size_t>{0, pairs.size()}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      auto pair = pairs[i];

      vtkSmartPointer<vtkPolyData> poly_data1 = vtkSmartPointer<vtkPolyData>::New();
      poly_data1->DeepCopy(meshes[pair.first].getVTKMesh());
      vtkSmartPointer<vtkPolyData> poly_data2 = vtkSmartPointer<vtkPolyData>::New();
      poly_data2->DeepCopy(meshes[pair.second].getVTKMesh());

      // register the two meshes
      auto matrix = MeshUtils::createICPTransform(poly_data1, poly_data2, Mesh::Rigid, 10, true);
      // transform
      auto transform = createMeshTransform(matrix);
      Mesh transformed = poly_data1;
      transformed.applyTransform(transform);

      // compute distance
      auto distances = transformed.distance(poly_data2)[0];
      double distance = mean(distances);
      {
        // lock and store results
        std::scoped_lock lock(mutex);
        results[i] = distance;
      }
    }
  });

  std::vector<double> means(meshes.size(), 0);

  double count = meshes.size() - 1;
  for (size_t i = 0; i < pairs.size(); i++) {
    auto pair = pairs[i];
    double result = results[i];
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

//---------------------------------------------------------------------------
static bool is_clockwise(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const std::vector<int>& loop) {
  Eigen::RowVector3d centroid{0.0, 0.0, 0.0};
  for (const auto& i : loop) {
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

//---------------------------------------------------------------------------
Mesh MeshUtils::extract_boundary_loop(Mesh mesh) {
  Eigen::MatrixXd V = mesh.points();
  Eigen::MatrixXi F = mesh.faces();

  std::vector<std::vector<int> > loops;
  igl::boundary_loop(F, loops);
  if (loops.size() < 1) {
    throw std::runtime_error("Expected at least one boundary loop in the mesh");
  }

  const auto& loop = loops[0];
  const auto is_cw = is_clockwise(V, F, loop);

  auto pts = vtkSmartPointer<vtkPoints>::New();
  for (const auto& i : loop) {
    pts->InsertNextPoint(V(i, 0), V(i, 1), V(i, 2));
  }

  auto lines = vtkSmartPointer<vtkCellArray>::New();
  for (size_t i = 0; i < loop.size(); i++) {
    auto line = vtkSmartPointer<vtkLine>::New();
    if (is_cw) {
      line->GetPointIds()->SetId(0, i);
      line->GetPointIds()->SetId(1, (i + 1) % loop.size());
    } else {
      line->GetPointIds()->SetId(1, i);
      line->GetPointIds()->SetId(0, (i + 1) % loop.size());
    }

    lines->InsertNextCell(line);
  }

  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(pts);
  polydata->SetLines(lines);

  Mesh output(polydata);
  return output;
}

/*
 * shared_boundary_extractor
 * shared_boundary_extractor <input_l.ply> <input_r.ply> <out_l.ply> <out_r.ply> <out_m.ply> <tol>
 *
 * Extract the shared boundary triangles between input_l and input_r. The meshes with the boundary removed are
 * saved in out_l and out_r. The boundary triangles are stored in out_m.ply. tol is a data specific value that
 * defines the threshold for two surfaces to be "close"
 */

//---------------------------------------------------------------------------
static std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> rem_into_eigen_mesh(const std::vector<int>& faces,
                                                                        const Eigen::MatrixXd& src_V,
                                                                        const Eigen::MatrixXi& src_F) {
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(src_F.rows() - faces_set.size(), 3);
  int next_idx = 0;
  for (int i = 0; i < src_F.rows(); i++) {
    if (faces_set.find(i) == faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);

  return std::make_tuple(V, F);
}

//---------------------------------------------------------------------------
static std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> shared_into_eigen_mesh(const std::vector<int>& faces,
                                                                           const Eigen::MatrixXd& src_V,
                                                                           const Eigen::MatrixXi& src_F) {
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(faces_set.size(), 3);
  int next_idx = 0;
  for (int i = 0; i < src_F.rows(); i++) {
    if (faces_set.find(i) != faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);
  return std::make_tuple(V, F);
}

//---------------------------------------------------------------------------
static bool is_empty(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) { return V.size() == 0 || F.size() == 0; }

//---------------------------------------------------------------------------
// Identifies faces from the source mesh that lie within tolerance distance of the other mesh surface.
// Uses spatial queries to determine which faces are "shared" between the two meshes.
// Returns both the shared faces as one mesh and the remaining non-shared faces as another mesh.
static std::tuple<Eigen::MatrixXd, Eigen::MatrixXi, Eigen::MatrixXd, Eigen::MatrixXi> find_shared_surface(
    const Eigen::MatrixXd& src_V, const Eigen::MatrixXi& src_F, const Eigen::MatrixXd& other_V,
    const Eigen::MatrixXi& other_F, double tol = 1e-3) {
  Eigen::MatrixXd out_V;
  Eigen::MatrixXi out_F;
  Eigen::MatrixXd rem_V;
  Eigen::MatrixXi rem_F;
  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(other_V, other_F);

  std::vector<Eigen::Vector3d> new_pts;

  std::vector<int> shared_faces;

  // find stuff
  for (int f = 0; f < src_F.rows(); f++) {
    bool keep = true;
    for (int i = 0; i < 3; i++) {
      const auto& vi = src_V.row(src_F(f, i));
      Eigen::VectorXd sqrD;
      Eigen::VectorXi I;
      Eigen::MatrixXd C;
      Eigen::MatrixXd P(1, 3);
      P.row(0) = vi;
      tree.squared_distance(other_V, other_F, P, sqrD, I, C);

      if (sqrD(0) > tol) {
        keep = false;
        break;
      }
    }

    if (keep) {
      shared_faces.push_back(f);
    }
  }

  std::tie(out_V, out_F) = shared_into_eigen_mesh(shared_faces, src_V, src_F);
  std::tie(rem_V, rem_F) = rem_into_eigen_mesh(shared_faces, src_V, src_F);
  return std::make_tuple(out_V, out_F, rem_V, rem_F);
}

//---------------------------------------------------------------------------
// Snaps boundary vertices of the source mesh to the closest points on the shared mesh boundary.
// This ensures the boundaries are geometrically aligned, which is typically needed for mesh
// stitching or boolean operations.
// Returns a copy of the source mesh with modified boundary vertex positions.
static void snap_boundary_to_mesh(const Eigen::MatrixXd& src_V, const Eigen::MatrixXi& src_F,
                                  const Eigen::MatrixXd& shared_V, const Eigen::MatrixXi& shared_F,
                                  Eigen::MatrixXd& out_V, Eigen::MatrixXi& out_F) {
  std::vector<std::vector<int> > src_loops, shared_loops;
  igl::boundary_loop(src_F, src_loops);
  igl::boundary_loop(shared_F, shared_loops);

  if (src_loops.size() < 1 || shared_loops.size() < 1) {
    SW_DEBUG("src_loops size: {}, shared_loops size: {}", src_loops.size(), shared_loops.size());
    throw std::runtime_error("Expected at least one boundary loop in each mesh");
  }

  const auto& src_loop = src_loops[0];
  const auto& shared_loop = shared_loops[0];

  Eigen::MatrixXi shared_F_boundary;
  shared_F_boundary.resize(shared_loop.size(), 3);
  for (int i = 0; i < shared_loop.size(); i++) {
    const int v0 = shared_loop[i];
    const int v1 = shared_loop[(i + 1) % shared_loop.size()];
    shared_F_boundary.row(i) = Eigen::Vector3i{v0, v1, v1};
  }

  out_V = src_V;
  out_F = src_F;

  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(shared_V, shared_F_boundary);

  for (int i = 0; i < src_loop.size(); i++) {
    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;
    Eigen::MatrixXd P(1, 3);
    P.row(0) = src_V.row(src_loop[i]);
    tree.squared_distance(shared_V, shared_F_boundary, P, sqrD, I, C);

    out_V.row(src_loop[i]) = C.row(0);
  }
}

//---------------------------------------------------------------------------
// Extracts shared boundary regions between two input meshes and prepares them for operations like stitching.
// Identifies shared surfaces on both meshes, then aligns the boundary of the remaining geometry to ensure
// compatibility. Returns three meshes: left remainder with aligned boundary, right remainder, and shared surface.
std::array<Mesh, 3> MeshUtils::shared_boundary_extractor(const Mesh& mesh_l, const Mesh& mesh_r, double tol) {
  Eigen::MatrixXd V_l, V_r;
  Eigen::MatrixXi F_l, F_r;
  V_l = mesh_l.points();
  F_l = mesh_l.faces();
  V_r = mesh_r.points();
  F_r = mesh_r.faces();

  Eigen::MatrixXd shared_V_l, shared_V_r, rem_V_l, rem_V_r;
  Eigen::MatrixXi shared_F_l, shared_F_r, rem_F_l, rem_F_r;
  std::tie(shared_V_l, shared_F_l, rem_V_l, rem_F_l) = find_shared_surface(V_l, F_l, V_r, F_r, tol);
  std::tie(shared_V_r, shared_F_r, rem_V_r, rem_F_r) = find_shared_surface(V_r, F_r, V_l, F_l, tol);

  if (is_empty(shared_V_l, shared_F_l) || is_empty(shared_V_r, shared_F_r) || is_empty(rem_V_l, rem_F_l) ||
      is_empty(rem_V_r, rem_F_r)) {
    // todo this should return a status code to the caller so that it can be displayed or handled based on the
    // downstream task
    throw std::runtime_error("No shared surface detected. Please check the input meshes and/or increase the tolerance");
  }

  Eigen::MatrixXd bridge_V;
  Eigen::MatrixXi bridge_F;

  snap_boundary_to_mesh(rem_V_l, rem_F_l, shared_V_r, shared_F_r, bridge_V, bridge_F);

  Mesh out_l(bridge_V, bridge_F);
  Mesh out_r(rem_V_r, rem_F_r);
  Mesh out_s(shared_V_r, shared_F_r);

  // pass ownership since they will just go out of scope and be deleted anyway
  return std::array<Mesh, 3>{std::move(out_l), std::move(out_r), std::move(out_s)};
}

//---------------------------------------------------------------------------
void MeshUtils::generateNormals(const std::vector<std::reference_wrapper<Mesh> >& meshes, bool forceRegen) {
  if (meshes.empty()) throw std::invalid_argument("No meshes provided to compute average normals");

  for (int i = 0; i < meshes.size(); i++) {
    bool hasNormals = true;
    try {
      meshes[i].get().getField("Normals", Mesh::Point);
    } catch (...) {
      hasNormals = false;
    }

    if ((!hasNormals) || (hasNormals && forceRegen)) meshes[i].get().computeNormals();
  }
}

//---------------------------------------------------------------------------
Field MeshUtils::computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals) {
  if (filenames.empty()) throw std::invalid_argument("No filenames provided to compute mean normals");

  std::vector<Mesh> meshes;
  meshes.reserve(filenames.size());  // create a vector large enough for all the meshes that will be loaded
  for (auto& filename : filenames) {
    meshes.push_back(Mesh(filename));
  }

  std::vector<std::reference_wrapper<Mesh> > rmeshes;
  rmeshes.reserve(meshes.size());
  for (Mesh& mesh : meshes) rmeshes.push_back(std::reference_wrapper<Mesh>(mesh));

  if (autoGenerateNormals) {
    MeshUtils::generateNormals(rmeshes);
  }

  std::vector<std::reference_wrapper<const Mesh> > cmeshes;
  for (Mesh& mesh : meshes) cmeshes.push_back(std::reference_wrapper<const Mesh>(mesh));

  return computeMeanNormals(cmeshes);
}

//---------------------------------------------------------------------------
Field MeshUtils::computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh> >& meshes) {
  if (meshes.empty()) throw std::invalid_argument("No meshes provided to compute average normals");

  auto num_normals = meshes[0].get().numPoints();
  auto num_meshes = meshes.size();

  // convert all normals from all meshes to spherical coords
  std::vector<std::vector<Point3> > sphericals(num_normals, std::vector<Point3>(num_meshes));
  for (int j = 0; j < num_meshes; j++) {
    if (meshes[j].get().numPoints() != num_normals)
      throw std::invalid_argument("Input meshes do not all have the same number of points");

    auto normals = meshes[j].get().getField("Normals", Mesh::Point);

    if (num_normals != normals->GetNumberOfTuples())
      throw std::invalid_argument(
          "Expected a normal for every point in mesh. Please call generateNormals to accomplish this");

    for (int i = 0; i < num_normals; i++) {
      auto n = normals->GetTuple3(i);

      // note: Utils::cartesian2spherical returns atypical (r, phi, theta)
      Utils::cartesian2spherical(n, sphericals[i][j].GetDataPointer());
    }
  }

  // prep data in 1d theta/phi arrays for averageThetaArc function
  std::vector<std::vector<double> > phis(num_normals, std::vector<double>(num_meshes));
  std::vector<std::vector<double> > thetas(num_normals, std::vector<double>(num_meshes));
  for (int i = 0; i < num_normals; i++) {
    for (int j = 0; j < num_meshes; j++) {
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
  for (int i = 0; i < num_normals; i++) {
    Vector3 avg_spherical_normal =
        makeVector({1.0, Utils::averageThetaArc(phis[i]), Utils::averageThetaArc(thetas[i])});

    // note: Utils::spherical2cartesian expects atypical (r, phi, theta)
    Utils::spherical2cartesian(avg_spherical_normal.GetDataPointer(), mean_normals[i].GetDataPointer());

    normals->SetTuple3(i, mean_normals[i][0], mean_normals[i][1], mean_normals[i][2]);
  }

  // std::cerr << "WARNING: Added a multi-component mesh field\n";

  return normals;
}

//---------------------------------------------------------------------------
void MeshUtils::visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh) {
  // std::cout << "VTK rendering" << std::endl;

  // Render VTK for debug
  // Creates mesh actor
  //    vtkNew<vtkLookupTable> lut;
  //    lut->SetNumberOfColors(16);
  //    lut->SetHueRange(0, 0.67);
  //    lut->Build();

  // std::cout << "Setting Mesh Mapper" << std::endl;

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  mesh->getIGLMesh(V, F);

  vtkNew<vtkPolyDataMapper> meshmapper;
  meshmapper->SetInputData(mesh->getVTKMesh());
  // meshmapper->SetLookupTable(lut);
  meshmapper->SetScalarModeToUsePointData();
  meshmapper->SetColorModeToMapScalars();
  meshmapper->ScalarVisibilityOn();
  // std::cout << "GetArrayName " << meshmapper->GetArrayName() << std::endl;
  vtkNew<vtkActor> meshactor;
  meshactor->SetMapper(meshmapper);

  vtkNew<vtkNamedColors> colors;

  // std::cout << "Setting Renderer" << std::endl;

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Mesh");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // std::cout << "Adding arrow actors" << std::endl;
  vtkSmartPointer<vtkDataArray> vff = mesh->getVTKMesh()->GetCellData()->GetArray(0);

  // Computes grad vec for each face
  for (size_t i = 0; i < F.rows(); i++) {
    const Eigen::Vector3d vert_dists(mesh->getFieldValue("value", F(i, 0)), mesh->getFieldValue("value", F(i, 1)),
                                     mesh->getFieldValue("value", F(i, 2)));

    Eigen::Vector3d v1(V(F(i, 0), 0), V(F(i, 0), 1), V(F(i, 0), 2));
    Eigen::Vector3d v2(V(F(i, 1), 0), V(F(i, 1), 1), V(F(i, 1), 2));
    Eigen::Vector3d v3(V(F(i, 2), 0), V(F(i, 2), 1), V(F(i, 2), 2));
    Eigen::Vector3d face_center = (v1 + v2 + v3) / 3;

    // Compute gradient of geodesics
    Eigen::Vector3d out_grad_eigen(vff->GetTuple3(i));
    renderer->AddActor(getArrow(face_center, face_center + out_grad_eigen));
    // out_grad_eigen *= geo_dist / out_grad_eigen.norm();
  }

  // Debug scalar and gradient queries
  // vvvvvvvvvvvvvvvvvvvvvvvvvvvv
  //  Eigen::Vector3d addedpt(1,1,1);
  //  Eigen::Vector3d querypt = V.row(0)+ addedpt.transpose();
  //  double val = GetFFCValue(querypt);
  //  Eigen::Vector3d grad = GetFFCGradient(querypt);
  //  std::cout << "Querypt " << querypt.transpose() << std::endl << "val " << val << ", grad " << grad.transpose() <<
  //  std::endl; renderer->AddActor(getArrow(querypt, querypt+grad*10));
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  renderer->AddActor(meshactor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  // std::cout << "Starting" << std::endl;

  renderWindow->SetWindowName("Mesh");
  renderWindow->Render();
  renderWindowInteractor->Start();
}

//---------------------------------------------------------------------------
void CreateArrowMidPoint(double midPoint[], double startPoint[], double endPoint[]) {
  midPoint[0] = (startPoint[0] + endPoint[0]) / 3;
  midPoint[1] = (startPoint[1] + endPoint[1]) / 3;
  midPoint[2] = (startPoint[2] + endPoint[2]) / 3;
}

//---------------------------------------------------------------------------
void PrintArray(std::string arrayName, double* arr, int size) {
  std::cout << arrayName << std::endl;

  for (int i = 0; i < size; i++) {
    std::cout << arr[i] << " ";
  }

  std::cout << std::endl;
}

//---------------------------------------------------------------------------
void PrintTransformParams(vtkSmartPointer<vtkTransform> transform) {
  PrintArray("Transform orientation: ", transform->GetOrientation(), 3);
  PrintArray("Transform position: ", transform->GetPosition(), 3);
  PrintArray("Transform scale: ", transform->GetScale(), 3);
}

//---------------------------------------------------------------------------
void Print4x4Matrix(std::string matrixName, vtkSmartPointer<vtkMatrix4x4> matrix) {
  std::cout << matrixName << std::endl;
  for (unsigned int i = 0; i < sizeof(matrix->Element) / sizeof(matrix->Element[0]); i++) {
    std::cout << matrix->GetElement(i, 0) << " " << matrix->GetElement(i, 1) << " " << matrix->GetElement(i, 2) << " "
              << matrix->GetElement(i, 3) << std::endl;
  }

  std::cout << std::endl;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkActor> MeshUtils::getArrow(Eigen::Vector3d start, Eigen::Vector3d end) {
  // Create an arrow source
  vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

  arrowSource->SetShaftResolution(50);
  arrowSource->SetTipResolution(50);

  // Generate a random start and end point
  double startPoint[3], endPoint[3], midPoint[3];
  startPoint[0] = start(0);
  startPoint[1] = start(1);
  startPoint[2] = start(2);

  endPoint[0] = end(0);
  endPoint[1] = end(1);
  endPoint[2] = end(2);

  CreateArrowMidPoint(midPoint, startPoint, endPoint);

  // Print points
  //     PrintArray("Start point: ", startPoint, 3);
  //     PrintArray("Mid point: ", midPoint, 3);
  //     PrintArray("End point: ", endPoint, 3);

  // Compute a basis
  double normalizedX[3];
  double normalizedY[3];
  double normalizedZ[3];

  // The X axis is a vector from start to end
  vtkMath::Subtract(endPoint, startPoint, normalizedX);
  double length = vtkMath::Norm(normalizedX);
  vtkMath::Normalize(normalizedX);

  // The Z axis is an arbitrary vector cross X
  double arbitrary[3];
  arbitrary[0] = vtkMath::Random(-10, 10);
  arbitrary[1] = vtkMath::Random(-10, 10);
  arbitrary[2] = vtkMath::Random(-10, 10);

  vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
  vtkMath::Normalize(normalizedZ);

  // The Y axis is the cross product of Z and X axes
  vtkMath::Cross(normalizedZ, normalizedX, normalizedY);

  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  // Create the direction cosine matrix
  matrix->Identity();
  for (unsigned int i = 0; i < 3; i++) {
    matrix->SetElement(i, 0, normalizedX[i]);
    matrix->SetElement(i, 1, normalizedY[i]);
    matrix->SetElement(i, 2, normalizedZ[i]);
  }

  //     Print4x4Matrix("4x4 Matrix: ", matrix);
  //     PrintArray("NormalizedX point: ", normalizedX, 3);
  //     PrintArray("NormalizedY point: ", normalizedY, 3);
  //     PrintArray("NormalizedZ point: ", normalizedZ, 3);

  // Apply the transforms
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Translate(startPoint);
  transform->Concatenate(matrix);
  transform->Scale(length, length, length);

  // Print transform params
  // PrintTransformParams(transform);

  // Transform the polydata
  vtkSmartPointer<vtkTransformPolyDataFilter> transformPD = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformPD->SetTransform(transform);
  transformPD->SetInputConnection(arrowSource->GetOutputPort());

  // Create a mapper and actor for the arrow
  vtkSmartPointer<vtkPolyDataMapper> arrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> arrowActor = vtkSmartPointer<vtkActor>::New();
#ifdef USER_MATRIX
  arrowMapper->SetInputConnection(arrowSource->GetOutputPort());
  arrowActor->SetUserMatrix(transform->GetMatrix());
#else
  arrowMapper->SetInputConnection(transformPD->GetOutputPort());
#endif
  arrowActor->SetMapper(arrowMapper);

  return arrowActor;
}

//-------------------------------------------------------------------------
//! From vtkTriangle::EvaluatePosition
int MeshUtils::evaluate_triangle_position(const double x[3], double closestPoint[3], int& subId, double pcoords[3],
                                          double& dist2, double weights[], double pt3[3], double pt1[3],
                                          double pt2[3]) {
  int i, j;
  double n[3], fabsn;
  double rhs[2], c1[2], c2[2];
  double det;
  double maxComponent;
  int idx = 0, indices[2];
  double dist2Point, dist2Line1, dist2Line2;
  double *closest, closestPoint1[3], closestPoint2[3], cp[3];

  subId = 0;
  pcoords[2] = 0.0;

  // Get normal for triangle, only the normal direction is needed, i.e. the
  // normal need not be normalized (unit length)
  vtkTriangle::ComputeNormalDirection(pt1, pt2, pt3, n);

  // Project point to plane
  vtkPlane::GeneralizedProjectPoint(x, pt1, n, cp);

  // Construct matrices.  Since we have over determined system, need to find
  // which 2 out of 3 equations to use to develop equations. (Any 2 should
  // work since we've projected point to plane.)
  //
  for (maxComponent = 0.0, i = 0; i < 3; i++) {
    // trying to avoid an expensive call to fabs()
    if (n[i] < 0) {
      fabsn = -n[i];
    } else {
      fabsn = n[i];
    }
    if (fabsn > maxComponent) {
      maxComponent = fabsn;
      idx = i;
    }
  }
  for (j = 0, i = 0; i < 3; i++) {
    if (i != idx) {
      indices[j++] = i;
    }
  }

  for (i = 0; i < 2; i++) {
    rhs[i] = cp[indices[i]] - pt3[indices[i]];
    c1[i] = pt1[indices[i]] - pt3[indices[i]];
    c2[i] = pt2[indices[i]] - pt3[indices[i]];
  }

  if ((det = vtkMath::Determinant2x2(c1, c2)) == 0.0) {
    pcoords[0] = pcoords[1] = 0.0;
    return -1;
  }

  pcoords[0] = vtkMath::Determinant2x2(rhs, c2) / det;
  pcoords[1] = vtkMath::Determinant2x2(c1, rhs) / det;

  // Okay, now find closest point to element
  //
  weights[0] = 1 - (pcoords[0] + pcoords[1]);
  weights[1] = pcoords[0];
  weights[2] = pcoords[1];

  if (weights[0] >= 0.0 && weights[0] <= 1.0 && weights[1] >= 0.0 && weights[1] <= 1.0 && weights[2] >= 0.0 &&
      weights[2] <= 1.0) {
    // projection distance
    if (closestPoint) {
      dist2 = vtkMath::Distance2BetweenPoints(cp, x);
      closestPoint[0] = cp[0];
      closestPoint[1] = cp[1];
      closestPoint[2] = cp[2];
    }
    return 1;
  } else {
    double t;
    if (closestPoint) {
      if (weights[1] < 0.0 && weights[2] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt3);
        dist2Line1 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt3, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt3;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[2] < 0.0 && weights[0] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt1);
        dist2Line1 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt1;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[1] < 0.0 && weights[0] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt2);
        dist2Line1 = vtkLine::DistanceToLine(x, pt2, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt2;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[0] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint);
      } else if (weights[1] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt2, pt3, t, closestPoint);
      } else if (weights[2] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint);
      }
    }
    return 0;
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshUtils::clean_mesh(vtkSmartPointer<vtkPolyData> mesh) {
  vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->ConvertPolysToLinesOff();
  clean->ConvertLinesToPointsOff();
  clean->ConvertStripsToPolysOff();
  clean->PointMergingOn();
  clean->SetInputData(mesh);
  clean->Update();
  return clean->GetOutput();
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshUtils::remove_zero_area_triangles(vtkSmartPointer<vtkPolyData> mesh) {
  // Create a new polydata to store the filtered triangles
  auto filtered_mesh = vtkSmartPointer<vtkPolyData>::New();

  // Get the points from the input mesh
  vtkSmartPointer<vtkPoints> points = mesh->GetPoints();
  filtered_mesh->SetPoints(points);

  // Create a new cell array to store triangles
  auto new_triangles = vtkSmartPointer<vtkCellArray>::New();

  // Iterate through all cells
  auto cell_point_ids = vtkSmartPointer<vtkIdList>::New();

  // First pass: find max area to establish scale
  double max_area = 0.0;
  for (vtkIdType cellId = 0; cellId < mesh->GetNumberOfCells(); cellId++) {
    mesh->GetCellPoints(cellId, cell_point_ids);
    if (cell_point_ids->GetNumberOfIds() == 3) {
      double p0[3], p1[3], p2[3];
      points->GetPoint(cell_point_ids->GetId(0), p0);
      points->GetPoint(cell_point_ids->GetId(1), p1);
      points->GetPoint(cell_point_ids->GetId(2), p2);
      double area = vtkTriangle::TriangleArea(p0, p1, p2);
      if (area > max_area) {
        max_area = area;
      }
    }
  }

  // Use relative epsilon based on max area, with absolute floor
  const double RELATIVE_EPSILON = 1e-6;
  const double ABSOLUTE_FLOOR = 1e-15;
  const double epsilon = std::max(max_area * RELATIVE_EPSILON, ABSOLUTE_FLOOR);

  // Second pass: filter triangles
  for (vtkIdType cellId = 0; cellId < mesh->GetNumberOfCells(); cellId++) {
    mesh->GetCellPoints(cellId, cell_point_ids);

    // Only process triangles
    if (cell_point_ids->GetNumberOfIds() == 3) {
      // Get the coordinates of the three vertices
      double p0[3], p1[3], p2[3];
      points->GetPoint(cell_point_ids->GetId(0), p0);
      points->GetPoint(cell_point_ids->GetId(1), p1);
      points->GetPoint(cell_point_ids->GetId(2), p2);

      // Calculate the area of the triangle using VTK's built-in function
      double area = vtkTriangle::TriangleArea(p0, p1, p2);

      if (area > epsilon) {
        // Add this triangle to the new cell array
        new_triangles->InsertNextCell(cell_point_ids);
      }
    }
  }

  // Set the triangles in the filtered mesh
  filtered_mesh->SetPolys(new_triangles);

  // Copy the point data
  filtered_mesh->GetPointData()->ShallowCopy(mesh->GetPointData());

  // Use vtkCleanPolyData to remove unused points
  auto cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
  cleaner->SetInputData(filtered_mesh);
  cleaner->PointMergingOff();  // Don't merge points, just remove unused ones
  cleaner->Update();

  return cleaner->GetOutput();
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshUtils::recreate_mesh(vtkSmartPointer<vtkPolyData> mesh) {
  vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();

  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> polys;

  // copy points
  for (vtkIdType i = 0; i < mesh->GetNumberOfPoints(); i++) {
    points->InsertNextPoint(mesh->GetPoint(i));
  }

  // copy triangles
  for (vtkIdType i = 0; i < mesh->GetNumberOfCells(); i++) {
    vtkCell* cell = mesh->GetCell(i);

    if (cell->GetCellType() != VTK_EMPTY_CELL) {  // VTK_EMPTY_CELL means it was deleted
      // create an array of vtkIdType
      vtkIdType* pts = new vtkIdType[cell->GetNumberOfPoints()];
      for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); j++) {
        pts[j] = cell->GetPointId(j);
      }
      polys->InsertNextCell(cell->GetNumberOfPoints(), pts);
      delete[] pts;
    }
  }

  poly_data->SetPoints(points);
  poly_data->SetPolys(polys);
  return poly_data;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshUtils::repair_mesh(vtkSmartPointer<vtkPolyData> mesh) {
  auto triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInputData(mesh);
  triangle_filter->PassLinesOff();
  triangle_filter->Update();

  auto connectivity = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  connectivity->SetInputConnection(triangle_filter->GetOutputPort());
  connectivity->SetExtractionModeToLargestRegion();
  connectivity->Update();

  auto cleaned = MeshUtils::clean_mesh(connectivity->GetOutput());

  auto fixed = Mesh(cleaned).fixNonManifold();

  // remove deleted triangles and clean up
  auto recreated = MeshUtils::recreate_mesh(fixed.getVTKMesh());

  auto final = MeshUtils::remove_zero_area_triangles(recreated);

  return final;
}

}  // namespace shapeworks
