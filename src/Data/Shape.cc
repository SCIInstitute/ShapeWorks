#include <Data/Shape.h>

Shape::Shape()
{}

Shape::~Shape()
{}

void Shape::import_initial_file( QString filename )
{
  this->initial_mesh_ = QSharedPointer<Mesh>( new Mesh( filename ) );
}

QSharedPointer<Mesh> Shape::get_initial_mesh()
{
  return this->initial_mesh_;
}
