/*
 * Shapeworks license
 */

#include <Data/MeshGenerator.h>
#include <limits>
#include <itkPoint.h>

MeshGenerator::MeshGenerator(Preferences& prefs, 
  Reconstruction& construct) : prefs_(prefs), construct_(construct) {}

MeshGenerator::~MeshGenerator() {}

vtkSmartPointer<vtkPolyData> MeshGenerator::buildMesh( 
  const vnl_vector<double>& shape ) {
  std::vector<itk::Point<float > > pts;
  for (size_t i = 0; i < shape.size(); i += 3) {
    float arr[] = {static_cast<float>(shape[i]),
      static_cast<float>(shape[i + 1]),
      static_cast<float>(shape[i + 2]) };
    itk::Point<float> pt(arr);
    pts.push_back(pt);
  }
  return this->construct_.getMesh(pts);
}
