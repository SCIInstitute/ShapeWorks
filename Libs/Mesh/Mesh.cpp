#include "Mesh.h"

// std
#include <math.h>
#include <tbb/parallel_for.h>

#include <algorithm>
#include <string>
#include <vector>

// libigl
#include <igl/cotmatrix.h>
#include <igl/exact_geodesic.h>
#include <igl/gaussian_curvature.h>
#include <igl/invert_diag.h>
#include <igl/massmatrix.h>
#include <igl/principal_curvature.h>

// vtk
#include <vtkAppendPolyData.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkCenterOfMass.h>
#include <vtkCleanPolyData.h>
#include <vtkClipClosedSurface.h>
#include <vtkClipPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkFeatureEdges.h>
#include <vtkFillHolesFilter.h>
#include <vtkGenericCell.h>
#include <vtkGradientFilter.h>
#include <vtkImageData.h>
#include <vtkImageStencil.h>
#include <vtkIncrementalPointLocator.h>
#include <vtkKdTreePointLocator.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkOBJWriter.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkPlaneCollection.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkPolyDataWriter.h>
#include <vtkProbeFilter.h>
#include <vtkReverseSense.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkStaticCellLocator.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

#include "FEFixMesh.h"
#include "Image.h"
#include "Libs/Optimize/Domain/Surface.h"
#include "Logging.h"
#include "MeshComputeThickness.h"
#include "MeshUtils.h"
#include "PreviewMeshQC/FEAreaCoverage.h"
#include "PreviewMeshQC/FEVTKExport.h"
#include "PreviewMeshQC/FEVTKImport.h"
#include "StringUtils.h"

// ACVD
#include <vtkIsotropicDiscreteRemeshing.h>

namespace shapeworks {

vtkSmartPointer<vtkPolyData> MeshReader::read(const std::string& pathname) {
  if (pathname.empty()) {
    throw std::invalid_argument("Empty pathname");
  }
  if (!ShapeWorksUtils::file_exists(pathname)) {
    throw std::invalid_argument(pathname + " does not exist");
  }

  try {
    if (StringUtils::hasSuffix(pathname, ".vtk")) {
      auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->SetReadAllScalars(1);
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, ".vtp")) {
      auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, ".stl")) {
      auto reader = vtkSmartPointer<vtkSTLReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, ".obj")) {
      auto reader = vtkSmartPointer<vtkOBJReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, ".ply")) {
      auto reader = vtkSmartPointer<vtkPLYReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    throw std::invalid_argument("Unsupported file type");
  } catch (const std::exception& exp) {
    throw std::invalid_argument("Failed to read: " + pathname);
  }
}

Mesh::Mesh(const Eigen::MatrixXd& points, const Eigen::MatrixXi& faces) {
  this->poly_data_ = vtkSmartPointer<vtkPolyData>::New();

  vtkNew<vtkPoints> vertices;
  vtkNew<vtkCellArray> polys;

  const int num_points = points.rows();

  double p[3];
  for (vtkIdType i = 0; i < num_points; ++i) {
    p[0] = points(i, 0);
    p[1] = points(i, 1);
    p[2] = points(i, 2);

    vertices->InsertNextPoint(p);
  }

  const int num_cells = faces.rows();

  vtkIdType pts[3];
  for (vtkIdType i = 0; i < num_cells; ++i) {
    pts[0] = faces(i, 0);
    pts[1] = faces(i, 1);
    pts[2] = faces(i, 2);
    polys->InsertNextCell(3, pts);
  }

  this->poly_data_->SetPoints(vertices);
  this->poly_data_->SetPolys(polys);
  this->computeNormals();
}

Mesh::Mesh(const std::string& pathname) : poly_data_(MeshReader::read(pathname)) { invalidateLocators(); }

Mesh& Mesh::write(const std::string& pathname, bool binaryFile) {
  if (!this->poly_data_) {
    throw std::invalid_argument("Mesh invalid");
  }
  if (pathname.empty()) {
    throw std::invalid_argument("Empty pathname");
  }

  try {
    if (StringUtils::hasSuffix(pathname, ".vtk")) {
      auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetFileVersion(42);
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->poly_data_);
      writer->WriteArrayMetaDataOff();  // needed for older readers to read these files
      if (binaryFile)
        writer->SetFileTypeToBinary();
      else
        writer->SetFileTypeToASCII();
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".vtp")) {
      auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->poly_data_);
      if (binaryFile)
        writer->SetDataModeToBinary();
      else
        writer->SetDataModeToAscii();
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".stl")) {
      if (getFieldNames().size() > 0)
        std::cerr << "WARNING: Trying to save mesh with new field. Only vtk and vtp files save associated fields\n";

      auto writer = vtkSmartPointer<vtkSTLWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->poly_data_);
      if (binaryFile)
        writer->SetFileTypeToBinary();
      else
        writer->SetFileTypeToASCII();
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".obj")) {
      if (getFieldNames().size() > 0)
        std::cerr << "WARNING: Trying to save mesh with new field. Only vtk and vtp files save associated fields\n";

      auto writer = vtkSmartPointer<vtkOBJWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->poly_data_);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".ply")) {
      if (getFieldNames().size() > 0)
        std::cerr << "WARNING: Trying to save mesh with new field. Only vtk and vtp files save associated fields\n";

      auto writer = vtkSmartPointer<vtkPLYWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->poly_data_);
      if (binaryFile)
        writer->SetFileTypeToBinary();
      else
        writer->SetFileTypeToASCII();
      writer->Update();
      return *this;
    }

    throw std::invalid_argument("Unsupported file type");
  } catch (const std::exception& exp) {
    throw std::invalid_argument("Failed to write mesh to " + pathname);
  }
}

Mesh& Mesh::coverage(const Mesh& otherMesh, bool allowBackIntersections, double angleThreshold,
                     double backSearchRadius) {
  FEVTKimport import;
  std::shared_ptr<FEMesh> surf1{import.Load(this->poly_data_)};
  std::shared_ptr<FEMesh> surf2{import.Load(otherMesh.poly_data_)};
  if (surf1 == nullptr || surf2 == nullptr) {
    throw std::invalid_argument("Mesh invalid");
  }

  FEAreaCoverage areaCoverage;
  areaCoverage.AllowBackIntersection(allowBackIntersections);
  areaCoverage.SetAngleThreshold(angleThreshold);
  areaCoverage.SetBackSearchRadius(backSearchRadius);

  vector<double> map1 = areaCoverage.Apply(surf1, surf2);

  for (int i = 0; i < surf1->Nodes(); ++i) {
    surf1->Node(i).m_ndata = map1[i];
  }

  FEVTKExport vtkOut;
  VTKEXPORT ops = {false, true};
  vtkOut.SetOptions(ops);

  this->poly_data_ = vtkOut.ExportToVTK(*surf1);
  this->invalidateLocators();

  return *this;
}

