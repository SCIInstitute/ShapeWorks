#include <Data/MeshGroup.h>
#include <vtkAppendPolyData.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
MeshGroup::MeshGroup() {}

//---------------------------------------------------------------------------
MeshGroup::MeshGroup(unsigned long num_meshes) : meshes_{num_meshes} {}

//---------------------------------------------------------------------------
MeshGroup::~MeshGroup() {}

//---------------------------------------------------------------------------
MeshList& MeshGroup::meshes() { return meshes_; }

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshGroup::get_combined_poly_data()
{
  auto append = vtkSmartPointer<vtkAppendPolyData>::New();
  for (int domain = 0; domain < meshes_.size(); domain++) {
    append->AddInputData(meshes_[domain]->get_poly_data());
  }
  append->Update();
  vtkSmartPointer<vtkPolyData> combined = append->GetOutput();
  return combined;
}

//---------------------------------------------------------------------------
bool MeshGroup::valid() {
  for (auto& item : meshes_) {
    if (!item) {
      return false;
    }
  }
  return !this->meshes_.empty();
}

//---------------------------------------------------------------------------
void MeshGroup::set_number_of_meshes(int n) { meshes_.resize(n); }

//---------------------------------------------------------------------------
void MeshGroup::set_mesh(int i, MeshHandle mesh) {
  if (i >= meshes_.size()) {
    set_number_of_meshes(i + 1);
  }
  meshes_[i] = mesh;
}
