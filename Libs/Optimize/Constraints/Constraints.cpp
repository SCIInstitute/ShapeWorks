#include "Constraints.h"

#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <nlohmann/json.hpp>

#include "Libs/Common/Logging.h"
using json = nlohmann::json;

namespace shapeworks {

//-----------------------------------------------------------------------------
static json get_json_field(vtkSmartPointer<vtkPolyData> polyData) {
  // store points from inout to json
  json j;

  if (!polyData) {
    return j;
  }

  json inoutJson;
  json pointsJson;
  // for each point in inout store x,y,z
  for (int i = 0; i < polyData->GetNumberOfPoints(); i++) {
    double p[3];
    polyData->GetPoint(i, p);
    pointsJson.push_back({p[0], p[1], p[2]});
  }

  // store scalars from inout to json
  vtkFloatArray* array = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetArray("ffc_paint"));
  if (array) {
    std::vector<int> scalars;
    for (int i = 0; i < polyData->GetNumberOfPoints(); i++) {
      scalars.push_back(array->GetValue(i));
    }
    j = {{"points", pointsJson}, {"scalars", scalars}};
  }

  return j;
}

//-----------------------------------------------------------------------------
static vtkSmartPointer<vtkPolyData> get_polydata_from_json_field(json j) {
  if (!j.contains("points") || !j.contains("scalars")) {
    SW_LOG("Invalid json field for polydata");
    return nullptr;
  }

  auto polyData = vtkSmartPointer<vtkPolyData>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();
  auto scalars = vtkSmartPointer<vtkFloatArray>::New();
  scalars->SetName("ffc_paint");
  for (auto p : j["points"]) {
    points->InsertNextPoint(p[0].get<double>(), p[1].get<double>(), p[2].get<double>());
  }
  for (auto s : j["scalars"]) {
    scalars->InsertNextValue(s.get<float>());
  }
  polyData->SetPoints(points);
  polyData->GetPointData()->AddArray(scalars);
  return polyData;
}

void Constraints::addPlane(const vnl_vector<double>& a, const vnl_vector<double>& b, const vnl_vector<double>& c) {
  // See http://mathworld.wolfram.com/Plane.html, for example
  vnl_vector<double> q;
  if (DIMENSION == 3) {
    q = vnl_cross_3d((b - a), (c - a));
  } else if (DIMENSION == 2) {
    q = vnl_cross_2d((b - a), (c - a));
  }

  if (q.magnitude() > 0.0) {
    Eigen::Vector3d qp;
    q = q / q.magnitude();
    qp(0) = q[0];
    qp(1) = q[1];
    qp(2) = q[2];
    PlaneConstraint plane_c;
    plane_c.setPlaneNormal(qp);
    plane_c.setPlanePoint(a);
    planeConstraints_.push_back(plane_c);
    active_ = true;
  }
}

void Constraints::addSphere(const vnl_vector_fixed<double, DIMENSION>& v, double r) {
  Eigen::Vector3d c;
  c(0) = v[0];
  c(1) = v[1];
  c(2) = v[2];
  SphereConstraint sphere_c;
  sphere_c.setCenter(c);
  sphere_c.setRadius(r);
  sphereConstraints_.push_back(sphere_c);
  active_ = true;
}

bool Constraints::transformConstraints(const vnl_matrix_fixed<double, 4, 4>& transform) {
  return transformPlanes(transform) & true;
}

