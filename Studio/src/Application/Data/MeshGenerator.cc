/*
 * Shapeworks license
 */

#include <Data/MeshGenerator.h>
#include <limits>
#include <itkPoint.h>

//---------------------------------------------------------------------------
MeshGenerator::MeshGenerator(Preferences& prefs)
  : prefs_(prefs) {}

//---------------------------------------------------------------------------
MeshGenerator::~MeshGenerator() {}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::buildMesh(const vnl_vector<double>& shape)
{
  if (this->surface_reconstructor_ &&
      this->surface_reconstructor_->get_surface_reconstruction_avaiable()) {
    vtkSmartPointer<vtkPolyData> poly_data = this->surface_reconstructor_->build_mesh(shape);
    return poly_data;
  }
  return vtkPolyData::New();
}

//---------------------------------------------------------------------------
void MeshGenerator::set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor)
{
  this->surface_reconstructor_ = reconstructor;
}
