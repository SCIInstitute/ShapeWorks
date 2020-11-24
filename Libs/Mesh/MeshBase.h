#pragma once

#include "Shapeworks.h"

#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPolyDataReader.h>

namespace shapeworks {

class MeshBase
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>;
  MeshBase();
  ~MeshBase();
  MeshType read() {};
};

class PolyData: public MeshBase
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>;
  PolyData();
  MeshType read(const std::string &pathname)
  {
    if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

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
};

class ImageData: public MeshBase
{
public:
  using MeshType = vtkSmartPointer<vtkImageData>;
  ImageData();
private:
//   std::unique_ptr<ITKMesh> mesh;
};

}