#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Mesh/MeshUtils.h>
#include <Mesh/MeshWarper.h>
#include <igl/biharmonic_coordinates.h>
#include <igl/boundary_loop.h>
#include <igl/edges.h>
#include <igl/facet_components.h>
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
  if (!warp_available_) {
    return nullptr;
  }

  if (!check_warp_ready()) {
    return nullptr;
  }

  if (particles.size() != reference_particles_.size()) {
    // This may be a stale mesh warper
    // don't return nullptr or the user will get an error
    auto blank = vtkSmartPointer<vtkPolyData>::New();
    return blank;
  }

  auto points = remove_bad_particles(particles);

  vtkSmartPointer<vtkPolyData> poly_data = MeshWarper::warp_mesh(points);

  for (int i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    double* p = poly_data->GetPoint(i);
    if (std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2])) {
      warp_available_ = false;  // failed
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

  if (incoming_reference_mesh_ == reference_mesh) {
    if (reference_particles_.size() == reference_particles.size()) {
      if (reference_particles_ == reference_particles && landmarks_points_ == landmarks) {
        // we can skip, nothing has changed
        return;
      }
    }
  }

  // clone with DeepCopy for thread safety
  incoming_reference_mesh_ = vtkSmartPointer<vtkPolyData>::New();
  incoming_reference_mesh_->DeepCopy(reference_mesh);

  landmarks_points_ = landmarks;
  reference_particles_ = reference_particles;

  // mark that the warp needs to be generated
  needs_warp_ = true;

  warp_available_ = true;

  // TODO This is temporary for detecting if contour until the contour type is fully supported
  if (reference_mesh->GetNumberOfCells() > 0 && reference_mesh->GetCell(0)->GetNumberOfPoints() == 2) {
    is_contour_ = true;
    update_progress(1.0);
  }
}

//---------------------------------------------------------------------------
bool MeshWarper::get_warp_available() { return warp_available_; }

//---------------------------------------------------------------------------
bool MeshWarper::check_warp_ready() {
  std::scoped_lock lock(mutex);

  if (!needs_warp_) {
    // warp already done
    return true;
  }

  return generate_warp();
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

    reference_mesh_->BuildLinks();
    auto locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetCacheCellBounds(true);
    locator->SetDataSet(reference_mesh_);
    locator->BuildLocator();

    std::vector<vtkSmartPointer<vtkIdList>> new_triangles;
    vtkSmartPointer<vtkPoints> new_points = vtkSmartPointer<vtkPoints>::New();
    int next_point = reference_mesh_->GetNumberOfPoints();

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
      vtkCell* cell = reference_mesh_->GetCell(cell_id);

      if (cell->GetCellType() == VTK_EMPTY_CELL) {  // this one was deleted
        not_all_done = true;
        continue;
      }

      // mark this particle as complete
      done[i] = true;

      update_progress(count++ / vertices.rows());

      double point[3] = {pt[0], pt[1], pt[2]};
      double closest[3];
      double pcoords[3];
      double dist2;
      double weights[3];
      reference_mesh_->GetCell(cell_id)->EvaluatePosition(point, closest, sub_id, pcoords, dist2, weights);

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
      reference_mesh_->GetCellEdgeNeighbors(cell_id, v0_index, v1_index, neighbors);

      if (on_edge) {                             // first confirm that the other cell hasn't been deleted already
        if (neighbors->GetNumberOfIds() == 1) {  // could be 0 for the boundary of an open mesh
          vtkCell* other_cell = reference_mesh_->GetCell(neighbors->GetId(0));
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
          split_cell_on_edge(neighbors->GetId(0), new_vertex, v0_index, v1_index, new_triangles);
        }

        // split the current cell into two triangles
        split_cell_on_edge(cell_id, new_vertex, v0_index, v1_index, new_triangles);
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

        reference_mesh_->DeleteCell(cell_id);
      }
    }

    // add new points
    for (int i = 0; i < new_points->GetNumberOfPoints(); i++) {
      reference_mesh_->GetPoints()->InsertNextPoint(new_points->GetPoint(i));
    }

    // add new triangles
    for (int i = 0; i < new_triangles.size(); i++) {
      reference_mesh_->InsertNextCell(VTK_TRIANGLE, new_triangles[i]);
    }

    // remove deleted triangles and clean up
    reference_mesh_ = MeshUtils::recreate_mesh(reference_mesh_);
  }
}