bool Constraints::transformPlanes(const vnl_matrix_fixed<double, 4, 4>& transform) {
  std::cout << "Transforming planes" << transform << std::endl;

  // Convert vnl_matrix to Eigen Matrix
  Eigen::Matrix4d trans;
  for (unsigned int i = 0; i < 4; i++) {
    for (unsigned int j = 0; j < 4; j++) {
      trans(i, j) = transform[i][j];
    }
  }

  // Maybe TODO Check if transformation matrix is invertible. Return false if not.

  // Transform each plane to new format
  for (unsigned int i = 0; i < planeConstraints_.size(); i++) {
    // Get points
    Eigen::Vector3d norm = planeConstraints_[i].getPlaneNormal();
    Eigen::Vector3d point = planeConstraints_[i].getPlanePoint();

    // Convert to homogeneous coordinates i.e. add an extra coordinate
    Eigen::Vector4d norm_homogeneous;
    norm_homogeneous(0) = norm(0);
    norm_homogeneous(1) = norm(1);
    norm_homogeneous(2) = norm(2);
    norm_homogeneous(3) = 0;
    Eigen::Vector4d point_homogeneous;
    point_homogeneous(0) = point(0);
    point_homogeneous(1) = point(1);
    point_homogeneous(2) = point(2);
    point_homogeneous(3) = 1;

    // Apply transformation
    // N = M^{-1}^{T} * N
    // O = M * O
    Eigen::Vector4d new_point_homogeneous = trans * point_homogeneous;
    Eigen::Vector4d new_norm_homogeneous = trans.inverse().transpose() * norm_homogeneous;
    new_point_homogeneous = new_point_homogeneous / new_point_homogeneous(3);
    new_norm_homogeneous = new_norm_homogeneous / new_norm_homogeneous(3);

    // Return to 3D coordinates
    Eigen::Vector3d new_point = new_point_homogeneous.head<3>();
    Eigen::Vector3d new_norm = new_norm_homogeneous.head<3>();
    new_norm = new_norm / new_norm.norm();

    // Set transformed planes
    planeConstraints_[i].setPlaneNormal(new_norm);
    planeConstraints_[i].setPlanePoint(new_point);
  }

  return true;
}

bool Constraints::isAnyViolated(const Point3& pos) {
  Eigen::Vector3d pt;
  pt(0) = pos[0];
  pt(1) = pos[1];
  pt(2) = pos[2];
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    if (planeConstraints_[i].isViolated(pt)) {
      return true;
    }
  }
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    if (sphereConstraints_[i].isViolated(pt)) {
      return true;
    }
  }
  if (freeFormConstraint_.getMesh()) {
    if (freeFormConstraint_.isViolated(pt)) {
      return true;
    }
  }
  return false;
}

void Constraints::printAll() {
  std::cout << "Cutting planes " << planeConstraints_.size() << std::endl;
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    planeConstraints_[i].print();
  }
  std::cout << "Cutting spheres " << sphereConstraints_.size() << std::endl;
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    sphereConstraints_[i].print();
  }
}

std::string Constraints::violationReport(const Point3& pos, size_t index) {
  Eigen::Vector3d pt;
  pt(0) = pos[0];
  pt(1) = pos[1];
  pt(2) = pos[2];
  std::stringstream stream;
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    if (planeConstraints_[i].constraintEval(pt) > 0)
      stream << "CuttingPlane " << i << "/" << planeConstraints_.size() << ": "
             << planeConstraints_[i].constraintEval(pt) << " gradient(c=1) "
             << planeConstraints_[i].lagragianGradient(pt, 1, index).transpose() << std::endl;
  }
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    if (sphereConstraints_[i].constraintEval(pt) > 0)
      stream << "Sphere " << i << "/" << planeConstraints_.size() << ": " << sphereConstraints_[i].constraintEval(pt)
             << " gradient(c=1) " << sphereConstraints_[i].lagragianGradient(pt, pt, 1, index).transpose() << std::endl;
  }
  if (freeFormConstraint_.getMesh()) {
    if (freeFormConstraint_.constraintEval(pt) > 0)
      stream << "FreeForm "
             << ": " << freeFormConstraint_.constraintEval(pt) << " gradient(c=1) "
             << freeFormConstraint_.lagragianGradient(pt, 1, index).transpose() << std::endl;
  }
  return stream.str();
}

std::vector<std::vector<double>> Constraints::violationReportData(const Point3& pos) {
  std::vector<std::vector<double>> alls;
  Eigen::Vector3d pt;
  pt(0) = pos[0];
  pt(1) = pos[1];
  pt(2) = pos[2];
  std::stringstream stream;
  std::vector<double> pl;
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    pl.push_back(planeConstraints_[i].constraintEval(pt));
  }
  alls.push_back(pl);
  std::vector<double> sp;
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    sp.push_back(sphereConstraints_[i].constraintEval(pt));
  }
  alls.push_back(sp);
  std::vector<double> ff;
  if (freeFormConstraint_.readyForOptimize()) {
    ff.push_back(freeFormConstraint_.constraintEval(pt));
  }
  alls.push_back(ff);
  return alls;
}

