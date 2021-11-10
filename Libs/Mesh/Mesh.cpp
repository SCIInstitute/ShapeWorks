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
#include "FECVDDecimationModifier.h"
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
#include <vtkDecimatePro.h>
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
#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkPlaneCollection.h>
#include <vtkClipClosedSurface.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkNew.h>
#include <vtkCellData.h>
#include <vtkSelectPolyData.h>
#include <vtkDijkstraGraphGeodesicPath.h>

#include <geometrycentral/surface/surface_mesh_factories.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <geometrycentral/surface/heat_method_distance.h>

namespace shapeworks {

Mesh::MeshType Mesh::read(const std::string &pathname)
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

Mesh& Mesh::write(const std::string &pathname)
{
  if (!this->mesh) { throw std::invalid_argument("Mesh invalid"); }
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  try {
    if (StringUtils::hasSuffix(pathname, ".vtk")) {
      auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->WriteArrayMetaDataOff(); // needed for older readers to read these files
      writer->SetFileTypeToBinary();
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".vtp")) {
      auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".stl")) {
      if (getFieldNames().size() > 0)
        std::cerr << "WARNING: Trying to save mesh with new field. Only vtk and vtp files save associated fields\n";

      auto writer = vtkSmartPointer<vtkSTLWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
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
  return *this;
}

Mesh &Mesh::decimate(double reduction, double angle, bool preserveTopology)
{
  vtkSmartPointer<vtkDecimatePro> decimator = vtkSmartPointer<vtkDecimatePro>::New();

  decimator->SetInputData(this->mesh);
  decimator->SetTargetReduction(reduction);
  decimator->SetFeatureAngle(angle);
  preserveTopology == true ? decimator->PreserveTopologyOn() : decimator->PreserveTopologyOff();
  decimator->BoundaryVertexDeletionOn();
  decimator->Update();
  this->mesh = decimator->GetOutput();

  return *this;
}

Mesh &Mesh::cvdDecimate(double percentage)
{
  FEVTKimport import;
  std::shared_ptr<FEMesh> meshFE(import.Load(this->mesh));

  if (meshFE == nullptr) { throw std::invalid_argument("Unable to read file"); }

  FECVDDecimationModifier cvd;
  cvd.m_pct = percentage;
  cvd.m_gradient = 1;
  meshFE = std::shared_ptr<FEMesh>(cvd.Apply(meshFE.get()));

  FEVTKExport vtkOut;
  this->mesh = vtkOut.ExportToVTK(*meshFE);

  return *this;
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

  return *this;
}

Mesh &Mesh::probeVolume(const Image &image)
{
  vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
  probeFilter->SetInputData(this->mesh);
  probeFilter->SetSourceData(image.getVTKImage());
  probeFilter->Update();
  this->mesh = probeFilter->GetPolyDataOutput();

  return *this;
}

Mesh &Mesh::clip(const Plane plane)
{
  vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
  clipper->SetInputData(this->mesh);
  clipper->SetClipFunction(plane);
  clipper->Update();
  this->mesh = clipper->GetOutput();

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

  return *this;
}

Mesh& Mesh::distance(const Mesh &target, const DistanceMethod method)
{
  if (target.numPoints() == 0 || numPoints() == 0)
    throw std::invalid_argument("meshes must have points");

  // allocate Array to store distances from each point to target
  vtkSmartPointer<vtkDoubleArray> distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("distance");

  // Find the nearest neighbors to each point and compute distance between them
  Point currentPoint, closestPoint;

  switch(method)
  {
    case PointToPoint:
    {
      vtkSmartPointer<vtkKdTreePointLocator> targetPointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
      targetPointLocator->SetDataSet(target.mesh);
      targetPointLocator->BuildLocator();

      for (int i = 0; i < numPoints(); i++)
      {
        mesh->GetPoint(i, currentPoint.GetDataPointer());
        vtkIdType closestPointId = targetPointLocator->FindClosestPoint(currentPoint.GetDataPointer());
        target.mesh->GetPoint(closestPointId, closestPoint.GetDataPointer());
        distance->SetValue(i, length(currentPoint - closestPoint));
      }
    }
    break;

    case PointToCell:
    {
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
        targetCellLocator->FindClosestPoint(currentPoint.GetDataPointer(), closestPoint.GetDataPointer(), cell, cellId, subId, dist2);
        distance->SetValue(i, std::sqrt(dist2));
      }
    }
    break;

    default:
      throw std::invalid_argument("invalid distance method");
  }

  // add distance field to this mesh
  this->setField("distance", distance);

