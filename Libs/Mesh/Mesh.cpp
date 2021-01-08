#include "Mesh.h"
#include "Image.h"
#include "StringUtils.h"
#include <PreviewMeshQC/FEAreaCoverage.h>
#include <PreviewMeshQC/FEVTKImport.h>
#include <PreviewMeshQC/FEVTKExport.h>

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
#include <vtkPointData.h>
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

namespace shapeworks {

Mesh::MeshType Mesh::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  try {
    if (StringUtils::hasSuffix(pathname, "vtk"))
    {
      auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->SetReadAllScalars(1);
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "vtp"))
    {
      auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "stl"))
    {
      auto reader = vtkSmartPointer<vtkSTLReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "obj"))
    {
      auto reader = vtkSmartPointer<vtkOBJReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "ply"))
    {
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
    if (StringUtils::hasSuffix(pathname, "vtk"))
    {
      auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, "vtp"))
    {
      auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, "stl"))
    {
      auto writer = vtkSmartPointer<vtkSTLWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, "obj"))
    {
      auto writer = vtkSmartPointer<vtkOBJWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
      return *this;
    }

    if (StringUtils::hasSuffix(pathname, "ply"))
    {
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

Mesh& Mesh::coverage(const Mesh &other_mesh)
{
  FEVTKimport importer;
  FEMesh* surf1 = importer.Load(this->mesh);
  FEMesh* surf2 = importer.Load(other_mesh.mesh);
  if (surf1 == nullptr || surf2 == nullptr) { throw std::invalid_argument("Mesh invalid"); }

  FEAreaCoverage areaCoverage;

  vector<double> map1 = areaCoverage.Apply(*surf1, *surf2);

  for (int i = 0; i < surf1->Nodes(); ++i) {
    surf1->Node(i).m_ndata = map1[i];
  }

  FEVTKExport vtkout;
  VTKEXPORT ops = { false, true };
  vtkout.SetOptions(ops);

  this->mesh = vtkout.ExportToVTK(*surf1);

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

Mesh &Mesh::decimate(double reduction, double angle, bool preservetopology)
{
  vtkSmartPointer<vtkDecimatePro> decimator = vtkSmartPointer<vtkDecimatePro>::New();

  decimator->SetInputData(this->mesh);
  decimator->SetTargetReduction(reduction);
  decimator->SetFeatureAngle(angle);
  preservetopology == true ? decimator->PreserveTopologyOn() : decimator->PreserveTopologyOff();
  decimator->BoundaryVertexDeletionOn();
  decimator->Update();
  this->mesh = decimator->GetOutput();

  return *this;
}

Mesh &Mesh::reflect(const Axis &axis, const Vector3 &origin)
{
  Vector scale(makeVector({1,1,1}));
  scale[axis] = -1;

  vtkTransform transform(vtkTransform::New());
  transform->Translate(-origin[0], -origin[1], -origin[2]);
  transform->Scale(scale[0], scale[1], scale[2]);
  transform->Translate(origin[0], origin[1], origin[2]);

  // handle flipping normals under negative scaling
  vtkSmartPointer<vtkReverseSense> reverseSense = vtkSmartPointer<vtkReverseSense>::New();
  reverseSense->SetInputData(this->mesh);
  reverseSense->ReverseNormalsOff();
  reverseSense->ReverseCellsOn();
  reverseSense->Update();
  this->mesh = reverseSense->GetOutput();

  return applyTransform(transform);
}

Mesh &Mesh::applyTransform(const vtkTransform transform)
{
  vtkSmartPointer<vtkTransformPolyDataFilter> resampler = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

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

  // Make the triangle window order consistent
  vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputConnection(filter->GetOutputPort());
  normals->ConsistencyOn();
  normals->SplittingOff();
  normals->Update();

  // Restore the original normals
  normals->GetOutput()->GetPointData()->SetNormals(mesh->GetPointData()->GetNormals());
  this->mesh = normals->GetOutput();

  return *this;
}

Mesh &Mesh::probeVolume(const Image &img)
{
  vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
  probeFilter->SetInputData(this->mesh);
  probeFilter->SetSourceData(ImageUtils::getVTK(img));
  probeFilter->Update();

  this->mesh = probeFilter->GetPolyDataOutput();
  return *this;
}

Mesh &Mesh::clip(const vtkSmartPointer<vtkPlane> plane)
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
  vtkTransform transform(vtkTransform::New());
  transform->Translate(v[0], v[1], v[2]);

  return applyTransform(transform);
}

Mesh &Mesh::scale(const Vector3 &v)
{
  vtkTransform transform(vtkTransform::New());
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

vtkSmartPointer<swHausdorffDistancePointSetFilter> Mesh::computeDistance(const Mesh &other_mesh, bool target)
{
  vtkSmartPointer<swHausdorffDistancePointSetFilter> filter = vtkSmartPointer<swHausdorffDistancePointSetFilter>::New();
  filter->SetInputData(this->mesh);
  filter->SetInputData(1, other_mesh.mesh);

  if (target)
    filter->SetTargetDistanceMethod(1);
  
  filter->Update();
  return filter;
}

double Mesh::hausdorffDistance(const Mesh &other_mesh, bool target)
{
  vtkSmartPointer<swHausdorffDistancePointSetFilter> filter = computeDistance(other_mesh, target);
  return filter->GetOutput(0)->GetFieldData()->GetArray("HausdorffDistance")->GetComponent(0,0);
}

double Mesh::relativeDistanceAtoB(const Mesh &other_mesh, bool target)
{
  vtkSmartPointer<swHausdorffDistancePointSetFilter> filter = computeDistance(other_mesh, target);
  return filter->GetOutputDataObject(0)->GetFieldData()->GetArray("RelativeDistanceAtoB")->GetComponent(0,0);
}

double Mesh::relativeDistanceBtoA(const Mesh &other_mesh, bool target)
{
  vtkSmartPointer<swHausdorffDistancePointSetFilter> filter = computeDistance(other_mesh, target);
  return filter->GetOutputDataObject(1)->GetFieldData()->GetArray("RelativeDistanceBtoA")->GetComponent(0,0);
}

Point3 Mesh::rasterizationOrigin(Region region, Vector3 spacing, int padding)
{
  Point3 origin;

  for (int i = 0; i < 3; i++)
  {
    region.min[i] -= padding * spacing[i];
    origin[i] = region.min[i] - 1;
  }

  return origin;
}

Dims Mesh::rasterizationSize(Region region, Vector3 spacing, int padding)
{
  Dims size;
  Point3 origin = rasterizationOrigin(region, spacing, padding);
  Coord offset = toCoord(origin/toPoint(spacing));

  for (int i = 0; i < 3; i++)
  {
    region.max[i] += padding * spacing[i];
    size[i] = ceil(region.max[i] - offset[i]) + 1;
  }

  return size;
}

Image Mesh::toImage(Vector3 spacing, Dims size, Point3 origin)
{
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

  // vtk image to itk img
  Region region(size);

  Image::ImageType::Pointer itkImg;
  itkImg->SetRegions(Image::ImageType::RegionType(region.origin(), region.size()));
  itkImg->SetOrigin(origin);
  itkImg->SetSpacing(spacing);
  itkImg->Allocate();

  Image::ImageType::IndexType index;
  int pixel;

  for (index[0] = 0; index[0] < size[0]; index[0]++)
  {
    for (index[1] = 0; index[1] < size[1]; index[1]++)
    {
      for (index[2] = 0; index[2] < size[2]; index[2]++)
      {
        pixel = imgstenc->GetOutput()->GetScalarComponentAsFloat(index[0], index[1], index[2], 0);
        itkImg->SetPixel(index, pixel);
      }
    }
  }

  return itkImg;
}

Image Mesh::toDistanceTransform(Vector3 spacing, Dims size, Point3 origin)
{
  Image img(toImage(spacing, size, origin));
  return img.antialias(30, 0.0).computeDT();
}

bool Mesh::compare_points_equal(const Mesh &other_mesh) const
{
  if (!this->mesh || !other_mesh.mesh)
  {
    std::cout << "both polydata don't exist";
    return false;
  }

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints())
  {
    std::cout << "both polydata differ in number of points";
    return false;
  }

  for (int i = 0; i < this->mesh->GetNumberOfPoints(); i++) 
  {
    double* point1 = this->mesh->GetPoint(i);
    double* point2 = other_mesh.mesh->GetPoint(i);
    if (!compare_double(point1[0], point2[0]) || !compare_double(point1[1], point2[1]) || !compare_double(point1[2], point2[2]))
      return false;
  }

  return true;
}

bool Mesh::compare_scalars_equal(const Mesh &other_mesh) const
{
  if (!this->mesh || !other_mesh.mesh)
  {
    std::cout << "both polydata don't exist";
    return false;
  }

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints())
  {
    std::cout << "both polydata differ in number of points";
    return false;
  }

  vtkDataArray* scalars1 = this->mesh->GetPointData()->GetScalars();
  vtkDataArray* scalars2 = other_mesh.mesh->GetPointData()->GetScalars();

  if (!scalars1 || !scalars2)
  {
    std::cout << "no scalars";
    return false;
  }

  if (scalars1->GetNumberOfValues() != scalars2->GetNumberOfValues())
  {
    std::cout << "different number of scalars";
    return false;
  }

  for (int i = 0; i < scalars1->GetNumberOfValues(); i++)
  {
    vtkVariant var1 = scalars1->GetVariantValue(i);
    vtkVariant var2 = scalars2->GetVariantValue(i);
    if (var1 != var2)
    {
      std::cout << "values differ: " << var1 << " != " << var2 << "\n";
      return false;
    }
  }

  return true;
}

// TODO: does this work?
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

std::ostream& operator<<(std::ostream &os, const Mesh& mesh)
{
  return os << "{\n\tnumber of vertices: " << mesh.numVertices()
            << ",\n\tnumber of faces: " << mesh.numFaces() << "\n}";
}

} // shapeworks
