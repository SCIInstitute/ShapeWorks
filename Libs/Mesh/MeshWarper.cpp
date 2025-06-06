#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshWarper.h>
#include <igl/biharmonic_coordinates.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/remove_unreferenced.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLine.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>

#include <set>

namespace shapeworks {

// for concurrent access
static std::mutex mutex;

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::build_mesh(const Eigen::MatrixXd& particles) {
  if (!this->warp_available_) {
    return nullptr;
  }

  if (!this->check_warp_ready()) {
    return nullptr;
  }

  if (particles.size() != reference_particles_.size()) {
    // This may be a stale mesh warper
    // don't return nullptr or the user will get an error
    auto blank = vtkSmartPointer<vtkPolyData>::New();
    return blank;
  }

  auto points = this->remove_bad_particles(particles);

  vtkSmartPointer<vtkPolyData> poly_data = MeshWarper::warp_mesh(points);

  for (int i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    double* p = poly_data->GetPoint(i);
    if (std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2])) {
      this->warp_available_ = false;  // failed
      SW_ERROR("Reconstruction failed. NaN detected in mesh.");
      return nullptr;
    }
  }
  return poly_data;
}

//---------------------------------------------------------------------------
void MeshWarper::set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                                    const Eigen::MatrixXd& reference_particles, const Eigen::MatrixXd& landmarks) {
  // lock so that we don't swap out the reference mesh while we are using it
  std::scoped_lock lock(mutex);

  if (this->incoming_reference_mesh_ == reference_mesh) {
    if (this->reference_particles_.size() == reference_particles.size()) {
      if (this->reference_particles_ == reference_particles && landmarks_points_ == landmarks) {
        // we can skip, nothing has changed
        return;
      }
    }
  }

  // clone with DeepCopy for thread safety
  incoming_reference_mesh_ = vtkSmartPointer<vtkPolyData>::New();
  incoming_reference_mesh_->DeepCopy(reference_mesh);

  this->landmarks_points_ = landmarks;
  this->reference_particles_ = reference_particles;

  // mark that the warp needs to be generated
  this->needs_warp_ = true;

  this->warp_available_ = true;

  // TODO This is temporary for detecting if contour until the contour type is fully supported
  if (reference_mesh->GetNumberOfCells() > 0 && reference_mesh->GetCell(0)->GetNumberOfPoints() == 2) {
    this->is_contour_ = true;
    this->update_progress(1.0);
  }
}

//---------------------------------------------------------------------------
bool MeshWarper::get_warp_available() { return this->warp_available_; }

//---------------------------------------------------------------------------
bool MeshWarper::check_warp_ready() {
  std::scoped_lock lock(mutex);

  if (!this->needs_warp_) {
    // warp already done
    return true;
  }

  return this->generate_warp();
}

