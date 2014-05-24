#include <Data/Shape.h>

//---------------------------------------------------------------------------
Shape::Shape()
{}

//---------------------------------------------------------------------------
Shape::~Shape()
{}

//---------------------------------------------------------------------------
void Shape::import_initial_file( QString filename )
{
  this->initial_mesh_ = QSharedPointer<Mesh>( new Mesh() );
  this->initial_mesh_->create_from_image( filename );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_initial_mesh()
{
  return this->initial_mesh_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_file( QString filename )
{
  this->groomed_mesh_ = QSharedPointer<Mesh>( new Mesh() );
  this->groomed_mesh_->create_from_image( filename );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  return this->groomed_mesh_;
}

//---------------------------------------------------------------------------
bool Shape::import_point_file( QString filename )
{
  this->reconstructed_mesh_ = QSharedPointer<Mesh> ( new Mesh() );
  return this->reconstructed_mesh_->create_from_pointset( filename );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
  return this->reconstructed_mesh_;
}
