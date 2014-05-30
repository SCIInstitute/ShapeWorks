#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>

#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

const QString Project::INITIAL_C( "initial" );
const QString Project::GROOMED_C( "groomed" );
const QString Project::OPTIMIZED_C( "optimized" );

//---------------------------------------------------------------------------
Project::Project()
{
  this->reset();
}

//---------------------------------------------------------------------------
Project::~Project()
{}

//---------------------------------------------------------------------------
bool Project::save_project( QString filename /* = "" */ )
{
  if ( filename == "" )
  {
    filename = this->filename_;
  }

  this->filename_ = filename;

  // open file
  QFile file( filename );

  if ( !file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::warning( 0, "Read only", "The file is in read only mode" );
    return false;
  }

  // setup XML
  QSharedPointer<QXmlStreamWriter> xml = QSharedPointer<QXmlStreamWriter>( new QXmlStreamWriter() );
  xml->setAutoFormatting( true );
  xml->setDevice( &file );
  xml->writeStartDocument();

  xml->writeStartElement( "project" );
  xml->writeAttribute( "version", "1" );

  // shapes
  xml->writeStartElement( "shapes" );
  for ( int i = 0; i < this->shapes_.size(); i++ )
  {
    xml->writeStartElement( "shape" );
    xml->writeAttribute( "id", QString::number( i ) );

    xml->writeTextElement( "initial_mesh", this->shapes_[i]->get_initial_mesh()->get_filename_with_path() );

    if ( this->groomed_present() )
    {
      xml->writeTextElement( "groomed_mesh", this->shapes_[i]->get_groomed_mesh()->get_filename_with_path() );
    }

    if ( this->reconstructed_present() )
    {
      xml->writeTextElement( "point_file", this->shapes_[i]->get_reconstructed_mesh()->get_filename_with_path() );
    }

    xml->writeEndElement(); // shape
  }
  xml->writeEndElement(); // shapes

  xml->writeEndElement(); // project

  return true;
}

//---------------------------------------------------------------------------
bool Project::load_project( QString filename )
{
  // clear the project out first
  this->reset();

  this->filename_ = filename;

  // open file
  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::warning( 0, "Unable to open file", "Error opening file" );
    return false;
  }

  // setup XML
  QSharedPointer<QXmlStreamReader> xml = QSharedPointer<QXmlStreamReader>( new QXmlStreamReader() );
  xml->setDevice( &file );

  QStringList import_files;
  QStringList groomed_files;
  QStringList point_files;

  while ( !xml->atEnd() && !xml->hasError() )
  {
    QXmlStreamReader::TokenType token = xml->readNext();
    if ( token == QXmlStreamReader::StartDocument )
    {
      continue;
    }
    if ( token == QXmlStreamReader::StartElement )
    {
      if ( xml->name() == "shapes" )
      {
        continue;
      }
      if ( xml->name() == "shape" )
      {}

      if ( xml->name() == "initial_mesh" )
      {
        import_files << xml->readElementText();
      }

      if ( xml->name() == "groomed_mesh" )
      {
        groomed_files << xml->readElementText();
      }

      if ( xml->name() == "point_file" )
      {
        point_files << xml->readElementText();
      }
    }
  }
  /* Error handling. */
  if ( xml->hasError() )
  {
    QMessageBox::critical( NULL, "ShapeWorksStudio", xml->errorString(), QMessageBox::Ok );
    return false;
  }

  this->import_files( import_files );

  this->load_groomed_files( groomed_files );

  this->load_point_files( point_files );

  return true;
}

//---------------------------------------------------------------------------
void Project::import_files( QStringList file_names )
{

  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << file_names[i].toStdString() << "\n";

    QSharedPointer<Shape> new_shape = QSharedPointer<Shape>( new Shape );
    new_shape->import_initial_file( file_names[i] );
    this->shapes_.push_back( new_shape );
  }

  this->renumber_shapes();

  if ( file_names.size() > 0 )
  {
    this->originals_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Project::load_groomed_files( QStringList file_names )
{

  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << file_names[i].toStdString() << "\n";
    this->shapes_[i]->import_groomed_file( file_names[i] );
  }

  if ( file_names.size() > 0 )
  {
    this->groomed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Project::load_point_files( QStringList file_names )
{
  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << file_names[i].toStdString() << "\n";
    if ( !this->shapes_[i]->import_point_file( file_names[i] ) )
    {
      std::cerr << "error!\n";
      // error
      return;
    }
  }
  if ( file_names.size() > 0 )
  {
    this->reconstructed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
std::vector<QSharedPointer<Shape> > Project::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void Project::remove_shapes( QList<int> list )
{
  foreach( int i, list ) {
    this->shapes_.erase( this->shapes_.begin() + i );
  }
  this->renumber_shapes();

  emit data_changed();
}

//---------------------------------------------------------------------------
void Project::reset()
{
  this->pipeline_state_ = INITIAL_C;
  this->shapes_.clear();

  this->originals_present_ = false;
  this->groomed_present_ = false;
  this->reconstructed_present_ = false;
}

//---------------------------------------------------------------------------
//QString Project::get_pipeline_state()
//{
//  return this->pipeline_state_;
//}

//---------------------------------------------------------------------------
bool Project::originals_present()
{
  return this->originals_present_;
}

//---------------------------------------------------------------------------
bool Project::groomed_present()
{
  return this->groomed_present_;
}

//---------------------------------------------------------------------------
bool Project::reconstructed_present()
{
  return this->reconstructed_present_;
}

//---------------------------------------------------------------------------
void Project::renumber_shapes()
{
  for ( int i = 0; i < this->shapes_.size(); i++ )
  {
    this->shapes_[i]->set_id( i + 1 );
  }
}
