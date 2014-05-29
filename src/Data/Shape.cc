#include <Data/Shape.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

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

  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    QMessageBox::warning( 0, "Unable to open file", "Error opening file: " + filename );
    return false;
  }

  QTextStream stream( &file );

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

  int num_points = 0;
  while ( !stream.atEnd() )
  {
    QString line = stream.readLine();
    QStringList list = line.split( ' ' );
    if ( list.size() != 4 ) // sanity check
    {
      std::cerr << "Error, line " << line.toStdString() << " does not contain 3 fields\n";
      std::cerr << "list.size = " << list.size() << "\n";
      return false;
    }

    float x = list[0].toFloat();
    float y = list[1].toFloat();
    float z = list[2].toFloat();

    points->InsertNextPoint( x, y, z );
    num_points++;
  }

  this->correspondence_points_.clear();
  this->correspondence_points_.set_size( num_points * 3 );

  int idx = 0;
  for ( int i = 0; i < num_points; i++ )
  {
    double* pos = points->GetPoint( i );
    this->correspondence_points_[idx++] = pos[0];
    this->correspondence_points_[idx++] = pos[1];
    this->correspondence_points_[idx++] = pos[2];
  }

  this->reconstructed_mesh_ = QSharedPointer<Mesh> ( new Mesh() );
  return this->reconstructed_mesh_->create_from_pointset( filename, this->correspondence_points_ );
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
  return this->reconstructed_mesh_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_correspondence_points()
{
  return this->correspondence_points_;
}
