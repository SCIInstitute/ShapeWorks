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

