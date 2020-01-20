#pragma once

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  Mesh() {}
  Mesh(const std::string &inFilename) { read(inFilename); }


  /// read
  /// \param filename
  bool read(const std::string &inFilename);

  /// write
  /// \param filename
  bool write(const std::string &outFilename);

  bool smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/);
  // bool nextfunction(...);

private:
  vtkSmartPointer<vtkPolyData> mesh;
};

} // shapeworks
