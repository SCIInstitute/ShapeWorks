#include "Mesh.h"
#include <PreviewMeshQC/FEAreaCoverage.h>
#include <PreviewMeshQC/FEVTKImport.h>
#include <PreviewMeshQC/FEVTKExport.h>

#include <vtkPolyDataReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPLYWriter.h>
#include <vtkPointData.h>
#include <vtkMarchingCubes.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkReverseSense.h>

static bool compare_double(double a, double b)
{
  const double EPSILON = 1e-1;
  return fabs(a - b) < EPSILON;
}

namespace shapeworks {

Mesh::MeshType Mesh::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  // TODO: enable reading of different kinds of meshes
  // if (pref == "ply")
  //   using ReaderType = vtkSmartPointer<vtkPLYReader>;
  // else 
  //   using ReaderType = vtkSmartPointer<vtkPolyDataReader>;

  using ReaderType = vtkSmartPointer<vtkPolyDataReader>;
  ReaderType reader = ReaderType::New();
  reader->SetFileName(pathname.c_str());

  try {
    reader->Update();
  }
  catch (const std::exception &exp)
  {
    throw std::invalid_argument(pathname + " does not exist");
  }

  return reader->GetOutput();
}

Mesh& Mesh::write(const std::string &pathname)
{
  if (!this->mesh) { throw std::invalid_argument("Mesh invalid"); }
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  // TODO: enable writing of different kinds of meshes
  // if (pref == "ply")
  //   using WriterType = vtkSmartPointer<vtkPLYWriter>;
  // else
  //   using WriterType = vtkSmartPointer<vtkPolyDataWriter>;

  using WriterType = vtkSmartPointer<vtkPolyDataWriter>;
  WriterType writer = WriterType::New();
  writer->SetInputData(this->mesh);
  writer->SetFileName(pathname.c_str());
  writer->Update();

  return *this;
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

Mesh& Mesh::march(double levelset)
{
  using FilterType = vtkSmartPointer<vtkMarchingCubes>;
  FilterType cube = FilterType::New();

  cube->SetInputData(this->mesh);
  cube->SetValue(0, levelset);
  cube->Update();
  this->mesh = cube->GetOutput();

  return *this;
}

Mesh &Mesh::smooth(int iterations, double relaxation)
{
  using FilterType = vtkSmartPointer<vtkSmoothPolyDataFilter>;
  FilterType smoother = FilterType::New();
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
  using FilterType = vtkSmartPointer<vtkDecimatePro>;
  FilterType decimator = FilterType::New();

  decimator->SetInputData(this->mesh);
  decimator->SetTargetReduction(reduction);
  decimator->SetFeatureAngle(angle);
  preservetopology == true ? decimator->PreserveTopologyOn() : decimator->PreserveTopologyOff();
  decimator->BoundaryVertexDeletionOn();
  decimator->Update();
  this->mesh = decimator->GetOutput();

  return *this;
}

Mesh &Mesh::reflect(const Vector3 &origin, const Axis &axis)
{
  Vector scale(makeVector({1,1,1}));
  scale[axis] = -1;

  vtkTransform transform(vtkTransform::New());
  transform->Translate(-origin[0], -origin[1], -origin[2]);
  transform->Scale(scale[0], scale[1], scale[2]);
  transform->Translate(origin[0], origin[1], origin[2]);

  // handle flipping normals under negative scaling
  using ReverseType = vtkSmartPointer<vtkReverseSense>;
  ReverseType reverseSense = ReverseType::New();
  reverseSense->SetInputData(this->mesh);
  reverseSense->ReverseNormalsOff();
  reverseSense->ReverseCellsOn();
  reverseSense->Update();
  this->mesh = reverseSense->GetOutput();

  return *this;
}

Mesh &Mesh::applyTransform(const vtkTransform transform)
{
  using FilterType = vtkSmartPointer<vtkTransformPolyDataFilter>;
  FilterType resampler = FilterType::New();

  resampler->SetTransform(transform);
  resampler->SetInputData(this->mesh);
  resampler->Update();
  this->mesh = resampler->GetOutput();

  return *this;
}

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
