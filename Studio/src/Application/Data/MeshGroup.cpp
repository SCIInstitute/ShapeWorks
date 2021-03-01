#include <Data/MeshGroup.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
MeshGroup::MeshGroup()
{
}

//---------------------------------------------------------------------------
MeshGroup::MeshGroup(unsigned long num_meshes) : meshes_{num_meshes}
{
}

//---------------------------------------------------------------------------
MeshGroup::~MeshGroup()
{
}

//---------------------------------------------------------------------------
MeshList& MeshGroup::meshes()
{
  return this->meshes_;
}

//---------------------------------------------------------------------------
bool MeshGroup::valid()
{
  int i = 0;
  for (auto&& item : meshes_) {
    if (!item) {
      return false;
    }
  }
  return true;
}

//---------------------------------------------------------------------------
void MeshGroup::set_number_of_meshes(int n)
{
  this->meshes_.resize(n);
}

//---------------------------------------------------------------------------
void MeshGroup::set_mesh(int i, MeshHandle mesh)
{
  if (i >= this->meshes_.size()) {
    this->set_number_of_meshes(i + 1);
  }
  this->meshes_[i] = mesh;
}