//---------------------------------------------------------------------------
void MeshWarper::find_good_particles() {
  // Create the tree
  auto tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tree->SetDataSet(reference_mesh_);
  tree->BuildLocator();

  std::vector<int> ids;
  for (int i = 0; i < vertices_.rows(); i++) {
    double p[3]{vertices_(i, 0), vertices_(i, 1), vertices_(i, 2)};
    int id = tree->FindClosestPoint(p);
    ids.push_back(id);
  }

  std::set<int> set;  // initially store in set to avoid duplicates
  for (int i = 0; i < vertices_.rows(); i++) {
    for (int j = i + 1; j < vertices_.rows(); j++) {
      if (ids[i] == ids[j]) {
        set.insert(i);
        set.insert(j);
      }
    }
  }

  good_particles_.clear();
  for (int i = 0; i < vertices_.rows(); i++) {
    if (set.find(i) == set.end()) {
      good_particles_.push_back(i);
    }
  }
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::remove_bad_particles(const Eigen::MatrixXd& particles) {
  Eigen::MatrixXd new_particles(good_particles_.size(), 3);
  for (int i = 0; i < good_particles_.size(); i++) {
    int id = good_particles_[i];
    new_particles(i, 0) = particles(id, 0);
    new_particles(i, 1) = particles(id, 1);
    new_particles(i, 2) = particles(id, 2);
  }

  return new_particles;
}

//---------------------------------------------------------------------------
void MeshWarper::split_cell_on_edge(int cell_id, int new_vertex, int v0, int v1,
                                    std::vector<vtkSmartPointer<vtkIdList>>& new_triangles) {
  vtkCell* cell = reference_mesh_->GetCell(cell_id);
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

  reference_mesh_->DeleteCell(cell_id);
}

//---------------------------------------------------------------------------
bool MeshWarper::generate_warp() {
  if (is_contour_) {
    update_progress(1.0);
    needs_warp_ = false;
    return true;
  }

  const int MAX_ATTEMPTS = 2;

  for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt) {
    // Prepare mesh (remesh on retry)
    if (attempt == 0) {
      reference_mesh_ = MeshUtils::repair_mesh(incoming_reference_mesh_);
    } else {
      SW_DEBUG("Initial igl::biharmonic failed, attempting again with remeshed reference mesh");
      Mesh mesh(reference_mesh_);
      mesh.remeshPercent(0.75);
      reference_mesh_ = MeshUtils::repair_mesh(mesh.getVTKMesh());
    }

    // Prep points
    vertices_ = reference_particles_;
    if (reference_mesh_->GetNumberOfPoints() == 0) {
      SW_ERROR("Unable to warp mesh, no points in surface mesh");
      update_progress(1.0);
      warp_available_ = false;
      return false;
    }

    add_particle_vertices(vertices_);
    if (landmarks_points_.size() > 0) {
      // to ensure that the landmark points sit on the ref mesh vertices
      add_particle_vertices(landmarks_points_);
      find_landmarks_vertices_on_ref_mesh();
    }

    find_good_particles();
    vertices_ = remove_bad_particles(vertices_);
    Mesh reference_mesh(reference_mesh_);
    Eigen::MatrixXd vertices = reference_mesh.points();
    faces_ = reference_mesh.faces();

    // Perform warp
    if (MeshWarper::generate_warp_matrix(vertices, faces_, vertices_, warp_)) {
      // Success!
      update_progress(1.0);
      needs_warp_ = false;
      return true;
    }
  }

  SW_ERROR("Mesh Warp Error: igl:biharmonic_coordinates failed");

  // All attempts failed
  update_progress(1.0);
  warp_available_ = false;
  return false;
}