Mesh& Mesh::smooth(int iterations, double relaxation) {
  auto smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();

  smoother->SetInputData(this->poly_data_);
  smoother->SetNumberOfIterations(iterations);
  if (relaxation) {
    smoother->SetRelaxationFactor(relaxation);
    smoother->FeatureEdgeSmoothingOff();
    smoother->BoundarySmoothingOn();
  }
  smoother->Update();
  this->poly_data_ = smoother->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::smoothSinc(int iterations, double passband) {
  auto smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  smoother->SetInputData(this->poly_data_);
  // minimum of 2.  See docs of vtkWindowedSincPolyDataFilter for explanation
  iterations = std::max<int>(iterations, 2);
  smoother->SetNumberOfIterations(iterations);
  smoother->SetPassBand(passband);
  smoother->Update();
  this->poly_data_ = smoother->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::remesh(int numVertices, double adaptivity) {
  // ACVD is very noisy to std::cout, even with console output set to zero
  // setting the failbit on std::cout will silence this until it's cleared below
  //  std::cout.setstate(std::ios_base::failbit);
  auto surf = vtkSmartPointer<vtkSurface>::New();
  auto remesh = vtkSmartPointer<vtkQIsotropicDiscreteRemeshing>::New();
  surf->CreateFromPolyData(this->poly_data_);
  surf->GetCellData()->Initialize();
  surf->GetPointData()->Initialize();
  // surf->DisplayMeshProperties();
  int subsamplingThreshold = 10;  // subsampling threshold

  numVertices = std::max<int>(numVertices, 1);

  remesh->SetForceManifold(true);
  remesh->SetInput(surf);
  remesh->SetFileLoadSaveOption(0);
  remesh->SetConsoleOutput(0);
  remesh->SetSubsamplingThreshold(subsamplingThreshold);
  remesh->GetMetric()->SetGradation(adaptivity);
  remesh->SetDisplay(false);
  remesh->SetUnconstrainedInitialization(1);
  remesh->SetNumberOfClusters(numVertices);
  remesh->Remesh();
  // Restore std::cout
  //  std::cout.clear();

  this->poly_data_ = remesh->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::remeshPercent(double percentage, double adaptivity) {
  int numVertices = poly_data_->GetNumberOfPoints() * percentage;
  return remesh(numVertices, adaptivity);
}

Mesh& Mesh::invertNormals() {
  auto reverseSense = vtkSmartPointer<vtkReverseSense>::New();

  reverseSense->SetInputData(this->poly_data_);
  reverseSense->ReverseNormalsOff();
  reverseSense->ReverseCellsOn();
  reverseSense->Update();
  this->poly_data_ = reverseSense->GetOutput();

  return *this;
}

Mesh& Mesh::reflect(const Axis& axis, const Vector3& origin) {
  Vector scale(makeVector({1, 1, 1}));
  scale[axis] = -1;

  MeshTransform transform = MeshTransform::New();
  transform->Translate(-origin[0], -origin[1], -origin[2]);
  transform->Scale(scale[0], scale[1], scale[2]);
  transform->Translate(origin[0], origin[1], origin[2]);

  this->invalidateLocators();
  return invertNormals().applyTransform(transform);
}

MeshTransform Mesh::createTransform(const Mesh& target, Mesh::AlignmentType align, unsigned iterations) {
  return createRegistrationTransform(target, align, iterations);
}

Mesh& Mesh::applyTransform(const MeshTransform transform) {
  auto resampler = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  resampler->SetTransform(transform);
  resampler->SetInputData(this->poly_data_);
  resampler->Update();
  this->poly_data_ = resampler->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::rotate(const double angle, const Axis axis) {
  Vector rotation(makeVector({0, 0, 0}));
  rotation[axis] = 1;
  auto com = center();

  MeshTransform transform = MeshTransform::New();
  transform->Translate(com[0], com[1], com[2]);
  transform->RotateWXYZ(angle, rotation[0], rotation[1], rotation[2]);
  transform->Translate(-com[0], -com[1], -com[2]);
  return applyTransform(transform);
}

Mesh& Mesh::fillHoles(double hole_size) {
  auto filter = vtkSmartPointer<vtkFillHolesFilter>::New();
  filter->SetInputData(this->poly_data_);
  filter->SetHoleSize(hole_size);
  filter->Update();
  this->poly_data_ = filter->GetOutput();

  auto origNormal = poly_data_->GetPointData()->GetNormals();

  // Make the triangle window order consistent
  computeNormals();

  // Restore the original normals
  poly_data_->GetPointData()->SetNormals(origNormal);

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::clean() {
  auto clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->ConvertPolysToLinesOff();
  clean->ConvertLinesToPointsOff();
  clean->ConvertStripsToPolysOff();
  clean->PointMergingOn();
  clean->SetInputData(poly_data_);
  clean->Update();
  poly_data_ = clean->GetOutput();
  invalidateLocators();
  return *this;
}

Mesh& Mesh::probeVolume(const Image& image) {
  auto probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
  probeFilter->SetInputData(this->poly_data_);
  probeFilter->SetSourceData(image.getVTKImage());
  probeFilter->Update();
  this->poly_data_ = probeFilter->GetPolyDataOutput();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::clip(const Plane plane) {
  auto clipper = vtkSmartPointer<vtkClipPolyData>::New();
  clipper->SetInputData(this->poly_data_);
  clipper->SetClipFunction(plane);
  clipper->Update();
  this->poly_data_ = clipper->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::translate(const Vector3& v) {
  MeshTransform transform = MeshTransform::New();
  transform->Translate(v[0], v[1], v[2]);

  return applyTransform(transform);
}

Mesh& Mesh::scale(const Vector3& v) {
  MeshTransform transform = MeshTransform::New();
  transform->Scale(v[0], v[1], v[2]);

  return applyTransform(transform);
}

PhysicalRegion Mesh::boundingBox() const {
  PhysicalRegion bbox;
  double bb[6];
  poly_data_->GetBounds(bb);

  for (int i = 0; i < 3; i++) {
    bbox.min[i] = bb[2 * i];
    bbox.max[i] = bb[2 * i + 1];
  }

  return bbox;
}

Mesh& Mesh::fixElement() {
  FEVTKimport import;
  FEMesh* meshFE = import.Load(this->poly_data_);

  if (meshFE == nullptr) {
    throw std::runtime_error("Unable to read mesh file");
  }

  FEFixMesh fix;
  FEMesh* meshFix;

  meshFix = fix.FixElementWinding(meshFE);

  FEVTKExport vtkOut;
  this->poly_data_ = vtkOut.ExportToVTK(*meshFix);

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::fixNonManifold() {
  int count = 0;

  while (count < 10) {
    auto features = vtkSmartPointer<vtkFeatureEdges>::New();
    features->SetInputData(this->poly_data_);
    features->BoundaryEdgesOff();
    features->FeatureEdgesOff();
    features->NonManifoldEdgesOn();
    features->ManifoldEdgesOff();
    features->Update();

    vtkSmartPointer<vtkPolyData> nonmanifold = features->GetOutput();
    if (nonmanifold->GetNumberOfPoints() == 0 && nonmanifold->GetNumberOfCells() == 0) {
      return *this;
    }

    SW_DEBUG("Number of non-manifold points: {}", nonmanifold->GetNumberOfPoints());
    SW_DEBUG("Number of non-manifold cells: {}", nonmanifold->GetNumberOfCells());
    SW_DEBUG("Attempting to fix non-manifold mesh");

    std::vector<int> remove;
    for (int j = 0; j < poly_data_->GetNumberOfPoints(); j++) {
      double p2[3];
      poly_data_->GetPoint(j, p2);
      for (int i = 0; i < nonmanifold->GetNumberOfPoints(); i++) {
        double p[3];
        nonmanifold->GetPoint(i, p);
        if (p[0] == p2[0] && p[1] == p2[1] && p[2] == p2[2]) {
          remove.push_back(j);
        }
      }
    }
    SW_DEBUG("Removing {} non-manifold vertices", remove.size());

    auto new_poly_data = vtkSmartPointer<vtkPolyData>::New();
    auto vtk_pts = vtkSmartPointer<vtkPoints>::New();
    auto vtk_triangles = vtkSmartPointer<vtkCellArray>::New();
    double max_area = 0;
    for (int i = 0; i < poly_data_->GetNumberOfCells(); i++) {
      vtkSmartPointer<vtkIdList> list = vtkIdList::New();
      poly_data_->GetCellPoints(i, list);
      bool match = false;
      for (int j = 0; j < list->GetNumberOfIds(); j++) {
        int id = list->GetId(j);
        for (unsigned int k = 0; k < remove.size(); k++) {
          if (id == remove[k]) {
            match = true;
          }
        }
      }
      if (match) {
        // update max_area
        double p0[3];
        double p1[3];
        double p2[3];
        poly_data_->GetPoint(list->GetId(0), p0);
        poly_data_->GetPoint(list->GetId(1), p1);
        poly_data_->GetPoint(list->GetId(2), p2);
        double area = vtkTriangle::TriangleArea(p0, p1, p2);
        if (area > max_area) {
          max_area = area;
        }

        poly_data_->DeleteCell(i);
      }
    }

    poly_data_->RemoveDeletedCells();

    fillHoles(max_area * 2.0);

    vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangle_filter->SetInputData(poly_data_);
    triangle_filter->Update();
    poly_data_ = triangle_filter->GetOutput();

    auto connectivityFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    connectivityFilter->SetExtractionModeToLargestRegion();
    connectivityFilter->SetInputData(poly_data_);
    connectivityFilter->Update();
    poly_data_ = connectivityFilter->GetOutput();

    // SW_DEBUG("done with fixing: ");
    // detectNonManifold();

    // SW_DEBUG("At end of fix non-manifold: ");
    // detectTriangular();
    this->invalidateLocators();
  }

  return *this;
}

bool Mesh::detectNonManifold() {
  auto features = vtkSmartPointer<vtkFeatureEdges>::New();
  features->SetInputData(this->poly_data_);
  features->BoundaryEdgesOff();
  features->FeatureEdgesOff();
  features->NonManifoldEdgesOn();
  features->ManifoldEdgesOff();
  features->Update();

  vtkSmartPointer<vtkPolyData> nonmanifold = features->GetOutput();
  // SW_DEBUG("Detected Number of non-manifold points: {}", nonmanifold->GetNumberOfPoints());
  // SW_DEBUG("Detected Number of non-manifold cells: {}", nonmanifold->GetNumberOfCells());
  if (nonmanifold->GetNumberOfPoints() != 0 || nonmanifold->GetNumberOfCells() != 0) {
    return true;
  }
  return false;
}

bool Mesh::detectTriangular() {
  for (vtkIdType i = 0; i < poly_data_->GetNumberOfCells(); i++) {
    vtkCell* cell = poly_data_->GetCell(i);
    if (cell->GetNumberOfPoints() != 3) {
      SW_WARN("Warning: non-triangular cell found (id = {}, n = {})", i, cell->GetNumberOfPoints());
      return false;
    }
  }
  return true;
}

std::vector<Field> Mesh::distance(const Mesh& target, const DistanceMethod method) const {
  if (target.numPoints() == 0 || numPoints() == 0) {
    throw std::invalid_argument("meshes must have points");
  }

  // allocate Arrays to store distances and ids from each point to target
  auto distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("distance");
  auto ids = vtkSmartPointer<vtkDoubleArray>::New();
  ids->SetNumberOfComponents(1);
  ids->SetNumberOfTuples(numPoints());
  ids->SetName("ids");

  // Find the nearest neighbors to each point and compute distance between them
  Point3 currentPoint, closestPoint;

  switch (method) {
    case PointToPoint: {
      // build point locator for target mesh
      auto targetPointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
      targetPointLocator->SetDataSet(target.poly_data_);
      targetPointLocator->BuildLocator();

      for (int i = 0; i < numPoints(); i++) {
        poly_data_->GetPoint(i, currentPoint.GetDataPointer());
        vtkIdType closestPointId = targetPointLocator->FindClosestPoint(currentPoint.GetDataPointer());
        target.poly_data_->GetPoint(closestPointId, closestPoint.GetDataPointer());
        ids->SetValue(i, closestPointId);
        distance->SetValue(i, length(currentPoint - closestPoint));
      }
    } break;

    case PointToCell: {
      // build cell locator for target mesh
      auto targetCellLocator = vtkSmartPointer<vtkStaticCellLocator>::New();
      targetCellLocator->SetDataSet(target.poly_data_);
      targetCellLocator->BuildLocator();

      double dist2;
      auto cell = vtkSmartPointer<vtkGenericCell>::New();
      vtkIdType cellId;
      int subId;

      for (int i = 0; i < numPoints(); i++) {
        poly_data_->GetPoint(i, currentPoint.GetDataPointer());
        targetCellLocator->FindClosestPoint(currentPoint.GetDataPointer(), closestPoint.GetDataPointer(), cell, cellId,
                                            subId, dist2);
        ids->SetValue(i, cellId);
        distance->SetValue(i, std::sqrt(dist2));
      }
    } break;

    default:
      throw std::invalid_argument("invalid distance method");
  }

  return std::vector<Field>{distance, ids};
}

// TODO^2: do the same thing for the other couple functions below that set a field and return the other field (they
// shouldn't be setting a field) (search for setField and you'll see the 2-3 places it needs to be done)

Mesh& Mesh::clipClosedSurface(const Plane plane) {
  auto planeCollection = vtkSmartPointer<vtkPlaneCollection>::New();
  planeCollection->AddItem(plane);

  auto clipper = vtkSmartPointer<vtkClipClosedSurface>::New();
  clipper->SetClippingPlanes(planeCollection);
  clipper->SetInputData(this->poly_data_);
  clipper->SetGenerateFaces(1);
  clipper->Update();
  this->poly_data_ = clipper->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::computeNormals() {
  auto normal = vtkSmartPointer<vtkPolyDataNormals>::New();

  normal->SetInputData(this->poly_data_);
  normal->ComputeCellNormalsOn();
  normal->ComputePointNormalsOn();
  normal->AutoOrientNormalsOn();
  normal->SplittingOff();
  normal->Update();
  this->poly_data_ = normal->GetOutput();

  return *this;
}

void Mesh::invalidateLocators() const {
  this->cellLocator = nullptr;
  this->pointLocator = nullptr;
}

void Mesh::updatePointLocator() const {
  if (!this->pointLocator) {
    this->pointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    this->pointLocator->SetDataSet(this->poly_data_);
    this->pointLocator->BuildLocator();
  }
}

void Mesh::updateCellLocator() const {
  if (!this->cellLocator) {
    this->cellLocator = vtkSmartPointer<vtkStaticCellLocator>::New();
    this->cellLocator->SetDataSet(this->poly_data_);
    this->cellLocator->BuildLocator();
  }
}

Point3 Mesh::closestPoint(const Point3 point, double& distance, vtkIdType& face_id) const {
  this->updateCellLocator();

  double dist2;
  Point3 closestPoint;
  auto cell = vtkSmartPointer<vtkGenericCell>::New();
  int subId;

  cellLocator->FindClosestPoint(point.GetDataPointer(), closestPoint.GetDataPointer(), cell, face_id, subId, dist2);

  // distance from point to closest point
  distance = sqrt(dist2);

  return closestPoint;
}

int Mesh::closestPointId(const Point3 point) const {
  this->updatePointLocator();

  vtkIdType closestPointId = pointLocator->FindClosestPoint(point.GetDataPointer());
  return closestPointId;
}

bool Mesh::isPointInside(const Point3 point) const {
  // create point set
  auto points = vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint(point.GetDataPointer());
  auto polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  auto select = vtkSmartPointer<vtkSelectEnclosedPoints>::New();

  select->SetInputData(polydata);
  select->SetSurfaceData(this->poly_data_);
  select->SetTolerance(0.0001);
  select->Update();

  return select->IsInside(0);
}

double Mesh::geodesicDistance(int source, int target) const {
  if (source < 0 || target < 0 || numPoints() < source || numPoints() < target) {
    throw std::invalid_argument("requested point ids outside range of points available in mesh");
  }

  Surface surface(this->poly_data_, true);
  return surface.compute_distance(getPoint(source), -1, getPoint(target), -1);
}

Field Mesh::geodesicDistance(const Point3 landmark) const {
  auto distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("GeodesicDistanceToLandmark");

  Surface surface(this->poly_data_, true);

  for (int i = 0; i < numPoints(); i++) {
    distance->SetValue(i, surface.compute_distance(landmark, -1, getPoint(i), -1));
  }

  return distance;
}

Field Mesh::geodesicDistance(const std::vector<Point3> curve) const {
  auto minDistance = vtkSmartPointer<vtkDoubleArray>::New();
  minDistance->SetNumberOfComponents(1);
  minDistance->SetNumberOfTuples(numPoints());
  minDistance->SetName("GeodesicDistanceToCurve");
  minDistance->Fill(1e20);

  for (int i = 0; i < curve.size(); i++) {
    Field distance = geodesicDistance(curve[i]);
    for (int j = 0; j < numPoints(); j++) {
      if (distance->GetTuple1(j) < minDistance->GetTuple1(j)) {
        minDistance->SetValue(j, distance->GetTuple1(j));
      }
    }
  }

  return minDistance;
}

Field Mesh::curvature(const CurvatureType type) const {
  Eigen::MatrixXd V = points();
  Eigen::MatrixXi F = faces();

  Eigen::MatrixXd PD1, PD2;
  Eigen::VectorXd PV1, PV2;

  Eigen::VectorXd C;

  auto curv = vtkSmartPointer<vtkDoubleArray>::New();
  curv->SetNumberOfComponents(1);
  curv->SetNumberOfTuples(numPoints());

  switch (type) {
    case Principal: {
      curv->SetName("principal curvature");

      // returns maximal curvature value for each vertex
      // igl::principal_curvature(V, F, PD1, PD2, PV1, PV2);

      // returns minimal curvature value for each vertex
      igl::principal_curvature(V, F, PD1, PD2, C, PV2);
      break;
    }
    case Gaussian: {
      curv->SetName("gaussian curvature");
      igl::gaussian_curvature(V, F, C);
      break;
    }
    case Mean: {
      curv->SetName("mean curvature");

      Eigen::MatrixXd HN;
      Eigen::SparseMatrix<double> L, M, Minv;
      igl::cotmatrix(V, F, L);
      igl::massmatrix(V, F, igl::MASSMATRIX_TYPE_VORONOI, M);
      igl::invert_diag(M, Minv);

      // Laplace-Beltrami of position
      HN = -Minv * (L * V);

      // Extract magnitude as mean curvature
      C = HN.rowwise().norm();

      // Compute curvature directions via quadric fitting
      igl::principal_curvature(V, F, PD1, PD2, PV1, PV2);

      // mean curvature
      C = 0.5 * (PV1 + PV2);
      break;
    }
    default:
      throw std::invalid_argument("Unknown Mesh::CurvatureType.");
  }

  for (int i = 0; i < numPoints(); i++) curv->SetValue(i, C[i]);

  return curv;
}

void computeGradient(vtkDataSet* inputDataSet, const char* scalarFieldName, const char* gradientFieldName) {
  vtkSmartPointer<vtkGradientFilter> gradientFilter = vtkSmartPointer<vtkGradientFilter>::New();
  gradientFilter->SetInputData(inputDataSet);
  gradientFilter->SetInputScalars(vtkDataSet::FIELD_ASSOCIATION_POINTS, scalarFieldName);
  gradientFilter->SetResultArrayName(gradientFieldName);
  gradientFilter->Update();
  /*

  vtkSmartPointer<vtkDoubleArray> gradientData = vtkSmartPointer<vtkDoubleArray>::New();
  gradientData->SetNumberOfComponents(3);
  gradientData->SetNumberOfTuples(inputDataSet->GetNumberOfPoints());
  gradientData->SetName(gradientFieldName);
*/
  vtkDoubleArray* gradPointArray = vtkArrayDownCast<vtkDoubleArray>(
      vtkDataSet::SafeDownCast(gradientFilter->GetOutput())->GetPointData()->GetArray(gradientFieldName));

  /*
    for (vtkIdType pointId = 0; pointId < inputDataSet->GetNumberOfPoints(); ++pointId) {
      double gradient[3];
      gradientFilter->GetOutput()->GetPointData()->GetTuple(pointId, gradient);
      gradientData->SetTuple(pointId, gradient);
    }
  */

  //  inputDataSet->GetPointData()->AddArray(gradientData);
  inputDataSet->GetPointData()->AddArray(gradPointArray);
}

void Mesh::computeFieldGradient(const std::string& field) const {
  computeGradient(poly_data_, field.c_str(), (std::string("gradient_") + field).c_str());
  return;
  auto arr = poly_data_->GetPointData()->GetArray(field.c_str());

  // for each vertex, compute the gradient of the field and store x,y,z
  // components in a vector
  auto gradient = vtkSmartPointer<vtkDoubleArray>::New();
  gradient->SetNumberOfComponents(3);
  gradient->SetNumberOfTuples(numPoints());
  gradient->SetName((std::string("gradient_") + field).c_str());

  for (int i = 0; i < numPoints(); i++) {
    // get the field value at this vertex
    double value = arr->GetTuple1(i);

    // collect all neighboring vertices using vtk
    auto cell = vtkSmartPointer<vtkGenericCell>::New();
    auto neighbors = vtkSmartPointer<vtkIdList>::New();
    poly_data_->GetPointCells(i, neighbors);
    int num_neighbors = neighbors->GetNumberOfIds();

    // for each neighbor vertex
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    for (int j = 0; j < num_neighbors; j++) {
      // get the neighbor vertex id
      int neighbor_id = neighbors->GetId(j);

      // get the neighbor vertex
      auto neighbor = poly_data_->GetPoint(neighbor_id);

      // get the neighbor vertex value
      double neighbor_value = arr->GetTuple1(neighbor_id);

      // compute the gradient
      x += (neighbor[0] - getPoint(i)[0]) * (neighbor_value - value);
      y += (neighbor[1] - getPoint(i)[1]) * (neighbor_value - value);
      z += (neighbor[2] - getPoint(i)[2]) * (neighbor_value - value);
    }

    gradient->SetTuple3(i, x, y, z);
  }

  poly_data_->GetPointData()->AddArray(gradient);
}

Eigen::Vector3d Mesh::computeFieldGradientAtPoint(const std::string& field, const Point3& query) const {
  this->updateCellLocator();

  // compute gradient if not already computed
  if (poly_data_->GetPointData()->GetArray((std::string("gradient_") + field).c_str()) == nullptr) {
    computeFieldGradient(field);
  }

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  cellLocator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

  auto cell = poly_data_->GetCell(cellId);

  size_t v1 = cell->GetPointId(0);
  size_t v2 = cell->GetPointId(1);
  size_t v3 = cell->GetPointId(2);

  auto gradient = poly_data_->GetPointData()->GetArray((std::string("gradient_") + field).c_str());

  Eigen::Vector3d grad1(gradient->GetTuple3(v1)[0], gradient->GetTuple3(v1)[1], gradient->GetTuple3(v1)[2]);
  Eigen::Vector3d grad2(gradient->GetTuple3(v2)[0], gradient->GetTuple3(v2)[1], gradient->GetTuple3(v2)[2]);
  Eigen::Vector3d grad3(gradient->GetTuple3(v3)[0], gradient->GetTuple3(v3)[1], gradient->GetTuple3(v3)[2]);

  // Compute barycentric distances
  Eigen::Vector3d cp(closestPoint[0], closestPoint[1], closestPoint[2]);
  Eigen::Vector3d bary = computeBarycentricCoordinates(cp, cellId);

  bary = bary / bary.sum();

  Eigen::Vector3d result;
  result = bary[0] * grad1 + bary[1] * grad2 + bary[2] * grad3;
  return result;
}

double Mesh::interpolateFieldAtPoint(const std::string& field, const Point3& query) const {
  this->updateCellLocator();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  this->cellLocator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

  auto cell = this->poly_data_->GetCell(cellId);

  size_t v1 = cell->GetPointId(0);
  size_t v2 = cell->GetPointId(1);
  size_t v3 = cell->GetPointId(2);

  // Compute barycentric distances
  Eigen::Vector3d cp(closestPoint[0], closestPoint[1], closestPoint[2]);
  Eigen::Vector3d bary = computeBarycentricCoordinates(cp, cellId);

  bary = bary / bary.sum();

  Eigen::Vector3d values(this->getFieldValue(field, v1), this->getFieldValue(field, v2),
                         this->getFieldValue(field, v3));

  return values[0] * bary[0] + values[1] * bary[1] + values[2] * bary[2];
}

Mesh& Mesh::applySubdivisionFilter(const SubdivisionType type, int subdivision) {
  if (type == Mesh::SubdivisionType::Loop) {
    auto filter = vtkSmartPointer<vtkLoopSubdivisionFilter>::New();
    filter->SetInputData(this->poly_data_);
    filter->SetNumberOfSubdivisions(subdivision);
    filter->Update();
    this->poly_data_ = filter->GetOutput();
  } else {
    auto filter = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
    filter->SetInputData(this->poly_data_);
    filter->SetNumberOfSubdivisions(subdivision);
    filter->Update();
    this->poly_data_ = filter->GetOutput();
  }

  return *this;
}

Image Mesh::toImage(PhysicalRegion region, Point3 spacing) const {
  // if no region, use mesh bounding box
  if (region == PhysicalRegion()) {
    region = boundingBox();
  }

  // compute output dimensions: size of the region / by the requested spacing
  auto dims = toDims(region.size() / spacing);

  // std::cout << "mesh extents are " << region << std::endl;
  // std::cout << "size is " << region.size() << std::endl;
  // std::cout << "spacing is " << spacing << std::endl;
  // std::cout << "dims are " << dims << std::endl;

  // allocate output image
  auto whiteImage = vtkSmartPointer<vtkImageData>::New();
  whiteImage->SetOrigin(region.origin()[0], region.origin()[1], region.origin()[2]);
  whiteImage->SetSpacing(spacing[0], spacing[1], spacing[2]);
  whiteImage->SetExtent(0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1);
  whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  // paint it white
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i) whiteImage->GetPointData()->GetScalars()->SetTuple1(i, 1);

  // cut stencil from mesh silhouette (same size as output image)
  auto pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  pol2stenc->SetInputData(this->poly_data_);
  pol2stenc->SetInformationInput(whiteImage);
  pol2stenc->Update();

  // spray output using stencil (use dark paint)
  auto imgstenc = vtkSmartPointer<vtkImageStencil>::New();
  imgstenc->SetInputData(whiteImage);
  imgstenc->SetStencilData(pol2stenc->GetOutput());
  imgstenc->ReverseStencilOff();
  imgstenc->SetBackgroundValue(0.0);
  imgstenc->Update();

  return Image(imgstenc->GetOutput());
}

Image Mesh::toDistanceTransform(PhysicalRegion region, const Point3 spacing, const Dims padding) const {
  invalidateLocators();
  this->updateCellLocator();

  // if no region, use mesh bounding box
  if (region == PhysicalRegion()) {
    region = boundingBox();
  }

  // compute dims
  auto dims = toDims(region.size() / spacing) + Dims({padding[0] * 2, padding[1] * 2, padding[2] * 2});

  // allocate output image, setting origin and spacing
  Image img(dims);
  img.setSpacing(toVector(spacing));
  auto origin = region.min - spacing * toPoint(padding);
  img.setOrigin(origin);

  auto itkimg = img.getITKImage();

  using IteratorType = itk::ImageRegionIterator<Image::ImageType>;
  IteratorType it(itkimg, itkimg->GetLargestPossibleRegion());

  std::vector<Image::ImageType::IndexType> indices;

  // add all points
  auto points = vtkSmartPointer<vtkPoints>::New();
  for (it.GoToBegin(); !it.IsAtEnd(); ++it) {
    Image::ImageType::PointType p;
    itkimg->TransformIndexToPhysicalPoint(it.GetIndex(), p);
    points->InsertNextPoint(p[0], p[1], p[2]);
    indices.push_back(it.GetIndex());
  }

  auto points_poly = vtkSmartPointer<vtkPolyData>::New();
  points_poly->SetPoints(points);

  auto enclosed = vtkSmartPointer<vtkSelectEnclosedPoints>::New();
  enclosed->SetInputData(points_poly);
  enclosed->SetSurfaceData(this->poly_data_);
  enclosed->Update();

  std::mutex cell_mutex;

  tbb::parallel_for(tbb::blocked_range<size_t>(0, indices.size()), [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i != r.end(); ++i) {
      Image::ImageType::PointType p;
      itkimg->TransformIndexToPhysicalPoint(indices[i], p);

      double distance = 0.0;
      vtkIdType face_id = 0;

      {
        // std::lock_guard<std::mutex> lock(cell_mutex);
        closestPoint(p, distance, face_id);
      }

      bool outside = !enclosed->IsInside(i);

      // NOTE: distance is positive inside, negative outside
      itkimg->SetPixel(indices[i], outside ? -distance : distance);
    }
  });

  return img;
}

Mesh& Mesh::computeThickness(Image& image, Image* dt, double max_dist, double median_radius,
                             std::string distance_mesh) {
  mesh::compute_thickness(*this, image, dt, max_dist, median_radius, distance_mesh);
  return *this;
}

Mesh& Mesh::computeLandmarkGeodesics(const std::vector<Point3>& landmarks) {
  for (int i = 0; i < landmarks.size(); i++) {
    auto field = geodesicDistance(landmarks[i]);
    std::string name = "geodesic_distance_to_" + std::to_string(i);
    setField(name, field, Mesh::FieldType::Point);
  }
  return *this;
}

Point3 Mesh::center() const {
  double c[3];
  poly_data_->GetCenter(c);
  return Point3({c[0], c[1], c[2]});
}

Point3 Mesh::centerOfMass() const {
  auto com = vtkSmartPointer<vtkCenterOfMass>::New();
  com->SetInputData(this->poly_data_);
  com->Update();
  double center[3];
  com->GetCenter(center);
  return center;
}

Point3 Mesh::getPoint(int id) const {
  if (this->numPoints() < id) {
    throw std::invalid_argument("mesh has fewer indices than requested");
  }

  double point[3];
  poly_data_->GetPoint(id, point);
  return Point3({point[0], point[1], point[2]});
}

IPoint3 Mesh::getFace(int id) const {
  if (poly_data_->GetNumberOfCells() < id) {
    throw std::invalid_argument("mesh has fewer indices than requested");
  }

  IPoint3 face;
  auto cells = vtkSmartPointer<vtkIdList>::New();
  poly_data_->GetCellPoints(id, cells);

  for (int i = 0; i < 3; i++) {
    face[i] = cells->GetId(i);
  }

  return face;
}

Eigen::MatrixXd Mesh::points() const {
  int num_points = numPoints();
  Eigen::MatrixXd points_(num_points, 3);

  vtkSmartPointer<vtkDataArray> data_array = poly_data_->GetPoints()->GetData();

  for (int i = 0; i < num_points; i++) {
    points_(i, 0) = data_array->GetComponent(i, 0);
    points_(i, 1) = data_array->GetComponent(i, 1);
    points_(i, 2) = data_array->GetComponent(i, 2);
  }

  return points_;
}

Eigen::MatrixXi Mesh::faces() const {
  int num_faces = numFaces();
  Eigen::MatrixXi faces(num_faces, 3);

  auto cells = vtkSmartPointer<vtkIdList>::New();

  for (int j = 0; j < num_faces; j++) {
    poly_data_->GetCellPoints(j, cells);
    faces(j, 0) = cells->GetId(0);
    faces(j, 1) = cells->GetId(1);
    faces(j, 2) = cells->GetId(2);
  }

  return faces;
}

std::vector<std::string> Mesh::getFieldNames() const {
  std::vector<std::string> fields;
  int numFields = poly_data_->GetPointData()->GetNumberOfArrays();

  for (int i = 0; i < numFields; i++) {
    auto name = poly_data_->GetPointData()->GetArrayName(i);
    fields.push_back(name ? std::string(name) : std::string("default"));
  }

  return fields;
}

Field Mesh::getField(const std::string& name, const FieldType type) const {
  if (type == Mesh::Point) {
    if (poly_data_->GetPointData()->GetNumberOfArrays() < 1) {
      throw std::invalid_argument("Mesh has no fields.");
    }

    Field rawarr = poly_data_->GetPointData()->GetArray(name.c_str());
    if (!rawarr) {
      throw std::invalid_argument("Mesh does not contain a point field called " + name);
    }

    return rawarr;
  } else if (type == Mesh::Face) {
    return getFieldForFaces(name);
  } else {
    throw std::invalid_argument("Incorrect Mesh::FieldType.");
  }
}

Field Mesh::getFieldForFaces(const std::string& name) const {
  if (poly_data_->GetCellData()->GetNumberOfArrays() < 1) {
    throw std::invalid_argument("Mesh has no face fields.");
  }

  Field rawarr = poly_data_->GetCellData()->GetArray(name.c_str());
  if (!rawarr) {
    throw std::invalid_argument("Mesh does not contain a cell field called " + name);
  }

  return rawarr;
}

Mesh& Mesh::setField(std::string name, Array array, const FieldType type) {
  if (type == Mesh::Point) {
    if (!array) {
      throw std::invalid_argument("Invalid array.");
    }

    if (name.empty()) {
      throw std::invalid_argument("Provide name for the new field");
    }

    int numVertices = numPoints();
    if (array->GetNumberOfTuples() != numVertices) {
      std::cerr << "WARNING: Added a mesh field with a different number of elements than points ("
                << array->GetNumberOfTuples() << " vs " << numVertices << ")\n";
    }

    array->SetName(name.c_str());
    poly_data_->GetPointData()->AddArray(array);

    return *this;
  } else if (type == Mesh::Face) {
    return setFieldForFaces(name, array);
  } else {
    throw std::invalid_argument("Incorrect Mesh::FieldType.");
  }
}

Mesh& Mesh::setFieldForFaces(std::string name, Array array) {
  if (!array) {
    throw std::invalid_argument("Invalid array.");
  }

  if (name.empty()) {
    throw std::invalid_argument("Provide name for the new field");
  }

  if (array->GetNumberOfTuples() != numFaces()) {
    std::cerr << "WARNING: Added a mesh field with a different number of elements than points\n";
  }
  if (array->GetNumberOfComponents() != 1) {
    // std::cerr << "WARNING: Added a multi-component mesh field\n";
  }

  array->SetName(name.c_str());
  poly_data_->GetCellData()->AddArray(array);

  return *this;
}

void Mesh::setFieldValue(const std::string& name, int idx, double val) {
  if (name.empty()) {
    throw std::invalid_argument("Provide name for the field");
  }

  if (poly_data_->GetPointData()->GetNumberOfArrays() < 1) {
    throw std::invalid_argument("Mesh has no fields for which to set a value.");
  }

  auto arr = poly_data_->GetPointData()->GetArray(name.c_str());
  if (arr->GetNumberOfTuples() > idx) {
    arr->SetTuple1(idx, val);
  } else {
    throw std::invalid_argument("Intended index in field is out of range");
  }
}

double Mesh::getFieldValue(const std::string& name, int idx) const {
  if (name.empty()) {
    throw std::invalid_argument("Provide name for field");
  }

  if (poly_data_->GetPointData()->GetNumberOfArrays() < 1) {
    throw std::invalid_argument("Mesh has no fields from which to retrieve a value.");
  }

  auto arr = poly_data_->GetPointData()->GetArray(name.c_str());
  if (!arr) {
    throw std::invalid_argument("Field does not exist.");
  }

  if (arr->GetNumberOfTuples() > idx) {
    return arr->GetTuple(idx)[0];
  } else {
    throw std::invalid_argument("Requested index in field is out of range");
  }
}

Eigen::VectorXd Mesh::getMultiFieldValue(const std::string& name, int idx) const {
  if (name.empty()) {
    throw std::invalid_argument("Provide name for field");
  }

  if (poly_data_->GetPointData()->GetNumberOfArrays() < 1) {
    throw std::invalid_argument("Mesh has no fields from which to retrieve a value.");
  }

  auto arr = poly_data_->GetPointData()->GetArray(name.c_str());
  if (!arr) {
    throw std::invalid_argument("Field does not exist.");
  }

  if (arr->GetNumberOfTuples() > idx) {
    size_t compnum = arr->GetNumberOfComponents();
    Eigen::VectorXd vec(compnum);
    for (size_t i = 0; i < compnum; i++) {
      vec(i) = arr->GetTuple(idx)[i];
    }
    return vec;
  } else {
    throw std::invalid_argument("Requested index in field is out of range");
  }
}

bool Mesh::compareAllPoints(const Mesh& other_mesh) const {
  if (!this->poly_data_ || !other_mesh.poly_data_) {
    throw std::invalid_argument("invalid meshes");
  }

  if (this->numPoints() != other_mesh.numPoints()) {
    std::cerr << "meshes differ in number of points";
    return false;
  }

  for (int i = 0; i < this->numPoints(); i++) {
    Point3 p1(this->poly_data_->GetPoint(i));
    Point3 p2(other_mesh.poly_data_->GetPoint(i));
    if (!epsEqual(p1, p2, 0.011)) {
      printf("%ith points not equal ([%0.8f, %0.8f, %0.8f], [%0.8f, %0.8f, %0.8f])\n", i, p1[0], p1[1], p1[2], p2[0],
             p2[1], p2[2]);

      return false;
    }
  }

  return true;
}

bool Mesh::compareAllFaces(const Mesh& other_mesh) const {
  if (!this->poly_data_ || !other_mesh.poly_data_) {
    throw std::invalid_argument("invalid meshes");
  }

  if (this->poly_data_->GetNumberOfCells() != other_mesh.poly_data_->GetNumberOfCells()) {
    std::cerr << "meshes differ in number of faces";
    return false;
  }

  // helper function to print out the cell indices
  auto printCells = [](vtkCell* cell1, vtkCell* cell2) {
    std::cout << "[ ";
    for (int i = 0; i < cell1->GetNumberOfPoints(); i++) {
      std::cout << cell1->GetPointId(i) << " ";
    }
    std::cout << "], [ ";
    for (int i = 0; i < cell2->GetNumberOfPoints(); i++) {
      std::cout << cell2->GetPointId(i) << " ";
    }
    std::cout << "]";
  };

  for (int i = 0; i < this->poly_data_->GetNumberOfCells(); i++) {
    vtkCell* cell1 = this->poly_data_->GetCell(i);
    vtkCell* cell2 = other_mesh.poly_data_->GetCell(i);

    if (cell1->GetNumberOfPoints() != cell2->GetNumberOfPoints()) {
      std::cout << i << "th face not equal (";
      printCells(cell1, cell2);
      std::cout << ")\n";
      return false;
    }

    std::vector<vtkIdType> cell1Points(cell1->GetNumberOfPoints());
    std::vector<vtkIdType> cell2Points(cell2->GetNumberOfPoints());

    for (int pi = 0; pi < cell1->GetNumberOfPoints(); pi++) {
      cell1Points[pi] = cell1->GetPointId(pi);
      cell2Points[pi] = cell2->GetPointId(pi);
    }

    std::sort(cell1Points.begin(), cell1Points.end());
    std::sort(cell2Points.begin(), cell2Points.end());

    if (cell1Points != cell2Points) {
      std::cout << i << "th face not equal (";
      printCells(cell1, cell2);
      std::cout << ")\n";
      return false;
    }
  }

  return true;
}

bool Mesh::compareAllFields(const Mesh& other_mesh, const double eps) const {
  if (!this->poly_data_ || !other_mesh.poly_data_) {
    throw std::invalid_argument("Invalid meshes");
  }

  auto fields1 = getFieldNames();
  auto fields2 = other_mesh.getFieldNames();

  // first make sure they even have the same fields to compare
  if (fields1.size() != fields2.size()) {
    std::cerr << "Mesh have different number of fields\n";
    return false;
  }
  for (int i = 0; i < fields1.size(); i++) {
    if (std::find(fields2.begin(), fields2.end(), fields1[i]) == fields2.end()) {
      std::cerr << "Both meshes don't have " << fields1[i] << " field\n";
      return false;
    }
  }

  // now compare the actual fields
  for (const auto& field : fields1) {
    if (!compareField(other_mesh, field, "", eps)) {
      std::cerr << field << " fields are not the same\n";
      return false;
    }
  }

  return true;
}

bool Mesh::compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2,
                        const double eps) const {
  auto field1 = getField(name1, Mesh::Point);
  auto field2 = other_mesh.getField(name2.empty() ? name1 : name2, Mesh::Point);

  if (!field1 || !field2) {
    std::cerr << "at least one mesh missing a field\n";
    return false;
  }

  if (field1->GetNumberOfTuples() != field2->GetNumberOfTuples() ||
      field1->GetNumberOfComponents() != field2->GetNumberOfComponents()) {
    std::cerr << "Fields are not the same size\n";
    return false;
  }

  for (int i = 0; i < field1->GetNumberOfTuples(); i++) {
    for (int c = 0; c < field1->GetNumberOfComponents(); c++) {
      auto v1(field1->GetTuple(i)[c]);
      auto v2(field2->GetTuple(i)[c]);
      if (eps > 0) {
        if (!(std::abs(v1 - v2) < eps)) {
          printf("%ith values not equal (%0.8f != %0.8f)\n", i, v1, v2);
          return false;
        }
      } else {
        if (!epsEqual(v1, v2, 1e-3)) {
          printf("%ith values not equal (%0.8f != %0.8f)\n", i, v1, v2);
          return false;
        }
      }
    }
  }

  return true;
}

bool Mesh::compare(const Mesh& other, const double eps) const {
  if (!epsEqual(center(), other.center(), 1e-2)) {
    std::cerr << "centers differ!\n";
    return false;
  }
  if (!epsEqual(centerOfMass(), other.centerOfMass(), 1e-3)) {
    std::cerr << "coms differ!\n";
    return false;
  }
  if (numPoints() != other.numPoints()) {
    std::cerr << "num pts differ\n";
    return false;
  }
  if (numFaces() != other.numFaces()) {
    std::cerr << "num faces differ\n";
    return false;
  }
  if (!compareAllPoints(other)) {
    std::cerr << "points differ\n";
    return false;
  }
  if (!compareAllFaces(other)) {
    std::cerr << "faces differ\n";
    return false;
  }
  if (!compareAllFields(other, eps)) {
    std::cerr << "fields differ\n";
    return false;
  }

  return true;
}

MeshTransform Mesh::createRegistrationTransform(const Mesh& target, Mesh::AlignmentType align,
                                                unsigned iterations) const {
  const vtkSmartPointer<vtkMatrix4x4> mat(
      MeshUtils::createICPTransform(this->poly_data_, target.getVTKMesh(), align, iterations, true));
  return createMeshTransform(mat);
}

std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
  os << "{\nnumber of points: " << mesh.numPoints() << ",\nnumber of faces: " << mesh.numFaces()
     << ",\ncenter: " << mesh.center() << ",\ncenter or mass: " << mesh.centerOfMass()
     << ",\nbounding box: " << mesh.boundingBox() << ",\nfield names: \n";

  auto fields = mesh.getFieldNames();
  for (auto field : fields) {
    os << "\t" << field << std::endl;
  }

  return os;
}

vtkSmartPointer<vtkPolyData> Mesh::clipByField(const std::string& name, double value) {
  auto poly_data = vtkSmartPointer<vtkPolyData>::New();

  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> polys;
  std::vector<bool> member;

  // copy points
  for (vtkIdType i = 0; i < poly_data_->GetNumberOfPoints(); i++) {
    points->InsertNextPoint(poly_data_->GetPoint(i));
    if (this->getFieldValue(name, i) == value) {
      member.push_back(true);
    } else {
      member.push_back(false);
    }
  }

  // copy triangles
  for (vtkIdType i = 0; i < poly_data_->GetNumberOfCells(); i++) {
    vtkCell* cell = this->poly_data_->GetCell(i);

    if (cell->GetCellType() != VTK_EMPTY_CELL) {  // VTK_EMPTY_CELL means it was deleted
      vtkIdType pts[3];
      pts[0] = cell->GetPointId(0);
      pts[1] = cell->GetPointId(1);
      pts[2] = cell->GetPointId(2);
      if (member[pts[0]] && member[pts[1]] && member[pts[2]]) {
        polys->InsertNextCell(3, pts);
      }
    }
  }

  poly_data->SetPoints(points);
  poly_data->SetPolys(polys);
  return poly_data;
}

int Mesh::getClosestFace(const Point3& point) const {
  this->updateCellLocator();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  this->cellLocator->FindClosestPoint(point.data(), closestPoint, cellId, subId, dist);
  return cellId;
}

Eigen::Vector3d Mesh::computeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const {
  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  Eigen::Vector3d bary;
  this->poly_data_->GetCell(face)->EvaluatePosition(pt.data(), closest, sub_id, pcoords, dist2, bary.data());
  return bary;
}

void Mesh::interpolate_scalars_to_mesh(std::string name, Eigen::VectorXd positions, Eigen::VectorXd scalar_values) {
  int num_points = positions.size() / 3;
  if (num_points == 0) {
    return;
  }

  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();

  vtk_points->SetNumberOfPoints(num_points);

  unsigned int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double x = positions[idx++];
    double y = positions[idx++];
    double z = positions[idx++];

    vtk_points->InsertPoint(i, x, y, z);
  }

  if (num_points != scalar_values.size()) {
    std::cerr << "Warning, mismatch of points and scalar values (num_points = " << num_points
              << ", scalar_values.size() = " << scalar_values.size() << ")\n";
    return;
  }

  vtkSmartPointer<vtkPolyData> point_data = vtkSmartPointer<vtkPolyData>::New();
  point_data->SetPoints(vtk_points);

  vtkSmartPointer<vtkPointLocator> point_locator = vtkPointLocator::New();
  point_locator->SetDataSet(point_data);
  point_locator->SetDivisions(100, 100, 100);
  point_locator->BuildLocator();

  if (!poly_data_ || poly_data_->GetNumberOfPoints() == 0) {
    return;
  }

  auto points = poly_data_->GetPoints();

  vtkFloatArray* scalars = vtkFloatArray::New();
  scalars->SetNumberOfValues(points->GetNumberOfPoints());
  scalars->SetName(name.c_str());

  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double pt[3];
    points->GetPoint(i, pt);

    // find the 8 closest correspondence points the to current point
    vtkSmartPointer<vtkIdList> closest_points = vtkSmartPointer<vtkIdList>::New();
    point_locator->FindClosestNPoints(8, pt, closest_points);

    // assign scalar value based on a weighted scheme
    float weighted_scalar = 0.0f;
    float distanceSum = 0.0f;
    float distance[8] = {0.0f};
    bool exactly_on_point = false;
    float exact_scalar = 0.0f;
    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      // get a particle position
      vtkIdType id = closest_points->GetId(p);

      // compute distance to current particle
      double x = pt[0] - point_data->GetPoint(id)[0];
      double y = pt[1] - point_data->GetPoint(id)[1];
      double z = pt[2] - point_data->GetPoint(id)[2];

      if (x == 0 && y == 0 && z == 0) {
        distance[p] = 0;
        exactly_on_point = true;
        exact_scalar = scalar_values[id];
      } else {
        distance[p] = 1.0f / (x * x + y * y + z * z);
      }

      // multiply scalar value by weight and add to running sum
      distanceSum += distance[p];
    }

    for (unsigned int p = 0; p < closest_points->GetNumberOfIds(); p++) {
      vtkIdType current_id = closest_points->GetId(p);
      weighted_scalar += distance[p] / distanceSum * scalar_values[current_id];
    }

    if (exactly_on_point) {
      weighted_scalar = exact_scalar;
    }

    scalars->SetValue(i, weighted_scalar);
  }

  setField(name, scalars, Mesh::FieldType::Point);
}

double Mesh::getFFCValue(Eigen::Vector3d query) const {
  Point3 point(query.data());
  return interpolateFieldAtPoint("value", point);
}

Eigen::Vector3d Mesh::getFFCGradient(Eigen::Vector3d query) const {
  this->updateCellLocator();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  this->cellLocator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

  double* gradAr = poly_data_->GetCellData()->GetArray("vff")->GetTuple3(cellId);
  Eigen::Vector3d grad(gradAr[0], gradAr[1], gradAr[2]);

  return grad;
}

// WARNING: Copied directly from Surface. TODO: When refactoring, take this into account.
vtkSmartPointer<vtkPoints> Mesh::getIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const {
  const int n_verts = this->poly_data_->GetNumberOfPoints();
  const int n_faces = this->poly_data_->GetNumberOfCells();

  V.resize(n_verts, 3);
  F.resize(n_faces, 3);

  auto points = this->poly_data_->GetPoints();
  for (int i = 0; i < n_verts; i++) {
    double p[3];
    points->GetPoint(i, p);
    V(i, 0) = p[0];
    V(i, 1) = p[1];
    V(i, 2) = p[2];
  }
  for (int i = 0; i < n_faces; i++) {
    auto cell = this->poly_data_->GetCell(i);
    assert(cell->GetNumberOfPoints() == 3);
    F(i, 0) = cell->GetPointId(0);
    F(i, 1) = cell->GetPointId(1);
    F(i, 2) = cell->GetPointId(2);
  }

  return points;
}

Mesh& Mesh::operator+=(const Mesh& otherMesh) {
  // Append the two meshes
  auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputData(this->poly_data_);
  appendFilter->AddInputData(otherMesh.poly_data_);

  // Remove any duplicate points.
  auto cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  this->poly_data_ = cleanFilter->GetOutput();

  this->invalidateLocators();
  return *this;
}

}  // namespace shapeworks
