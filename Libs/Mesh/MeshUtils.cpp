#include "MeshUtils.h"
#include "ParticleSystem.h"
#include "Utils.h"

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLookupTable.h>
#include <vtkArrowSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>

#include <igl/matrix_to_list.h>


//boundary loop extractor libraries
#include <igl/boundary_loop.h>
#include <vtkXMLPolyDataWriter.h>
#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkPolyData.h>
//shared boundary extractor libraries 
#include <igl/readPLY.h>
#include <igl/writePLY.h>
#include <igl/AABB.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <igl/remove_unreferenced.h>



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
  if (source.numPoints() == 0 || target.numPoints() == 0) {
    throw std::invalid_argument("empty mesh passed to MeshUtils::createICPTransform");
  }  

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
    vtkMatrix4x4::Invert(icp->GetMatrix(), m); // It's inversed because when an image is transformed,
                                               // a new image is created in the target space and samples through the transform back to the original space

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

size_t MeshUtils::findReferenceMesh(std::vector<Mesh>& meshes)
{
  std::vector<std::pair<int, int>> pairs;

  // enumerate all pairs of meshes
  for (size_t i = 0; i < meshes.size(); i++) {
    for (size_t j = i + 1; j < meshes.size(); j++) {
      pairs.push_back(std::make_pair(i, j));
    }
  }

  // map of pair to distance value
  std::map<size_t, double> results;
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


/*
 * shared_boundary_extractor
 * shared_boundary_extractor <input_l.ply> <input_r.ply> <out_l.ply> <out_r.ply> <out_m.ply> <tol>
 *
 * Extract the shared boundary triangles between input_l and input_r. The meshes with the boundary removed are
 * saved in out_l and out_r. The boundary triangles are stored in out_m.ply. tol is a data specific value that
 * defines the threshold for two surfaces to be "close"
 */

std::tuple<Eigen::MatrixXd,Eigen::MatrixXi> rem_into_eigen_mesh(const std::vector<int>& faces,
                         const Eigen::MatrixXd& src_V,
                         const Eigen::MatrixXi& src_F) 
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(src_F.rows() - faces_set.size(), 3);
  int next_idx = 0;
  for(int i=0; i<src_F.rows(); i++) {
    if(faces_set.find(i) == faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);

  return std::make_tuple(V,F);
}



std::tuple<Eigen::MatrixXd,Eigen::MatrixXi> shared_into_eigen_mesh(const std::vector<int>& faces,
                            const Eigen::MatrixXd& src_V,
                            const Eigen::MatrixXi& src_F) 
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(faces_set.size(), 3);
  int next_idx = 0;
  for(int i=0; i<src_F.rows(); i++) {
    if(faces_set.find(i) != faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);
  return std::make_tuple(V,F);
}



bool is_empty(const Eigen::MatrixXd& V,
              const Eigen::MatrixXi& F) {
  return V.size() == 0 || F.size() == 0;
}







std::tuple<Eigen::MatrixXd,Eigen::MatrixXi,Eigen::MatrixXd,Eigen::MatrixXi> find_shared_surface(const Eigen::MatrixXd& src_V, const Eigen::MatrixXi& src_F,
                         const Eigen::MatrixXd& other_V, const Eigen::MatrixXi& other_F,
                         double tol=1e-3) 
{
  Eigen::MatrixXd out_V; 
  Eigen::MatrixXi out_F;
  Eigen::MatrixXd rem_V; 
  Eigen::MatrixXi rem_F;
  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(other_V, other_F);

  std::vector<Eigen::Vector3d> new_pts;

  std::vector<int> shared_faces;

  // find stuff
  for(int f=0; f<src_F.rows(); f++) {
    bool keep = true;
    for(int i=0; i<3; i++) {
      const auto& vi = src_V.row(src_F(f, i));
      Eigen::VectorXd sqrD;
      Eigen::VectorXi I;
      Eigen::MatrixXd C;
      Eigen::MatrixXd P(1, 3);
      P.row(0) = vi;
      tree.squared_distance(other_V, other_F, P, sqrD, I, C);


      if(sqrD(0) > tol) {
        keep = false;
        break;
      }
    }

    if(keep) {
      shared_faces.push_back(f);
    }
  }

  std::tie(out_V,out_F) = shared_into_eigen_mesh(shared_faces, src_V, src_F);
  std::tie(rem_V,rem_F) = rem_into_eigen_mesh(shared_faces, src_V, src_F);
  return std::make_tuple(out_V,out_F,rem_V,rem_F);
}


void move_to_boundary(const Eigen::MatrixXd& src_V,
                      const Eigen::MatrixXi& src_F,
                      const Eigen::MatrixXd& shared_V,
                      const Eigen::MatrixXi& shared_F,
                      Eigen::MatrixXd& out_V,
                      Eigen::MatrixXi& out_F) 
{
  // Eigen::MatrixXd out_V;
  // Eigen::MatrixXi out_F;

  std::vector<std::vector<int>> src_loops, shared_loops;
  igl::boundary_loop(src_F, src_loops);
  igl::boundary_loop(shared_F, shared_loops);

  assert(src_loops.size() == 1);
  assert(shared_loops.size() == 1);

  const auto& src_loop = src_loops[0];
  const auto& shared_loop = shared_loops[0];

  Eigen::MatrixXi shared_F_boundary;
  shared_F_boundary.resize(shared_loop.size(), 3);
  for(int i=0; i<shared_loop.size(); i++) {
    const int v0 = shared_loop[i];
    const int v1 = shared_loop[(i+1)%shared_loop.size()];
    shared_F_boundary.row(i) = Eigen::Vector3i{v0, v1, v1};
  }


  out_V = src_V;
  out_F = src_F;

  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(shared_V, shared_F_boundary);

  for(int i=0; i<src_loop.size(); i++) {
    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;
    Eigen::MatrixXd P(1, 3);
    P.row(0) = src_V.row(src_loop[i]);
    tree.squared_distance(shared_V, shared_F_boundary, P, sqrD, I, C);

    out_V.row(src_loop[i]) = C.row(0);
  }

  // std::make_tuple(out_V,out_F);
}

int MeshUtils::sharedBoundaryExtractor(Mesh mesh_l, Mesh mesh_r, std::string filename_l,std::string filename_r,std::string filename_shared,double tol)
{
  


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

  if (is_empty(shared_V_l, shared_F_l) || is_empty(shared_V_r, shared_F_r)
      || is_empty(rem_V_l, rem_F_l) || is_empty(rem_V_r, rem_F_r)) {
    //todo this should return a status code to the caller so that it can be displayed or handled based on the
    //downstream task
    throw std::runtime_error("No shared surface detected. Please check the input meshes and/or increase the tolerance");
  }

  Eigen::MatrixXd bridge_V;
  Eigen::MatrixXi bridge_F;
  // std::tie(bridge_V, bridge_F) = move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r);
  move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r,bridge_V,bridge_F);

  igl::writePLY(filename_l, bridge_V, bridge_F);
  igl::writePLY(filename_r, rem_V_r, rem_F_r);
  igl::writePLY(filename_shared, shared_V_r, shared_F_r);

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

void MeshUtils::visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh){
    //std::cout << "VTK rendering" << std::endl;

    // Render VTK for debug
    //Creates mesh actor
//    vtkNew<vtkLookupTable> lut;
//    lut->SetNumberOfColors(16);
//    lut->SetHueRange(0, 0.67);
//    lut->Build();

    //std::cout << "Setting Mesh Mapper" << std::endl;

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    mesh->getIGLMesh(V, F);

    vtkNew<vtkPolyDataMapper> meshmapper;
    meshmapper->SetInputData(mesh->getVTKMesh());
    //meshmapper->SetLookupTable(lut);
    meshmapper->SetScalarModeToUsePointData();
    meshmapper->SetColorModeToMapScalars();
    meshmapper->ScalarVisibilityOn();
    //std::cout << "GetArrayName " << meshmapper->GetArrayName() << std::endl;
    vtkNew<vtkActor> meshactor;
    meshactor->SetMapper(meshmapper);

  vtkNew<vtkNamedColors> colors;

  //std::cout << "Setting Renderer" << std::endl;

  // Visualize
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetWindowName("Mesh");
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //std::cout << "Adding arrow actors" << std::endl;
  vtkSmartPointer<vtkDataArray> vff = mesh->getVTKMesh()->GetCellData()->GetArray(0);

  // Computes grad vec for each face
  for(size_t i = 0; i < F.rows(); i++){
      const Eigen::Vector3d vert_dists(mesh->getFieldValue("value",F(i,0)), mesh->getFieldValue("value",F(i,1)), mesh->getFieldValue("value",F(i,2)));

      Eigen::Vector3d v1(V(F(i,0),0), V(F(i,0),1), V(F(i,0),2));
      Eigen::Vector3d v2(V(F(i,1),0), V(F(i,1),1), V(F(i,1),2));
      Eigen::Vector3d v3(V(F(i,2),0), V(F(i,2),1), V(F(i,2),2));
      Eigen::Vector3d face_center = (v1+v2+v3)/3;

      // Compute gradient of geodesics
      Eigen::Vector3d out_grad_eigen(vff->GetTuple3(i));
      renderer->AddActor(getArrow(face_center, face_center+out_grad_eigen));
      //out_grad_eigen *= geo_dist / out_grad_eigen.norm();
  }

  // Debug scalar and gradient queries
  //vvvvvvvvvvvvvvvvvvvvvvvvvvvv
//  Eigen::Vector3d addedpt(1,1,1);
//  Eigen::Vector3d querypt = V.row(0)+ addedpt.transpose();
//  double val = GetFFCValue(querypt);
//  Eigen::Vector3d grad = GetFFCGradient(querypt);
//  std::cout << "Querypt " << querypt.transpose() << std::endl << "val " << val << ", grad " << grad.transpose() << std::endl;
//  renderer->AddActor(getArrow(querypt, querypt+grad*10));
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  renderer->AddActor(meshactor);
  renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

  //std::cout << "Starting" << std::endl;

  renderWindow->SetWindowName("Mesh");
  renderWindow->Render();
  renderWindowInteractor->Start();
}