vnl_vector_fixed<double, 3> Constraints::constraintsLagrangianGradient(const Point3& pos, const Point3& prepos,
                                                                       double C, size_t index) {
  Eigen::Vector3d pt;
  pt(0) = pos[0];
  pt(1) = pos[1];
  pt(2) = pos[2];
  Eigen::Vector3d prept;
  prept(0) = prepos[0];
  prept(1) = prepos[1];
  prept(2) = prepos[2];
  Eigen::Vector3d grad = Eigen::Vector3d(0, 0, 0);
  // std::stringstream stream;
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    // if(planeConsts[i].ConstraintEval(pt)>0) stream << "CuttingPlane " << i << "/" << planeConsts.size() << ": "
    // << planeConsts[i].LagragianGradient(pt, C).transpose() << " ::: " << planeConsts[i].ConstraintEval(pt) <<
    // std::endl;
    grad += planeConstraints_[i].lagragianGradient(pt, C, index);
  }
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    grad += sphereConstraints_[i].lagragianGradient(prept, pt, C, index);
  }
  if (freeFormConstraint_.readyForOptimize()) {
    grad += freeFormConstraint_.lagragianGradient(pt, C, index);
  }
  vnl_vector_fixed<double, 3> gradE;
  for (size_t i = 0; i < 3; i++) {
    gradE[i] = grad(i);
  }
  // stream << "gradE " << gradE << std::endl;
  //  std::cout << stream.str();
  return gradE;
}

//-----------------------------------------------------------------------------
void Constraints::InitializeLagrangianParameters(std::vector<double> mus) {
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    planeConstraints_[i].setMus(mus);
  }
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    sphereConstraints_[i].setMus(mus);
  }
  if (freeFormConstraint_.readyForOptimize()) {
    freeFormConstraint_.setMus(mus);
  }
}

//-----------------------------------------------------------------------------
void Constraints::UpdateMus(const Point3& pos, double C, size_t index) {
  Eigen::Vector3d pt;
  pt(0) = pos[0];
  pt(1) = pos[1];
  pt(2) = pos[2];
  for (size_t i = 0; i < planeConstraints_.size(); i++) {
    planeConstraints_[i].updateMu(pt, C, index);
  }
  for (size_t i = 0; i < sphereConstraints_.size(); i++) {
    sphereConstraints_[i].updateMu(pt, C, index);
  }

  if (freeFormConstraint_.readyForOptimize()) {
    freeFormConstraint_.updateMu(pt, C, index);
  }
}

//-----------------------------------------------------------------------------
void Constraints::addFreeFormConstraint(std::shared_ptr<shapeworks::Mesh> mesh) {
  freeFormConstraint_.setMesh(mesh);
  active_ = true;
}

// Cutting plane constraints using truncated gradients. Replaced with dominant constraint above.
/*
bool Constraints::applyPlaneConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos){

    // Convert points and grads to eigen vectors
    Eigen::Vector3d l0; l0(0) = pos[0]; l0(1) = pos[1]; l0(2) = pos[2];
    Eigen::Vector3d l; l(0) = -gradE[0]; l(1) = -gradE[1]; l(2) = -gradE[2];

    // Apply each cutting plane constraint
    for(size_t i = 0; i < planeConsts->size(); i++){

        // If constraint is violated, update gradient
        if(planeConsts[i].isViolated(l0+l)){

            // Get points
            Eigen::Vector3d n = planeConsts[i].GetPlaneNormal();
            Eigen::Vector3d p0 = planeConsts[i].GetPlanePoint();

            // Find intersection between cutting plane (p0, n) and update line (l0, l)
            // Eq of plane:             (p - p0).n = 0
            // Eq of line segment:      p = l0 + ld
            // Solving for d, we have d = (p0 - l0).n / l.n
            double d = (p0-l0).dot(n)/l.dot(n);
            Eigen::Vector3d p = l0 + l*d;

            l = p - l0;
        }
    }

    // Update gradient
    gradE[0] = -l(0); gradE[1] = -l(1); gradE[2] = -l(2);

    // CHECK: Is returning false correct? This is what the previous function returns
    return false;
}
*/

