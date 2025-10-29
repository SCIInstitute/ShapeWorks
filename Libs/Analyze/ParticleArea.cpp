#include "ParticleArea.h"

#include <Logging.h>
#include <Mesh/Mesh.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkMassProperties.h>
#include <vtkPointData.h>
#include <vtkTriangle.h>

// geometry central
#include <geometrycentral/surface/heat_method_distance.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <geometrycentral/surface/surface_mesh_factories.h>
#include <nlohmann/json.hpp>
namespace shapeworks {

//-----------------------------------------------------------------------------
void ParticleArea::assign_vertex_particles(vtkSmartPointer<vtkPolyData> poly_data,
                                           std::vector<itk::Point<double>> particles) {
  SW_DEBUG("Assigning vertex particles");
  // geodesics enabled mesh
  Mesh mesh(poly_data);

  // create "closest_particle" array
  auto closest_particle_array = vtkSmartPointer<vtkIntArray>::New();
  closest_particle_array->SetName("closest_particle");
  closest_particle_array->SetNumberOfComponents(1);
  closest_particle_array->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  poly_data->GetPointData()->AddArray(closest_particle_array);

  // set up geometry central for geodesic distances

  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  vtkSmartPointer<vtkPoints> points;
  points = mesh.getIGLMesh(V, F);

  using namespace geometrycentral::surface;
  std::unique_ptr<SurfaceMesh> gcmesh;
  std::unique_ptr<VertexPositionGeometry> gcgeometry;
  std::tie(gcmesh, gcgeometry) = makeSurfaceMeshAndGeometry(V, F);
  HeatMethodDistanceSolver heat_solver(*gcgeometry, 1.0, true);

  std::vector<double> min_dists(poly_data->GetNumberOfPoints(), std::numeric_limits<double>::max());

  for (int i = 0; i < particles.size(); ++i) {
    auto point = particles[i];
    // construct SurfacePoint from face and barycentric coordinates
    auto face = mesh.getClosestFace(point);
    Eigen::Vector3d pt;
    pt[0] = point[0];
    pt[1] = point[1];
    pt[2] = point[2];
    auto bary = mesh.computeBarycentricCoordinates(pt, face);
    Face f = gcmesh->face(face);
    geometrycentral::Vector3 bary_vec;
    bary_vec[0] = bary[0];
    bary_vec[1] = bary[1];
    bary_vec[2] = bary[2];

    auto targetP = SurfacePoint(f, bary_vec);

    VertexData<double> distToSource = heat_solver.computeDistance(targetP);

    for (int j = 0; j < poly_data->GetNumberOfPoints(); j++) {
      auto dist = distToSource[gcmesh->vertex(j)];
      if (dist < min_dists[j]) {
        min_dists[j] = dist;
        // assign the particle id
        closest_particle_array->SetTuple1(j, i);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void ParticleArea::assign_vertex_colors(vtkSmartPointer<vtkPolyData> poly_data, std::vector<QColor> colors) {
  SW_DEBUG("Assigning vertex colors");
  // create rgb colors array
  auto colors_array = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors_array->SetNumberOfComponents(3);
  colors_array->SetName("colors");
  colors_array->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  poly_data->GetPointData()->AddArray(colors_array);
  auto closest_particles = poly_data->GetPointData()->GetArray("closest_particle");

  // for each vertex
  for (int i = 0; i < poly_data->GetNumberOfPoints(); ++i) {
    // get the particle id from the "closest_point" array
    auto particle_id = closest_particles->GetTuple1(i);
    auto color = colors[particle_id];
    colors_array->SetTuple3(i, color.red(), color.green(), color.blue());
  }
}

//-----------------------------------------------------------------------------
void ParticleArea::assign_vertex_areas(vtkSmartPointer<vtkPolyData> poly_data, Eigen::VectorXd areas) {
  SW_DEBUG("Assigning vertex areas");
  // create rgb colors array
  auto array = vtkSmartPointer<vtkFloatArray>::New();
  array->SetNumberOfComponents(1);
  array->SetName("particle_area");
  array->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  poly_data->GetPointData()->AddArray(array);

  auto closest_particles = poly_data->GetPointData()->GetArray("closest_particle");

  // for each vertex
  for (int i = 0; i < poly_data->GetNumberOfPoints(); ++i) {
    // get the particle id from the "closest_point" array
    auto particle_id = closest_particles->GetTuple1(i);
    array->SetTuple1(i, areas[particle_id]);
  }
}

//-----------------------------------------------------------------------------
std::vector<QColor> ParticleArea::colors_from_lut(vtkSmartPointer<vtkLookupTable> lut) {
  std::vector<QColor> colors;
  for (int i = 0; i < lut->GetNumberOfTableValues(); ++i) {
    double *color = lut->GetTableValue(i);
    colors.push_back(QColor(color[0] * 255, color[1] * 255, color[2] * 255));
  }
  return colors;
}

//-----------------------------------------------------------------------------
Eigen::VectorXd ParticleArea::compute_particle_triangle_areas(vtkSmartPointer<vtkPolyData> poly_data,
                                                              std::vector<itk::Point<double>> particles) {
  auto closest_particles = poly_data->GetPointData()->GetArray("closest_particle");

  // for each cell
  Eigen::VectorXd areas(particles.size());
  areas.setZero();
  for (int i = 0; i < poly_data->GetNumberOfCells(); ++i) {
    // get the area of this cell
    auto cell = poly_data->GetCell(i);
    auto points = cell->GetPoints();
    double p0[3], p1[3], p2[3];
    points->GetPoint(0, p0);
    points->GetPoint(1, p1);
    points->GetPoint(2, p2);
    auto area = vtkTriangle::TriangleArea(p0, p1, p2);

    // for each vertex of the cell, give 1/3 of the area to the particle
    for (int j = 0; j < 3; ++j) {
      auto vertex_id = cell->GetPointId(j);
      auto particle_id = closest_particles->GetTuple1(vertex_id);
      areas[particle_id] += area / 3.0;
    }
  }

  // ideally each particle has 1/num_particles of the total area
  // scale based on this value
  int num_particles = particles.size();
  double total_area = areas.sum();
  areas *= num_particles / total_area * 100;

  return areas;
}

//-----------------------------------------------------------------------------
void ParticleArea::create_ffc_from_particles(
    const std::string& mesh_path, const Eigen::MatrixXd& particles,
    const std::string& ffc_filename,
    bool constraints_flag,
    double alpha
) {

  using json = nlohmann::json;

  Mesh mesh(mesh_path);
  auto poly_data = mesh.getVTKMesh();

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  vtkSmartPointer<vtkPoints> points;
  points = mesh.getIGLMesh(V, F);

  using namespace geometrycentral::surface;
  std::unique_ptr<SurfaceMesh> gcmesh;
  std::unique_ptr<VertexPositionGeometry> gcgeometry;
  std::tie(gcmesh, gcgeometry) = makeSurfaceMeshAndGeometry(V, F);

  HeatMethodDistanceSolver heat_solver(*gcgeometry, 1.0, true);

  std::vector<SurfacePoint> sources;
  sources.reserve(particles.rows());

  for (int i = 0; i < particles.rows(); ++i) {
    Eigen::Vector3d pt = particles.row(i).transpose();
    auto face_id = mesh.getClosestFace(Point3({pt[0], pt[1], pt[2]}));
    auto bary = mesh.computeBarycentricCoordinates(pt, face_id);

    Face f = gcmesh->face(face_id);
    geometrycentral::Vector3 bary_vec{bary[0], bary[1], bary[2]};
    sources.emplace_back(f, bary_vec);
  }

  // Compute geodesic distances to all sources at once
  VertexData<double> distToSources = heat_solver.computeDistance(sources);

  double totalLength = 0.0;
  int edgeCount = 0;
  for (Edge e : gcmesh->edges()) {
    totalLength += gcgeometry->edgeLength(e);
    edgeCount++;
  }
  double meanEdge = (edgeCount > 0) ? totalLength / edgeCount : 1.0;

  // double radius = 2.5 * meanEdge;
  double radius = alpha * meanEdge;

  std::vector<int> scalars(poly_data->GetNumberOfPoints(), 1);
  for (Vertex v : gcmesh->vertices()) {
    if (distToSources[v] < radius) {
      // scalars[v.getIndex()] = 0;  // inside constraint region
      scalars[v.getIndex()] = constraints_flag;  // inside constraint region
    }
    else {
      scalars[v.getIndex()] = !constraints_flag;  // outside constraint region
      
    }
  }

  json ffcJson;
  json pointsJson;

  for (int j = 0; j < poly_data->GetNumberOfPoints(); j++) {
    double p[3];
    poly_data->GetPoint(j, p);
    pointsJson.push_back({p[0], p[1], p[2]});
  }

  ffcJson["field"] = {{"points", pointsJson}, {"scalars", scalars}};

  json constraints_json;
  constraints_json["free_form_constraints"] = ffcJson;

  std::ofstream file(ffc_filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open constraint file \"" +
                             ffc_filename + "\" for writing");
  }
  file << constraints_json.dump(4);
}

}  // namespace shapeworks
