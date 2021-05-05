#include "Mesh.h"
#include "MeshUtils.h"
#include "Image.h"
#include "StringUtils.h"
#include "MeshWarper.h"
#include "PreviewMeshQC/FEAreaCoverage.h"
#include "PreviewMeshQC/FEVTKImport.h"
#include "PreviewMeshQC/FEVTKExport.h"
#include "FEFixMesh.h"
#include "FEMeshSmoothingModifier.h"
#include "FECVDDecimationModifier.h"

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
#include <vtkMarchingCubes.h>
#include <vtkSmoothPolyDataFilter.h>
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
#include <igl/exact_geodesic.h>

namespace shapeworks {

Mesh::MeshType Mesh::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }
  if (!ShapeworksUtils::exists(pathname)) { throw std::invalid_argument("File doesn't exist"); }

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
      auto writer = vtkSmartPointer<vtkSTLWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".obj")) {
      auto writer = vtkSmartPointer<vtkOBJWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, ".ply")) {
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

MeshTransform Mesh::createTransform(const Mesh &target, XFormType type, Mesh::AlignmentType align, unsigned iterations)
{
  MeshTransform transform;

  switch (type) {
    case IterativeClosestPoint:
      transform = createRegistrationTransform(target, align, iterations);
      break;
    default:
      throw std::invalid_argument("Unknown Mesh::TranformType");
  }

  return transform;
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
  generateNormals();

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

Region Mesh::boundingBox(bool center) const
{
  Region bbox;
  double bb[6];
  mesh->GetBounds(bb);

  for(int i = 0; i < 3; i++)
  {
    bbox.min[i] = floor(bb[2*i]);
    bbox.max[i] = ceil(bb[2*i+1]);
  }

  if (center)
  {
    Dims size = bbox.size();
    Dims offset = size * -0.5 - toDims(bbox.min);
    bbox.min = toCoord(size * -0.5);
    bbox.max = toCoord(size * 0.5);
  }

  return bbox;
}

Mesh& Mesh::distance(const Mesh &target, const DistanceMethod method)
{
  if (target.numPoints() == 0 || numPoints() == 0)
    throw std::invalid_argument("meshes must have points");

  vtkSmartPointer<vtkKdTreePointLocator> targetPointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();

  if (method == POINT_TO_POINT)
  {
    targetPointLocator->SetDataSet(target.mesh);
    targetPointLocator->BuildLocator();
  }

  // allocate Array to store distances from each point to target
  vtkSmartPointer<vtkDoubleArray> distance = vtkSmartPointer<vtkDoubleArray>::New();
  distance->SetNumberOfComponents(1);
  distance->SetNumberOfTuples(numPoints());
  distance->SetName("distance");

  // Find the nearest neighbors to each point and compute distance between them
  double dist, currentPoint[3], closestPoint[3];
  if (method == POINT_TO_POINT)
  {
    for (int i = 0; i < numPoints(); i++)
    {
      mesh->GetPoint(i, currentPoint);
      vtkIdType closestPointId = targetPointLocator->FindClosestPoint(currentPoint);
      target.mesh->GetPoint(closestPointId, closestPoint);
      dist = std::sqrt(std::pow(currentPoint[0] - closestPoint[0], 2) +
                       std::pow(currentPoint[1] - closestPoint[1], 2) +
                       std::pow(currentPoint[2] - closestPoint[2], 2));
      distance->SetValue(i, dist);
    }
  }
  else
  {
    for (int i = 0; i < numPoints(); i++)
    {
      mesh->GetPoint(i, currentPoint);
      dist = projectPoint(target, currentPoint);
      distance->SetValue(i, std::sqrt(dist));
    }
  }

  // add distance field to this mesh
  this->setField("distance", distance);

  return *this;
}

double Mesh::projectPoint(const Mesh &target, Point point)
{
  vtkSmartPointer<vtkCellLocator> targetCellLocator = vtkSmartPointer<vtkCellLocator>::New();
  targetCellLocator->SetDataSet(target.mesh);
  targetCellLocator->BuildLocator();

  double dist, closestPoint[3];
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  vtkIdType cellId;
  int subId;

  targetCellLocator->FindClosestPoint(point.GetDataPointer(), closestPoint, cell, cellId, subId, dist);
  return dist;
}

double Mesh::projectPoint(Point point)
{
  return projectPoint(*this, point);
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

Mesh& Mesh::generateNormals()
{
  vtkSmartPointer<vtkPolyDataNormals> normal = vtkSmartPointer<vtkPolyDataNormals>::New();

  normal->SetInputData(this->mesh);
  normal->ComputeCellNormalsOn();
  normal->AutoOrientNormalsOn();
  normal->SplittingOff();
  normal->Update();
  this->mesh = normal->GetOutput();

  return *this;
}

Image Mesh::toImage(Vector3 spacing, Dims size, Point3 origin) const
{
  if (std::abs(spacing[0]) < 1E-4 || std::abs(spacing[1]) < 1E-4 || std::abs(spacing[2]) < 1E-4)
  {
    throw std::invalid_argument("error: rasterization spacing must be non-zero");
  }

  if (size != Dims({0, 0, 0}) && spacing != makeVector({1.0, 1.0, 1.0}))
  {
    throw std::invalid_argument("error: cannot specify both size and spacing. Instead, scale the Mesh by spacing first.");
  }

  // identify the logical region containing the mesh
  Region bbox(boundingBox());
  bbox.pad(1); // give it some padding

  // compute dims based on specified spacing...
  if (size == Dims({0, 0, 0}))
  {
    bbox.pad(1); // give it some more padding, why not
    Vector3 sz = toVector(bbox.size());
    sz[0] /= spacing[0];
    sz[1] /= spacing[1];
    sz[2] /= spacing[2];
    size[0] = ceil(sz[0]);
    size[1] = ceil(sz[1]);
    size[2] = ceil(sz[2]);
  }
  // ...or spacing based on specified dims
  else
  {
    Vector3 sz_user = toVector(size);
    Vector3 sz_mesh = toVector(bbox.size());
    spacing[0] = sz_mesh[0] / sz_user[0];
    spacing[1] = sz_mesh[1] / sz_user[1];
    spacing[2] = sz_mesh[2] / sz_user[2];
  }

  // determine origin from bounding box if user didn't explicitly specify
  if (origin == Point3({-1.0, -1.0, -1.0}))
  {
    origin = toPoint(bbox.min);
  }

  vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();
  whiteImage->SetSpacing(spacing[0], spacing[1], spacing[2]);
  whiteImage->SetDimensions(size[0], size[1], size[2]);
  whiteImage->SetExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
  whiteImage->SetOrigin(origin[0], origin[1], origin[2]);
  whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
    whiteImage->GetPointData()->GetScalars()->SetTuple1(i, 1);

  // polygonal data --> image stencil:
  vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  pol2stenc->SetInputData(this->mesh);
  pol2stenc->SetOutputOrigin(origin[0], origin[1], origin[2]);
  pol2stenc->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
  pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
  pol2stenc->Update();

  // cut the corresponding white image and set the background:
  vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
  imgstenc->SetInputData(whiteImage);
  imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
  imgstenc->ReverseStencilOff();
  imgstenc->SetBackgroundValue(0);
  imgstenc->Update();

  return Image(imgstenc->GetOutput());
}

Image Mesh::toDistanceTransform(Vector3 spacing, Dims size, Point3 origin) const
{
  // TODO: convert directly to DT (github #810)
  Image image(toImage(spacing, size, origin));
  image.antialias(50, 0.00).computeDT(); // need maxrms = 0 and iterations = 30 to reproduce results
  return image;
}

Mesh& Mesh::fix(bool smoothBefore, bool smoothAfter, double lambda, int iterations, bool decimate, double percentage)
{
	FEVTKimport import;
  FEMesh* meshFE = import.Load(this->mesh);

	if (meshFE == 0) { throw std::invalid_argument("Unable to read file"); }

	FEFixMesh fix;
  FEMesh* meshFix;

  meshFix = fix.FixElementWinding(meshFE);

  if (smoothBefore)
  {
    FEMeshSmoothingModifier lap;
    lap.m_threshold1 = lambda;
    lap.m_iteration = iterations;
    meshFix = lap.Apply(meshFix);
  }

  if (decimate)
  {
    FECVDDecimationModifier cvd;
    cvd.m_pct = percentage;
    cvd.m_gradient = 1;
    meshFix = cvd.Apply(meshFix);

    if (smoothAfter)
    {
      FEMeshSmoothingModifier lap;
      lap.m_threshold1 = lambda;
      lap.m_iteration = iterations;
      meshFix = lap.Apply(meshFix);
    }
  }

	FEVTKExport vtkOut;
  this->mesh = vtkOut.ExportToVTK(*meshFix);

  return *this;
}

double Mesh::geodesicDistance(int source, int target)
{
  Eigen::MatrixXd V = MeshWarper::distill_vertex_info(this->mesh);
	Eigen::MatrixXi F = MeshWarper::distill_face_info(this->mesh);
  Eigen::VectorXd VS(1), VT(1);
  VS[0] = source;
  VT[0] = target;
  Eigen::VectorXd FS, FT;
  Eigen::VectorXd d;

  igl::exact_geodesic(V,F,VS,FS,VT,FT,d);
  return d[0];
}

Mesh& Mesh::setField(std::string name, Array array)
{
  if (!array)
    throw std::invalid_argument("Invalid array.");

  if (name.empty())
    throw std::invalid_argument("Provide name for the new field");

  int numVertices = mesh->GetPoints()->GetNumberOfPoints();
  if (array->GetNumberOfTuples() != numVertices) {
    std::cerr << "WARNING: Added a mesh field with a different number of elements than points\n";
  }
  if (array->GetNumberOfComponents() != 1) {
    std::cerr << "WARNING: Added a multi-component mesh field\n";
  }

  array->SetName(name.c_str());
  mesh->GetPointData()->AddArray(array);

  return *this;
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

bool Mesh::compareAllPoints(const Mesh &other_mesh) const
{
  if (!this->mesh || !other_mesh.mesh)
    throw std::invalid_argument("invalid meshes");

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints())
  {
    std::cerr << "meshes differ in number of points";
    return false;
  }

  for (int i = 0; i < this->mesh->GetNumberOfPoints(); i++)
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

bool Mesh::compareAllFields(const Mesh &other_mesh) const
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
    if (!compareField(other_mesh, field)) {
      std::cerr << field << " fields are not the same\n";
      return false;
    }
  }

  return true;
}

bool Mesh::compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2) const
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
      if (!equalNSigDigits(v1, v2, 5)) {
        printf("%ith values not equal (%0.8f != %0.8f)\n", i, v1, v2);
        return false;
      }
    }
  }

  return true;
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

