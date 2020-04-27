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

  bool write(const std::string &pathname);

  bool coverage(const Mesh& other_mesh);
  bool smooth(unsigned iterations = 1);
  bool decimate(float reduction = 0.01, float angle = 30, bool preservetopology = false);

  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

private:
  MeshType mesh;

  static bool read(const std::string &pathname);
};

} // shapeworks