//---------------------------------------------------------------------------
bool MeshWarper::generate_warp_matrix(Eigen::MatrixXd target_vertices, Eigen::MatrixXi target_faces,
                                      const Eigen::MatrixXd& references_vertices, Eigen::MatrixXd& warp) {
  Eigen::VectorXi closest_vertex_indices;

  Eigen::VectorXi vertex_indices = Eigen::VectorXi::LinSpaced(target_vertices.rows(), 0, target_vertices.rows() - 1);
  Eigen::VectorXd squared_distances;
  Eigen::MatrixXd unused_closest_points;
  igl::point_mesh_squared_distance(references_vertices, target_vertices, vertex_indices, squared_distances,
                                   closest_vertex_indices, unused_closest_points);

  std::vector<std::vector<int>> handle_sets;
  igl::matrix_to_list(closest_vertex_indices, handle_sets);

  // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
  // faster and looks OK
  const int k = 2;
  if (!igl::biharmonic_coordinates(target_vertices, target_faces, handle_sets, k, warp)) {
    diagnose_biharmonic_failure(target_vertices, target_faces, handle_sets, k);
    return false;
  }

  // Throw away interior tet-vertices, keep weights and indices of boundary
  Eigen::VectorXi old_to_new_indices, referenced_vertices;
  igl::remove_unreferenced(target_vertices.rows(), target_faces, old_to_new_indices, referenced_vertices);
  std::for_each(target_faces.data(), target_faces.data() + target_faces.size(),
                [&old_to_new_indices](int& a) { a = old_to_new_indices(a); });
  std::for_each(closest_vertex_indices.data(), closest_vertex_indices.data() + closest_vertex_indices.size(),
                [&old_to_new_indices](int& a) { a = old_to_new_indices(a); });
  igl::slice(Eigen::MatrixXd(target_vertices), referenced_vertices, 1, target_vertices);
  igl::slice(Eigen::MatrixXd(warp), referenced_vertices, 1, warp);
  return true;
}

