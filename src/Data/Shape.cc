#include <Data/Shape.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

//---------------------------------------------------------------------------
Shape::Shape()
{
  this->id_ = 0;
}

//---------------------------------------------------------------------------
Shape::~Shape()
{}

//---------------------------------------------------------------------------
void Shape::import_original_image( QString filename, float iso_value )
{
  this->original_mesh_ = QSharedPointer<Mesh>( new Mesh() );
  this->original_mesh_->create_from_image( filename, iso_value );
  this->original_mesh_filename_ = filename;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_original_mesh()
{
  return this->original_mesh_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_image( QString filename )
{
  this->groomed_mesh_ = QSharedPointer<Mesh>( new Mesh() );
  this->groomed_mesh_->create_from_image( filename, 0.0 );
  this->groomed_mesh_filename_ = filename;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  return this->groomed_mesh_;
}

//---------------------------------------------------------------------------
bool Shape::import_global_point_file( QString filename )
{

  if ( !Shape::import_point_file( filename, this->global_correspondence_points_ ) )
  {
    return false;
  }

  this->reconstructed_mesh_ = QSharedPointer<Mesh> ( new Mesh() );
  this->global_point_filename_ = filename;

  return this->reconstructed_mesh_->create_from_pointset( this->global_correspondence_points_ );
}

//---------------------------------------------------------------------------
bool Shape::import_local_point_file( QString filename )
{
  if ( !Shape::import_point_file( filename, this->local_correspondence_points_ ) )
  {
    return false;
  }

  this->local_point_filename_ = filename;
  return true;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
  return this->reconstructed_mesh_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_global_correspondence_points()
{
  return this->global_correspondence_points_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_local_correspondence_points()
{
  return this->local_correspondence_points_;
}

//---------------------------------------------------------------------------
int Shape::get_id()
{
  return this->id_;
}

//---------------------------------------------------------------------------
void Shape::set_id( int id )
{
  this->id_ = id;
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename()
{
  QFileInfo qfi( this->original_mesh_filename_ );
  return qfi.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename_with_path()
{
  return this->original_mesh_filename_;
}
//---------------------------------------------------------------------------
QString Shape::get_groomed_filename()
{
  QFileInfo qfi( this->groomed_mesh_filename_ );
  return qfi.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename_with_path()
{
  return this->groomed_mesh_filename_;
}

//---------------------------------------------------------------------------
QString Shape::get_global_point_filename()
{
  return QFileInfo( this->global_point_filename_ ).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_global_point_filename_with_path()
{
  return this->global_point_filename_;
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename()
{
  return QFileInfo( this->local_point_filename_ ).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename_with_path()
{
  return this->local_point_filename_;
}

//---------------------------------------------------------------------------
bool Shape::import_point_file( QString filename, vnl_vector<double> &points )
{

  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    QMessageBox::warning( 0, "Unable to open file", "Error opening file: " + filename );
    return false;
  }

  QTextStream stream( &file );

  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();

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

    double x = list[0].toDouble();
    double y = list[1].toDouble();
    double z = list[2].toDouble();

    vtk_points->InsertNextPoint( x, y, z );
    num_points++;
  }

  points.clear();
  points.set_size( num_points * 3 );

  int idx = 0;
  for ( int i = 0; i < num_points; i++ )
  {
    double* pos = vtk_points->GetPoint( i );
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }
  return true;
}
