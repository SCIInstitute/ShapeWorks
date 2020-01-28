#include "Mesh.h"
#include <PreviewMeshQC/FEAreaCoverage.h>
#include <PreviewMeshQC/FEVTKImport.h>
#include <PreviewMeshQC/FEVTKExport.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPointData.h>

//#include <vtkVersion.h>
//#include <vtkPointData.h>
//#include <vtkPolyDataNormals.h>
//#include <vtkSmoothPolyDataFilter.h>

static bool compare_double(double a, double b)
{
  const double EPSILON = 1e-6;
  return fabs(a - b) < EPSILON;
}

///////////////////////////////////////////////////////////////////////////////
namespace shapeworks {

//<ctc> TODO: mesh

///////////////////////////////////////////////////////////////////////////////
bool Mesh::read(const std::string &inFilename)
{
  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(inFilename.c_str());
  reader->Update();
  this->mesh = vtkSmartPointer<vtkPolyData>::New();
  this->mesh->DeepCopy(reader->GetOutput());
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::write(const std::string &outFilename)
{
  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetFileName(outFilename.c_str());
  writer->SetInputData(this->mesh);
  writer->Update();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
bool Mesh::smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/)
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
bool Mesh::compare_scalars_equal(const Mesh &other_mesh)
{
  if (!this->mesh || !other_mesh.mesh) {
    return false;
  }

  if (this->mesh->GetNumberOfPoints() != other_mesh.mesh->GetNumberOfPoints()) {
    return false;
  }

  vtkDataArray* scalars1 = this->mesh->GetPointData()->GetScalars();
  vtkDataArray* scalars2 = other_mesh.mesh->GetPointData()->GetScalars();

  if (!scalars1 || !scalars2) {
    return false;
  }

  if (scalars1->GetNumberOfValues() != scalars2->GetNumberOfValues()) {
    return false;
  }

  for (int i = 0; i < scalars1->GetNumberOfValues(); i++) {
    vtkVariant var1 = scalars1->GetVariantValue(i);
    vtkVariant var2 = scalars2->GetVariantValue(i);
    if (var1 != var2) {
      return false;
    }
  }

  return true;
}

// bool Mesh::antialias(float maxRMSErr, int numIter)
// {
//   if (!this->mesh) return false;

//   typedef itk::AntiAliasBinaryMeshFilter<MeshType, MeshType> FilterType;
//   FilterType::Pointer antialiasFilter = FilterType::New();
//   antialiasFilter->SetInput(this->mesh);
//   antialiasFilter->SetMaximumRMSError(maxRMSErr);
//   antialiasFilter->SetNumberOfIterations(numIter);
//   //antialiasFilter->SetNumberOfLayers(numLayers);  // TODO: should we specify this parameters?
//   try
//   {
//     antialiasFilter->Update();
//   }
//   catch (itk::ExceptionObject &exp)
//   {
//     std::cerr << "Antialias filter failed:" << std::endl;
//     std::cerr << exp << std::endl;
//     return false;
//   }

//   this->mesh = antialiasFilter->GetOutput();
//   return true;
// }
} // shapeworks