//---------------------------------------------------------------------------
void MeshWarper::add_particle_vertices(Eigen::MatrixXd& vertices) {
  std::vector<bool> done(vertices.rows(), false);

  bool not_all_done = true;
  float count = 0;

  if (!Mesh(reference_mesh_).detectTriangular()) {
    throw std::runtime_error("Mesh is not fully triangular");
  }

  // Iteratively process the mesh: Find all new vertices that can be added without conflicting
  // with each other.  If multiple particles (new vertices) land on the same triangle, the we will need
  // multiple passes since we will have to make the changes and rebuild the locator.
  while (not_all_done) {
    not_all_done = false;

    this->reference_mesh_->BuildLinks();
    auto locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetCacheCellBounds(true);
    locator->SetDataSet(this->reference_mesh_);
    locator->BuildLocator();

    std::vector<vtkSmartPointer<vtkIdList>> new_triangles;
    vtkSmartPointer<vtkPoints> new_points = vtkSmartPointer<vtkPoints>::New();
    int next_point = this->reference_mesh_->GetNumberOfPoints();

    for (int i = 0; i < vertices.rows(); i++) {
      if (done[i]) {
        continue;
      }

      double pt[3]{vertices(i, 0), vertices(i, 1), vertices(i, 2)};
      double closest_point[3];     // the coordinates of the closest point will be returned here
      double closest_point_dist2;  // the squared distance to the closest point will be returned here
      vtkIdType cell_id;           // the cell id of the cell containing the closest point will be returned here
      int sub_id;                  // this is rarely used (in triangle strips only, I believe)
      locator->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

      pt[0] = closest_point[0];
      pt[1] = closest_point[1];
      pt[2] = closest_point[2];

      // grab the closest cell
      vtkCell* cell = this->reference_mesh_->GetCell(cell_id);

      if (cell->GetCellType() == VTK_EMPTY_CELL) {  // this one was deleted
        not_all_done = true;
        continue;
      }

      // mark this particle as complete
      done[i] = true;

      this->update_progress(count++ / vertices.rows());

      double point[3] = {pt[0], pt[1], pt[2]};
      double closest[3];
      double pcoords[3];
      double dist2;
      double weights[3];
      this->reference_mesh_->GetCell(cell_id)->EvaluatePosition(point, closest, sub_id, pcoords, dist2, weights);

      if (weights[0] > 0.99 || weights[1] > 0.99 || weights[2] > 0.99) {
        // close enough to a vertex
        continue;
      }

      // now we need to check if we are along an edge already.
      bool on_edge = false;
      int v0_index = 0, v1_index = 0;
      double p0[3], p1[3], p2[3];
      cell->GetPoints()->GetPoint(0, p0);
      cell->GetPoints()->GetPoint(1, p1);
      cell->GetPoints()->GetPoint(2, p2);
      if (weights[2] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(0);
        v1_index = cell->GetPointId(1);
      } else if (weights[0] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(1);
        v1_index = cell->GetPointId(2);
      } else if (weights[1] < 0.01) {
        on_edge = true;
        v0_index = cell->GetPointId(0);
        v1_index = cell->GetPointId(2);
      }

      auto neighbors = vtkSmartPointer<vtkIdList>::New();
      this->reference_mesh_->GetCellEdgeNeighbors(cell_id, v0_index, v1_index, neighbors);

      if (on_edge) {                             // first confirm that the other cell hasn't been deleted already
        if (neighbors->GetNumberOfIds() == 1) {  // could be 0 for the boundary of an open mesh
          vtkCell* other_cell = this->reference_mesh_->GetCell(neighbors->GetId(0));
          if (other_cell->GetCellType() == VTK_EMPTY_CELL) {  // this one was deleted
            not_all_done = true;
            continue;
          }
        }
      }

      // add the new vertex
      int new_vertex = next_point++;
      new_points->InsertNextPoint(pt);

      if (on_edge) {
        if (neighbors->GetNumberOfIds() == 1) {  // could be 0 for the boundary of an open mesh
          // split the neighbor cell into two triangles as well
          this->split_cell_on_edge(neighbors->GetId(0), new_vertex, v0_index, v1_index, new_triangles);
        }

        // split the current cell into two triangles
        this->split_cell_on_edge(cell_id, new_vertex, v0_index, v1_index, new_triangles);
      } else {  // interior to a current triangle
        vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
        list->SetNumberOfIds(3);

        list->SetId(0, cell->GetPointId(1));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(0));
        new_triangles.push_back(list);

        list = vtkSmartPointer<vtkIdList>::New();
        list->SetNumberOfIds(3);
        list->SetId(0, cell->GetPointId(2));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(1));
        new_triangles.push_back(list);

        list = vtkSmartPointer<vtkIdList>::New();
        list->SetNumberOfIds(3);
        list->SetId(0, cell->GetPointId(0));
        list->SetId(1, new_vertex);
        list->SetId(2, cell->GetPointId(2));
        new_triangles.push_back(list);

        this->reference_mesh_->DeleteCell(cell_id);
      }
    }

    // add new points
    for (int i = 0; i < new_points->GetNumberOfPoints(); i++) {
      this->reference_mesh_->GetPoints()->InsertNextPoint(new_points->GetPoint(i));
    }

    // add new triangles
    for (int i = 0; i < new_triangles.size(); i++) {
      this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, new_triangles[i]);
    }

    // remove deleted triangles and clean up
    this->reference_mesh_ = MeshWarper::recreate_mesh(this->reference_mesh_);
  }
}

