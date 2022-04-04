#include <math.h>
#include <algorithm>
#include <vector>
#include <string>

#include "Mesh.h"
#include "MeshUtils.h"
#include "Image.h"
#include "StringUtils.h"
#include "PreviewMeshQC/FEAreaCoverage.h"
#include "PreviewMeshQC/FEVTKImport.h"
#include "PreviewMeshQC/FEVTKExport.h"
#include "FEFixMesh.h"
#include "Libs/Optimize/ParticleSystem/VtkMeshWrapper.h"

#include <igl/exact_geodesic.h>
#include <igl/gaussian_curvature.h>
#include <igl/principal_curvature.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/grad.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkOBJReader.h>
#include <vtkOBJWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkReverseSense.h>
#include <vtkFillHolesFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkProbeFilter.h>
#include <vtkClipPolyData.h>
#include <vtkCenterOfMass.h>
#include <vtkImageData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkDoubleArray.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointLocator.h>
#include <vtkIncrementalOctreePointLocator.h>
#include <vtkIncrementalPointLocator.h>
#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkPlaneCollection.h>
#include <vtkClipClosedSurface.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkNew.h>
#include <vtkSelectPolyData.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkVector.h>

#include <geometrycentral/surface/surface_mesh_factories.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <geometrycentral/surface/heat_method_distance.h>

// ACVD
#include <vtkIsotropicDiscreteRemeshing.h>

namespace shapeworks {

Mesh::MeshType MeshReader::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }
  if (!ShapeworksUtils::exists(pathname)) { throw std::invalid_argument(pathname + " does not exist"); }

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
  }
  catch (const std::exception &exp) {
    throw std::invalid_argument("Failed to read: " + pathname);
  }
}

Mesh::Mesh(const Eigen::MatrixXd& points, const Eigen::MatrixXi& faces)
{
    this->mesh = MeshType::New();

    vtkNew<vtkPoints> vertices;
    vtkNew<vtkCellArray> polys;

    const int num_points = points.rows();

    double p[3];
    for (vtkIdType i = 0; i < num_points; ++i)
    {
        p[0] = points(i, 0);
        p[1] = points(i, 1);
        p[2] = points(i, 2);

        vertices->InsertNextPoint(p);
    }

    const int num_cells = faces.rows();

    vtkIdType pts[3];
    for (vtkIdType i = 0; i < num_cells; ++i)
    {
        pts[0] = faces(i, 0);
        pts[1] = faces(i, 1);
        pts[2] = faces(i, 2);
        polys->InsertNextCell(3, pts);
    }

    this->mesh->SetPoints(vertices);
    this->mesh->SetPolys(polys);
    this->computeNormals();
}


Mesh::Mesh(const std::string& pathname) : mesh(MeshReader::read(pathname)) {
  invalidateLocators();
}

Mesh& Mesh::write(const std::string &pathname, bool binaryFile)
{
  if (!this->mesh) { throw std::invalid_argument("Mesh invalid"); }
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  try {
    if (StringUtils::hasSuffix(pathname, ".vtk")) {
      auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->WriteArrayMetaDataOff(); // needed for older readers to read these files
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
      writer->SetInputData(this->mesh);
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
      writer->SetInputData(this->mesh);
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
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".ply")) {
      if (getFieldNames().size() > 0)
        std::cerr << "WARNING: Trying to save mesh with new field. Only vtk and vtp files save associated fields\n";

      auto writer = vtkSmartPointer<vtkPLYWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      if (binaryFile)
        writer->SetFileTypeToBinary();
      else
        writer->SetFileTypeToASCII();
      writer->Update();
      return *this;
    }

    throw std::invalid_argument("Unsupported file type");
  }
  catch (const std::exception &exp) {
    throw std::invalid_argument("Failed to write mesh to " + pathname);
  }
}

Mesh& Mesh::coverage(const Mesh &otherMesh, bool allowBackIntersections, double angleThreshold, double backSearchRadius)
{
  FEVTKimport import;
  std::shared_ptr<FEMesh> surf1{import.Load(this->mesh)};
  std::shared_ptr<FEMesh> surf2{import.Load(otherMesh.mesh)};
  if (surf1 == nullptr || surf2 == nullptr) { throw std::invalid_argument("Mesh invalid"); }

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

  this->mesh = vtkOut.ExportToVTK(*surf1);
  this->invalidateLocators();

  return *this;
}