void CreateArrowMidPoint(double midPoint[], double startPoint[], double endPoint[])
{
  midPoint[0] = (startPoint[0] + endPoint[0]) /3;
  midPoint[1] = (startPoint[1] + endPoint[1]) /3;
  midPoint[2] = (startPoint[2] + endPoint[2]) /3;
}

void PrintArray(std::string arrayName, double *arr, int size)
{
  std::cout << arrayName << std::endl;

  for (int i = 0; i < size; i++)
    {
    std::cout << arr[i] << " " ;
    }

  std::cout << std::endl;
}

void PrintTransformParams(vtkSmartPointer<vtkTransform> transform)
{
  PrintArray("Transform orientation: ", transform->GetOrientation(), 3);
  PrintArray("Transform position: ", transform->GetPosition(), 3);
  PrintArray("Transform scale: ", transform->GetScale(), 3);
}

void Print4x4Matrix(std::string matrixName, vtkSmartPointer<vtkMatrix4x4> matrix)
{
  std::cout << matrixName << std::endl;
  for (unsigned int i = 0; i < sizeof(matrix->Element)/sizeof(matrix->Element[0]); i++)
    {
    std::cout << matrix->GetElement(i,0) << " " <<
      matrix->GetElement(i,1) << " " <<
      matrix->GetElement(i,2) << " " <<
      matrix->GetElement(i,3) << std::endl;
    }

  std::cout << std::endl;
}