//---------------------------------------------------------------------------
void MeshWarper::find_good_particles() {
  // Create the tree
  auto tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tree->SetDataSet(this->reference_mesh_);
  tree->BuildLocator();

  std::vector<int> ids;
  for (int i = 0; i < this->vertices_.rows(); i++) {
    double p[3]{this->vertices_(i, 0), this->vertices_(i, 1), this->vertices_(i, 2)};
    int id = tree->FindClosestPoint(p);
    ids.push_back(id);
  }

  std::set<int> set;  // initially store in set to avoid duplicates
  for (int i = 0; i < this->vertices_.rows(); i++) {
    for (int j = i + 1; j < this->vertices_.rows(); j++) {
      if (ids[i] == ids[j]) {
        set.insert(i);
        set.insert(j);
      }
    }
  }

  this->good_particles_.clear();
  for (int i = 0; i < this->vertices_.rows(); i++) {
    if (set.find(i) == set.end()) {
      this->good_particles_.push_back(i);
    }
  }
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::remove_bad_particles(const Eigen::MatrixXd& particles) {
  Eigen::MatrixXd new_particles(this->good_particles_.size(), 3);
  for (int i = 0; i < this->good_particles_.size(); i++) {
    int id = this->good_particles_[i];
    new_particles(i, 0) = particles(id, 0);
    new_particles(i, 1) = particles(id, 1);
    new_particles(i, 2) = particles(id, 2);
  }

  return new_particles;
}

//---------------------------------------------------------------------------
void MeshWarper::split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                                    std::vector<vtkSmartPointer<vtkIdList>>& new_triangles) {
  vtkCell* cell = this->reference_mesh_->GetCell(cell_id);
  int p0 = cell->GetPointId(0);
  int p1 = cell->GetPointId(1);
  int p2 = cell->GetPointId(2);
  int edge = -1;
  if ((v0 == p0 && v1 == p1) || (v0 == p1 && v1 == p0)) {
    edge = 0;
  } else if ((v0 == p1 && v1 == p2) || (v0 == p2 && v1 == p1)) {
    edge = 1;
  } else if ((v0 == p0 && v1 == p2) || (v0 == p2 && v1 == p0)) {
    edge = 2;
  }

  if (edge == 0) {
    vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(1));
    list->SetId(2, cell->GetPointId(2));
    new_triangles.push_back(list);

    list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(2));
    list->SetId(2, cell->GetPointId(0));
    new_triangles.push_back(list);
  } else if (edge == 1) {
    vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(0));
    list->SetId(2, cell->GetPointId(1));
    new_triangles.push_back(list);

    list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(2));
    list->SetId(2, cell->GetPointId(0));
    new_triangles.push_back(list);
  } else if (edge == 2) {
    vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(0));
    list->SetId(2, cell->GetPointId(1));
    new_triangles.push_back(list);

    list = vtkSmartPointer<vtkIdList>::New();
    list->SetNumberOfIds(3);
    list->SetId(0, new_vertex);
    list->SetId(1, cell->GetPointId(1));
    list->SetId(2, cell->GetPointId(2));
    new_triangles.push_back(list);
  }

  this->reference_mesh_->DeleteCell(cell_id);
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::prep_mesh(vtkSmartPointer<vtkPolyData> mesh) {
  vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInputData(mesh);
  triangle_filter->PassLinesOff();
  triangle_filter->Update();

  vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivity = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
  connectivity->SetInputConnection(triangle_filter->GetOutputPort());
  connectivity->SetExtractionModeToLargestRegion();
  connectivity->Update();

  auto cleaned = MeshWarper::clean_mesh(connectivity->GetOutput());

  auto fixed = Mesh(cleaned).fixNonManifold();

  // remove deleted triangles and clean up
  auto recreated = MeshWarper::recreate_mesh(fixed.getVTKMesh());

  return MeshWarper::remove_zero_area_triangles(recreated);
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::clean_mesh(vtkSmartPointer<vtkPolyData> mesh) {
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
vtkSmartPointer<vtkPolyData> MeshWarper::remove_zero_area_triangles(vtkSmartPointer<vtkPolyData> mesh) {
  // Create a new polydata to store the filtered triangles
  auto filtered_mesh = vtkSmartPointer<vtkPolyData>::New();

  // Get the points from the input mesh
  vtkSmartPointer<vtkPoints> points = mesh->GetPoints();
  filtered_mesh->SetPoints(points);

  // Create a new cell array to store triangles
  auto newTriangles = vtkSmartPointer<vtkCellArray>::New();

  // Get the triangle cells from the input mesh
  vtkCellArray* triangles = mesh->GetPolys();

  // Iterate through all cells
  auto cell_point_ids = vtkSmartPointer<vtkIdList>::New();

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

      // If the area is not zero (use a small epsilon for floating point comparison)
      const double EPSILON = 1e-10;
      if (area > EPSILON) {
        // Add this triangle to the new cell array
        newTriangles->InsertNextCell(cell_point_ids);
      }
    }
  }

  // Set the triangles in the filtered mesh
  filtered_mesh->SetPolys(newTriangles);

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
vtkSmartPointer<vtkPolyData> MeshWarper::recreate_mesh(vtkSmartPointer<vtkPolyData> mesh) {
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
bool MeshWarper::generate_warp() {
  if (is_contour_) {
    this->update_progress(1.0);
    this->needs_warp_ = false;
    return true;
  }
  this->reference_mesh_ = MeshWarper::prep_mesh(this->incoming_reference_mesh_);

  // prep points
  this->vertices_ = this->reference_particles_;

  if (reference_mesh_->GetNumberOfPoints() == 0) {
    SW_ERROR("Unable to warp mesh, no points in surface mesh");
    update_progress(1.0);
    warp_available_ = false;
    return false;
  }

  this->add_particle_vertices(this->vertices_);

  if (this->landmarks_points_.size() > 0) {
    // to ensure that the landmark points sit on the ref mesh vertices
    this->add_particle_vertices(this->landmarks_points_);
    this->find_landmarks_vertices_on_ref_mesh();
  }

  this->find_good_particles();
  this->vertices_ = this->remove_bad_particles(this->vertices_);

  Mesh referenceMesh(reference_mesh_);
  Eigen::MatrixXd vertices = referenceMesh.points();
  this->faces_ = referenceMesh.faces();

  // perform warp
  if (!MeshWarper::generate_warp_matrix(vertices, this->faces_, this->vertices_, this->warp_)) {
    this->update_progress(1.0);
    this->warp_available_ = false;
    return false;
  }
  this->update_progress(1.0);
  this->needs_warp_ = false;
  return true;
}

//---------------------------------------------------------------------------
bool MeshWarper::generate_warp_matrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF, const Eigen::MatrixXd& Vref,
                                      Eigen::MatrixXd& W) {
  Eigen::VectorXi b;
  {
    Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(), 0, TV.rows() - 1);
    Eigen::VectorXd sqrD;
    Eigen::MatrixXd _2;
    // using J which is N by 1 instead of a matrix that represents faces of N by 3
    // so that we will find the closest vertices instead of closest point on the face
    // so far the two meshes are not separated. So what we are really doing here
    // is computing handles from low resolution and use that for the high resolution one
    igl::point_mesh_squared_distance(Vref, TV, J, sqrD, b, _2);
    // assert(sqrD.maxCoeff() < 1e-7 && "Particles must exist on vertices");
  }

  // list of points --> list of singleton lists
  std::vector<std::vector<int>> S;
  igl::matrix_to_list(b, S);

  // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
  // faster and looks OK
  const int k = 2;
  if (!igl::biharmonic_coordinates(TV, TF, S, k, W)) {
    SW_ERROR("Mesh Warp Error: igl:biharmonic_coordinates failed");
    return false;
  }
  // Throw away interior tet-vertices, keep weights and indices of boundary
  Eigen::VectorXi I, J;
  igl::remove_unreferenced(TV.rows(), TF, I, J);
  std::for_each(TF.data(), TF.data() + TF.size(), [&I](int& a) { a = I(a); });
  std::for_each(b.data(), b.data() + b.size(), [&I](int& a) { a = I(a); });
  igl::slice(Eigen::MatrixXd(TV), J, 1, TV);
  igl::slice(Eigen::MatrixXd(W), J, 1, W);
  return true;
}