//---------------------------------------------------------------------------
bool MeshWarper::find_landmarks_vertices_on_ref_mesh() {
  auto tree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tree->SetDataSet(reference_mesh_);
  tree->BuildLocator();
  for (int i = 0; i < landmarks_points_.rows(); i++) {
    double p[3]{landmarks_points_(i, 0), landmarks_points_(i, 1), landmarks_points_(i, 2)};
    int id = tree->FindClosestPoint(p);
    landmarks_map_.insert({i, id});
    //  std::cout << "Landmark id: " << i << " Vertex id: " << id << std::endl;
  }
  return true;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::warp_mesh(const Eigen::MatrixXd& points) {
  int num_vertices = warp_.rows();
  int num_faces = faces_.rows();
  Eigen::MatrixXd v_out = warp_ * (points.rowwise() + Eigen::RowVector3d(0, 0, 0));
  vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkPoints> out_points = vtkSmartPointer<vtkPoints>::New();
  out_points->SetNumberOfPoints(num_vertices);
  for (vtkIdType i = 0; i < num_vertices; i++) {
    out_points->SetPoint(i, v_out(i, 0), v_out(i, 1), v_out(i, 2));
  }
  vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  for (vtkIdType i = 0; i < num_faces; i++) {
    polys->InsertNextCell(3);
    polys->InsertCellPoint(faces_(i, 0));
    polys->InsertCellPoint(faces_(i, 1));
    polys->InsertCellPoint(faces_(i, 2));
  }
  poly_data->SetPoints(out_points);
  poly_data->SetPolys(polys);

  return poly_data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MeshWarper::diagnose_biharmonic_failure(const Eigen::MatrixXd& TV, const Eigen::MatrixXi& TF,
                                             const std::vector<std::vector<int>>& S, int k) {
  SW_LOG_ONLY("=== Biharmonic Coordinates Failure Diagnostics ===");
  // 1. Check matrix dimensions and consistency
  SW_LOG_ONLY("Vertices: {}x{}", TV.rows(), TV.cols());
  SW_LOG_ONLY("Faces: {}x{}", TF.rows(), TF.cols());
  SW_LOG_ONLY("Handles: {}", S.size());
  SW_LOG_ONLY("k parameter: {}", k);

  // 2. Check handle indices validity
  for (size_t i = 0; i < S.size(); ++i) {
    for (int handle_idx : S[i]) {
      if (handle_idx < 0 || handle_idx >= TV.rows()) {
        SW_LOG_ONLY("ERROR: Handle {} contains invalid vertex index: {} (valid range: 0-{})", i, handle_idx,
                    TV.rows() - 1);
      }
    }
  }

  // 3. Check face indices validity
  int max_face_idx = TF.maxCoeff();
  int min_face_idx = TF.minCoeff();
  if (min_face_idx < 0 || max_face_idx >= TV.rows()) {
    SW_LOG_ONLY("ERROR: Face indices out of range. Min: {}, Max: {} (valid range: 0-{})", min_face_idx, max_face_idx,
                TV.rows() - 1);
  }

  // 4. Check mesh connectivity
  Eigen::VectorXi components;
  int num_components = igl::facet_components(TF, components);
  SW_LOG_ONLY("Connected components: {}", num_components);
  if (num_components > 1) {
    SW_LOG_ONLY("WARNING: Mesh has multiple disconnected components");
  }

  // 5. Check for boundary vertices (important for biharmonic coords)
  Eigen::VectorXi boundary;
  igl::boundary_loop(TF, boundary);
  SW_LOG_ONLY("Boundary vertices: {}", boundary.rows());

  // 6. Check mesh genus/Euler characteristic
  int V = TV.rows();
  int F = TF.rows();
  Eigen::MatrixXi E;
  igl::edges(TF, E);
  int euler_char = V - E.rows() + F;
  SW_LOG_ONLY("Euler characteristic: {} (genus ≈ {})", euler_char, (2 - euler_char) / 2);

  // 7. Check for numerical issues
  double min_coord = TV.minCoeff();
  double max_coord = TV.maxCoeff();
  double coord_range = max_coord - min_coord;
  SW_LOG_ONLY("Coordinate range: [{}, {}] (span: {})", min_coord, max_coord, coord_range);
  if (coord_range < 1e-10) {
    SW_LOG_ONLY("ERROR: Coordinates too small - numerical precision issues likely");
  }
  if (coord_range > 1e6) {
    SW_LOG_ONLY("WARNING: Large coordinate range - consider normalizing mesh");
  }

  // 8. Check triangle quality
  Eigen::VectorXd areas;
  igl::doublearea(TV, TF, areas);
  areas *= 0.5;
  double min_area = areas.minCoeff();
  double max_area = areas.maxCoeff();
  if (min_area > 0) {
    SW_LOG_ONLY("Triangle areas - Min: {}, Max: {}, Ratio: {}", min_area, max_area, max_area / min_area);
  } else {
    SW_LOG_ONLY("Triangle areas - Min: {}, Max: {}", min_area, max_area);
  }

  if (min_area < 1e-15) {
    SW_LOG_ONLY("ERROR: Extremely small triangles detected - likely degenerate");
  }

  // 9. Test if we can build the Laplacian matrix (common failure point)
  Eigen::SparseMatrix<double> L;
  try {
    igl::cotmatrix(TV, TF, L);
    SW_LOG_ONLY("Cotangent Laplacian: {}x{} ({} non-zeros)", L.rows(), L.cols(), L.nonZeros());
    // Check for NaN or inf in Laplacian
    bool has_invalid = false;
    for (int k = 0; k < L.outerSize(); ++k) {
      for (Eigen::SparseMatrix<double>::InnerIterator it(L, k); it; ++it) {
        if (!std::isfinite(it.value())) {
          has_invalid = true;
          break;
        }
      }
      if (has_invalid) break;
    }
    if (has_invalid) {
      SW_LOG_ONLY("ERROR: Laplacian matrix contains invalid values (NaN/inf)");
    }
  } catch (const std::exception& e) {
    SW_LOG_ONLY("ERROR: Failed to build Laplacian matrix: {}", e.what());
  }
  SW_LOG_ONLY("=== End Diagnostics ===");
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