vtkSmartPointer<vtkActor> MeshUtils::getArrow(Eigen::Vector3d start, Eigen::Vector3d end){
    // Create an arrow source
      vtkSmartPointer<vtkArrowSource> arrowSource =
        vtkSmartPointer<vtkArrowSource>::New();

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
     arbitrary[0] = vtkMath::Random(-10,10);
     arbitrary[1] = vtkMath::Random(-10,10);
     arbitrary[2] = vtkMath::Random(-10,10);

     vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
     vtkMath::Normalize(normalizedZ);

     // The Y axis is the cross product of Z and X axes
     vtkMath::Cross(normalizedZ, normalizedX, normalizedY);

     vtkSmartPointer<vtkMatrix4x4> matrix =
       vtkSmartPointer<vtkMatrix4x4>::New();
     // Create the direction cosine matrix
     matrix->Identity();
     for (unsigned int i = 0; i < 3; i++)
       {
       matrix->SetElement(i, 0, normalizedX[i]);
       matrix->SetElement(i, 1, normalizedY[i]);
       matrix->SetElement(i, 2, normalizedZ[i]);
       }

//     Print4x4Matrix("4x4 Matrix: ", matrix);
//     PrintArray("NormalizedX point: ", normalizedX, 3);
//     PrintArray("NormalizedY point: ", normalizedY, 3);
//     PrintArray("NormalizedZ point: ", normalizedZ, 3);

     // Apply the transforms
     vtkSmartPointer<vtkTransform> transform =
       vtkSmartPointer<vtkTransform>::New();
     transform->Translate(startPoint);
     transform->Concatenate(matrix);
     transform->Scale(length, length, length);

     // Print transform params
     //PrintTransformParams(transform);

     // Transform the polydata
     vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
       vtkSmartPointer<vtkTransformPolyDataFilter>::New();
     transformPD->SetTransform(transform);
     transformPD->SetInputConnection(arrowSource->GetOutputPort());

     // Create a mapper and actor for the arrow
     vtkSmartPointer<vtkPolyDataMapper> arrowMapper =
       vtkSmartPointer<vtkPolyDataMapper>::New();
     vtkSmartPointer<vtkActor> arrowActor =
       vtkSmartPointer<vtkActor>::New();
   #ifdef USER_MATRIX
     arrowMapper->SetInputConnection(arrowSource->GetOutputPort());
     arrowActor->SetUserMatrix(transform->GetMatrix());
   #else
     arrowMapper->SetInputConnection(transformPD->GetOutputPort());
   #endif
     arrowActor->SetMapper(arrowMapper);

     return arrowActor;
}


} // shapeworks
