#include "Mesh.h"

//#include <vtkVersion.h>
//#include <vtkPointData.h>
//#include <vtkPolyDataNormals.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
//#include <vtkSmoothPolyDataFilter.h>

namespace shapeworks {

//<ctc> TODO: mesh

///////////////////////////////////////////////////////////////////////////////
bool Mesh::read(const std::string &inFilename)
{
  vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
  reader->SetFileName(inFilename.c_str());
  reader->Update();
  this->mesh->DeepCopy(reader->GetOutput());
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::write(const std::string &outFilename)
{

  vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
  writer->SetFileName(outFilename);
  writer->SetInputData(this->mesh);
  writer->Update();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Mesh::smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/)
{
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
