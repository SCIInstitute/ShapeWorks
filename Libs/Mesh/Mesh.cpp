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
bool Mesh::read(const std::string &inFilename){return true;}
// {
//   typedef itk::MeshFileReader<MeshType> ReaderType;

//   ReaderType::Pointer reader = ReaderType::New();
//   reader->SetFileName(inFilename);
//   try
//   {
//     reader->Update();
//   }
//   catch (itk::ExceptionObject &exp)
//   {
//     std::cerr << "Failed to read mesh " << inFilename << std::endl;
//     std::cerr << exp << std::endl;
//     return false;
//   }

//   this->mesh = reader->GetOutput();
//   return true;
// }

///////////////////////////////////////////////////////////////////////////////
bool Mesh::write(const std::string &outFilename){return true;}
// {
//   if (!this->mesh) return false;

//   typedef itk::MeshFileWriter<MeshType> WriterType;

//   WriterType::Pointer writer = WriterType::New();
//   writer->SetInput(this->mesh);
//   writer->SetFileName(outFilename);

//   //<ctc> todo: check for empty filename, and do something more sensible if an exception is thrown -- otherwise there's an infinite loop!
//   try
//   {
//     writer->Update();
//   }
//   catch (itk::ExceptionObject &exp)
//   {
//     std::cerr << "Failed to write mesh to " << outFilename << std::endl;
//     std::cerr << exp << std::endl;
//     return false;
//   }

//   return true;
// }

///////////////////////////////////////////////////////////////////////////////
bool Mesh::smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/) {return true;}
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
