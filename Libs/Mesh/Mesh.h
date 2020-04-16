#pragma once

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>;

  Mesh() {}
  Mesh(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &pathname);
  bool write(const std::string &pathname);

  bool coverage(const Mesh& other_mesh);
  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

private:
  MeshType mesh;
};

} // shapeworks