  return *this;
}

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

Point3 Mesh::closestPoint(const Point3 point)
{
  vtkSmartPointer<vtkCellLocator> targetCellLocator = vtkSmartPointer<vtkCellLocator>::New();
  targetCellLocator->SetDataSet(this->mesh);
  targetCellLocator->BuildLocator();

  double dist2;
  Point3 closestPoint;
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  vtkIdType cellId;
  int subId;

  targetCellLocator->FindClosestPoint(point.GetDataPointer(), closestPoint.GetDataPointer(), cell, cellId, subId, dist2);
  return closestPoint;
}

int Mesh::closestPointId(const Point3 point)
{
  vtkSmartPointer<vtkKdTreePointLocator> pointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  pointLocator->SetDataSet(this->mesh);
  pointLocator->BuildLocator();

  vtkIdType closestPointId = pointLocator->FindClosestPoint(point.GetDataPointer());
  return closestPointId;
}

double Mesh::geodesicDistance(int source, int target)
{
  if (source < 0 || target < 0 || numPoints() < source || numPoints() < target) {
    throw std::invalid_argument("requested point ids outside range of points available in mesh");
  }

  VtkMeshWrapper wrap(this->mesh, true);
  return wrap.ComputeDistance(getPoint(source), -1, getPoint(target), -1);
}

Field Mesh::geodesicDistance(const Point3 landmark)
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

Field Mesh::geodesicDistance(const std::vector<Point3> curve)
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

Field Mesh::curvature(const CurvatureType type)
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

Image Mesh::toImage(PhysicalRegion region, Point spacing) const
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

Image Mesh::toDistanceTransform(PhysicalRegion region, Point spacing) const
{
  // TODO: convert directly to DT (github #810)
  Image image(toImage(region, spacing));
  image.antialias(50, 0.00).computeDT();
  return image;
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

Mesh& Mesh::setField(std::string name, Array array)
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
    std::cerr << "WARNING: Added a multi-component mesh field\n";
  }

  array->SetName(name.c_str());
  mesh->GetCellData()->SetAttribute(array, 0);

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

Eigen::VectorXd Mesh::getMultiFieldValue(const std::string& name, int idx) const{
    if (name.empty())
      throw std::invalid_argument("Provide name for field");

    if (mesh->GetPointData()->GetNumberOfArrays() < 1)
      throw std::invalid_argument("Mesh has no fields from which to retrieve a value.");

    auto arr = mesh->GetPointData()->GetArray(name.c_str());
    if (!arr)
      throw std::invalid_argument("Field does not exist.");

    if (arr->GetNumberOfTuples() > idx){
      size_t compnum = arr->GetNumberOfComponents();
      Eigen::VectorXd vec(compnum);
      for(size_t i = 0; i < compnum; i++){
          vec(i) = arr->GetTuple(idx)[i];
      }
      return vec;
    }
    else
      throw std::invalid_argument("Requested index in field is out of range");
}

std::vector<double> Mesh::getFieldRange(const std::string& name) const
{
  if (name.empty())
    throw std::invalid_argument("Provide name for field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields for which to compute range.");

  std::vector<double> range(2);

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (!arr)
    throw std::invalid_argument("Field does not exist.");

  arr->GetRange(&range[0]);

  return range;
}

double Mesh::getFieldMean(const std::string& name) const
{
  if (name.empty())
    throw std::invalid_argument("Provide name for field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields for which to compute mean.");

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (!arr)
    throw std::invalid_argument("Field does not exist.");

  double mean{0.0};
  for (int i=0; i<arr->GetNumberOfTuples(); i++) {
    // maybe compute running mean to avoid overflow? mean = (mean+value)/(i+1)
    mean += arr->GetTuple1(i);
  }

  return mean / arr->GetNumberOfTuples();
}

