#include "Mesh.h"
#include <PreviewMeshQC/FEAreaCoverage.h>
#include <PreviewMeshQC/FEVTKImport.h>
#include <PreviewMeshQC/FEVTKExport.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>

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
bool Mesh::read(const std::string &pathname)
{
  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(pathname.c_str());
  reader->Update();
  this->mesh = vtkSmartPointer<vtkPolyData>::New();
  this->mesh->DeepCopy(reader->GetOutput());
  return true;
}

/// write
///
/// writes the mesh
///
/// \param filename
bool Mesh::write(const std::string &filename)
{
  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(this->mesh);
  writer->Update();
  return true;
}

/// coverage
///
/// Not really sure what this does... (TODO: add desc here and also add desc in ShapeworksPython.cpp)
///
/// \param mesh
bool Mesh::coverage(const Mesh &other_mesh)
{
  if (!this->mesh) {
    std::cerr << "No mesh loaded, so returning false." << std::endl;
    return false;
  }

  FEVTKimport importer;
  FEMesh* surf1 = importer.Load(this->mesh);
  FEMesh* surf2 = importer.Load(other_mesh.mesh);
  if (surf1 == nullptr || surf2 == nullptr) {
    std::cerr << "Error reading mesh\n";
    return false;
  }

  FEAreaCoverage areaCoverage;

  vector<double> map1 = areaCoverage.Apply(*surf1, *surf2);

  for (int i = 0; i < surf1->Nodes(); ++i) {
    surf1->Node(i).m_ndata = map1[i];
  }

  FEVTKExport vtkout;
  VTKEXPORT ops = { false, true };
  vtkout.SetOptions(ops);

  this->mesh = vtkout.ExportToVTK(*surf1);

  return true;
}

/// smooth
///
bool Mesh::smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/)
{
  return true;
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

} // shapeworks
