#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <Application/ShapeWorksStudioApp.h>

#include <Optimize/OptimizeTool.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs)
{

  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi( this );
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::on_export_xml_button_clicked()
{
  std::cerr << "Export XML\n";

  QString filename = QFileDialog::getSaveFileName( this, tr( "Save as..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->export_xml( filename );
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked()
{
  this->app_->set_status_bar( "Please wait: running optimize step..." );

  QTemporaryFile file;
  file.open();

  QString temp_file_name = file.fileName() + ".xml";

  this->export_xml( temp_file_name );
  file.close();

  QStringList args;

  args << temp_file_name;

  QProcess* optimize = new QProcess( this );
  optimize->setProcessChannelMode( QProcess::MergedChannels );
  
  std::string optimizeLocation = QCoreApplication::applicationFilePath().toStdString();
  optimizeLocation = optimizeLocation.substr(0,optimizeLocation.find_last_of("/")+1) + OPTIMIZE_EXECUTABLE;
  optimize->start(QString::fromStdString(optimizeLocation), args );

  if ( !optimize->waitForStarted() )
  {
    std::cerr << "Error: failed to start ShapeWorksRun\n";
    QMessageBox::critical( 0, "Error", "Failed to start ShapeWorksRun" );
    return;
  }

  //optimize.closeWriteChannel();

  std::cerr << "running...";

  while ( !optimize->waitForFinished( 1000 ) )
  {
    QByteArray result = optimize->readAll();
    std::cerr << "output: " << result.data() << "\n";

    QString strOut = optimize->readAllStandardOutput();
    std::cerr << strOut.toStdString() << "\n";

    strOut = optimize->readAllStandardError();
    std::cerr << strOut.toStdString() << "\n";
  }

/*
   if ( !optimize->waitForFinished() )
   {
    std::cerr << "Error running ShapeWorksRun\n";




    QMessageBox::critical( 0, "Error", "Error running ShapeWorksRun" );
    return;
   }
 */

  QByteArray result = optimize->readAll();
  std::cerr << "output: " << result.data() << "\n";

  QString strOut = optimize->readAllStandardOutput();
  std::cerr << strOut.toStdString() << "\n";

  strOut = optimize->readAllStandardError();
  std::cerr << strOut.toStdString() << "\n";

  std::cerr << "Finished running!\n";

  delete optimize;

  // load files!

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  QFileInfo fi( shapes[0]->get_original_filename_with_path() );
  QString project_path = fi.dir().absolutePath();

  QString prefix = project_path + QDir::separator() + "studio_run";

  QStringList list;

  int pad = 1;
  if ( shapes.size() >= 10 && shapes.size() < 100 )
  {
    pad = 2;
  }
  else if ( shapes.size() >= 100 && shapes.size() < 1000 )
  {
    pad = 3;
  }

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QString number = QString( "%1" ).arg( i, pad, 'd', QChar( '0' ) ).toUpper();

    list << prefix + "." + number + ".wpts";
  }

  this->project_->load_point_files( list );
  emit optimize_complete();
}

//---------------------------------------------------------------------------
bool OptimizeTool::export_xml( QString filename )
{
  std::cerr << "export to " << filename.toStdString() << "\n";

  QFile file( filename );

  if ( !file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::warning( 0, "Read only", "The file is in read only mode" );
    return false;
  }

  QSharedPointer<QXmlStreamWriter> xml_writer = QSharedPointer<QXmlStreamWriter>( new QXmlStreamWriter() );
  xml_writer->setAutoFormatting( true );
  xml_writer->setDevice( &file );
  xml_writer->writeStartDocument();

  xml_writer->writeTextElement( "number_of_particles",
                                QString::number( this->ui_->number_of_points_->value() ) );

  xml_writer->writeTextElement( "iterations_per_split",
                                QString::number( this->ui_->iterations_per_split_->value() ) );

  xml_writer->writeTextElement( "starting_regularization",
                                QString::number( this->ui_->starting_regularization_->value() ) );

  xml_writer->writeTextElement( "ending_regularization",
                                QString::number( this->ui_->ending_regularization_->value() ) );

  xml_writer->writeTextElement( "optimization_iterations",
                                QString::number( this->ui_->optimization_iterations_->value() ) );

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  QFileInfo fi( shapes[0]->get_original_filename_with_path() );
  QString project_path = fi.dir().absolutePath();

  xml_writer->writeTextElement( "output_points_prefix",
                                project_path + QDir::separator() + "studio_run" );

  // inputs
  xml_writer->writeStartElement( "inputs" );
  xml_writer->writeCharacters( "\n" );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    xml_writer->writeCharacters( shapes[i]->get_groomed_filename_with_path() + "\n" );
  }
  xml_writer->writeEndElement(); // inputs

  xml_writer->writeEndDocument();

  return true;
}

//---------------------------------------------------------------------------
void OptimizeTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void OptimizeTool::set_app( ShapeWorksStudioApp* app )
{
  this->app_ = app;
}
