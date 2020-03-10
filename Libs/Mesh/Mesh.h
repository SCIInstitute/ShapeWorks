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

  bool read(const std::string &filename);
  bool write(const std::string &filename);

  bool coverage(const Mesh& other_mesh);
  bool smooth(/*iterations, relaxation_factor, edge_smoothing, boundary_smoothing*/);
  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

private:
  vtkSmartPointer<vtkPolyData> poly_data_;
};

} // shapeworks