double Mesh::getFieldStd(const std::string& name) const
{
  if (name.empty())
    throw std::invalid_argument("Provide name for field");

  if (mesh->GetPointData()->GetNumberOfArrays() < 1)
    throw std::invalid_argument("Mesh has no fields for which to compute mean.");

  auto arr = mesh->GetPointData()->GetArray(name.c_str());
  if (!arr)
    throw std::invalid_argument("Field does not exist.");

  double mean = getFieldMean(name);
  double squaredDiff{0.0};
  for (int i=0; i<arr->GetNumberOfTuples(); i++) {
    squaredDiff += pow(arr->GetTuple1(i) - mean, 2);
  }

  return sqrt(squaredDiff / arr->GetNumberOfTuples());
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
    Point p1(this->mesh->GetPoint(i));
    Point p2(other_mesh.mesh->GetPoint(i));
    if (!epsEqualN(p1, p2, 5)) {
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
  auto field1 = getField<vtkDataArray>(name1);
  auto field2 = other_mesh.getField<vtkDataArray>(name2.empty() ? name1 : name2);

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
        if (!equalNSigDigits(v1, v2, 5)) {
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
  if (!epsEqualN(center(), other.center(), 3))             { std::cerr << "centers differ!\n"; return false; }
  if (!epsEqualN(centerOfMass(), other.centerOfMass(), 3)) { std::cerr << "coms differ!\n"; return false; }
  if (numPoints() != other.numPoints())                    { std::cerr << "num pts differ\n"; return false; }
  if (numFaces() != other.numFaces())                      { std::cerr << "num faces differ\n"; return false; }
  if (!compareAllPoints(other))                            { std::cerr << "points differ\n"; return false; }
  if (!compareAllFaces(other))                             { std::cerr << "faces differ\n"; return false; }
  if (!compareAllFields(other, eps))                            { std::cerr << "fields differ\n"; return false; }

  return true;
}

MeshTransform Mesh::createRegistrationTransform(const Mesh &target, Mesh::AlignmentType align, unsigned iterations)
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

bool Mesh::splitMesh(std::vector<std::vector<Eigen::Vector3d> > boundaries, Eigen::Vector3d query,
                     size_t dom, size_t num)
{
  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  vtkSmartPointer<vtkPoints> points = getIGLMesh(V, F);

  for (size_t bound = 0; bound < boundaries.size(); bound++) {
    //std::cout << "Boundaries " << bound << " size " << boundaries[bound].size() << std::endl;

    // Creating cutting loop
    vtkPoints* selectionPoints = vtkPoints::New();
    vtkSmartPointer<vtkKdTreePointLocator> locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    locator->SetDataSet(this->mesh);
    std::vector<size_t> boundaryVerts;

    // Create path creator
    vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra =
      vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInputData(this->mesh);

    vtkIdType lastId = 0;
    for (size_t i = 0; i < boundaries[bound].size(); i++) {
      Eigen::Vector3d pt = boundaries[bound][i];
      double ptdob[3] = {pt[0], pt[1], pt[2]};
      vtkIdType ptid = locator->FindClosestPoint(ptdob);
      mesh->GetPoint(ptid, ptdob);

      // Add first point in boundary
      if (i == 0) {
        lastId = ptid;
        Point3 pathpt;
        pathpt = getPoint(ptid);
        selectionPoints->InsertNextPoint(pathpt[0], pathpt[1], pathpt[2]);
        boundaryVerts.push_back(ptid);
      }
      // If the current and last vetices are different, then add all vertices in the path to the boundaryVerts list
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

    vtkSmartPointer<vtkSelectPolyData> select = vtkSelectPolyData::New();
    select->SetLoop(selectionPoints);
    select->SetInputData(this->mesh);
    select->GenerateSelectionScalarsOn();
    select->SetSelectionModeToLargestRegion();

    // Clipping mesh
    vtkSmartPointer<vtkClipPolyData> selectclip = vtkClipPolyData::New();
    selectclip->SetInputConnection(select->GetOutputPort());
    selectclip->SetValue(0.0);

    selectclip->Update();

    MeshType halfmesh = selectclip->GetOutput();

    vtkSmartPointer<vtkDoubleArray> inout = computeInOutForFFCs(query, halfmesh);

    vtkSmartPointer<vtkDoubleArray> values = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> absvalues = setDistanceToBoundaryValueFieldForFFCs(values,
                                                                                       points,
                                                                                       boundaryVerts,
                                                                                       inout, V, F,
                                                                                       dom);

    this->mesh->GetPointData()->SetActiveScalars("value");

    std::vector<Eigen::Matrix3d> face_grad_ = this->setGradientFieldForFFCs(absvalues, V, F);

  } // Per boundary for loop end

  locator = vtkSmartPointer<vtkCellLocator>::New();
  locator->SetDataSet(this->mesh);
  locator->BuildLocator();

  // Write mesh for debug purposes
//    std::string fnin = "dev/mesh_" + std::to_string(dom) + "_" + std::to_string(num) + "_in.vtk";
//    this->write(fnin);

  return true;
}

// WARNING: Copied directly from Meshwrapper. TODO: When refactoring, take this into account.
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

double Mesh::getFFCValue(Eigen::Vector3d query){
  locator->BuildLocatorIfNeeded();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  locator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

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

Eigen::Vector3d Mesh::getFFCGradient(Eigen::Vector3d query){
  locator->BuildLocatorIfNeeded();

  double closestPoint[3];
  vtkIdType cellId;
  int subId;
  double dist;
  locator->FindClosestPoint(query.data(), closestPoint, cellId, subId, dist);

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
  auto arr = mesh->GetPointData()->GetArray("inout"); // Check if an inout already exists

  // Create half-mesh tree
  vtkSmartPointer<vtkKdTreePointLocator> kdhalf =
    vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdhalf->SetDataSet(halfmesh);
  kdhalf->BuildLocator();

  // Create full-mesh tree
  vtkSmartPointer<vtkKdTreePointLocator> kdmesh =
    vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdmesh->SetDataSet(this->mesh);
  kdmesh->BuildLocator();

  // Checking which mesh is closer to the query point. Recall that the query point must not necessarely lie on the mesh, so we check both the half mesh and the full mesh.
  double querypt[3] = {query[0], query[1], query[2]};

  vtkIdType halfi = kdhalf->FindClosestPoint(querypt);
  vtkIdType fulli = kdmesh->FindClosestPoint(querypt);

  double halfp[3];
  this->mesh->GetPoint(halfi, halfp);

  double fullp[3];
  halfmesh->GetPoint(fulli, fullp);

  if (halfp[0] != fullp[0] || halfp[1] != fullp[1] || halfp[2] != fullp[2])
    halfmeshisin = false; // If the closest point in halfmesh is not the closest point in fullmesh, then halfmesh is not the in mesh.

  vtkSmartPointer<vtkDoubleArray> inout = vtkSmartPointer<vtkDoubleArray>::New();
  inout->SetNumberOfComponents(1);
  inout->SetNumberOfTuples(this->mesh->GetNumberOfPoints());
  inout->SetName("inout");

  for (vtkIdType i = 0; i < this->mesh->GetNumberOfPoints(); i++) {
    this->mesh->GetPoint(i, fullp);

    halfi = kdhalf->FindClosestPoint(fullp);
    halfmesh->GetPoint(halfi, halfp);
    //std::cout << i <<  " (" << fullp[0] << " " << fullp[1] << " " << fullp[2] << ") " << halfi << " (" << halfp[0] << " " << halfp[1] << " " << halfp[2] << " )" << std::endl;
    bool ptinhalfmesh;
    if (fullp[0] == halfp[0] && fullp[1] == halfp[1] && fullp[2] == halfp[2]) {
      // If in halfmesh
      ptinhalfmesh = true;
    }
    else {
      // Else, not in half mesh
      ptinhalfmesh = false;
    }
    // The relationship becomes an xor operation between halfmeshisin and ptinhalfmesh to determine whether each point is in or out. Thus we set values for the scalar field.
    if (!halfmeshisin ^ ptinhalfmesh) {
      if (arr)
        inout->SetValue(i, std::min(1., this->getFieldValue("inout", i)));
      else
        inout->SetValue(i, 1.);
    }
    else {
      inout->SetValue(i, 0.);
    }
  }

  // Setting scalar field
  this->setField("inout", inout);

  return inout;
}

vtkSmartPointer<vtkDoubleArray>
Mesh::setDistanceToBoundaryValueFieldForFFCs(vtkSmartPointer<vtkDoubleArray> values,
                                             vtkSmartPointer<vtkPoints> points,
                                             std::vector<size_t> boundaryVerts,
                                             vtkSmartPointer<vtkDoubleArray> inout,
                                             Eigen::MatrixXd V, Eigen::MatrixXi F, size_t dom)
{

  auto arr = mesh->GetPointData()->GetArray("value"); // Check if a value field already exists

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
          if (inout->GetValue(i) == 0.) {
            if (arr)
              values->SetValue(i, std::max(this->getFieldValue("value", i), -distToSource[i]));
            else
              values->SetValue(i, -distToSource[i]);
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

  // Setting scalar field for value
  this->setField("value", values);

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
  std::vector<Eigen::Matrix3d> face_grad_;

  Eigen::MatrixXd grads(this->mesh->GetNumberOfPoints(), 3);
  grads.fill(0.);

  // Flatten the gradient operator so we can quickly compute the gradient at a given point
  face_grad_.resize(F.rows());
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
          face_grad_[f](axis, i) = val;
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
    const auto& G = face_grad_[i];
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

  this->setField("Gradient", vf);

  this->mesh->GetCellData()->AddArray(vff);

  return face_grad_;
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
  return *this;
}

} // shapeworks


