#include "Mesh.h"
#include <PreviewMeshQC/FEAreaCoverage.h>
#include <PreviewMeshQC/FEVTKImport.h>
#include <PreviewMeshQC/FEVTKExport.h>

#include <Libs/Utils/StringUtils.h>

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
#include <vtkCenterOfMass.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include <itkImageToVTKImageFilter.h>

static bool compare_double(double a, double b)
{
  const double EPSILON = 1e-1;
  return fabs(a - b) < EPSILON;
}

namespace shapeworks {

/// read
///
/// reads a mesh
///
/// \param filename
Mesh::MeshType Mesh::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  try {

    if (StringUtils::hasSuffix(pathname, "vtk")) {
      auto reader = vtkSmartPointer<vtkPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->SetReadAllScalars(1);
      reader->Update();
      vtkSmartPointer<vtkPolyData> poly_data = reader->GetOutput();
      if (poly_data->GetNumberOfPolys() < 1) {
        throw std::invalid_argument("Failed to read: " + pathname);
      }
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "vtp")) {
      auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "stl")) {
      auto reader = vtkSmartPointer<vtkSTLReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "obj")) {
      auto reader = vtkSmartPointer<vtkOBJReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    if (StringUtils::hasSuffix(pathname, "ply")) {
      auto reader = vtkSmartPointer<vtkPLYReader>::New();
      reader->SetFileName(pathname.c_str());
      reader->Update();
      return reader->GetOutput();
    }

    throw std::invalid_argument("Unsupported file type");
  }
  catch (const std::exception &exp)
  {
    throw std::invalid_argument("Failed to read: " + pathname);
  }
}

/// write
///
/// writes the mesh
///
/// \param filename
bool Mesh::write(const std::string &pathname)
{
  if (!this->mesh) { throw std::invalid_argument("Mesh invalid"); }
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  try {
    if (StringUtils::hasSuffix(pathname, "vtk")) {
      auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
    }

    if (StringUtils::hasSuffix(pathname, "vtp")) {
      auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
    }

    if (StringUtils::hasSuffix(pathname, "stl")) {
      auto writer = vtkSmartPointer<vtkSTLWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
    }

    if (StringUtils::hasSuffix(pathname, "obj")) {
      auto writer = vtkSmartPointer<vtkOBJWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
    }

    if (StringUtils::hasSuffix(pathname, "ply")) {
      auto writer = vtkSmartPointer<vtkPLYWriter>::New();
      writer->SetFileName(pathname.c_str());
      writer->SetInputData(this->mesh);
      writer->Update();
    }
  }
  catch (const std::exception &exp) {
    std::cerr << "Failed to write mesh to " << pathname << std::endl;
    return false;
  }
  return true;
}

/// creates mesh with scalars describing the "coverage" between two meshes
Mesh& Mesh::coverage(const Mesh &other_mesh, bool allow_back_intersections,
                     double angle_threshold, double back_search_radius)
{
  FEVTKimport importer;
  FEMesh* surf1 = importer.Load(this->mesh);
  FEMesh* surf2 = importer.Load(other_mesh.mesh);
  if (surf1 == nullptr || surf2 == nullptr) { throw std::invalid_argument("Mesh invalid"); }

  FEAreaCoverage areaCoverage;
  areaCoverage.AllowBackIntersection(allow_back_intersections);
  areaCoverage.SetAngleThreshold(angle_threshold);
  areaCoverage.SetBackSearchRadius(back_search_radius);

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

/// compare_points_equal
///
/// Compare if points in two meshes are equal
///
/// \param other_mesh
bool Mesh::compare_points_equal(const Mesh &other_mesh)
{
  if (!this->mesh || !other_mesh.mesh) {
    return false;
  }

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints()) {
    return false;
  }

  for (int i = 0; i < this->mesh->GetNumberOfPoints(); i++) {
    double* point1 = this->mesh->GetPoint(i);
    double* point2 = other_mesh.mesh->GetPoint(i);
    if (!compare_double(point1[0], point2[0])
        || !compare_double(point1[1], point2[1])
        || !compare_double(point1[2], point2[2])) {
      return false;
    }
  }
  return true;
}

/// compare_scalars_equal
///
/// Compare if scalars in two meshes are equal
///
/// \param other_mesh
bool Mesh::compare_scalars_equal(const Mesh &other_mesh)
{
  if (!this->mesh || !other_mesh.mesh) {
    std::cout << "Mesh Compare: both polydata don't exist";
    return false;
  }

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints()) {
    std::cout << "Mesh Compare: both polydata differ in number of points";
    return false;
  }

  vtkDataArray* scalars1 = this->mesh->GetPointData()->GetScalars();
  vtkDataArray* scalars2 = other_mesh.mesh->GetPointData()->GetScalars();

  if (!scalars1 || !scalars2) {
    std::cout << "Mesh Compare: no scalars";
    return false;
  }

  if (scalars1->GetNumberOfValues() != scalars2->GetNumberOfValues()) {
    std::cout << "Mesh Compare: different number of scalars";
    return false;
  }

  for (int i = 0; i < scalars1->GetNumberOfValues(); i++) {
    vtkVariant var1 = scalars1->GetVariantValue(i);
    vtkVariant var2 = scalars2->GetVariantValue(i);
    if (var1 != var2) {
      std::cout << "Mesh Compare: values differ: " << var1 << " != " << var2 << "\n";
      return false;
    }
  }

  return true;
}

std::ostream& operator<<(std::ostream &os, const Mesh& mesh)
{
  return os << "{\n\tnumber of vertices: " << mesh.numVertices()
            << ",\n\tnumber of faces: " << mesh.numFaces()
            << ",\n\tmin x: " << mesh.bounds()[0] << ",\n\tmax x: " << mesh.bounds()[1]
            << ",\n\tmin y: " << mesh.bounds()[2] << ",\n\tmax y: " << mesh.bounds()[3]
            << ",\n\tmin z: " << mesh.bounds()[4] << ",\n\tmax z: " << mesh.bounds()[5] << "\n}";
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

Mesh& Mesh::translate_mesh(const Vector3& v)
{
  auto translation = vtkSmartPointer<vtkTransform>::New();
  translation->Translate(v[0], v[1], v[2]);
  auto transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transform_filter->SetInputData(this->mesh);
  transform_filter->SetTransform(translation);
  transform_filter->Update();
  this->mesh = transform_filter->GetOutput();

  return *this;
}

Mesh::MeshType Mesh::get_poly_data()
{
  return this->mesh;
}

} // shapeworks
