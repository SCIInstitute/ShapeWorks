#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>

#include <Visualization/Visualizer.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QProgressDialog>

#include <TinyXML/tinyxml.h>

const QString Project::DATA_C( "data" );
const QString Project::GROOM_C( "groom" );
const QString Project::OPTIMIZE_C( "optimize" );

//---------------------------------------------------------------------------
Project::Project()
{
  this->parent_ = NULL;
  this->reset();
}

//---------------------------------------------------------------------------
Project::~Project()
{}

//---------------------------------------------------------------------------
void Project::set_parent( QWidget* parent )
{
  this->parent_ = parent;
}

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

  xml->writeTextElement( "tool_state", this->tool_state_ );
  xml->writeTextElement( "display_state", this->display_state_ );
  xml->writeTextElement( "zoom_state", QString::number( this->zoom_state_ ) );

  // shapes
  xml->writeStartElement( "shapes" );
  for ( int i = 0; i < this->shapes_.size(); i++ )
  {
    xml->writeStartElement( "shape" );
    xml->writeAttribute( "id", QString::number( i ) );

    if ( this->original_present() )
    {
      xml->writeTextElement( "initial_mesh", this->shapes_[i]->get_original_filename_with_path() );
    }

    if ( this->groomed_present() )
    {
      xml->writeTextElement( "groomed_mesh", this->shapes_[i]->get_groomed_filename_with_path() );
    }

    if ( this->reconstructed_present() )
    {
      xml->writeTextElement( "point_file", this->shapes_[i]->get_global_point_filename_with_path() );
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
  if ( !QFile::exists( filename ) )
  {
    QMessageBox::critical( NULL, "ShapeWorksStudio", "File does not exist: " + filename, QMessageBox::Ok );
    return false;
  }

  if ( Project::check_if_legacy( filename ) )
  {
    return this->load_legacy( filename );
  }

  // clear the project out first
  this->reset();

  this->filename_ = filename;

  // open file
  QFile file( filename );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::warning( 0, "Unable to open file", "Error opening file: " + filename );
    return false;
  }

  // setup XML
  QSharedPointer<QXmlStreamReader> xml = QSharedPointer<QXmlStreamReader>( new QXmlStreamReader() );
  xml->setDevice( &file );

  QStringList import_files;
  QStringList groomed_files;
  QStringList point_files;

  QString display_state;

  while ( !xml->atEnd() && !xml->hasError() )
  {
    QXmlStreamReader::TokenType token = xml->readNext();
    if ( token == QXmlStreamReader::StartDocument )
    {
      continue;
    }
    if ( token == QXmlStreamReader::StartElement )
    {
      if ( xml->name() == "tool_state" )
      {
        this->tool_state_ = xml->readElementText();
      }

      if ( xml->name() == "display_state" )
      {
        display_state = xml->readElementText();
      }
      if ( xml->name() == "zoom_state" )
      {
        this->zoom_state_ = xml->readElementText().toInt();
      }

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

  std::cerr << "tool state = " << this->tool_state_.toStdString() << "\n";

  /* Error handling. */
  if ( xml->hasError() )
  {
    QMessageBox::critical( NULL, "ShapeWorksStudio", xml->errorString(), QMessageBox::Ok );
    return false;
  }

  QStringList fixed_import_files;
  QStringList fixed_groom_files;
  QStringList fixed_point_files;

  if ( !Project::find_files( import_files, filename, fixed_import_files ) )
  {
    return false;
  }
  if ( !Project::find_files( groomed_files, filename, fixed_groom_files ) )
  {
    return false;
  }
  if ( !Project::find_files( point_files, filename, fixed_point_files ) )
  {
    return false;
  }

  this->load_original_files( fixed_import_files );

  this->load_groomed_files( fixed_groom_files );

  this->load_point_files( fixed_point_files );

  // set this after loading files so it doesn't get fiddled with
  this->display_state_ = display_state;

  return true;
}

//---------------------------------------------------------------------------
bool Project::load_legacy( QString filename )
{

  if ( !QFile::exists( filename ) )
  {
    QMessageBox::critical( NULL, "ShapeWorksStudio", "File does not exist: " + filename, QMessageBox::Ok );
    return false;
  }

  QString xml_path = QFileInfo( filename ).absolutePath();

  std::cerr << "load_legacy(" << filename.toStdString() << ")\n";
  TiXmlDocument doc( filename.toStdString().c_str() );

  if ( !doc.LoadFile() )
  {
    std::cerr << "Failed to load!\n";
    QMessageBox::critical( NULL, "ShapeWorksStudio", "Error parsing XML", QMessageBox::Ok );
    return false;
  }

  TiXmlHandle doc_handle( &doc );
  TiXmlElement* elem;

  int numShapes = 0;

  QStringList input_files;
  QStringList output_files;
  QStringList point_files;

  // point files
  elem = doc_handle.FirstChild( "point_files" ).Element();
  if ( elem )
  {
    std::istringstream buffer;
    std::string name;
    buffer.str( elem->GetText() );
    while ( buffer >> name )
    {
      std::cerr << "tinyxml: found point file: " << name << "\n";
      point_files.push_back( QString::fromStdString( name ) );
    }
  }

  // input files
  elem = doc_handle.FirstChild( "inputs" ).Element();
  if ( elem )
  {
    std::istringstream buffer;
    std::string name;
    buffer.str( elem->GetText() );
    while ( buffer >> name )
    {
      std::cerr << "tinyxml: found input file: " << name << "\n";
      input_files.push_back( QString::fromStdString( name ) );
    }
  }

  // output files
  elem = doc_handle.FirstChild( "outputs" ).Element();
  if ( elem )
  {
    std::istringstream buffer;
    std::string name;
    buffer.str( elem->GetText() );
    while ( buffer >> name )
    {
      std::cerr << "tinyxml: found output file: " << name << "\n";
      output_files.push_back( QString::fromStdString( name ) );
    }
  }

  bool is_correspondence = doc_handle.FirstChild( "number_of_particles" ).Element() != 0;

  bool has_points = doc_handle.FirstChild( "point_files" ).Element() != 0;
  bool has_inputs = doc_handle.FirstChild( "inputs" ).Element() != 0;
  bool has_outputs = doc_handle.FirstChild( "outputs" ).Element() != 0;

  // fix up file locations, if necessary

  QStringList fixed_input_files;
  QStringList fixed_output_files;
  QStringList fixed_point_files;

  if ( !Project::find_files( point_files, filename, fixed_point_files ) )
  {
    return false;
  }
  if ( !Project::find_files( input_files, filename, fixed_input_files ) )
  {
    return false;
  }
  if ( !Project::find_files( output_files, filename, fixed_output_files ) )
  {
    return false;
  }

  if ( has_inputs && !is_correspondence )
  {
    // must be a groom file
    std::cerr << "Identified as a groom file\n";

    this->load_original_files( fixed_input_files );
    this->load_groomed_files( fixed_output_files );
  }
  else if ( has_inputs && is_correspondence )
  {
    // must be a optimization file
    std::cerr << "Identified as an optimize file\n";

    this->load_original_files( input_files );
    if ( has_points )
    {
      this->load_point_files( fixed_point_files );
    }
  }
  else if ( has_points )
  {
    // must be an analysis file
    std::cerr << "Identified as an analysis file\n";

    if ( this->reconstructed_present() )
    {
      // we already have point files loaded

      if ( this->get_num_shapes() != fixed_point_files.size() )
      {
        // what to do?
        std::cerr << "different number of shapes!\n";
      }
      else
      {
        // overwrite the current point files
        this->load_point_files( fixed_point_files );
      }
    }
    else
    {
      this->load_point_files( fixed_point_files );
    }
  }

  return true;
}

//---------------------------------------------------------------------------
void Project::load_original_files( QStringList file_names )
{

  QProgressDialog progress( "Loading images...", "Abort", 0, file_names.size(), this->parent_ );
  progress.setWindowModality( Qt::WindowModal );
  progress.setMinimumDuration( 2000 );

  for ( int i = 0; i < file_names.size(); i++ )
  {

    progress.setValue( i );

    if ( progress.wasCanceled() )
    {
      break;
    }

    std::cerr << file_names[i].toStdString() << "\n";

    QSharedPointer<Shape> new_shape = QSharedPointer<Shape>( new Shape );
    new_shape->import_original_image( file_names[i], 0.5 );
    this->shapes_.push_back( new_shape );
  }

  progress.setValue( file_names.size() );

  this->renumber_shapes();

  if ( file_names.size() > 0 )
  {
    this->original_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Project::load_groomed_files( QStringList file_names )
{
  QProgressDialog progress( "Loading groomed images...", "Abort", 0, file_names.size(), this->parent_ );
  progress.setWindowModality( Qt::WindowModal );
  progress.setMinimumDuration( 2000 );

  for ( int i = 0; i < file_names.size(); i++ )
  {
    progress.setValue( i );

    if ( progress.wasCanceled() )
    {
      break;
    }

    std::cerr << file_names[i].toStdString() << "\n";
    this->shapes_[i]->import_groomed_image( file_names[i] );
  }

  progress.setValue( file_names.size() );

  if ( file_names.size() > 0 )
  {
    this->groomed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
bool Project::load_point_files( QStringList file_names )
{
  QProgressDialog progress( "Loading point files...", "Abort", 0, file_names.size(), this->parent_ );
  progress.setWindowModality( Qt::WindowModal );
  progress.setMinimumDuration( 2000 );

  std::cerr << "num file = " << file_names.size() << "\n";

  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << "Loading file " << file_names[i].toStdString() << "\n";
    progress.setValue( i );

    if ( progress.wasCanceled() )
    {
      break;
    }

    std::cerr << file_names[i].toStdString() << "\n";

    QSharedPointer<Shape> shape;

    if ( this->shapes_.size() > i )
    {
      shape = this->shapes_[i];
    }
    else
    {
      shape = QSharedPointer<Shape>( new Shape );
      this->shapes_.push_back( shape );
    }

    QFileInfo fi( file_names[i] );
    QString basename = fi.completeBaseName();
    QString ext = fi.suffix();

    if ( ext != "wpts" && ext != "lpts" )
    {
      // hmm, load only global?
      if ( QFile::exists( file_names[i] ) )
      {
        if ( !shape->import_global_point_file( file_names[i] ) )
        {
          return false;
        }
      }
    }
    else
    {
      QString local_name = fi.absolutePath() + QDir::separator() + basename + ".lpts";
      QString global_name = fi.absolutePath() + QDir::separator() + basename + ".wpts";

      if ( QFile::exists( local_name ) )
      {
        if ( !shape->import_local_point_file( local_name ) )
        {
          return false;
        }
      }

      if ( QFile::exists( global_name ) )
      {
        if ( !shape->import_global_point_file( global_name ) )
        {
          return false;
        }
      }
    }
  }

  progress.setValue( file_names.size() );

  if ( file_names.size() > 0 )
  {
    this->reconstructed_present_ = true;
    emit data_changed();
  }

  return true;
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape> > Project::get_shapes()
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
  this->filename_ = "";
  this->tool_state_ = DATA_C;
  this->display_state_ = Visualizer::MODE_ORIGINAL_C;
  this->zoom_state_ = 5;

  this->shapes_.clear();

  this->original_present_ = false;
  this->groomed_present_ = false;
  this->reconstructed_present_ = false;
  emit data_changed();
}

//---------------------------------------------------------------------------
void Project::set_tool_state( QString tool )
{
  this->tool_state_ = tool;
}

//---------------------------------------------------------------------------
QString Project::get_tool_state()
{
  return this->tool_state_;
}

//---------------------------------------------------------------------------
bool Project::original_present()
{
  return this->original_present_;
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

//---------------------------------------------------------------------------
QString Project::get_filename()
{
  return this->filename_;
}

//---------------------------------------------------------------------------
void Project::set_display_state( QString mode )
{
  this->display_state_ = mode;
}

//---------------------------------------------------------------------------
QString Project::get_display_state()
{
  return this->display_state_;
}

//---------------------------------------------------------------------------
void Project::set_zoom_state( int zoom )
{
  this->zoom_state_ = zoom;
}

//---------------------------------------------------------------------------
int Project::get_zoom_state()
{
  return this->zoom_state_;
}

//---------------------------------------------------------------------------
bool Project::check_if_legacy( QString filename )
{
  TiXmlDocument doc( filename.toStdString().c_str() );

  if ( !doc.LoadFile() )
  {
    return false;
  }

  TiXmlHandle doc_handle( &doc );

  if ( doc_handle.FirstChild( "point_files" ).Element() )
  {
    return true;
  }

  if ( doc_handle.FirstChild( "inputs" ).Element() )
  {
    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
int Project::get_num_shapes()
{
  return this->shapes_.size();
}

//---------------------------------------------------------------------------
bool Project::find_files( QStringList list, QString project_file, QStringList &fixed_list )
{
  QString xml_path = QFileInfo( project_file ).absolutePath();

  foreach( QString file, list ) {
    QString test_file = file;

    if ( !QFile::exists( test_file ) )
    {
      std::cerr << "Could not find: " << test_file.toStdString() << "\n";
      // 1. try appending the parent dir of the project file
      test_file = xml_path + QDir::separator() + file;
    }

    if ( !QFile::exists( test_file ) )
    {
      std::cerr << "Could not find: " << test_file.toStdString() << "\n";
      // 2. Try xml directory's parent
      test_file = QFileInfo( xml_path ).dir().absolutePath() + QDir::separator() + file;
    }

    if ( !QFile::exists( test_file ) )
    {
      std::cerr << "Could not find: " << test_file.toStdString() << "\n";
      // 3. Try stripping path and using project directory
      test_file = xml_path + QDir::separator() + QFileInfo( file ).fileName();
    }

    if ( !QFile::exists( test_file ) )
    {
      std::cerr << "Could not find: " << test_file.toStdString() << "\n";
      std::cerr << "Could not find file: " << file.toStdString() << "\n";
      QMessageBox::critical( NULL, "ShapeWorksStudio", "File does not exist: " + file, QMessageBox::Ok );
      return false;
    }

    std::cerr << "Fixed up " << file.toStdString() << " => " << test_file.toStdString() << "\n";

    fixed_list << test_file;
  }

  return true;
}