//---------------------------------------------------------------------------
bool MeshWarper::find_landmarks_vertices_on_ref_mesh() {
  auto tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tree->SetDataSet(this->reference_mesh_);
  tree->BuildLocator();
  for (int i = 0; i < this->landmarks_points_.rows(); i++) {
    double p[3]{this->landmarks_points_(i, 0), this->landmarks_points_(i, 1), this->landmarks_points_(i, 2)};
    int id = tree->FindClosestPoint(p);
    landmarks_map_.insert({i, id});
    //  std::cout << "Landmark id: " << i << " Vertex id: " << id << std::endl;
  }
  return true;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::warp_mesh(const Eigen::MatrixXd& points) {
  int num_vertices = this->warp_.rows();
  int num_faces = this->faces_.rows();
  Eigen::MatrixXd v_out = this->warp_ * (points.rowwise() + Eigen::RowVector3d(0, 0, 0));
  vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> out_points = vtkSmartPointer<vtkPoints>::New();
  out_points->SetNumberOfPoints(num_vertices);
  for (vtkIdType i = 0; i < num_vertices; i++) {
    out_points->SetPoint(i, v_out(i, 0), v_out(i, 1), v_out(i, 2));
  }
  vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  for (vtkIdType i = 0; i < num_faces; i++) {
    polys->InsertNextCell(3);
    polys->InsertCellPoint(this->faces_(i, 0));
    polys->InsertCellPoint(this->faces_(i, 1));
    polys->InsertCellPoint(this->faces_(i, 2));
  }
  poly_data->SetPoints(out_points);
  poly_data->SetPolys(polys);

  return poly_data;
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::extract_landmarks(vtkSmartPointer<vtkPolyData> warped_mesh) {
  Eigen::MatrixXd landmarks;
  landmarks.resize(landmarks_map_.size(), 3);

  vtkSmartPointer<vtkDataArray> data_array = warped_mesh->GetPoints()->GetData();

  for (auto ids : landmarks_map_) {
    auto id_landmark = ids.first;
    auto id_vertex = ids.second;

    landmarks(id_landmark, 0) = data_array->GetComponent(id_vertex, 0);
    landmarks(id_landmark, 1) = data_array->GetComponent(id_vertex, 1);
    landmarks(id_landmark, 2) = data_array->GetComponent(id_vertex, 2);
  }

  return landmarks;
}

}  // namespace shapeworks

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
