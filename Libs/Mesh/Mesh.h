#pragma once

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>

namespace Shapeworks {

class Mesh
{
public:
  Mesh() {}
  Mesh(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &inFilename);
  bool write(const std::string &outFilename);
  bool smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/);

private:
  vtkSmartPointer<vtkPolyData> mesh;
};

} // Shapeworks
