/*
 * Shapeworks license
 */

#include <Data/MeshGenerator.h>
#include <limits>
#include <itkPoint.h>

//---------------------------------------------------------------------------
MeshGenerator::MeshGenerator(Preferences& prefs)
  : prefs_(prefs),
  legacy_reconstructor_(new LegacyMeshGenerator())
{}

//---------------------------------------------------------------------------
MeshGenerator::~MeshGenerator() {}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGenerator::buildMesh(const vnl_vector<double>& shape)
{
  if (this->surface_reconstructor_ &&
      this->surface_reconstructor_->get_surface_reconstruction_available()) {
    vtkSmartPointer<vtkPolyData> poly_data = this->surface_reconstructor_->build_mesh(shape);
    return poly_data;
  }
  else {
    return this->legacy_reconstructor_->buildMesh(shape);
  }
  return vtkPolyData::New();
}

//---------------------------------------------------------------------------
void MeshGenerator::set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor)
{
  this->surface_reconstructor_ = reconstructor;
}