//-----------------------------------------------------------------------------
void Constraints::read(std::string filename) {
  std::ifstream in(filename);
  if (!in.good()) {
    throw std::runtime_error("Unable to open " + filename + " for reading");
  }
  try {
    json j;
    in >> j;
    planeConstraints_.clear();
    if (j.contains("planes")) {
      for (const auto& planeJson : j["planes"]) {
        PlaneConstraint plane;
        if (planeJson.contains("points")) {
          for (auto p : planeJson["points"]) {
            plane.points().push_back({p[0].get<double>(), p[1].get<double>(), p[2].get<double>()});
          }
          if (plane.points().size() != 3) {
            throw std::runtime_error("Planes should have three points");
          }
        }
        plane.updatePlaneFromPoints();
        planeConstraints_.push_back(plane);
      }
    }
    if (j.contains("free_form_constraints")) {
      auto ffcJson = j["free_form_constraints"];
      if (ffcJson.contains("field") && !ffcJson["field"].is_null()) {  // new constraints (field storage)
        SW_DEBUG("Loading free-form constraint from field");
        freeFormConstraint_.setInoutPolyData(get_polydata_from_json_field(ffcJson["field"]));
      } else {  // old constraints (boundary/query point)
        SW_DEBUG("Loading free-form constraint from boundary/query point");
        for (const auto& boundaryJson : ffcJson["boundaries"]) {
          std::vector<Eigen::Vector3d> boundary;
          for (auto p : boundaryJson["points"]) {
            boundary.push_back({p[0].get<double>(), p[1].get<double>(), p[2].get<double>()});
          }
          freeFormConstraint_.boundaries().push_back(boundary);
        }
        auto p = ffcJson["query_point"];
        freeFormConstraint_.setQueryPoint({p[0].get<double>(), p[1].get<double>(), p[2].get<double>()});
      }
    }
  } catch (json::exception& e) {
    throw std::runtime_error("Unabled to parse constraint file " + filename + " : " + e.what());
  }
}

//-----------------------------------------------------------------------------
void Constraints::write(std::string filename) {
  json planes;
  std::vector<json> planeJsons;
  for (auto& plane : planeConstraints_) {
    json planeJson;
    std::vector<json> points;
    for (auto& point : plane.points()) {
      points.push_back({point[0], point[1], point[2]});
    }
    planeJson["points"] = points;
    planeJsons.push_back(planeJson);
  }

  json j;
  if (!planeJsons.empty()) {
    j["planes"] = planeJsons;
  }

  freeFormConstraint_.computeBoundaries();
  freeFormConstraint_.createInoutPolyData();

  if (freeFormConstraint_.isSet()) {
    json ffcJson;
    ffcJson["field"] = get_json_field(freeFormConstraint_.getInoutPolyData());
    if (!freeFormConstraint_.boundaries().empty()) {
      std::vector<json> boundariesJson;
      for (const auto& boundary : freeFormConstraint_.boundaries()) {
        json boundaryJson;
        std::vector<json> points;
        for (const auto& point : boundary) {
          points.push_back({point[0], point[1], point[2]});
        }
        boundaryJson["points"] = points;
        boundariesJson.push_back(boundaryJson);
      }

      ffcJson["boundaries"] = boundariesJson;
      auto query = freeFormConstraint_.getQueryPoint();
      ffcJson["query_point"] = {query[0], query[1], query[2]};
    }
    j["free_form_constraints"] = ffcJson;
  }

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open \"" + filename + "\" for writing");
  }
  file << j.dump(4);
}

//-----------------------------------------------------------------------------
FreeFormConstraint& Constraints::getFreeformConstraint() { return freeFormConstraint_; }

//-----------------------------------------------------------------------------
bool Constraints::hasConstraints() {
  if (!planeConstraints_.empty() || !sphereConstraints_.empty()) {
    return true;
  }

  if (freeFormConstraint_.isSet()) {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
void Constraints::clipMesh(Mesh& mesh) {
  for (auto& plane : getPlaneConstraints()) {
    mesh.clip(plane.getVTKPlane());
  }

  if (getFreeformConstraint().isSet()) {
    auto& ffc = getFreeformConstraint();
    ffc.applyToPolyData(mesh.getVTKMesh());
    mesh = Mesh(mesh.clipByField("ffc_paint", 1.0));
  }
}

}  // namespace shapeworks