Mesh &Mesh::smooth(int iterations, double relaxation)
{
  vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();

  smoother->SetInputData(this->mesh);
  smoother->SetNumberOfIterations(iterations);
  if (relaxation)
  {
    smoother->SetRelaxationFactor(relaxation);
    smoother->FeatureEdgeSmoothingOff();
    smoother->BoundarySmoothingOn();
  }
  smoother->Update();
  this->mesh = smoother->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::smoothSinc(int iterations, double passband)
{
  vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  smoother->SetInputData(this->mesh);
  // minimum of 2.  See docs of vtkWindowedSincPolyDataFilter for explanation
  iterations = std::max<int>(iterations, 2);
  smoother->SetNumberOfIterations(iterations);
  smoother->SetPassBand(passband);
  smoother->Update();
  this->mesh = smoother->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::remesh(int numVertices, double adaptivity)
{
  // ACVD is very noisy to std::cout, even with console output set to zero
  // setting the failbit on std::cout will silence this until it's cleared below
  std::cout.setstate(std::ios_base::failbit);
  auto surf = vtkSmartPointer<vtkSurface>::New();
  auto remesh = vtkSmartPointer<vtkQIsotropicDiscreteRemeshing>::New();
  surf->CreateFromPolyData(this->mesh);
  surf->GetCellData()->Initialize();
  surf->GetPointData()->Initialize();
  surf->DisplayMeshProperties();
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
  std::cout.clear();

  this->mesh = remesh->GetOutput();

  // must regenerate normals after smoothing
  computeNormals();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::remeshPercent(double percentage, double adaptivity)
{
  int numVertices = mesh->GetNumberOfPoints() * percentage;
  return remesh(numVertices, adaptivity);
}

Mesh &Mesh::invertNormals()
{
  vtkSmartPointer<vtkReverseSense> reverseSense = vtkSmartPointer<vtkReverseSense>::New();

  reverseSense->SetInputData(this->mesh);
  reverseSense->ReverseNormalsOff();
  reverseSense->ReverseCellsOn();
  reverseSense->Update();
  this->mesh = reverseSense->GetOutput();

  return *this;
}

Mesh &Mesh::reflect(const Axis &axis, const Vector3 &origin)
{
  Vector scale(makeVector({1,1,1}));
  scale[axis] = -1;

  MeshTransform transform = MeshTransform::New();
  transform->Translate(-origin[0], -origin[1], -origin[2]);
  transform->Scale(scale[0], scale[1], scale[2]);
  transform->Translate(origin[0], origin[1], origin[2]);

  this->invalidateLocators();
  return invertNormals().applyTransform(transform);
}

MeshTransform Mesh::createTransform(const Mesh &target, Mesh::AlignmentType align, unsigned iterations)
{
  return createRegistrationTransform(target, align, iterations);
}

Mesh &Mesh::applyTransform(const MeshTransform transform)
{
  auto resampler = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  resampler->SetTransform(transform);
  resampler->SetInputData(this->mesh);
  resampler->Update();
  this->mesh = resampler->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh &Mesh::fillHoles()
{
  vtkSmartPointer<vtkFillHolesFilter> filter = vtkSmartPointer<vtkFillHolesFilter>::New();
  filter->SetInputData(this->mesh);
  filter->SetHoleSize(1000.0);
  filter->Update();
  this->mesh = filter->GetOutput();

  auto origNormal = mesh->GetPointData()->GetNormals();

  // Make the triangle window order consistent
  computeNormals();

  // Restore the original normals
  mesh->GetPointData()->SetNormals(origNormal);

  this->invalidateLocators();
  return *this;
}

Mesh &Mesh::probeVolume(const Image &image)
{
  vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
  probeFilter->SetInputData(this->mesh);
  probeFilter->SetSourceData(image.getVTKImage());
  probeFilter->Update();
  this->mesh = probeFilter->GetPolyDataOutput();

  this->invalidateLocators();
  return *this;
}

Mesh &Mesh::clip(const Plane plane)
{
  vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
  clipper->SetInputData(this->mesh);
  clipper->SetClipFunction(plane);
  clipper->Update();
  this->mesh = clipper->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh &Mesh::translate(const Vector3 &v)
{
  MeshTransform transform = MeshTransform::New();
  transform->Translate(v[0], v[1], v[2]);

  return applyTransform(transform);
}

Mesh &Mesh::scale(const Vector3 &v)
{
  MeshTransform transform = MeshTransform::New();
  transform->Scale(v[0], v[1], v[2]);

  return applyTransform(transform);
}

PhysicalRegion Mesh::boundingBox() const
{
  PhysicalRegion bbox;
  double bb[6];
  mesh->GetBounds(bb);

  for(int i = 0; i < 3; i++)
  {
    bbox.min[i] = bb[2*i];
    bbox.max[i] = bb[2*i+1];
  }

  return bbox;
}

Mesh& Mesh::fixElement()
{
  FEVTKimport import;
  FEMesh* meshFE = import.Load(this->mesh);

  if (meshFE == nullptr) { throw std::invalid_argument("Unable to read file"); }

  FEFixMesh fix;
  FEMesh* meshFix;

  meshFix = fix.FixElementWinding(meshFE);

  FEVTKExport vtkOut;
  this->mesh = vtkOut.ExportToVTK(*meshFix);

  this->invalidateLocators();
  return *this;
}

std::vector<Field> Mesh::distance(const Mesh &target, const DistanceMethod method) const
{
  if (target.numPoints() == 0 || numPoints() == 0)
    throw std::invalid_argument("meshes must have points");

  // allocate Arrays to store distances and ids from each point to target
  vtkSmartPointer<vtkDoubleArray> distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("distance");
  vtkSmartPointer<vtkDoubleArray> ids = vtkSmartPointer<vtkDoubleArray>::New();
  ids->SetNumberOfComponents(1);
  ids->SetNumberOfTuples(numPoints());
  ids->SetName("ids");

  // Find the nearest neighbors to each point and compute distance between them
  Point3 currentPoint, closestPoint;

  switch(method)
  {
    case PointToPoint:
    {
      // build point locator for target mesh
      vtkSmartPointer<vtkKdTreePointLocator> targetPointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
      targetPointLocator->SetDataSet(target.mesh);
      targetPointLocator->BuildLocator();

      for (int i = 0; i < numPoints(); i++)
      {
        mesh->GetPoint(i, currentPoint.GetDataPointer());
        vtkIdType closestPointId = targetPointLocator->FindClosestPoint(currentPoint.GetDataPointer());
        target.mesh->GetPoint(closestPointId, closestPoint.GetDataPointer());
        ids->SetValue(i, closestPointId);
        distance->SetValue(i, length(currentPoint - closestPoint));
      }
    }
    break;

    case PointToCell:
    {
      // build cell locator for target mesh
      vtkSmartPointer<vtkCellLocator> targetCellLocator = vtkSmartPointer<vtkCellLocator>::New();
      targetCellLocator->SetDataSet(target.mesh);
      targetCellLocator->BuildLocator();

      double dist2;
      vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
      vtkIdType cellId;
      int subId;

      for (int i = 0; i < numPoints(); i++)
      {
        mesh->GetPoint(i, currentPoint.GetDataPointer());
        targetCellLocator->FindClosestPoint(currentPoint.GetDataPointer(),
                                            closestPoint.GetDataPointer(),
                                            cell, cellId, subId, dist2);
        ids->SetValue(i, cellId);
        distance->SetValue(i, std::sqrt(dist2));
      }
    }
    break;

    default:
      throw std::invalid_argument("invalid distance method");
  }

  return std::vector<Field>{ distance, ids };
}

// TODO^2: do the same thing for the other couple functions below that set a field and return the other field (they shouldn't be setting a field)
// (search for setField and you'll see the 2-3 places it needs to be done)

Mesh& Mesh::clipClosedSurface(const Plane plane)
{
  vtkSmartPointer<vtkPlaneCollection> planeCollection = vtkSmartPointer<vtkPlaneCollection>::New();
  planeCollection->AddItem(plane);

  vtkSmartPointer<vtkClipClosedSurface> clipper = vtkSmartPointer<vtkClipClosedSurface>::New();
  clipper->SetClippingPlanes(planeCollection);
  clipper->SetInputData(this->mesh);
  clipper->SetGenerateFaces(1);
  clipper->Update();
  this->mesh = clipper->GetOutput();

  this->invalidateLocators();
  return *this;
}

Mesh& Mesh::computeNormals()
{
  vtkSmartPointer<vtkPolyDataNormals> normal = vtkSmartPointer<vtkPolyDataNormals>::New();

  normal->SetInputData(this->mesh);
  normal->ComputeCellNormalsOn();
  normal->ComputePointNormalsOn();
  normal->AutoOrientNormalsOn();
  normal->SplittingOff();
  normal->Update();
  this->mesh = normal->GetOutput();

  return *this;
}

void Mesh::invalidateLocators() const
{
  this->cellLocator = nullptr;
  this->pointLocator = nullptr;
}

void Mesh::updatePointLocator() const
{
  if (!this->pointLocator)
  {
    this->pointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    this->pointLocator->SetDataSet(this->mesh);
    this->pointLocator->BuildLocator();
  }
}

void Mesh::updateCellLocator() const
{
  if (!this->cellLocator)
  {
    this->cellLocator = vtkSmartPointer<vtkCellLocator>::New();
    this->cellLocator->SetDataSet(this->mesh);
    this->cellLocator->BuildLocator();
  }
}

Point3 Mesh::closestPoint(const Point3 point, bool& outside, double& distance, vtkIdType& face_id) const
{
  this->updateCellLocator();

  double dist2;
  Point3 closestPoint;
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  int subId;

  cellLocator->FindClosestPoint(point.GetDataPointer(), closestPoint.GetDataPointer(),
                                cell, face_id, subId, dist2);

  // distance from point to closest point
  distance = sqrt(dist2);

  // compute face normal: (p2-p1) x (p0-p1)
  double pt0[3], pt1[3], pt2[3];
  cell->GetPoints()->GetPoint(0, pt0);
  cell->GetPoints()->GetPoint(1, pt1);
  cell->GetPoints()->GetPoint(2, pt2);
  vtkVector3d v0(pt2[0] - pt1[0], pt2[1] - pt1[1], pt2[2] - pt1[2]);
  vtkVector3d v1(pt0[0] - pt1[0], pt0[1] - pt1[1], pt0[2] - pt1[2]);
  auto norm = v0.Cross(v1);

  // use dot product to determine whether point is outside mesh
  vtkVector3d pvec(point[0] - closestPoint[0],
                   point[1] - closestPoint[1],
                   point[2] - closestPoint[2]);
  outside = pvec.Dot(norm) > 0.0;

  return closestPoint;
}

int Mesh::closestPointId(const Point3 point) const
{
  this->updatePointLocator();

  vtkIdType closestPointId = pointLocator->FindClosestPoint(point.GetDataPointer());
  return closestPointId;
}

double Mesh::geodesicDistance(int source, int target) const
{
  if (source < 0 || target < 0 || numPoints() < source || numPoints() < target) {
    throw std::invalid_argument("requested point ids outside range of points available in mesh");
  }

  VtkMeshWrapper wrap(this->mesh, true);
  return wrap.ComputeDistance(getPoint(source), -1, getPoint(target), -1);
}

Field Mesh::geodesicDistance(const Point3 landmark) const
{
  vtkSmartPointer<vtkDoubleArray> distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("GeodesicDistanceToLandmark");

  VtkMeshWrapper wrap(this->mesh, true);

  for (int i = 0; i < numPoints(); i++)
  {
    distance->SetValue(i, wrap.ComputeDistance(landmark, -1, getPoint(i), -1));
  }

  return distance;
}

Field Mesh::geodesicDistance(const std::vector<Point3> curve) const
{
  vtkSmartPointer<vtkDoubleArray> minDistance = vtkSmartPointer<vtkDoubleArray>::New();
  minDistance->SetNumberOfComponents(1);
  minDistance->SetNumberOfTuples(numPoints());
  minDistance->SetName("GeodesicDistanceToCurve");
  minDistance->Fill(1e20);

  for (int i = 0; i < curve.size(); i++)
  {
    Field distance = geodesicDistance(curve[i]);
    for (int j = 0; j < numPoints(); j++)
    {
      if (distance->GetTuple1(j) < minDistance->GetTuple1(j))
        minDistance->SetValue(j, distance->GetTuple1(j));
    }
  }

  return minDistance;
}

Field Mesh::curvature(const CurvatureType type) const
{
  Eigen::MatrixXd V = points();
	Eigen::MatrixXi F = faces();

  Eigen::MatrixXd PD1,PD2;
  Eigen::VectorXd PV1,PV2;

  Eigen::VectorXd C;

  vtkSmartPointer<vtkDoubleArray> curv = vtkSmartPointer<vtkDoubleArray>::New();
  curv->SetNumberOfComponents(1);
  curv->SetNumberOfTuples(numPoints());

  switch (type) {
    case Principal:
    {
      curv->SetName("principal curvature");

      // returns maximal curvature value for each vertex
      // igl::principal_curvature(V, F, PD1, PD2, PV1, PV2);

      // returns minimal curvature value for each vertex
      igl::principal_curvature(V, F, PD1, PD2, C, PV2);
      break;
    }
    case Gaussian:
    {
      curv->SetName("gaussian curvature");
      igl::gaussian_curvature(V, F, C);
      break;
    }
    case Mean:
    {
      curv->SetName("mean curvature");

      Eigen::MatrixXd HN;
      Eigen::SparseMatrix<double> L,M,Minv;
      igl::cotmatrix(V,F,L);
      igl::massmatrix(V,F,igl::MASSMATRIX_TYPE_VORONOI,M);
      igl::invert_diag(M,Minv);

      // Laplace-Beltrami of position
      HN = -Minv*(L*V);

      // Extract magnitude as mean curvature
      C = HN.rowwise().norm();

      // Compute curvature directions via quadric fitting
      igl::principal_curvature(V,F,PD1,PD2,PV1,PV2);

      // mean curvature
      C = 0.5*(PV1+PV2);
      break;
    }
    default:
      throw std::invalid_argument("Unknown Mesh::CurvatureType.");
  }

  for (int i = 0; i < numPoints(); i++)
    curv->SetValue(i, C[i]);

  return curv;
}

Mesh& Mesh::applySubdivisionFilter(const SubdivisionType type, int subdivision)
{
  if (type == Mesh::SubdivisionType::Loop)
  {
    vtkSmartPointer<vtkLoopSubdivisionFilter> filter = vtkSmartPointer<vtkLoopSubdivisionFilter>::New();
    filter->SetInputData(this->mesh);
    filter->SetNumberOfSubdivisions(subdivision);
    filter->Update();
    this->mesh = filter->GetOutput();
  }
  else
  {
    vtkSmartPointer<vtkButterflySubdivisionFilter> filter = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
    filter->SetInputData(this->mesh);
    filter->SetNumberOfSubdivisions(subdivision);
    filter->Update();
    this->mesh = filter->GetOutput();
  }

  return *this;
}

Image Mesh::toImage(PhysicalRegion region, Point3 spacing) const
{
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
  vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();
  whiteImage->SetOrigin(region.origin()[0], region.origin()[1], region.origin()[2]);
  whiteImage->SetSpacing(spacing[0], spacing[1], spacing[2]);
  whiteImage->SetExtent(0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1);
  whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  // paint it white
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
    whiteImage->GetPointData()->GetScalars()->SetTuple1(i, 1);

  // cut stencil from mesh silhouette (same size as output image)
  vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  pol2stenc->SetInputData(this->mesh);
  pol2stenc->SetInformationInput(whiteImage);
  pol2stenc->Update();

  // spray output using stencil (use dark paint)
  vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
  imgstenc->SetInputData(whiteImage);
  imgstenc->SetStencilData(pol2stenc->GetOutput());
  imgstenc->ReverseStencilOff();
  imgstenc->SetBackgroundValue(0.0);
  imgstenc->Update();

  return Image(imgstenc->GetOutput());
}

Image Mesh::toDistanceTransform(PhysicalRegion region, const Point3 spacing, const Dims padding) const
{
  this->updateCellLocator();

  // if no region, use mesh bounding box
  if (region == PhysicalRegion()) {
    region = boundingBox();
  }

  // compute dims
  auto dims = toDims(region.size() / spacing) + Dims({padding[0]*2, padding[1]*2, padding[2]*2});

  // allocate output image, setting origin and spacing
  Image img(dims);
  img.setSpacing(toVector(spacing));
  auto origin = region.min - spacing * toPoint(padding);
  img.setOrigin(origin);

  auto itkimg = img.getITKImage();

  bool outside;
  double distance;
  vtkIdType face_id;

  // for each pixel, set value to be its distance to the mesh
  for (int z = 0; z < dims[0]; z++)
  {
    for (int y = 0; y < dims[1]; y++)
    {
      for (int x = 0; x < dims[2]; x++)
      {
        auto idx = Image::ImageType::IndexType({z,y,x}); // itk images are indexed by slice,row,col
        Point3 loc = Point3({origin[0]+x*spacing[0], origin[1]+y*spacing[1], origin[2]+z*spacing[2]});

        auto cp = closestPoint(loc, outside, distance, face_id);

        // NOTE: distance is positive inside, negative outside
        itkimg->SetPixel(idx, outside ? -distance : distance);
      }
    }
  }

  return img;
}

Point3 Mesh::center() const
{
  double c[3];
  mesh->GetCenter(c);
  return Point3({c[0], c[1], c[2]});
}

Point3 Mesh::centerOfMass() const
{
  auto com = vtkSmartPointer<vtkCenterOfMass>::New();
  com->SetInputData(this->mesh);
  com->Update();
  double center[3];
  com->GetCenter(center);
  return center;
}

Point3 Mesh::getPoint(int id) const
{
  if (this->numPoints() < id) { throw std::invalid_argument("mesh has fewer indices than requested"); }

  double point[3];
  mesh->GetPoint(id, point);
  return Point3({point[0], point[1], point[2]});
}

IPoint3 Mesh::getFace(int id) const
{
  if (mesh->GetNumberOfCells() < id) { throw std::invalid_argument("mesh has fewer indices than requested"); }

  IPoint3 face;
  auto cells = vtkSmartPointer<vtkIdList>::New();
  mesh->GetCellPoints(id, cells);

  for(int i = 0; i <3; i++)
  {
    face[i] = cells->GetId(i);
  }

  return face;
}

Eigen::MatrixXd Mesh::points() const
{
  int num_points = numPoints();
  Eigen::MatrixXd points_(num_points, 3);

  vtkSmartPointer<vtkDataArray> data_array = mesh->GetPoints()->GetData();

  for (int i = 0; i < num_points; i++) {
    points_(i, 0) = data_array->GetComponent(i, 0);
    points_(i, 1) = data_array->GetComponent(i, 1);
    points_(i, 2) = data_array->GetComponent(i, 2);
  }

  return points_;
}

Eigen::MatrixXi Mesh::faces() const
{
  int num_faces = numFaces();
  Eigen::MatrixXi faces(num_faces, 3);

  vtkSmartPointer<vtkIdList> cells = vtkSmartPointer<vtkIdList>::New();

  for (int j = 0; j < num_faces; j++) {
    mesh->GetCellPoints(j, cells);
    faces(j, 0) = cells->GetId(0);
    faces(j, 1) = cells->GetId(1);
    faces(j, 2) = cells->GetId(2);
  }

  return faces;
}

std::vector<std::string> Mesh::getFieldNames() const
{
  std::vector<std::string> fields;
  int numFields = mesh->GetPointData()->GetNumberOfArrays();

  for (int i=0; i<numFields; i++) {
    auto name = mesh->GetPointData()->GetArrayName(i);
    fields.push_back(name ? std::string(name) : std::string("default"));
  }

  return fields;
}

Field Mesh::getField(const std::string& name, const FieldType type) const
{
  if (type == Mesh::Point)
  {
    if (mesh->GetPointData()->GetNumberOfArrays() < 1)
      throw std::invalid_argument("Mesh has no fields.");

    Field rawarr = mesh->GetPointData()->GetArray(name.c_str());
    if (!rawarr)
      throw std::invalid_argument("Mesh does not contain a point field called " + name);

    return rawarr;
  }
  else if (type == Mesh::Face)
  {
    return getFieldForFaces(name);
  }
  else
    throw std::invalid_argument("Incorrect Mesh::FieldType.");
}

Field Mesh::getFieldForFaces(const std::string& name) const
{
  if (mesh->GetCellData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no face fields.");

  Field rawarr = mesh->GetCellData()->GetArray(name.c_str());
  if (!rawarr)
    throw std::invalid_argument("Mesh does not contain a cell field called " + name);

  return rawarr;
}

Mesh& Mesh::setField(std::string name, Array array, const FieldType type)
{
  if (type == Mesh::Point)
  {
    if (!array)
    throw std::invalid_argument("Invalid array.");

    if (name.empty())
      throw std::invalid_argument("Provide name for the new field");

    int numVertices = numPoints();
    if (array->GetNumberOfTuples() != numVertices) {
      std::cerr << "WARNING: Added a mesh field with a different number of elements than points\n";
    }

    array->SetName(name.c_str());
    mesh->GetPointData()->AddArray(array);

    return *this;
  }
  else if(type == Mesh::Face)
    return setFieldForFaces(name, array);
  else
    throw std::invalid_argument("Incorrect Mesh::FieldType.");
}

Mesh& Mesh::setFieldForFaces(std::string name, Array array)
{
  if (!array)
    throw std::invalid_argument("Invalid array.");

  if (name.empty())
    throw std::invalid_argument("Provide name for the new field");

  if (array->GetNumberOfTuples() != numFaces()) {
    std::cerr << "WARNING: Added a mesh field with a different number of elements than points\n";
  }
  if (array->GetNumberOfComponents() != 1) {
    //std::cerr << "WARNING: Added a multi-component mesh field\n";
  }

  array->SetName(name.c_str());
  mesh->GetCellData()->AddArray(array);

  return *this;
}

void Mesh::setFieldValue(const std::string& name, int idx, double val)
{
  if (name.empty())
    throw std::invalid_argument("Provide name for the field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields for which to set a value.");

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (arr->GetNumberOfTuples() > idx)
    arr->SetTuple1(idx, val);
  else
    throw std::invalid_argument("Intended index in field is out of range");
}

double Mesh::getFieldValue(const std::string& name, int idx) const
{
  if (name.empty())
    throw std::invalid_argument("Provide name for field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields from which to retrieve a value.");

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (!arr)
    throw std::invalid_argument("Field does not exist.");

  if (arr->GetNumberOfTuples() > idx)
    return arr->GetTuple(idx)[0];
  else
    throw std::invalid_argument("Requested index in field is out of range");
}

Eigen::VectorXd Mesh::getMultiFieldValue(const std::string& name, int idx) const
{
  if (name.empty())
    throw std::invalid_argument("Provide name for field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields from which to retrieve a value.");

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (!arr)
    throw std::invalid_argument("Field does not exist.");

  if (arr->GetNumberOfTuples() > idx) {
    size_t compnum = arr->GetNumberOfComponents();
    Eigen::VectorXd vec(compnum);
    for(size_t i = 0; i < compnum; i++) {
        vec(i) = arr->GetTuple(idx)[i];
    }
    return vec;
  }
  else
    throw std::invalid_argument("Requested index in field is out of range");
}

bool Mesh::compareAllPoints(const Mesh &other_mesh) const
{
  if (!this->mesh || !other_mesh.mesh)
    throw std::invalid_argument("invalid meshes");

  if (this->numPoints() != other_mesh.numPoints())
  {
    std::cerr << "meshes differ in number of points";
    return false;
  }

  for (int i = 0; i < this->numPoints(); i++)
  {
    Point3 p1(this->mesh->GetPoint(i));
    Point3 p2(other_mesh.mesh->GetPoint(i));
    if (!epsEqual(p1, p2, 0.011)) {
      printf("%ith points not equal ([%0.8f, %0.8f, %0.8f], [%0.8f, %0.8f, %0.8f])\n",
             i, p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);

      return false;
    }
  }

  return true;
}

bool Mesh::compareAllFaces(const Mesh &other_mesh) const
{
  if (!this->mesh || !other_mesh.mesh)
    throw std::invalid_argument("invalid meshes");

  if (this->mesh->GetNumberOfCells() != other_mesh.mesh->GetNumberOfCells())
  {
    std::cerr << "meshes differ in number of faces";
    return false;
  }

  // helper function to print out the cell indices
  auto printCells = [](vtkCell* cell1, vtkCell* cell2){
    printf("[ ");
    for(int i = 0; i < cell1->GetNumberOfPoints(); i++) {
      printf("%lld ", cell1->GetPointId(i));
    }
    printf("], [ ");
    for(int i = 0; i < cell2->GetNumberOfPoints(); i++) {
      printf("%lld ", cell2->GetPointId(i));
    }
    printf("]");
  };

  for (int i = 0; i < this->mesh->GetNumberOfCells(); i++)
  {
    vtkCell* cell1 = this->mesh->GetCell(i);
    vtkCell* cell2 = other_mesh.mesh->GetCell(i);

    if(cell1->GetNumberOfPoints() != cell2->GetNumberOfPoints())
    {
      printf("%ith face not equal (", i);
      printCells(cell1, cell2);
      printf(")\n");
      return false;
    }

    for(int pi=0; pi<cell1->GetNumberOfPoints(); pi++)
    {
      if(cell1->GetPointId(pi) != cell2->GetPointId(pi))
      {
        printf("%ith face not equal (", i);
        printCells(cell1, cell2);
        printf(")\n");
        return false;
      }
    }
  }

  return true;
}

bool Mesh::compareAllFields(const Mesh &other_mesh, const double eps) const
{
  if (!this->mesh || !other_mesh.mesh)
    throw std::invalid_argument("Invalid meshes");

  auto fields1 = getFieldNames();
  auto fields2 = other_mesh.getFieldNames();

  // first make sure they even have the same fields to compare
  if (fields1.size() != fields2.size()) {
    std::cerr << "Mesh have different number of fields\n";
    return false;
  }
  for (int i=0; i<fields1.size(); i++) {
    if (std::find(fields2.begin(), fields2.end(), fields1[i]) == fields2.end()) {
      std::cerr << "Both meshes don't have " << fields1[i] << " field\n";
      return false;
    }
  }

  // now compare the actual fields
  for (auto field: fields1) {
    if (!compareField(other_mesh, field, "", eps)) {
      std::cerr << field << " fields are not the same\n";
      return false;
    }
  }

  return true;
}

bool Mesh::compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2, const double eps) const
{
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

  for (int i = 0; i < field1->GetNumberOfTuples(); i++)
  {
    for (int c = 0; c < field1->GetNumberOfComponents(); c++)
    {
      auto v1(field1->GetTuple(i)[c]);
      auto v2(field2->GetTuple(i)[c]);
      if (eps > 0) {
        if (!(std::abs(v1-v2) < eps)) {
          printf("%ith values not equal (%0.8f != %0.8f)\n", i, v1, v2);
          return false;
        }
      }
      else {
        if (!epsEqual(v1, v2, 1e-5)) {
          printf("%ith values not equal (%0.8f != %0.8f)\n", i, v1, v2);
          return false;
        }
      }
    }
  }

  return true;
}

bool Mesh::compare(const Mesh& other, const double eps) const
{
  if (!epsEqual(center(), other.center(), 1e-2))             { std::cerr << "centers differ!\n"; return false; }
  if (!epsEqual(centerOfMass(), other.centerOfMass(), 1e-3)) { std::cerr << "coms differ!\n"; return false; }
  if (numPoints() != other.numPoints())                      { std::cerr << "num pts differ\n"; return false; }
  if (numFaces() != other.numFaces())                        { std::cerr << "num faces differ\n"; return false; }
  if (!compareAllPoints(other))                              { std::cerr << "points differ\n"; return false; }
  if (!compareAllFaces(other))                               { std::cerr << "faces differ\n"; return false; }
  if (!compareAllFields(other, eps))                         { std::cerr << "fields differ\n"; return false; }

  return true;
}

MeshTransform Mesh::createRegistrationTransform(const Mesh &target, Mesh::AlignmentType align, unsigned iterations) const
{
  const vtkSmartPointer<vtkMatrix4x4> mat(MeshUtils::createICPTransform(this->mesh, target.getVTKMesh(), align, iterations, true));
  return createMeshTransform(mat);
}

std::ostream& operator<<(std::ostream &os, const Mesh& mesh)
{
  os << "{\nnumber of points: " << mesh.numPoints()
     << ",\nnumber of faces: " << mesh.numFaces()
     << ",\ncenter: " << mesh.center()
     << ",\ncenter or mass: " << mesh.centerOfMass()
     << ",\nbounding box: " << mesh.boundingBox()
     << ",\nfield names: \n";

  auto fields = mesh.getFieldNames();
  for (auto field: fields) {
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
  for (vtkIdType i = 0; i < mesh->GetNumberOfPoints(); i++) {
    points->InsertNextPoint(mesh->GetPoint(i));
    if (this->getFieldValue(name, i) == value) {
      member.push_back(true);
    } else {
      member.push_back(false);
    }
  }

  // copy triangles
  for (vtkIdType i = 0; i < mesh->GetNumberOfCells(); i++) {
    vtkCell* cell = this->mesh->GetCell(i);

    if (cell->GetCellType() != VTK_EMPTY_CELL) { // VTK_EMPTY_CELL means it was deleted
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

// TODO: Use Mesh's functions for many of the items in these functions copied from Meshwrapper.
bool Mesh::prepareFFCFields(std::vector<std::vector<Eigen::Vector3d>> boundaries, Eigen::Vector3d query, bool onlyGenerateInOut)
{
  if (mesh->GetPointData()->GetArray("inout")) {
    // clear out any old versions of the inout array or else they will get merged in
    mesh->GetPointData()->RemoveArray("inout");
  }

  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  vtkSmartPointer<vtkPoints> points;
  if (!onlyGenerateInOut) {
    points = getIGLMesh(V, F);
  }


  for (size_t bound = 0; bound < boundaries.size(); bound++) {
    //std::cout << "Boundaries " << bound << " size " << boundaries[bound].size() << std::endl;

    // Creating cutting loop
    vtkPoints* selectionPoints = vtkPoints::New();
    std::vector<size_t> boundaryVerts;

    // the locator is continuously rebuilt during this function, so don't use the cached version
    vtkSmartPointer<vtkKdTreePointLocator> tmp_locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    tmp_locator->SetDataSet(this->mesh);
    tmp_locator->BuildLocator();

    // Create path creator
    auto dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInputData(this->mesh);

    vtkIdType lastId = 0;
    for (size_t i = 0; i < boundaries[bound].size(); i++) {
      Eigen::Vector3d pt = boundaries[bound][i];
      double ptdob[3] = {pt[0], pt[1], pt[2]};
      vtkIdType ptid = tmp_locator->FindClosestPoint(ptdob);
      mesh->GetPoint(ptid, ptdob);

      // Add first point in boundary
      if (i == 0) {
        lastId = ptid;
        Point3 pathpt;
        pathpt = getPoint(ptid);
        selectionPoints->InsertNextPoint(pathpt[0], pathpt[1], pathpt[2]);
        boundaryVerts.push_back(ptid);
      }

      /* AKM: I'm not convinced this dijkstra stuff is necessary.  It's super slow with multiple dense boundaries.
       * Assuming the free form constraint is defined by the user in Studio, it will be very dense already and there is no
       * requirement of vtkSelectPolyData that a contigious set of vertices be supplied.  Indeed, they need not even be vertices */

      // If the current and last vertices are different, then add all vertices in the path to the boundaryVerts list
      if (lastId != ptid) {
        //std::cout << pt[0] << " " << pt[1] << " " << pt[2] << " -> " << ptdob[0] << " " << ptdob[1] << " " << ptdob[2] << std::endl;
        // Add points in path
        dijkstra->SetStartVertex(lastId);
        dijkstra->SetEndVertex(ptid);
        dijkstra->Update();
        vtkSmartPointer<vtkIdList> idL = dijkstra->GetIdList();
        for (size_t j = 1; j < idL->GetNumberOfIds(); j++) {
          vtkIdType id = idL->GetId(j);
          Point3 pathpt;
          pathpt = getPoint(ptid);
          selectionPoints->InsertNextPoint(pathpt[0], pathpt[1], pathpt[2]);
          boundaryVerts.push_back(id);
        }
      }
      lastId = ptid;
    }

    //std::cout << "Number of boundary vertices " << boundaryVerts.size() << std::endl;

    if (selectionPoints->GetNumberOfPoints() < 3) {
      /// TODO: log an event that this occurred.  It's not really fatal as we may be applying to a mesh where this doesn't apply
      continue;
    }

    auto select = vtkSmartPointer<vtkSelectPolyData>::New();
    select->SetLoop(selectionPoints);
    select->SetInputData(this->mesh);
    select->GenerateSelectionScalarsOn();
    select->SetSelectionModeToLargestRegion();

    // Clipping mesh
    auto selectclip = vtkSmartPointer<vtkClipPolyData>::New();
    selectclip->SetInputConnection(select->GetOutputPort());
    selectclip->SetValue(0.0);
    selectclip->Update();

    MeshType halfmesh = selectclip->GetOutput();

    if (halfmesh->GetNumberOfPoints() == 0) {
      /// TODO: log an event that this occurred.  It's not really fatal as we may be applying to a mesh where this doesn't apply
      continue;
    }

    vtkSmartPointer<vtkDoubleArray> inout = computeInOutForFFCs(query, halfmesh);

    if (!onlyGenerateInOut) {
      auto values = vtkSmartPointer<vtkDoubleArray>::New();
      auto absvalues = setDistanceToBoundaryValueFieldForFFCs(values, points, boundaryVerts, inout, V, F);
      this->mesh->GetPointData()->SetActiveScalars("value");
      std::vector<Eigen::Matrix3d> face_grad = this->setGradientFieldForFFCs(absvalues, V, F);
    }

  } // Per boundary for loop end

  // Write mesh for debug purposes
//    std::string fnin = "dev/mesh_" + std::to_string(dom) + "_" + std::to_string(num) + "_in.vtk";
//    this->write(fnin);

  this->invalidateLocators();
  return true;
}


Eigen::Vector3d Mesh::computeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const
{
  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  Eigen::Vector3d bary;
  this->mesh->GetCell(face)->EvaluatePosition(pt.data(), closest, sub_id, pcoords, dist2, bary.data());
  return bary;
}

double Mesh::getFFCValue(Eigen::Vector3d query) const
{
  this->updateCellLocator();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  this->cellLocator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

  auto cell = this->mesh->GetCell(cellId);

  size_t v1 = cell->GetPointId(0);
  size_t v2 = cell->GetPointId(1);
  size_t v3 = cell->GetPointId(2);

  // Compute barycentric distances
  Eigen::Vector3d cp(closestPoint[0], closestPoint[1], closestPoint[2]);
  Eigen::Vector3d bary = computeBarycentricCoordinates(cp, cellId);

  bary = bary / bary.sum();

  Eigen::Vector3d values(this->getFieldValue("value", v1), this->getFieldValue("value", v2),
                         this->getFieldValue("value", v3));

  return (bary * values.transpose()).mean();
}

Eigen::Vector3d Mesh::getFFCGradient(Eigen::Vector3d query) const
{
  this->updateCellLocator();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  this->cellLocator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

  double* gradAr = mesh->GetCellData()->GetArray("vff")->GetTuple3(cellId);
  Eigen::Vector3d grad(gradAr[0], gradAr[1], gradAr[2]);

  return grad;
}

// WARNING: Copied directly from Meshwrapper. TODO: When refactoring, take this into account.
vtkSmartPointer<vtkPoints> Mesh::getIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const
{
  const int n_verts = this->mesh->GetNumberOfPoints();
  const int n_faces = this->mesh->GetNumberOfCells();

  V.resize(n_verts, 3);
  F.resize(n_faces, 3);

  auto points = this->mesh->GetPoints();
  for (int i = 0; i < n_verts; i++) {
    double p[3];
    points->GetPoint(i, p);
    V(i, 0) = p[0];
    V(i, 1) = p[1];
    V(i, 2) = p[2];
  }
  for (int i = 0; i < n_faces; i++) {
    auto cell = this->mesh->GetCell(i);
    assert (cell->GetNumberOfPoints() == 3);
    F(i, 0) = cell->GetPointId(0);
    F(i, 1) = cell->GetPointId(1);
    F(i, 2) = cell->GetPointId(2);
  }

  return points;
}

vtkSmartPointer<vtkDoubleArray> Mesh::computeInOutForFFCs(Eigen::Vector3d query, MeshType halfmesh)
{

  // Finding which half is in and which is out.
  bool halfmeshisin = true;
  auto* arr = vtkDoubleArray::SafeDownCast(mesh->GetPointData()->GetArray("inout")); // Check if an inout already exists

  // Create half-mesh tree
  auto kdhalf_locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdhalf_locator->SetDataSet(halfmesh);
  kdhalf_locator->BuildLocator();

  // Create full-mesh tree
  updatePointLocator();

  // Checking which mesh is closer to the query point. Recall that the query point must not necessarely lie on the mesh, so we check both the half mesh and the full mesh.
  double querypt[3] = {query[0], query[1], query[2]};

  vtkIdType halfi = kdhalf_locator->FindClosestPoint(querypt);
  vtkIdType fulli = this->pointLocator->FindClosestPoint(querypt);

  double halfp[3];
  halfmesh->GetPoint(halfi, halfp);

  double fullp[3];
  this->mesh->GetPoint(fulli, fullp);

  if (halfp[0] != fullp[0] || halfp[1] != fullp[1] || halfp[2] != fullp[2]) {
    halfmeshisin = false; // If the closest point in halfmesh is not the closest point in fullmesh, then halfmesh is not the in mesh.
  }

  vtkSmartPointer<vtkDoubleArray> inout = vtkSmartPointer<vtkDoubleArray>::New();
  inout->SetNumberOfComponents(1);
  inout->SetNumberOfTuples(this->mesh->GetNumberOfPoints());
  inout->SetName("inout");

  for (vtkIdType i = 0; i < this->mesh->GetNumberOfPoints(); i++) {
    this->mesh->GetPoint(i, fullp);

    halfi = kdhalf_locator->FindClosestPoint(fullp);
    halfmesh->GetPoint(halfi, halfp);
    //std::cout << i <<  " (" << fullp[0] << " " << fullp[1] << " " << fullp[2] << ") " << halfi << " (" << halfp[0] << " " << halfp[1] << " " << halfp[2] << " )" << std::endl;
    bool ptinhalfmesh = false;
    if (fullp[0] == halfp[0] && fullp[1] == halfp[1] && fullp[2] == halfp[2]) {
      // If in halfmesh
      ptinhalfmesh = true;
    }
    // The relationship becomes an xor operation between halfmeshisin and ptinhalfmesh to determine whether each point is in or out. Thus we set values for the scalar field.
    if (!halfmeshisin ^ ptinhalfmesh) {
      if (arr) {
        inout->SetValue(i, std::min(1., arr->GetValue(i)));
      } else {
        inout->SetValue(i, 1.);
      }
    }
    else {
      inout->SetValue(i, 0.);
    }
  }

  // Setting scalar field
  this->setField("inout", inout, Mesh::Point);

  return inout;
}

vtkSmartPointer<vtkDoubleArray>
Mesh::setDistanceToBoundaryValueFieldForFFCs(vtkSmartPointer<vtkDoubleArray> values,
                                             vtkSmartPointer<vtkPoints> points,
                                             std::vector<size_t> boundaryVerts,
                                             vtkSmartPointer<vtkDoubleArray> inout,
                                             Eigen::MatrixXd V, Eigen::MatrixXi F)
{
  auto* arr = vtkDoubleArray::SafeDownCast(mesh->GetPointData()->GetArray("value")); // Check if a value field already exists

  values->SetNumberOfComponents(1);
  values->SetNumberOfTuples(this->mesh->GetNumberOfPoints());
  values->SetName("values");
  for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
    values->SetValue(i, INFINITY);
  }
  vtkSmartPointer<vtkDoubleArray> absvalues = vtkSmartPointer<vtkDoubleArray>::New();
  absvalues->SetNumberOfComponents(1);
  absvalues->SetNumberOfTuples(this->mesh->GetNumberOfPoints());
  absvalues->SetName("absvalues");

  //std::cout << "Loading eval values for FFCs in domain " << dom << std::endl;

  // debug
  Eigen::MatrixXd C(this->mesh->GetNumberOfPoints(), 3);

  // Load the mesh on Geometry central
  {
    using namespace geometrycentral::surface;
    std::unique_ptr<SurfaceMesh> gcmesh;
    std::unique_ptr<VertexPositionGeometry> gcgeometry;
    std::tie(gcmesh, gcgeometry) = makeSurfaceMeshAndGeometry(V, F);
    HeatMethodDistanceSolver heatSolver(*gcgeometry);

    // Some vertices as source set
    std::vector<Vertex> sourceVerts;
    for (size_t i = 0; i < boundaryVerts.size(); i++) {
      sourceVerts.push_back(gcmesh->vertex(boundaryVerts[i]));
    }
    // Finds minimum distance to any boundary vertex from any other vertex
    for (Vertex v : sourceVerts) {
      VertexData<double> distToSource = heatSolver.computeDistance(v);
      //std::cout << distToSource[0] << " (" << values->GetValue(0) << ") ";
      for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
        if (distToSource[i] < std::abs(values->GetValue(i))) {
          absvalues->SetValue(i, distToSource[i]);
          if (inout->GetValue(i) == 1.) {
            if (arr) {
              values->SetValue(i, std::max<double>(arr->GetValue(i), -distToSource[i]));
            } else {
              values->SetValue(i, -distToSource[i]);
            }
            C(i, 0) = -distToSource[i];
            C(i, 1) = -distToSource[i];
            C(i, 2) = -distToSource[i];
          }
          else {
            values->SetValue(i, distToSource[i]);
            C(i, 0) = distToSource[i];
            C(i, 1) = distToSource[i];
            C(i, 2) = distToSource[i];
          }
        }
      }
    }
  }

  //std::cout << "Setting field" << std::endl;

  // TODO: don't set local field since user can do this if needed, and the
  // function can be const. (return vector of Fields like Mesh::distance)
  this->setField("value", values, Mesh::Point);

  return absvalues;
}

std::vector<Eigen::Matrix3d>
Mesh::setGradientFieldForFFCs(vtkSmartPointer<vtkDoubleArray> absvalues, Eigen::MatrixXd V,
                              Eigen::MatrixXi F)
{
  // Definition of gradient field
  vtkSmartPointer<vtkDoubleArray> vf = vtkSmartPointer<vtkDoubleArray>::New();
  vf->SetNumberOfComponents(3);
  vf->SetNumberOfTuples(this->mesh->GetNumberOfPoints());
  vf->SetName("vf");

  // *Computing gradients for FFCs for each face
  // Compute gradient operator
  //std::cout << "Gradient preprocessing like in Karthik's code" << std::endl;
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);
  // Flattened version of libigl's gradient operator
  std::vector<Eigen::Matrix3d> face_grad;

  Eigen::MatrixXd grads(this->mesh->GetNumberOfPoints(), 3);
  grads.fill(0.);

  // Flatten the gradient operator so we can quickly compute the gradient at a given point
  face_grad.resize(F.rows());
  size_t n_insertions = 0;
  for (int k = 0; k < G.outerSize(); k++) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(G, k); it; ++it) {
      const double val = it.value();
      const auto r = it.row();
      const auto c = it.col();

      const auto f = r % F.rows();
      const auto axis = r / F.rows();
      for (int i = 0; i < 3; i++) {
        if (F(f, i) == c) {
          face_grad[f](axis, i) = val;
          n_insertions++;
          break;
        }
      }
    }
  }

  //std::cout << "Computing grads" << std::endl;

  // Definition of gradient field for faces
  vtkSmartPointer<vtkDoubleArray> vff = vtkSmartPointer<vtkDoubleArray>::New();
  vff->SetNumberOfComponents(3);
  vff->SetNumberOfTuples(this->mesh->GetNumberOfCells());
  vff->SetName("vff");

  // Computes grad vec for each face
  for (size_t i = 0; i < F.rows(); i++) {
    const Eigen::Vector3d vert_dists(absvalues->GetValue(F(i, 0)), absvalues->GetValue(F(i, 1)),
                                     absvalues->GetValue(F(i, 2)));

    // Compute gradient of geodesics
    const auto& G = face_grad[i];
    Eigen::Vector3d out_grad_eigen = (G * vert_dists).rowwise().sum();
    grads.row(F(i, 0)) += out_grad_eigen;
    grads.row(F(i, 1)) += out_grad_eigen;
    grads.row(F(i, 2)) += out_grad_eigen;
    vff->SetTuple3(i, out_grad_eigen(0), out_grad_eigen(1), out_grad_eigen(2));
    //out_grad_eigen *= geo_dist / out_grad_eigen.norm();
  }

  // Setting gradient field
  for (size_t i = 0; i < this->mesh->GetNumberOfPoints(); i++) {
    vf->SetTuple3(i, grads(i, 0), grads(i, 1), grads(i, 2));
  }

  // TODO: don't set field in this mesh since the user can do this if they want
  // it to be part of the mesh, and the function can therefore be const.
  this->setField("Gradient", vf, Mesh::Point);
  this->setField("vff", vff, Mesh::Face);

  return face_grad;
}

Mesh& Mesh::operator+=(const Mesh& otherMesh)
{
  // Append the two meshes
  vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputData(this->mesh);
  appendFilter->AddInputData(otherMesh.mesh);

  // Remove any duplicate points.
  vtkSmartPointer<vtkCleanPolyData> cleanFilter= vtkSmartPointer<vtkCleanPolyData>::New();
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  this->mesh = cleanFilter->GetOutput();

  this->invalidateLocators();
  return *this;
}

} // shapeworks