Point3 Mesh::center() const
{
  double c[3];
  mesh->GetCenter(c);
  return Point3({c[0], c[1], c[2]});
}

Point3 Mesh::getPoint(int p) const
{
  double val[3];
  mesh->GetPoint(p, val);
  return Point3({val[0], val[1], val[2]});
}

bool Mesh::compare(const Mesh& other) const
{
  if (!epsEqualN(center(), other.center(), 3))             { std::cerr << "centers differ!\n"; return false; }
  if (!epsEqualN(centerOfMass(), other.centerOfMass(), 3)) { std::cerr << "coms differ!\n"; return false; }
  if (numPoints() != other.numPoints())                    { std::cerr << "num pts differ\n"; return false; }
  if (numFaces() != other.numFaces())                      { std::cerr << "num faces differ\n"; return false; }
  if (!compareAllPoints(other))                            { std::cerr << "points differ\n"; return false; }
  if (!compareAllFaces(other))                             { std::cerr << "faces differ\n"; return false; }
  if (!compareAllFields(other))                            { std::cerr << "fields differ\n"; return false; }

  return true;
}

MeshTransform Mesh::createRegistrationTransform(const Mesh &target, Mesh::AlignmentType align, unsigned iterations)
{
  const vtkSmartPointer<vtkMatrix4x4> mat(MeshUtils::createICPTransform(this->mesh, target.getVTKMesh(), align, iterations, true));
  return createMeshTransform(mat);
}

std::ostream& operator<<(std::ostream &os, const Mesh& mesh)
{
  os << "{\n\tnumber of points: " << mesh.numPoints()
     << ",\n\tnumber of faces: " << mesh.numFaces()
     << ",\n\tcenter: " << mesh.center()
     << ",\n\tcenter or mass: " << mesh.centerOfMass()
     << ",\n\tfield names: \n";

  auto fields = mesh.getFieldNames();
  for (auto field: fields) {
    os << "\t\t" << field << std::endl;
  }

  return os;
}

} // shapeworks
