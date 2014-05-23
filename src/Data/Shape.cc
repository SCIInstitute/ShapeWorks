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
  this->initial_mesh_ = QSharedPointer<Mesh>( new Mesh( filename ) );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_initial_mesh()
{
  return this->initial_mesh_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_file( QString filename )
{
  this->groomed_mesh_ = QSharedPointer<Mesh>( new Mesh( filename ) );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  return this->groomed_mesh_;
}

//---------------------------------------------------------------------------
void Shape::import_point_file( QString filename )
{

}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
  return this->groomed_mesh_;
}
